#!/usr/bin/env python3
"""Verify original assignment bytes (ignoring only WRONG comment lines)
and every precepts path/content/mode against the start-of-task snapshot.
This command performs no writes.
"""
import hashlib
import json
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
PATTERN = rb'(?m)^(?:/\* WRONG: [^\r\n]* \*/|# WRONG: [^\r\n]*)\r?\n'


def verify():
    manifest = json.loads((ROOT/'tools/integrity.json').read_text())
    for relative, expected in manifest['original_assignments'].items():
        path = ROOT/relative
        data = path.read_bytes()
        if expected['strip_wrong_comments']:
            data = re.sub(PATTERN, b'', data)
        assert hashlib.sha256(data).hexdigest() == expected['sha256'], relative
        assert path.stat().st_mode & 0o777 == expected['mode'], relative+' (mode)'
    base = ROOT.parent/'precepts'
    current = {str(p.relative_to(base)): {
        'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),
        'mode':p.stat().st_mode & 0o777}
        for p in base.rglob('*') if p.is_file()}
    assert current == manifest['precepts'], 'precepts changed since task start'
    assert not list(ROOT.glob('*/old/originals')), 'temporary originals folder remains'
    print(f'Integrity: {len(manifest["original_assignments"])} original files preserved; '
          f'{len(current)} precepts files unchanged.', flush=True)
    return len(current)


if __name__ == '__main__':
    verify()
