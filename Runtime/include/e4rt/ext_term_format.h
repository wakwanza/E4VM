// This is an open source non-commercial project. Dear PVS-Studio, please check
// it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//

#pragma once

#include "e4platf/byte_stream_reader.h"
#include "e4rt/dist.h"
#include "e4rt/heap.h"
#include "e4rt/term.h"
#include "e4rt/vm.h"

namespace e4 {

class ExtTerm {
 private:
  static constexpr Word ETF_MARKER = 131;

  enum class Tag : Word {
    DistHeader = 68,  // contains atom cache
    // 69
    IeeeFloatExt = 70,  // 8-byte double
    // ...
    BitBinaryExt = 77,
    // ...
    Compressed = 80,
    // 81
    AtomCacheRef = 82,  // used with dist header
    // ...
    SmallIntegerExt = 97,    // 8bit integer
    IntegerExt = 98,         // 32bit big endian integer
    OldFloatStringExt = 99,  // superceded by ieee_float_ext
    AtomExt = 100,           // atom as string
    ReferenceExt = 101,      // encoded make_ref()
    PortExt = 102,           // port, similar to ref()
    PidExt = 103,
    SmallTupleExt = 104,
    LargeTupleExt = 105,
    NilExt = 106,     // empty list []
    StringExt = 107,  // 16bit size + bytes
    ListExt = 108,    // 32bit length, elements, tail (or nil)
    BinaryExt = 109,
    SmallBigExt = 110,
    LargeBigExt = 111,
    // NEW_FUN_EXT = 112,
    // EXPORT_EXT = 113,
    // NEW_REFERENCE_EXT = 114,
    SmallAtomExt = 115,
    MapExt = 116,
    // FUN_EXT = 117,
    AtomUtf8Ext = 118,
    SmallAtomUtf8Ext = 119,
  };  // enum

  static Term read_atom_string_i16(tool::Reader& r);
  static Term read_atom_string_i8(tool::Reader& r);
  static Term read_tagged_atom_string(tool::Reader& r);
  static Node* get_node(Term /*sysname*/, dist::Creation /*creation*/);
  static Term make_pid(Term sysname,
                       Word id,
                       Word serial,
                       uint8_t creation);
  static Term read_tuple(Heap& heap, tool::Reader& r, Word arity);
  static Term read_string_ext(Heap& heap, tool::Reader& r);
  static Term read_list_ext(Heap& heap, tool::Reader& r);
  static Term read_binary(Heap& heap, tool::Reader& r);

 public:
  // Term will be stored on heap (reads byte=131 first as an ETF tag)
  static Term read_with_marker(Heap& heap, tool::Reader& r);

  // Term will be stored on heap (reads type tag first)
  static Term read(Heap& heap, tool::Reader& r);
};

}  // ns e4
