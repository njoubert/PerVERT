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

#include <cassert>
#include <cstring>

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>

#include "pin.H"

#include "CallStack.h"

using namespace std;

#if defined(TARGET_MAC)
  #define MALLOC "_malloc"
  #define FREE "_free"
#else
  #define MALLOC "malloc"
  #define FREE "free"
#endif

/* ===================================================================== */
/* Forward Declarations                                                  */
/* ===================================================================== */

const string& Target2RtnName(ADDRINT ip, ADDRINT target);
const string& Target2LibName(ADDRINT target);

/* ===================================================================== */
/* Command Line Knobs                                                    */
/* ===================================================================== */

// Output file stream
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "pervert.out", "trace file");
ofstream trace;

/* ===================================================================== */
/* Stack tracing                                                         */
/* ===================================================================== */

CallStack callStack(Target2RtnName, Target2LibName);

bool main_entry_seen = false;
bool prevIpDoesPush = FALSE;

set<ADDRINT> pushIps;

void RecordPush (INS ins)
{
  pushIps.insert(INS_Address (ins));
}

bool IpDoesPush (ADDRINT ip)
{
  return (pushIps.find(ip) != pushIps.end());
}

const string& Target2RtnName(ADDRINT ip, ADDRINT target)
{
  if ( ip == 0 )
  {
    const string & name = RTN_FindNameByAddress(target);
    if (name == "")
      return *new string("[Unknown routine]");
    else
      return *new string(name);
  }
  else
  {
    ostringstream oss;
    oss << (void*) ip;
    return *new string(oss.str());
  }
}

const string& Target2LibName(ADDRINT target)
{
  PIN_LockClient();

  const RTN rtn = RTN_FindByAddress(target);
  static const string _invalid_rtn("[Unknown image]");

  string name;
  if( RTN_Valid(rtn) ) {
    name = IMG_Name(SEC_Img(RTN_Sec(rtn)));
  } else {
    name = _invalid_rtn;
  }

  PIN_UnlockClient();

  return *new string(name);
}

void ProcessInst(ADDRINT ip)
{
  prevIpDoesPush = IpDoesPush(ip);  
}

void A_ProcessSyscall(ADDRINT ip, UINT32 num, ADDRINT sp, ADDRINT arg0)
{
  // TODO: Why does MAID ignore this case?
}

void A_ProcessDirectCall(ADDRINT ip, ADDRINT target, ADDRINT sp)
{
  callStack.ProcessCall(ip, sp, target);
}

void A_ProcessIndirectCall(ADDRINT ip, ADDRINT target, ADDRINT sp)
{
  callStack.ProcessCall(ip, sp, target);
}

static void A_ProcessStub(ADDRINT ip, ADDRINT target, ADDRINT sp) 
{
  callStack.ProcessCall(ip, sp, target);
}

static void A_ProcessReturn(ADDRINT ip, ADDRINT sp) 
{
  callStack.ProcessReturn(ip, sp, prevIpDoesPush);
}

static void A_EnterMainImage(ADDRINT ip, ADDRINT target, ADDRINT sp)
{
  main_entry_seen = true;
  callStack.ProcessMainEntry(ip, sp, target);
}

static BOOL IsPLT(TRACE trace)
{
  RTN rtn = TRACE_Rtn(trace);

  // All .plt thunks have a valid RTN
  if (!RTN_Valid(rtn))
    return FALSE;

  if (".plt" == SEC_Name(RTN_Sec(rtn)))
    return TRUE;
  return FALSE;
}

static void I_Trace(TRACE trace, void *v)
{
  //FIXME if (PIN_IsSignalHandler()) {Sequence_ProcessSignalHandler(head)};

  for(BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
    INS tail = BBL_InsTail(bbl);

    // on ia-32 windows need to identify 
    // push 
    // ret
    // in order to process callstack correctly
    #if defined(TARGET_IA32)  && defined (TARGET_WINDOWS)
    for( INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins) ) {
      if (ins != tail) 
      {
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE,
            (AFUNPTR)ProcessInst,
            IARG_INST_PTR,
            IARG_END);
        if (INS_Opcode(ins)==XED_ICLASS_PUSH)
        {
          RecordPush (ins);
        }
      }
    }
    #endif

    // All calls and returns
    if( INS_IsSyscall(tail) ) {
      INS_InsertPredicatedCall(tail, IPOINT_BEFORE,
          (AFUNPTR)A_ProcessSyscall,
          IARG_INST_PTR,
          IARG_SYSCALL_NUMBER,
          IARG_REG_VALUE, REG_STACK_PTR,
          IARG_SYSCALL_ARG0,
          IARG_END);
    } else {
      if( INS_IsCall(tail) ) {
        if( INS_IsDirectBranchOrCall(tail) ) {
          ADDRINT target = INS_DirectBranchOrCallTargetAddress(tail);
          INS_InsertPredicatedCall(tail, IPOINT_BEFORE,
              (AFUNPTR)A_ProcessDirectCall,
              IARG_INST_PTR,
              IARG_ADDRINT, target,
              IARG_REG_VALUE, REG_STACK_PTR,
              IARG_END);
        } else if( !IsPLT(trace) ) {
          INS_InsertPredicatedCall(tail, IPOINT_BEFORE,
              (AFUNPTR)A_ProcessIndirectCall,
              IARG_INST_PTR,
              IARG_BRANCH_TARGET_ADDR,
              IARG_REG_VALUE, REG_STACK_PTR,
              IARG_END);
        }
      }
      if( IsPLT(trace) ) {
        INS_InsertCall(tail, IPOINT_BEFORE, 
            (AFUNPTR)A_ProcessStub,
            IARG_INST_PTR,
            IARG_BRANCH_TARGET_ADDR,
            IARG_REG_VALUE, REG_STACK_PTR,
            IARG_END);
      }
      if( INS_IsRet(tail) ) {
        INS_InsertPredicatedCall(tail, IPOINT_BEFORE,
            (AFUNPTR)A_ProcessReturn,
            IARG_INST_PTR,
            IARG_REG_VALUE, REG_STACK_PTR,
            IARG_END);
      }
    }
  }
}

static void I_ImageLoad(IMG img, void *v) 
{
  static bool main_rtn_instrumented = false;

  if( !main_rtn_instrumented ) {
    RTN rtn = RTN_FindByName(img, "main");
    if( rtn == RTN_Invalid() ) {
      rtn = RTN_FindByName(img, "__libc_start_main");      
    }
    // Instrument main
    if( rtn != RTN_Invalid() ) {
      main_rtn_instrumented = true;
      RTN_Open(rtn);
      RTN_InsertCall(rtn, IPOINT_BEFORE,
          (AFUNPTR)A_EnterMainImage,
          IARG_INST_PTR,
          IARG_ADDRINT, RTN_Address(rtn),
          IARG_REG_VALUE, REG_STACK_PTR,
          IARG_END);
      RTN_Close(rtn);
    }
  }
}

/* ===================================================================== */
/* malloc/free/read/write tracing                                        */
/* ===================================================================== */

// The last size request passed to malloc
ADDRINT request;
// All currently active heap locations
map<ADDRINT, ADDRINT> records;

// Read - print a record if the address is in a valid malloc'ed region
VOID RecordMemRead(ADDRINT ip, ADDRINT addr)
{
  for ( map<ADDRINT,ADDRINT>::iterator i = records.begin(), ie = records.end(); i != ie; ++i )
    if ( addr >= (*i).first && addr < (*i).second )
    {
      trace << "R " << addr << endl;
      trace << (void*) ip;
      callStack.DumpStack(&trace);
      trace << endl;

      return;
    }
}

// Write - print a record if the address is in a valid malloc'ed region
VOID RecordMemWrite(ADDRINT ip, ADDRINT addr)
{
  for ( map<ADDRINT,ADDRINT>::iterator i = records.begin(), ie = records.end(); i != ie; ++i )
    if ( addr >= (*i).first && addr < (*i).second )
    {
      trace << "W " << addr << endl;
      trace << (void*) ip;
      callStack.DumpStack(&trace);
      trace << endl;

      return;
    }
}

// Malloc - record the number of bytes requested
VOID RecordMalloc(ADDRINT size)
{
  request = size;
}

// Malloc return - print a record and update the currently active heap locations
VOID RecordMallocResult(ADDRINT ip, ADDRINT addr)
{
  ADDRINT end = addr + request;
  records[addr] = end;

  trace << "M " << addr << " " << end << endl;
  trace << (void*) ip;
  callStack.DumpStack(&trace);
  trace << endl;
}

// Free - print a record and update the currently active heap locations
VOID RecordFree(ADDRINT ip, ADDRINT addr)
{
  if ( records.find(addr) != records.end() )
  {
    records.erase(addr);
    trace << "F " << addr << endl;
    trace << (void*) ip;
    callStack.DumpStack(&trace);
    trace << endl;
  }
}

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
                       IARG_INST_PTR,
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
                       IARG_INST_PTR,
                       IARG_FUNCARG_ENTRYPOINT_VALUE, 0,
                       IARG_END);
        RTN_Close(freeRtn);
    }
}

/* ===================================================================== */
/* Global Cleanup                                                        */
/* ===================================================================== */

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

int main(int argc, char* argv[])
{
  if ( PIN_Init(argc, argv) ) 
    return Usage();
  PIN_InitSymbols();

  // Initialize trace
  trace.open(KnobOutputFile.Value().c_str());
  trace << hex;
  trace.setf(ios::showbase);

  // Intialize stack tracing code
  IMG_AddInstrumentFunction(I_ImageLoad, 0);
  TRACE_AddInstrumentFunction(I_Trace, 0);

  // Initialize memory tracing code
  INS_AddInstrumentFunction(Instruction, 0);
  IMG_AddInstrumentFunction(Image, 0);

  // Initialize cleanup code
  PIN_AddFiniFunction(Fini, 0);

  PIN_StartProgram();
  assert(false && "Control should never reach here!");

  return 0;
}
