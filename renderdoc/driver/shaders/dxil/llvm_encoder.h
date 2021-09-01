/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Baldur Karlsson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#pragma once

#include "api/replay/rdcarray.h"
#include "api/replay/rdcstr.h"
#include "llvm_bitwriter.h"
#include "llvm_common.h"

namespace LLVMBC
{
class BitcodeWriter
{
public:
  BitcodeWriter(bytebuf &buf);
  ~BitcodeWriter();

  void ConfigureSizes(size_t numTypes, size_t numSections, uint64_t maxAlign, uint32_t maxGlobalType);

  void BeginBlock(KnownBlock block);
  void EndBlock();

  void ModuleBlockInfo();
  void EmitGlobalVarAbbrev();

  void AutoRecord(uint32_t record, bool param, uint64_t val);
  void AutoRecord(uint32_t record, const rdcarray<uint64_t> &vals);

  template <typename RecordType>
  void Record(RecordType record)
  {
    AutoRecord((uint32_t)record, false, 0U);
  }
  template <typename RecordType>
  void Record(RecordType record, uint64_t val)
  {
    AutoRecord((uint32_t)record, true, val);
  }
  template <typename RecordType>
  void Record(RecordType record, const rdcarray<uint64_t> &vals)
  {
    AutoRecord((uint32_t)record, vals);
  }
  template <typename RecordType>
  void Record(RecordType record, const rdcstr &str)
  {
    rdcarray<uint64_t> vals;
    vals.resize(str.size());
    for(size_t i = 0; i < vals.size(); i++)
      vals[i] = str[i];
    AutoRecord((uint32_t)record, vals);
  }

private:
  void WriteAbbrevDefinition(AbbrevParam *abbrev);

  void Unabbrev(uint32_t record, bool param, uint64_t val);
  void Unabbrev(uint32_t record, const rdcarray<uint64_t> &vals);

  void Abbrev(AbbrevParam *abbrev, uint32_t record, uint64_t val);
  void Abbrev(AbbrevParam *abbrev, uint32_t record, const rdcarray<uint64_t> &vals);

  void WriteAbbrevParam(const AbbrevParam &abbrev, uint64_t val);

  uint32_t GetAbbrevID(uint32_t id);

  BitWriter b;

  uint32_t m_NumTypeBits;
  uint32_t m_GlobalTypeBits;
  uint32_t m_NumSectionBits;
  uint32_t m_AlignBits;

  size_t abbrevSize;
  uint32_t numAbbrevs;
  KnownBlock curBlock;

  uint32_t m_GlobalVarAbbrev;
  AbbrevParam m_GlobalVarAbbrevDef[10] = {};

  struct BlockContext
  {
    KnownBlock block;
    size_t offset;
    uint32_t numAbbrevs;
  };

  rdcarray<BlockContext> blockStack;
};

};    // namespace LLVMBC
