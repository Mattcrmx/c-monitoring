from __future__ import annotations

import re
from enum import Enum
from dataclasses import dataclass
from typing import List, Literal


class Token(str, Enum):
    # structural tokens
    LN_BREAK = "\n"
    INDENT = "\t"
    END_STMT = ";"
    WHITESPACE = " "

    DEFINITION_START = "{"
    DEFINITION_END = "}"
    BEG_OR_END_HEADER_OR_MACRO = "#"
    MACRO_DEF = "#define"
    IFNDEF = "#ifndef"
    MACRO_END = "#endif"
    HEADER_DECL = "#include"
    CYTHON_DECL_KW = "cdef"
    ENUM_DECL = "enum"
    STRUCT_DECL = "struct"
    TYPEDEF_STRUCT_DECL = "typedef struct"
    STRUCT_END = "};"

    # special keywords declaration
    ENUM_START = "e"
    STRUCT_START = "s"
    TYPEDEF_START = "t"

    # types start letter
    INT_START = "i"
    LONG_START = "l"
    CHAR_START = "c"
    FLOAT_START = "f"
    VOID_START = "v"


class Type(str, Enum):
    INT = "int"
    FLOAT = "float"
    LONG = "long"
    LONG_LONG = "long long"
    VOID = "void"
    CHAR = "char"


class Declaration:
    def _get_indent(self, base_indent: int) -> str:
        raise f"{(base_indent + 1) * Token.INDENT}"

    def _cython_repr(self, base_indent: int) -> str:
        raise NotImplementedError

    def to_cython(self, base_indent) -> str:
        return self._get_indent(base_indent) + self._cython_repr(base_indent)


@dataclass(frozen=True)
class Argument(Declaration):
    name: str
    type: Type

    def _cython_repr(self, base_indent: int) -> str:
        return f"{self.type} {self.name}"


@dataclass(frozen=True)
class Prototype(Declaration):
    name: str
    type: Type
    arguments: List[Argument]

    def _cython_repr(self, base_indent: int) -> str:
        args = ", ".join([arg.to_cython(0) for arg in self.arguments])
        return f"{self.type} {self.name}({args})"


@dataclass(frozen=True)
class Header:
    name: str
    type: Literal["custom", "standard"]

    def expand(self):
        pass


@dataclass(frozen=True)
class Macro:
    name: str
    type: Token


@dataclass(frozen=True)
class CEnum(Declaration):
    name: str
    attributes: List[str]

    def _cython_repr(self, base_indent: int) -> str:
        attrs = "\n".join(self.attributes)
        return f"{Token.CYTHON_DECL_KW} {Token.ENUM_DECL} {self.name}:{attrs}"


@dataclass(frozen=True)
class Struct(Declaration):
    name: str
    attributes: List[Argument]

    def _cython_repr(self, base_indent: int) -> str:
        attrs = "\n".join([attr.to_cython(base_indent) for attr in self.attributes])
        return f"{Token.CYTHON_DECL_KW} {Token.STRUCT_DECL} {self.name}:{attrs}"


def parse_statement(statement: str):
    if Token.HEADER_DECL in statement:
        custom = re.search(fr'{Token.HEADER_DECL}\s+"([^"]+)"', statement)
        standard = re.search(fr'{Token.HEADER_DECL}\s+<([^>]+)>', statement)
        return Header(
            name=standard.group(1) if standard else custom.group(1),
            type="standard" if standard else "custom"
        )

    if Token.IFNDEF in statement:
        macro_type = Token.IFNDEF
    elif Token.MACRO_DEF in statement:
        macro_type = Token.MACRO_DEF
    else:
        raise ValueError("Statement is not a valid Macro")
    return Macro(
        name=re.search(fr"{macro_type}\s+(\w+)", statement).group(1),
        type=macro_type
    )


def parse_enum(raw_enum: str):
    name = re.search(r"enum\s+(\w+)\s*{", raw_enum).group(1)
    # TODO: handle multiline enums
    return CEnum(
        name=name,
        attributes=raw_enum.strip(f"{Token.ENUM_DECL} {name}" + "{ }").split(", ")
    )


def parse_struct(struct_name: str, raw_struct_declaration: str):
    # TODO: handle inline definition of Enums
    attributes = []
    for attr in raw_struct_declaration.strip('{} \n').split(';'):
        attr = attr.strip()
        if attr:  # Add only non-empty values and handle simple enums
            if Token.ENUM_DECL in attr:
                _, name, tp = attr.split(Token.WHITESPACE)
            else:
                name, tp = attr.split(Token.WHITESPACE)
            attributes.append(
                Argument(name=name, type=tp)
            )
    return Struct(name=struct_name, attributes=attributes)


def tokenize(raw_declaration: str):
    toks = []
    idx = 0
    while idx < len(raw_declaration):
        if raw_declaration[idx] in [Token.LN_BREAK, Token.END_STMT]:
            # discard line breaks and end of declaration or statement
            idx += 1

        elif raw_declaration[idx] == Token.BEG_OR_END_HEADER_OR_MACRO:
            # consume until line break or ;
            statement = ""
            while raw_declaration[idx] not in [Token.LN_BREAK, Token.END_STMT]:
                statement += raw_declaration[idx]
                idx += 1
            toks.append(parse_statement(statement))

        elif raw_declaration[idx] == Token.ENUM_START:
            # Handle enums
            if raw_declaration[idx: idx + len(Token.ENUM_DECL)] == Token.ENUM_DECL:
                raw_enum = ""
                while raw_declaration[idx] != Token.END_STMT:
                    raw_enum += raw_declaration[idx]
                    idx += 1
                toks.append(parse_enum(raw_enum))

        elif raw_declaration[idx] in [Token.STRUCT_START, Token.TYPEDEF_START]:
            # Handle structs or typedef structs
            raw_struct_declaration = ""
            if raw_declaration[idx: idx + len(Token.STRUCT_DECL)] == Token.STRUCT_DECL:
                idx = idx + len(Token.STRUCT_DECL) + 1  # include whitespace
                struct_name = ""
                while raw_declaration[idx] != Token.WHITESPACE:
                    struct_name += raw_declaration[idx]
                    idx += 1
                # peek 2 chars to see end of declaration
                while raw_declaration[idx:idx+len(Token.STRUCT_END)] != Token.STRUCT_END:
                    raw_struct_declaration += raw_declaration[idx]
                    idx += 1
                toks.append(parse_struct(struct_name, raw_struct_declaration))
                idx = idx + len(Token.STRUCT_DECL)

            elif raw_declaration[idx: idx + len(Token.TYPEDEF_STRUCT_DECL)] == Token.TYPEDEF_STRUCT_DECL:
                # first extract name
                idx = idx + len(Token.TYPEDEF_STRUCT_DECL) + 1  # include whitespace
                struct_name = ""
                while raw_declaration[idx] != Token.WHITESPACE:
                    struct_name += raw_declaration[idx]
                    idx += 1

                # extract struct definition until typedef end
                while raw_declaration[idx: idx + len(struct_name) + 1] != struct_name + Token.END_STMT:
                    raw_struct_declaration += raw_declaration[idx]
                    idx += 1

                toks.append(parse_struct(struct_name, raw_struct_declaration))
                # move pointer
                idx = idx + len(struct_name)

        elif raw_declaration[idx] == Token.INT_START:
            # TODO
            pass
        elif raw_declaration[idx] == Token.CHAR_START:
            # TODO
            pass
        elif raw_declaration[idx] == Token.FLOAT_START:
            # TODO
            pass
        elif raw_declaration[idx] == Token.LONG_START:
            # TODO
            # handle long long
            pass
        elif raw_declaration[idx] == Token.VOID_START:
            # TODO
            pass

    return toks



def parse_header():
    # remove macros, store functions declarations in a stack, discard doubles when expanding headers
    # same for struct
    pass


def expand_header(path: str):
    # everytime we encounter an include statement, expand the header and recursively traverse header files
    # replace include statement by imports
    pass


def generate_cython_proto(c_proto):
    # remove void param and ;
    # embed in cdef from module
    pass


if __name__ == '__main__':
    with open("/home/matthias/Projects/fd-watcher/src/fd_watcher/core/utils.h") as f:
        header = f.read()

    toks = tokenize(header)
    print(toks)
