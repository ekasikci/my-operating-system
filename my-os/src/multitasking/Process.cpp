#include <multitasking/Process.h>

using namespace myos;
using namespace myos::common;

Process::Process()
{
    pid = -1;
    status = Ready;
    priority = 0;
    dynmaicPriority = 0; // Adjusts the priority of the process based on its behavior
}

Process::~Process() {}

void Process::InitializeDefaultCpuState(uint16_t cs)
{
    cpustate = (CPUState*)(stack + STACK_SIZE - sizeof(CPUState));
    
    cpustate->eax = 0;
    cpustate->ebx = 0;
    cpustate->ecx = 0;
    cpustate->edx = 0;

    cpustate->esi = 0;
    cpustate->edi = 0;
    cpustate->ebp = 0;
    
    cpustate->error = 0;    

    //cpustate-> eip = (uint32_t)entrypoint;
    cpustate->cs = cs; //gdt->CodeSegmentSelector();
    cpustate->eflags = 0x202;
    cpustate->esp = (uint32_t)cpustate;
    // cpustate -> ss	state segment is not used because we don't deal with different securit levels 
}
