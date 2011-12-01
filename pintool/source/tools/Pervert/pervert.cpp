/*BEGIN_LEGAL 
  Intel Open Source License 

  Copyright (c) 2002-2011 Intel Corporation. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
met:

Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.  Redistributions
in binary form must reproduce the above copyright notice, this list of
conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.  Neither the name of
the Intel Corporation nor the names of its contributors may be used to
endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE INTEL OR
ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
END_LEGAL */

/*
 *  This file contains an ISA-portable PIN tool for tracing memory events.
 */

/*
 * NOTES:
 * This is how you use GetSourceLocation:

 -> ADDRINT ip

 INT32 line;
 string file;

 PIN_LockClient();
 PIN_GetSourceLocation(ip, NULL, &line, &file);
 PIN_UnlockClient();

 */

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "pin.H"

using namespace std;

#if defined(TARGET_MAC)
  #define MALLOC "_malloc"
  #define FREE "_free"
#else
  #define MALLOC "malloc"
  #define FREE "free"
#endif

// Output file stream
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "pervert.out", "trace file");
ofstream trace;

// The last size request passed to malloc
ADDRINT request;
// All currently active heap locations
map<ADDRINT, ADDRINT> records;

// A read happened
VOID RecordMemRead(ADDRINT ip, ADDRINT addr)
{
  for ( map<ADDRINT,ADDRINT>::iterator i = records.begin(), ie = records.end(); i != ie; ++i )
    if ( addr >= (*i).first && addr < (*i).second )
    {
      trace << "R " << addr << endl;
      return;
    }
}

// A write happened
VOID RecordMemWrite(ADDRINT ip, ADDRINT addr)
{
  for ( map<ADDRINT,ADDRINT>::iterator i = records.begin(), ie = records.end(); i != ie; ++i )
    if ( addr >= (*i).first && addr < (*i).second )
    {
      trace << "W " << addr << endl;
      return;
    }
}

// Malloc was called, requested size bites
VOID RecordMalloc(ADDRINT size)
{
  request = size;
}

// Malloc returned, with size bytes at addr
VOID RecordMallocResult(ADDRINT addr)
{
  ADDRINT end = addr + request;
  records[addr] = end;
  trace << "M " << addr << " " << end << endl;
}

// Free was called on addr
VOID RecordFree(ADDRINT addr)
{
  if ( records.find(addr) != records.end() )
  {
    records.erase(addr);
    trace << "F " << addr << endl;
  }
}

// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v)
{
  // Instruments memory accesses using a predicated call, i.e.
  // the instrumentation is called iff the instruction will actually be executed.
  //
  // The IA-64 architecture has explicitly predicated instructions. 
  // On the IA-32 and Intel(R) 64 architectures conditional moves and REP 
  // prefixed instructions appear as predicated instructions in Pin.
  UINT32 memOperands = INS_MemoryOperandCount(ins);

  // Iterate over each memory operand of the instruction.
  for (UINT32 memOp = 0; memOp < memOperands; memOp++)
  {
    if (INS_MemoryOperandIsRead(ins, memOp))
    {
      INS_InsertPredicatedCall(
          ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead,
          IARG_INST_PTR,
          IARG_MEMORYOP_EA, memOp,
          IARG_END);
    }
    // Note that in some architectures a single memory operand can be 
    // both read and written (for instance incl (%eax) on IA-32)
    // In that case we instrument it once for read and once for write.
    if (INS_MemoryOperandIsWritten(ins, memOp))
    {
      INS_InsertPredicatedCall(
          ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,
          IARG_INST_PTR,
          IARG_MEMORYOP_EA, memOp,
          IARG_END);
    }
  }
}

// Instrument the malloc() and free() functions.  Print the input argument
// of each malloc() or free(), and the return value of malloc().
VOID Image(IMG img, VOID *v)
{
    //  Find the malloc() function.
    RTN mallocRtn = RTN_FindByName(img, MALLOC);
    if (RTN_Valid(mallocRtn))
    {
        RTN_Open(mallocRtn);

        // Instrument malloc() to print the input argument value and the return value.
        RTN_InsertCall(mallocRtn, IPOINT_BEFORE, (AFUNPTR)RecordMalloc,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_END);
        RTN_InsertCall(mallocRtn, IPOINT_AFTER, (AFUNPTR)RecordMallocResult,
                       IARG_FUNCRET_EXITPOINT_VALUE, 
                       IARG_END);

        RTN_Close(mallocRtn);
    }

    // Find the free() function.
    RTN freeRtn = RTN_FindByName(img, FREE);
    if (RTN_Valid(freeRtn))
    {
        RTN_Open(freeRtn);
        // Instrument free() to print the input argument value.
        RTN_InsertCall(freeRtn, IPOINT_BEFORE, (AFUNPTR)RecordFree,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_END);
        RTN_Close(freeRtn);
    }
}

VOID Fini(INT32 code, VOID *v)
{
  trace << "E " << endl;
  trace.close();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
  cerr << "This tool produces a trace of memory evernts" << endl;
  cerr << endl;
  cerr << KNOB_BASE::StringKnobSummary() << endl;

  return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char *argv[])
{
  PIN_InitSymbols();
  if ( PIN_Init(argc, argv) ) 
    return Usage();

  trace.open(KnobOutputFile.Value().c_str());
  trace << hex;
  trace.setf(ios::showbase);

  INS_AddInstrumentFunction(Instruction, 0);
  IMG_AddInstrumentFunction(Image, 0);
  PIN_AddFiniFunction(Fini, 0);

  // Never returns
  PIN_StartProgram();

  return 0;
}
