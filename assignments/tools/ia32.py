#!/usr/bin/env python3
"""Run this course's ELF32 assembly objects on Unicorn, including on arm64.

This is a small relocatable-object loader plus a libc bridge, not Linux.
Unsupported symbols/relocations fail explicitly. Never translates assembly
into a C replacement. ECX/EDX are deliberately clobbered by external calls.
"""
from __future__ import annotations
import argparse
import io
import re
import struct
import sys
from elftools.elf.elffile import ELFFile
from elftools.elf.relocation import RelocationSection
from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE
from unicorn.x86_const import (UC_X86_REG_EAX, UC_X86_REG_ECX,
    UC_X86_REG_EDX, UC_X86_REG_ESP, UC_X86_REG_EIP)

PAGE = 4096
STUB = 0x01000000
STACK = 0x70000000
STACK_SIZE = 8 * 1024 * 1024
STOP = STUB + 0xF000
MASK = 0xFFFFFFFF


def signed(value):
    return value if value < 0x80000000 else value - 0x100000000


class Machine:
    def __init__(self, path, limit=50_000_000):
        self.uc = Uc(UC_ARCH_X86, UC_MODE_32)
        self.limit = limit
        self.status = None
        self.pending = []
        self.functions = {}
        self.uc.mem_map(STUB, 0x10000)
        self.uc.mem_map(STACK, STACK_SIZE)
        self._load(path)
        self.uc.hook_add(UC_HOOK_CODE, self._bridge,
                         begin=STUB, end=STUB + 0xFFFF)

    def word(self, address):
        return struct.unpack('<I', self.uc.mem_read(address, 4))[0]

    def write_word(self, address, value):
        self.uc.mem_write(address, struct.pack('<I', value & MASK))

    def string(self, address):
        data = bytearray()
        for offset in range(1 << 20):
            byte = self.uc.mem_read(address + offset, 1)[0]
            if byte == 0:
                return bytes(data)
            data.append(byte)
        raise ValueError('unterminated string passed to libc bridge')

    def _load(self, path):
        with open(path, 'rb') as stream:
            elf = ELFFile(stream)
            if elf.elfclass != 32 or elf['e_machine'] != 'EM_386':
                raise ValueError('expected a 32-bit i386 ELF object')
            sections = {}
            cursor = 0x00100000
            for index, section in enumerate(elf.iter_sections()):
                if section['sh_flags'] & 2 and section['sh_size']:
                    size = (section['sh_size'] + PAGE - 1) & -PAGE
                    self.uc.mem_map(cursor, size)
                    if section['sh_type'] != 'SHT_NOBITS':
                        self.uc.mem_write(cursor, section.data())
                    sections[index] = cursor
                    cursor += size
            symbols = elf.get_section_by_name('.symtab')
            if symbols is None:
                raise ValueError('object has no symbol table')
            addresses = {}
            for index, symbol in enumerate(symbols.iter_symbols()):
                section = symbol['st_shndx']
                if section == 'SHN_UNDEF':
                    if not symbol.name:
                        addresses[index] = 0
                        continue
                    supported = {'printf', 'scanf', '__isoc99_scanf',
                        'getchar', 'putchar', 'puts', 'isdigit',
                        'isspace', 'atoi', 'exit'}
                    if symbol.name not in supported:
                        raise ValueError(f'unsupported libc symbol: {symbol.name}')
                    address = STUB + len(self.functions) * 16
                    self.functions[address] = symbol.name
                    self.uc.mem_write(address, b'\xc3')
                    addresses[index] = address
                elif section == 'SHN_ABS':
                    addresses[index] = symbol['st_value']
                elif section == 'SHN_COMMON':
                    size = (symbol['st_size'] + PAGE - 1) & -PAGE
                    self.uc.mem_map(cursor, max(PAGE, size))
                    addresses[index] = cursor
                    cursor += max(PAGE, size)
                elif section in sections:
                    addresses[index] = sections[section] + symbol['st_value']
                else:
                    continue
                if symbol.name == 'main':
                    self.entry = addresses[index]
            if not hasattr(self, 'entry'):
                raise ValueError('object has no main function')
            for section in elf.iter_sections():
                if not isinstance(section, RelocationSection):
                    continue
                if section['sh_info'] not in sections:
                    continue
                for relocation in section.iter_relocations():
                    place = sections[section['sh_info']] + relocation['r_offset']
                    symbol = addresses[relocation['r_info_sym']]
                    addend = (relocation['r_addend'] if section.is_RELA()
                              else self.word(place))
                    kind = relocation['r_info_type']
                    if kind == 1:       # R_386_32: S + A
                        value = symbol + addend
                    elif kind in (2, 4): # R_386_PC32 / PLT32: S + A - P
                        value = symbol + addend - place
                    else:
                        raise ValueError(f'unsupported i386 relocation: {kind}')
                    self.write_word(place, value)

    def getbyte(self):
        return self.pending.pop() if self.pending else sys.stdin.buffer.read(1)

    def nonspace(self):
        byte = self.getbyte()
        while byte and byte in b' \t\r\n\v\f':
            byte = self.getbyte()
        return byte

    def scan(self, fmt, args):
        # Only the integer/string conversions used in the course examples.
        # Respect field widths and leave unmatched input available.
        assigned = 0
        for match in re.finditer(rb'%(\d*)([dus])', fmt):
            width = int(match[1] or b'1048576')
            byte = self.nonspace()
            if not byte:
                return assigned if assigned else -1
            token = bytearray()
            if match[2] == b's':
                while byte and byte not in b' \t\r\n\v\f' and len(token) < width:
                    token.extend(byte)
                    byte = self.getbyte()
                if byte:
                    self.pending.append(byte)
                self.uc.mem_write(args[assigned], bytes(token) + b'\0')
            else:
                if byte in (b'+', b'-'):
                    token.extend(byte)
                    byte = self.getbyte()
                digits = 0
                while byte and byte in b'0123456789' and len(token) < width:
                    token.extend(byte)
                    digits += 1
                    byte = self.getbyte()
                if byte:
                    self.pending.append(byte)
                if not digits:
                    return assigned
                self.write_word(args[assigned], int(token))
            assigned += 1
        return assigned

    def printf(self, fmt, args):
        output = bytearray()
        last = 0
        index = 0
        for match in re.finditer(rb'%(?:%|[-+ #0]*\d*(?:\.\d+)?[ducsxX])', fmt):
            output.extend(fmt[last:match.start()])
            conversion = match[0][-1:]
            if conversion == b'%':
                output.extend(b'%')
            else:
                value = args[index]
                index += 1
                if conversion == b's':
                    output.extend(self.string(value))
                elif conversion == b'c':
                    output.append(value & 255)
                else:
                    number = signed(value) if conversion == b'd' else value
                    if conversion in (b'x', b'X'):
                        output.extend(format(number, conversion.decode()).encode())
                    else:
                        output.extend(str(number).encode())
            last = match.end()
        output.extend(fmt[last:])
        sys.stdout.buffer.write(output)
        sys.stdout.buffer.flush()
        return len(output)

    def _bridge(self, uc, address, size, unused):
        if address == STOP:
            uc.emu_stop()
            return
        if address not in self.functions:
            raise ValueError(f'execution at unknown libc bridge address {address:#x}')
        name = self.functions[address]
        sp = uc.reg_read(UC_X86_REG_ESP)
        # Read only actual arguments; the operand stack may be empty.
        arg = lambda index: self.word(sp + 4 + index * 4)
        if name == 'printf':
            fmt = self.string(arg(0))
            count = len(re.findall(rb'%(?!%)[-+ #0]*\d*(?:\.\d+)?[ducsxX]', fmt))
            result = self.printf(fmt, [arg(i + 1) for i in range(count)])
        elif name in ('scanf', '__isoc99_scanf'):
            fmt = self.string(arg(0))
            count = len(re.findall(rb'%\d*[dus]', fmt))
            result = self.scan(fmt, [arg(i + 1) for i in range(count)])
        elif name == 'getchar':
            byte = self.getbyte()
            result = byte[0] if byte else -1
        elif name == 'putchar':
            result = arg(0) & 255
            sys.stdout.buffer.write(bytes([result]))
            sys.stdout.buffer.flush()
        elif name == 'puts':
            sys.stdout.buffer.write(self.string(arg(0)) + b'\n')
            sys.stdout.buffer.flush()
            result = 1
        elif name in ('isdigit', 'isspace'):
            value = signed(arg(0))
            result = int(48 <= value <= 57 if name == 'isdigit'
                         else value in (9, 10, 11, 12, 13, 32))
        elif name == 'atoi':
            match = re.match(rb'\s*([+-]?\d+)', self.string(arg(0)))
            result = int(match[1]) if match else 0
        elif name == 'exit':
            self.status = arg(0) & 255
            uc.emu_stop()
            return
        else:
            raise ValueError(f'unimplemented bridge: {name}')
        uc.reg_write(UC_X86_REG_EAX, result & MASK)
        uc.reg_write(UC_X86_REG_ECX, 0xCCCCCCCC)
        uc.reg_write(UC_X86_REG_EDX, 0xDDDDDDDD)
        uc.reg_write(UC_X86_REG_ESP, sp + 4)
        uc.reg_write(UC_X86_REG_EIP, self.word(sp))

    def run(self):
        sp = STACK + STACK_SIZE - 20 # cdecl main entry, ESP+4 aligned
        self.write_word(sp, STOP)
        self.uc.reg_write(UC_X86_REG_ESP, sp)
        self.uc.emu_start(self.entry, STOP, count=self.limit)
        if self.status is not None:
            return self.status
        if self.uc.reg_read(UC_X86_REG_EIP) != STOP:
            raise ValueError(f'instruction budget ({self.limit}) exhausted')
        return self.uc.reg_read(UC_X86_REG_EAX) & 255


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('object')
    parser.add_argument('--limit', type=int, default=50_000_000)
    args = parser.parse_args()
    try:
        return Machine(args.object, args.limit).run()
    except (OSError, ValueError, UcError) as error:
        print(f'ia32: {error}', file=sys.stderr)
        return 1


if __name__ == '__main__':
    sys.exit(main())
