#include <multitasking/ProcessTable.h>
#include <lib/utility.h>

// Initializes the process table with default CPU states for each entry.
ProcessTable::ProcessTable(GlobalDescriptorTable *gdt)
{
    currentProcess = -1;
    numProcesses = 0;

    for (uint32_t i = 0; i < PROCESS_TABLE_SIZE; ++i)
        table[i].InitializeDefaultCpuState(gdt->CodeSegmentSelector());
}

ProcessTable::~ProcessTable() {}

// Returns the PID of the currently running process.
uint32_t ProcessTable::GetCurrentProcess()
{
    return currentProcess;
}

// Returns the number of active processes (excluding terminated ones).
uint32_t ProcessTable::GetSize()
{
    uint32_t ret = 0;

    // Count the number of processes that are not terminated
    for (uint32_t i = 0; i < numProcesses; ++i)
        if (table[i].status != Process::Terminated)
            ++ret;
    return ret;
}

uint32_t ProcessTable::GetPriority(uint32_t pid)
{
    return table[pid].priority;
}

Process::Status ProcessTable::GetStatus(uint32_t pid)
{
    return table[pid].status;
}

// Initializes the first process in the table with the given entry point.
void ProcessTable::CreateInitProcess(void (*entry_point)(void))
{
    Process *init = &table[0];

    init->pid = 0;
    init->cpustate->eip = (uint32_t)entry_point;
    numProcesses = 1;
    init->priority = 0; // Default priority for the init process
}

// Adds a new process to the table with the given CPU state.
pid_t ProcessTable::CreateProcessFrom(CPUState *cpustate)
{
    if (numProcesses >= PROCESS_TABLE_SIZE)
        return 0;

    Process *child = &table[numProcesses];

    child->cpustate->eip = cpustate->ebx; // Entry point from ebx
    child->pid = numProcesses++;
    child->priority = 0;

    return child->pid;
}

void ProcessTable::TerminateProcess(uint32_t entry, uint32_t status)
{
    table[entry].status = Process::Terminated;
}
/*
// Schedules the next process to run based on the Round Robin algorithm.
CPUState *ProcessTable::Schedule(CPUState *cpustate)
{
    if (numProcesses <= 0)
        return cpustate;

    if (currentProcess >= 0)
    {
        table[currentProcess].cpustate = cpustate; // Store the old CPU state
        if (table[currentProcess].status == Process::Running)
            table[currentProcess].status = Process::Ready;
    }

    // Store the current process to print screen
    int32_t printScreen[3];
    printScreen[0] = currentProcess;

    // Get the next process to run (Round Robin)
    do
    {
        if (++currentProcess >= numProcesses)
            currentProcess = 0;

    } while (table[currentProcess].status == Process::Terminated); // Skip terminated processes (they are not ready to run)

    table[currentProcess].status = Process::Running;

    // Store the next process to print screen
    printScreen[1] = currentProcess;

    // Print the context switching information
    if (printScreen[0] == -1) // First process
    {
        printScreen[0] = printScreen[1];
        printScreen[1] = GetSize();
        print("---> Context switching: PID %d is on CPU. Process Table size: %d\n", 2, printScreen);
    }
    else if (printScreen[0] == printScreen[1]) // The same process is still running
        // print("---> Context switching: PID %d is on CPU STILL\n", 1, printScreen);
        ;
    else // Context switching
    {
        printScreen[2] = GetSize();
        print("---> Context switching Happened: PID %d to PID %d. Process Table size: %d\n", 3, printScreen);
    }

    return table[currentProcess].cpustate;
}  */

// Schedule function that selects the process with the highest priority
CPUState *ProcessTable::Schedule(CPUState *cpustate)
{
    if (numProcesses <= 0)
        return cpustate;

    if (currentProcess >= 0)
    {
        table[currentProcess].cpustate = cpustate; // Store the current CPU state
        if (table[currentProcess].status == Process::Running)
            table[currentProcess].status = Process::Ready;
    }
    uint16_t currentPriority = table[currentProcess].priority; // Get the current process priority

    // Store the current process to print screen
    int32_t printScreen[5];
    printScreen[0] = currentProcess;

    int32_t smallestPriorty = 10;
    int32_t smallestPriortyIndex = 0;

    bool readyProcessFound = false;

    do
    {
        if (++currentProcess >= numProcesses)
            currentProcess = 0;

    } while (table[currentProcess].status == Process::Terminated); // Skip terminated processes (they are not ready to run) */

    table[currentProcess].status = Process::Running;

    // Store the next process to print screen
    printScreen[1] = currentProcess;

    print("Before Process %d After Process %d\n", 2, printScreen);

    // Print the context switching information
    if (printScreen[0] == -1) // First process
    {
        table[currentProcess].dynmaicPriority = 1;
        printScreen[0] = printScreen[1];
        printScreen[1] = table[currentProcess].priority;
        printScreen[2] = table[currentProcess].dynmaicPriority;
        printScreen[3] = GetSize();
        print("---> Context switching: PID %d is on CPU. Process priority: %d. The process is running for %d Process Table size: %d\n", 2, printScreen);
    }
    else if (printScreen[0] == printScreen[1]) // The same process is still running
        printScreen[1] = table[currentProcess].priority;
    printScreen[2] = table[currentProcess].dynmaicPriority;
    printScreen[3] = GetSize();
    print("---> Context switching: PID %d is on CPU STILL. Process priority: %d. Running for %d times.Process Table size: %d\n", 4, printScreen);
    table[currentProcess].dynmaicPriority++; // Increase the dynamic priority

    // If the dynamic priority is greater than 5, reset it and increase the process
    if (table[currentProcess].dynmaicPriority > 5)
    {
        table[currentProcess].dynmaicPriority = 0;
        table[currentProcess].priority + 5;
    }
    else // Context switching
    {
        table[currentProcess].dynmaicPriority = 1;
        printScreen[2] = table[currentProcess].priority;
        printScreen[3] = table[currentProcess].dynmaicPriority;
        printScreen[4] = GetSize();
        print("---> Context switching Happened: PID %d to PID %d. Process priority: %d. Running for %d times.Process Table size: %d\n", 5, printScreen);
    }

    // Return the CPU state of the selected process
    return table[currentProcess].cpustate;
}