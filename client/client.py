import enum
import socket
from ctypes import *

IP = "127.0.0.1"
PORT = 5500
ADDR = (IP, PORT)
SIZE = 1024
FORMAT = "utf-8"

#
#   Prerequisits:
#   -------------
#   If you are using Python < 3.4 run `pip install enum34`.
#
#   Problem Definition
#   ------------------
#   Structure class doesn't support enumerations out-of-the-box.
#   Consider the following (non-working) example:
#
#   class EnableDisableType(enum.IntEnum):
#       ENABLE = 1
#       DISABLE = 2

#   class SomeStructure(Structure):
#       _fields_ = [
#           ("value", c_uint32),
#           ("switch", EnableDisableType),
#       ]
#
#    Solution:
#    ---------
#    1). Use the following `StructureWithEnums` class instead of `Structure`.
#    2). Add `_map`attribute to your structure definition (which maps field names to enumeration types).
#    3)  Replace enumeration types in your `_fields_` list with `c_int`.
#    Voilà!
#
#    class SomeStructure(StructureWithEnums):
#        _fields_ = [
#            ("value", c_uint32),
#            ("switch", c_int),
#        ]
#        _map = {
#            "switch":  EnableDisableType
#        }
#


class StructureWithEnums(Structure):
    """Add missing enum feature to Structures.
    """
    _map = {}

    def __getattribute__(self, name):
        _map = Structure.__getattribute__(self, '_map')
        value = Structure.__getattribute__(self, name)
        if name in _map:
            EnumClass = _map[name]
            if isinstance(value, Array):
                return [EnumClass(x) for x in value]
            else:
                return EnumClass(value)
        else:
            return value

    def __str__(self):
        result = []
        result.append("struct {0} {{".format(self.__class__.__name__))
        for field in self._fields_:
            attr, attrType = field
            if attr in self._map:
                attrType = self._map[attr]
            value = getattr(self, attr)
            result.append("    {0} [{1}] = {2!r};".format(
                attr, attrType.__name__, value))
        result.append("};")
        return '\n'.join(result)

    __repr__ = __str__
##
## --- SNIP
# Example Code follows:
##


class OPCODE(enum.IntEnum):
    USER = 0,
    PASS = 1,
    REGISTER = 2,
    LOGOUT = 3,
    CHECK = 4,
    CHOOSE_ANWSER = 5,
    TOPIC_LEVEL = 6,
    HELP = 7,
    INFORMATION = 8


class Request(StructureWithEnums):
    _fields_ = [
        ("opcode", c_int32),
        ("data", c_char_p),
    ]
    _map = {
        "opcode": OPCODE
    }


def main():
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.connect(ADDR)

    request = Request(1, "USER haha".encode('utf-8'))
    client.send(request)

    client.close()


if __name__ == "__main__":
    main()
