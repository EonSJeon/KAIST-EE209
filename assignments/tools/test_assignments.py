"""Regression tests derived from the supplied assignment PDFs.

The historical sample binaries/test clients are absent; these are our
own independent oracles and regression cases, not course grader results.
"""
from __future__ import annotations
import itertools
import json
import os
from pathlib import Path
import random
import re
import resource
import select
import signal
import subprocess
import tempfile
import time
from run import ROOT, PYTHON, project, build


def invoke(program, args=(), data=b'', cwd=None, env=None, timeout=15, **kwargs):
    result = subprocess.run([*program, *map(str, args)], input=data,
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, cwd=cwd,
        env=env, timeout=timeout, **kwargs)
    return result


def successful(result, expected=None):
    assert result.returncode == 0, (result.returncode, result.stderr[:2000])
    assert result.stderr == b'', result.stderr[:2000]
    if expected is not None:
        assert result.stdout == expected, (result.stdout[:2000], expected[:2000])


def wc_oracle(data):
    position, text = 0, bytearray()
    while position < len(data):
        start = data.find(b'/*', position)
        if start < 0:
            text.extend(data[position:]); break
        text.extend(data[position:start])
        end = data.find(b'*/', start + 2)
        if end < 0:
            line = data[:start].count(b'\n') + 1
            return 1, b'', f'Error: line {line}: unterminated comment\n'.encode()
        text.extend(b' ' + b'\n'*data[start:end+2].count(b'\n'))
        position = end + 2
    lines = data.count(b'\n') + bool(data)
    return 0, f'{lines} {len(text.split())} {len(text)}\n'.encode(), b''


def test_wc(program, work):
    cases = [b'', b'\n', b'abc\n', b'/', b'//', b'/**/', b'/*',
        b'abc/*def*/ghi\n', b'abc/*def\nghi*/jkl\nmno\n',
        b'abc/*def/*ghi*/jkl*/mno\n', b'abcdef\nghi/*\n',
        b'/*x**/', b'/***/', b'///**/', b'a\t\v\f\r\nb', b'\xff /*\xff*/\xfe']
    rng = random.Random(209)
    for _ in range(300):
        cases.append(bytes(rng.choice(b'aa /*\n\t') for _ in range(rng.randrange(200))))
    cases += [b'a'*100000 + b'/*'+b'x'*100000+b'*/b\n']
    for data in cases:
        result = invoke(program, data=data)
        expected = wc_oracle(data)
        assert (result.returncode, result.stdout, result.stderr) == expected, (data[:300], result, expected)
    # A directory descriptor produces a real read error on POSIX systems.
    descriptor = os.open(work, os.O_RDONLY)
    try:
        result = subprocess.run(program, stdin=descriptor,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, timeout=5)
    finally:
        os.close(descriptor)
    assert result.returncode != 0 and b'cannot read standard input' in result.stderr
    return len(cases)+1


def test_sgrep(program, work):
    successful(invoke(program, ['-f', ''], b'a\n\nb'), b'a\n\nb')
    successful(invoke(program, ['-f', 'a'], b'abc\nx\na'), b'abc\na')
    successful(invoke(program, ['-r', 'aba', 'X'], b'ababa\n'), b'Xba\n')
    successful(invoke(program, ['-r', 'a', ''], b'aaa\nb'), b'\nb')
    successful(invoke(program, ['-r', 'a', 'a'*1023], b'a'*1022+b'\n'), b'a'*(1022*1023)+b'\n')
    for option in (['-f', 'a'], ['-r', 'a', 'b']):
        successful(invoke(program, option, b'a'*1022+b'\n'))
        successful(invoke(program, option, b'a'*1023))
        result = invoke(program, option, b'a'*1023+b'\n')
        assert result.returncode != 0 and b'too long' in result.stderr
    for args in ([], ['-f'], ['-x', 'a'], ['-r', '', 'a'],
                 ['-f', 'a'*1024], ['-r', 'a', 'b'*1024], ['-d', 'a'*1024, 'x']):
        result = invoke(program, args)
        assert result.returncode != 0 and result.stderr
    one, two = work/'one.txt', work/'two.txt'
    one.write_bytes(b'same\nleft\n'); two.write_bytes(b'same\nright\n')
    expected = f'{one}@2:left\n{two}@2:right\n'.encode()
    successful(invoke(program, ['-d', one, two]), expected)
    one.write_bytes(b''); two.write_bytes(b'extra\n')
    result = invoke(program, ['-d', one, two])
    assert result.returncode != 0 and b'ends early at line 1' in result.stderr
    one.write_bytes(b'same\n'); two.write_bytes(b'same\nextra\n')
    result = invoke(program, ['-d', one, two])
    assert result.returncode != 0 and b'ends early at line 2' in result.stderr
    one.write_bytes(b'line\n'); two.write_bytes(b'line')
    successful(invoke(program, ['-d', one, two]), b'')
    result = invoke(program, ['-d', one, work/'absent'])
    assert result.returncode != 0 and b'failed to open' in result.stderr
    two.write_bytes(b'x'*1024)
    result = invoke(program, ['-d', one, two])
    assert result.returncode != 0 and b'too long' in result.stderr
    client = project(2)/'.build/test_str'
    successful(invoke([str(client)]))
    return 25


def test_customer(program, work):
    folder = project(3)/'.build'
    outputs = []
    for version in (1, 2):
        result = invoke([str(folder/f'testclient{version}')], timeout=30)
        successful(result)
        outputs.append(result.stdout.decode().strip())
    build(3, faults=True)
    for version in (1, 2):
        result = invoke([str(folder/f'testclient{version}_faults')], timeout=30)
        # CreateCustomerDB's original diagnostics are intentionally retained.
        assert result.returncode == 0, (result.returncode, result.stderr)
        assert b'allocation failure/rollback/leak checks passed' in result.stdout
        outputs.append(result.stdout.decode().strip())
    (work/'customer-results.txt').write_text('\n'.join(outputs)+'\n')
    return 4


def test_dc(program, work):
    commands, expected = [], []
    def case(tokens, output):
        commands.extend(['c', *map(str, tokens)])
        expected.extend(map(str, output))
    case(['2','0','^','p'], [1])
    case(['0','0','^','p'], [1])
    case(['_2','31','^','p'], [-2147483648])
    case(['1','2147483647','^','p'], [1])
    for left, right, quotient, remainder in [('_7','3',-2,-1),
            ('7','_3',-2,1),('_7','_3',2,-1),('_2147483648','1',-2147483648,0)]:
        case([left,right,'/','p'], [quotient])
        case([left,right,'%','p'], [remainder])
    case(['1','2','3','r','f'], [2,3,1])
    case(['4','d','*','p'], [16])
    case(['+','1','+','p'], ['dc: stack empty','dc: stack empty',1])
    case(['p','d','r','f','c','f'], ['dc: stack empty']*3)
    case(['9','r','p'], ['dc: stack empty',9])
    case(['12abc','_3abc','_','qfoo','999999999999999999999','p'], ['dc: stack empty'])
    case(['8','0','/','f'], ['dc: divide by zero',0,8])
    case(['2','_1','^','f'], [-1,2])
    case(['x'*3000,'12x','5','p'], [5])
    rng = random.Random(209)
    for _ in range(100):
        left, right = rng.randrange(-1000,1000), rng.randrange(-1000,1000)
        operator = rng.choice(['+','-','*'])
        value = {'+':left+right,'-':left-right,'*':left*right}[operator]
        convert = lambda number: '_'+str(-number) if number<0 else str(number)
        case([convert(left),convert(right),operator,'p'], [value])
    # Stress printing across several libc calls without consuming data.
    case([*range(200),'f','r','p'], [*range(199,-1,-1),198])
    commands += ['q']
    result = invoke(program, data=('\n'.join(commands)+'\n').encode(), timeout=30)
    successful(result, ('\n'.join(expected)+'\n').encode())
    successful(invoke(program, data=b''), b'')
    successful(invoke(program, data=b'7\np'), b'7\n')
    return 124


def without_prompts(output): return output.replace(b'% ', b'')


def shell_env(home):
    env = os.environ.copy()
    env['HOME'] = str(home)
    return env


def read_until(proc, needle, timeout=4):
    data = bytearray()
    deadline = time.monotonic()+timeout
    while needle not in data:
        left = deadline-time.monotonic()
        if left <= 0: raise AssertionError(f'timed out waiting for {needle!r}: {bytes(data)!r}')
        ready, _, _ = select.select([proc.stdout], [], [], left)
        if not ready: continue
        byte = os.read(proc.stdout.fileno(), 1)
        if not byte: raise AssertionError(f'shell exited early: {proc.poll()}, {bytes(data)!r}')
        data.extend(byte)
    return bytes(data)


def signal_cases(program, work, env):
    def start():
        proc = subprocess.Popen(program, cwd=work, env=env, stdin=subprocess.PIPE,
            stdout=subprocess.PIPE, stderr=subprocess.PIPE, start_new_session=True)
        read_until(proc,b'% ')
        return proc
    def cleanup(proc):
        if proc.poll() is None:
            os.killpg(proc.pid,signal.SIGKILL)
        proc.communicate(timeout=5)
    proc=start()
    try:
        os.kill(proc.pid,signal.SIGINT)
        proc.stdin.write(b'printf alive\n');proc.stdin.flush()
        read_until(proc,b'alive')
        os.kill(proc.pid,signal.SIGQUIT)
        read_until(proc,b'within 5 seconds to exit.')
        os.kill(proc.pid,signal.SIGQUIT)
        assert proc.wait(timeout=4)==0
    finally: cleanup(proc)
    proc=start()
    try:
        os.kill(proc.pid,signal.SIGQUIT)
        read_until(proc,b'within 5 seconds to exit.')
        time.sleep(5.3)
        os.kill(proc.pid,signal.SIGQUIT)
        read_until(proc,b'within 5 seconds to exit.')
        assert proc.poll() is None
        os.kill(proc.pid,signal.SIGQUIT)
        assert proc.wait(timeout=4)==0
    finally: cleanup(proc)
    for number in (signal.SIGINT,signal.SIGQUIT):
        proc=start()
        try:
            proc.stdin.write(b'sleep 30\n');proc.stdin.flush()
            deadline=time.monotonic()+3
            while True:
                table=subprocess.check_output(['ps','-axo','pid=,ppid=']).splitlines()
                children=[row.split()[0] for row in table
                          if len(row.split())==2 and int(row.split()[1])==proc.pid]
                if children: break
                if time.monotonic()>deadline: raise AssertionError('child did not start')
                time.sleep(.03)
            # Allow exec to install the child image before terminal-style delivery.
            time.sleep(.1)
            os.killpg(proc.pid,number)
            read_until(proc,b'% ')
            proc.stdin.write(b'printf resumed\nexit\n');proc.stdin.flush()
            output,error=proc.communicate(timeout=5)
            assert proc.returncode==0 and b'resumed' in output and not error,(output,error)
        finally: cleanup(proc)
    # Second SIGQUIT while waiting for foreground children must also exit/reap.
    proc=start()
    try:
        proc.stdin.write(b'sleep 30\n');proc.stdin.flush()
        time.sleep(.2)
        os.kill(proc.pid,signal.SIGQUIT)
        read_until(proc,b'within 5 seconds to exit.')
        os.kill(proc.pid,signal.SIGQUIT)
        assert proc.wait(timeout=4)==0
    finally: cleanup(proc)


def test_shell(program, work):
    home, cwd = work/'home', work/'cwd'
    home.mkdir();cwd.mkdir()
    env=shell_env(home)
    (cwd/'.ishrc').write_text('printf SHOULD_NOT_RUN\n')
    def run(data, **kwargs):
        return invoke(program, data=data, cwd=cwd, env=env, **kwargs)
    successful(run(b''),b'% ')
    successful(run(b'   \t \nexit\n'),b'% % ')
    result=run(b'printf "[%s][%s][%s]\\n" one""two "" "a|b"\nexit\n')
    successful(result)
    assert b'[onetwo][][a|b]\n' in result.stdout,result.stdout
    result=run(b'printf "[%s]\\n" "a""b"\nexit\n')
    successful(result); assert b'[ab]\n' in result.stdout
    result=run(b'printf last')
    successful(result); assert without_prompts(result.stdout)==b'last'
    result=run(b'printf "abc\\n"|tr a-z A-Z|wc -c\nexit\n')
    successful(result); assert without_prompts(result.stdout).strip()==b'4'
    large=(f'{PYTHON} -c "import sys;sys.stdout.write(\'x\'*200000)"|cat|wc -c\nexit\n').encode()
    result=run(large)
    successful(result); assert without_prompts(result.stdout).strip()==b'200000'
    def low_limit(): resource.setrlimit(resource.RLIMIT_NOFILE,(64,64))
    result=run(b'printf x|cat|cat\n'*160+b'exit\n',preexec_fn=low_limit,timeout=30)
    successful(result); assert without_prompts(result.stdout)==b'x'*160
    (cwd/'source').write_text('abc\n')
    result=run(b'cat<source|tr a-z A-Z>target\ncat target\nexit\n')
    successful(result);assert (cwd/'target').read_bytes()==b'ABC\n'
    assert (cwd/'target').stat().st_mode & 0o777 == 0o600
    (cwd/'target').chmod(0o644)
    result=run(b'printf z>target\nexit\n')
    successful(result);assert (cwd/'target').read_bytes()==b'z'
    assert (cwd/'target').stat().st_mode & 0o777 == 0o600
    result=run(b'setenv EE209_TEST "some value"\nprintenv EE209_TEST\n'
               b'unsetenv EE209_TEST\nprintenv EE209_TEST\n'
               b'setenv EE209_TEST\nprintenv EE209_TEST\nexit\n')
    successful(result);assert b'some value\n' in result.stdout
    result=run(b'cd\npwd\nexit\n')
    successful(result);assert str(home).encode() in result.stdout
    invalid=[b'|cat',b'cat|',b'cat||wc',b'cat <',b'cat >|wc',
        b'cat >bad|wc',b'cat|cat<source',b'cat<source<source',
        b'printf x>first>second',b'<source',b'"unterminated',
        b'setenv',b'unsetenv',b'cd a b',b'exit argument',
        b'cd|cat',b'printf x|cd',b'setenv X y>bad',b'cat|exit']
    result=run(b'\n'.join(invalid)+b'\nprintf recovered\nexit\n')
    assert result.returncode==0 and b'recovered' in result.stdout
    assert len(result.stderr.splitlines())==len(invalid),result.stderr
    assert all(line.startswith((program[0]+': ').encode()) for line in result.stderr.splitlines())
    assert not (cwd/'first').exists() and not (cwd/'second').exists() and not (cwd/'bad').exists()
    result=run(b'EE209_no_such_program\ncat<absent\ncd absent\nsetenv A=B C\n'
               b'printf survived\nexit\n')
    assert result.returncode==0 and b'survived' in result.stdout
    assert b'No such file or directory' in result.stderr and b'Invalid argument' in result.stderr
    result=run(b'x'*5000+b'\nprintf afterlong\nexit\n')
    assert result.returncode==0 and b'too long' in result.stderr and b'afterlong' in result.stdout
    # HOME/.ishrc should be read/echoed, while cwd/.ishrc is ignored.
    (home/'.ishrc').write_text('printf startup\nsetenv EE209_RC fromhome\n')
    result=run(b'printenv EE209_RC\nexit\n')
    successful(result)
    assert result.stdout.startswith(b'% printf startup\nstartup% setenv EE209_RC fromhome\n')
    assert b'fromhome\n' in result.stdout and b'SHOULD_NOT_RUN' not in result.stdout
    (home/'.ishrc').write_text('exit\nprintf MUST_NOT_RUN\n')
    successful(run(b'printf ALSO_MUST_NOT_RUN\n'),b'% exit\n')
    (home/'.ishrc').unlink()
    result=invoke(program,data=b'cd\nexit\n',cwd=cwd,
                  env={k:v for k,v in env.items() if k!='HOME'})
    assert result.returncode==0 and b'HOME is not set' in result.stderr
    signal_cases(program,cwd,env)
    return 45


def test_assignment(number, program):
    base=ROOT/'.build/tests';base.mkdir(parents=True,exist_ok=True)
    with tempfile.TemporaryDirectory(prefix=f'assignment{number}-',dir=base) as temporary:
        work=Path(temporary)
        count={1:test_wc,2:test_sgrep,3:test_customer,4:test_dc,5:test_shell}[number](program,work)
        if number==3:
            (ROOT/'.build/customer-results.txt').write_bytes((work/'customer-results.txt').read_bytes())
    result={'assignment':number,'result':'passed','groups':count,
            'sanitizers':os.environ.get('EE209_SANITIZE')=='1'}
    out=ROOT/'.build';out.mkdir(exist_ok=True)
    (out/f'assignment{number}-results.json').write_text(json.dumps(result,indent=2)+'\n')
    print(f'Assignment {number}: {count} regression groups passed.',flush=True)
    return 0
