#!/usr/bin/env python3
"""Build/run EE209 assignments and read-only precept sources."""
from __future__ import annotations
import argparse
import os
from pathlib import Path
import re
import shlex
import signal
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
REPO = ROOT.parent
PYTHON = ROOT / '.venv/bin/python'
FLAGS = ['-std=c99', '-D_DEFAULT_SOURCE', '-D_GNU_SOURCE',
         '-D_POSIX_C_SOURCE=200809L', '-Wall', '-Wextra', '-Wpedantic', '-g']


def project(number):
    return next(ROOT.glob(f'{number} *')) / 'corrected'


def cc():
    return shlex.split(os.environ.get('CC', 'cc'))


def flags():
    result = FLAGS.copy()
    if os.environ.get('EE209_SANITIZE') == '1':
        result += ['-fsanitize=address,undefined', '-fno-omit-frame-pointer']
    return result


def command(args, cwd=None, log=None):
    result = subprocess.run([str(a) for a in args], cwd=cwd,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    if log:
        Path(log).write_bytes(result.stdout)
    if result.returncode:
        sys.stderr.buffer.write(result.stdout)
        raise RuntimeError(f'build failed: {shlex.join([str(a) for a in args])}')
    if result.stdout and not log:
        sys.stderr.buffer.write(result.stdout)


def build(number, faults=False):
    folder = project(number)
    out = folder / '.build'
    out.mkdir(exist_ok=True)
    strict = flags() + ['-Werror']
    if number == 1:
        # Keep every compilation stage requested by Assignment 1.
        command(cc() + strict + ['-E', 'wc209.c', '-o', out/'wc209.i'], folder)
        command(cc() + strict + ['-S', 'wc209.c', '-o', out/'wc209.s'], folder)
        command(cc() + strict + ['-c', 'wc209.c', '-o', out/'wc209.o'], folder)
        command(cc() + strict + [out/'wc209.o', '-o', out/'wc209'], folder)
        return [str(out/'wc209')]
    if number == 2:
        command(cc() + strict + ['str.c', 'sgrep.c', '-o', out/'sgrep'], folder)
        command(cc() + strict + ['str.c', 'test_str.c', '-o', out/'test_str'], folder)
        return [str(out/'sgrep')]
    if number == 3:
        for version in (1, 2):
            suffix = '_faults' if faults else ''
            obj = out/f'customer_manager{version}{suffix}.o'
            wrap = (['-DEE209_WRAP_ALLOC', '-include', ROOT/'tools/alloc_fail.h']
                    if faults else [])
            command(cc() + strict + wrap + ['-c', f'customer_manager{version}.c',
                                           '-o', obj], folder)
            extra = (['-DTEST_ALLOC_FAULTS', '-I', ROOT/'tools', ROOT/'tools/alloc_fail.c']
                     if faults else [])
            command(cc() + strict + extra + ['testclient.c', obj, '-o',
                       out/f'testclient{version}{suffix}'], folder)
        return [str(out/('testclient1_faults' if faults else 'testclient1'))]
    if number == 4:
        command(['clang', '-target', 'i386-unknown-linux-gnu', '-c',
                 folder/'mydc.s', '-o', out/'mydc.o'])
        return [str(PYTHON), str(ROOT/'tools/ia32.py'), str(out/'mydc.o')]
    if number == 5:
        # Separate sanitizer objects prevent stale instrumented/normal mixing.
        build_dir = '.build/sanitized' if os.environ.get('EE209_SANITIZE') == '1' else '.build/normal'
        command(['make', '-s', f'CC={os.environ.get("CC", "cc")}',
                 'CFLAGS='+' '.join(strict), f'BUILD={build_dir}'], folder)
        return [str(folder/build_dir/'ish')]
    raise ValueError('assignment number must be 1..5')


def precepts():
    base = REPO/'precepts/src'
    result = []
    for path in sorted(base.rglob('*')):
        if path.suffix == '.s' or (path.suffix == '.c' and
                re.search(r'\bmain\s*\(', path.read_text())):
            result.append(path)
    return result


def precept_source(value):
    base = (REPO/'precepts/src').resolve()
    candidates = [Path(value), base/value, REPO/value]
    path = next((p.resolve() for p in candidates if p.is_file()), None)
    if path is None:
        matches = [p for p in base.rglob('*') if p.name == value]
        if len(matches) != 1:
            raise ValueError('use an unambiguous path from precepts.sh --list')
        path = matches[0].resolve()
    if not path.is_relative_to(base) or path.suffix not in ('.c', '.s'):
        raise ValueError('precept source must be a .c/.s file under precepts/src')
    if path.suffix == '.c' and not re.search(r'\bmain\s*\(', path.read_text()):
        client = path.parent/('test'+path.name)
        if not client.exists():
            raise ValueError('module has no main; select its test client')
        path = client
    return path


def build_precept(path):
    relative = path.relative_to(REPO/'precepts/src')
    key = '--'.join(relative.parts)
    out = ROOT/'.build/precepts'/key
    out.mkdir(parents=True, exist_ok=True)
    if path.suffix == '.s':
        obj = out/'program.o'
        command(['clang', '-target', 'i386-unknown-linux-gnu', '-c', path,
                 '-o', obj], log=out/'build.log')
        program = [str(PYTHON), str(ROOT/'tools/ia32.py'), str(obj)]
    else:
        sources, includes = [path], []
        if path.name in ('testintmath.c', 'testdynarray.c'):
            sources.append(path.parent/path.name[4:])
        if path.name == 'dfa.c':
            dynamic = REPO/'precepts/src/06-dynamic-arrays/dynarray'
            sources += [dynamic/'dynarray.c', ROOT/'tools/precept_lexer.c']
            includes = ['-I', dynamic, '-include', ROOT/'tools/precept_lexer.h']
        # These examples intentionally retain warnings/teaching bugs.
        # Relax only diagnostic errors needed by historical C sources.
        precept_flags = flags() + ['-Wno-error=implicit-function-declaration',
                                  '-Wno-error=int-conversion']
        command(cc() + precept_flags + includes + sources + ['-lm', '-o', out/'program'],
                log=out/'build.log')
        program = [str(out/'program')]
    work = out/'work'
    work.mkdir(exist_ok=True)
    (work/'tempfile').write_text('somedata\n')
    (work/'input.txt').write_text('EE209 copy demo\n')
    if path.name == 'testexec.c':
        command(cc()+flags()+[REPO/'precepts/src/11-lexing-and-process-control/processes/hello.c',
                             '-o', work/'hello'], log=out/'hello-build.log')
    return program, work, out


def precept_input(path):
    name = path.stem
    if 'circle' in name: return b'5\n'
    if 'testintmath' in name: return b'6\n3\n'
    if 'power' in name: return b'2\n5\n'
    if 'sumarray' in name: return b'3\n1\n2\n3\n'
    if 'revfn' in name: return b'1\n2\n3\n4\n5\n'
    if name in ('sort2',): return b'3\n3\n1\n2\n'
    if name.startswith('sort'): return b'3\n1\n2\n'
    if name == 'dfa': return b'one 123 two\n'
    return b''


def test_precepts():
    records = []
    for path in precepts():
        program, work, out = build_precept(path)
        args = ['input.txt', 'copied.txt'] if path.name == 'cp.c' else []
        bounded = path.stem in ('testsignal', 'testsignalignore', 'testforkwaitexec')
        proc = subprocess.Popen(program + args, cwd=work, stdin=subprocess.PIPE,
                stdout=subprocess.PIPE, stderr=subprocess.PIPE, start_new_session=True)
        try:
            stdout, stderr = proc.communicate(precept_input(path), timeout=1 if bounded else 12)
            if bounded:
                raise AssertionError(f'{path.name}: expected an ongoing demonstration')
        except subprocess.TimeoutExpired:
            os.killpg(proc.pid, signal.SIGTERM)
            stdout, stderr = proc.communicate(timeout=3)
            if not bounded: raise
        (out/'stdout.txt').write_bytes(stdout)
        (out/'stderr.txt').write_bytes(stderr)
        if not bounded and proc.returncode != 0:
            raise AssertionError(f'{path}: exit {proc.returncode}: {stderr.decode(errors="replace")}')
        if path.name == 'cp.c':
            assert (work/'copied.txt').read_bytes() == (work/'input.txt').read_bytes()
        if path.suffix == '.s':
            if path.stem == 'hello': assert stdout == b'hello, world\n'
            if 'power' in path.stem: assert b'2 to the 5 power is 32.' in stdout
            if 'sumarray' in path.stem: assert b'The sum is 6.' in stdout
        records.append({'source': str(path.relative_to(REPO)),
                        'result': 'bounded-demo' if bounded else 'passed',
                        'exit': proc.returncode})
    import json
    (ROOT/'.build/precepts/results.json').write_text(json.dumps(records, indent=2))
    print(f'Precepts: {len(records)} built/executed; 3 infinite demos stopped after observation.', flush=True)
    from verify_integrity import verify
    verify()


def assignment(number, args):
    program = build(number)
    mode = args[0] if args else ''
    if mode == '--build':
        print(shlex.join(program))
        return 0
    if mode == '--test':
        from test_assignments import test_assignment
        return test_assignment(number, program)
    if mode == '--demo':
        data = {1: b'abc/*def*/ghi\n', 2: b'KAIST\nEE209\n',
                3: b'', 4: b'2\n0\n^\np\nq\n', 5: b'printf "hello EE209"\nexit\n'}[number]
        extra = ['-f', 'EE'] if number == 2 else []
        if number == 5:
            # A demo must not run unrelated commands from the real HOME/.ishrc.
            import tempfile
            demo_dir = ROOT/'.build/demos'
            demo_dir.mkdir(parents=True, exist_ok=True)
            with tempfile.TemporaryDirectory(dir=demo_dir) as temporary:
                env = os.environ.copy()
                env['HOME'] = temporary
                return subprocess.run(program, input=data, env=env,
                                      cwd=temporary).returncode
        return subprocess.run(program+extra, input=data).returncode
    if args[:1] == ['--']: args = args[1:]
    if number == 3 and not args:
        for version in (1, 2):
            result = subprocess.run([str(project(3)/'.build'/f'testclient{version}')])
            if result.returncode: return result.returncode
        return 0
    return subprocess.run(program+args).returncode


def main():
    if len(sys.argv) < 2:
        print('Usage: ee209 assignment 1..5 [--build|--test|--demo|-- args]\n'
              '       ee209 precept --list|--build-all|--test|source [args]\n'
              '       ee209 all [--with-precepts]')
        return 0
    action, *args = sys.argv[1:]
    if action == 'assignment':
        if not args: raise ValueError('specify assignment number 1..5')
        return assignment(int(args[0]), args[1:])
    if action == 'all':
        from test_assignments import test_assignment
        for number in range(1, 6):
            print(f'Assignment {number}: building and testing...', flush=True)
            test_assignment(number, build(number))
        if '--with-precepts' in args: test_precepts()
        else:
            from verify_integrity import verify
            verify()
        return 0
    if action == 'precept':
        if not args or args[0] == '--list':
            for path in precepts(): print(path.relative_to(REPO/'precepts/src'))
            return 0
        if args[0] == '--test': test_precepts(); return 0
        if args[0] == '--build-all':
            for path in precepts(): build_precept(path)
            print(f'Built {len(precepts())} precept programs under assignments/.build/precepts.')
            return 0
        path = precept_source(args[0])
        program, work, _ = build_precept(path)
        return subprocess.run(program+args[1:], cwd=work).returncode
    raise ValueError(f'unknown action: {action}')


if __name__ == '__main__':
    try:
        sys.exit(main())
    except (OSError, ValueError, RuntimeError, AssertionError, subprocess.TimeoutExpired) as error:
        print(f'ee209: {error}', file=sys.stderr)
        sys.exit(1)
