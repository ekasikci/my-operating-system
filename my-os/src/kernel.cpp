#include <common/types.h>
#include <gdt.h>
#include <hardwarecommunication/interrupts.h>
#include <lib/utility.h>
#include <multitasking/ProcessTable.h>
#include <syscalls/syscallhandler.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>

using namespace myos;
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

class PrintfKeyboardEventHandler : public KeyboardEventHandler
{
public:
    void OnKeyDown(char c)
    {
        char *foo = " ";
        foo[0] = c;
        printf(foo);
    }
};

class MouseToConsole : public MouseEventHandler
{
    int8_t x, y;

public:
    MouseToConsole()
    {
        uint16_t *VideoMemory = (uint16_t *)0xb8000;
        x = 40;
        y = 12;
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 | (VideoMemory[80 * y + x] & 0xF000) >> 4 | (VideoMemory[80 * y + x] & 0x00FF);
    }

    virtual void OnMouseMove(int xoffset, int yoffset)
    {
        static uint16_t *VideoMemory = (uint16_t *)0xb8000;
        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 | (VideoMemory[80 * y + x] & 0xF000) >> 4 | (VideoMemory[80 * y + x] & 0x00FF);

        x += xoffset;
        if (x >= 80)
            x = 79;
        if (x < 0)
            x = 0;
        y += yoffset;
        if (y >= 25)
            y = 24;
        if (y < 0)
            y = 0;

        VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0x0F00) << 4 | (VideoMemory[80 * y + x] & 0xF000) >> 4 | (VideoMemory[80 * y + x] & 0x00FF);
    }
};

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor *i = &start_ctors; i != &end_ctors; i++)
        (*i)();
}

void initProcess();
void binarySearch();
void linearSearch();
void collatz();
void idleProcess();              // Loops forever
void idleProcess2();             // Loops forever
void interruptAwaitingProcess(); // Waits for an interrupt

int32_t long_running_program(int32_t n);

int32_t n = 1000;

void long_running_program_wrapper()
{
    int32_t result = long_running_program(n);
    int32_t printArr[] = {result};
    print("Long running program: Result: %d\n", 1, printArr);
    exit(exit_success);
}

uint32_t func()
{
    uint32_t ret = -1;

    __asm__ volatile("movl %0, %%eax; int $0x80;" : : "r"(sys_execve) : "eax");
    __asm__ volatile("" : "=a"(ret));

    return ret;
}

extern "C" void kernelMain(const void *multiboot_structure, uint32_t /*multiboot_magic*/)
{
    GlobalDescriptorTable gdt;
    ProcessTable processTable(&gdt);
    InterruptManager interrupts(0x20, &gdt, &processTable);
    SyscallHandler syscalls(&interrupts, 0x80);

    processTable.CreateInitProcess(initProcess);
    interrupts.Activate();

    DriverManager drvManager;

#ifdef GRAPHICSMODE
    KeyboardDriver keyboard(&interrupts, &desktop);
#else
    PrintfKeyboardEventHandler kbhandler;
    KeyboardDriver keyboard(&interrupts, &kbhandler);
#endif
    drvManager.AddDriver(&keyboard);

#ifdef GRAPHICSMODE
    MouseDriver mouse(&interrupts, &desktop);
#else
    MouseToConsole mousehandler;
    MouseDriver mouse(&interrupts, &mousehandler);
#endif
    drvManager.AddDriver(&mouse);

#ifdef GRAPHICSMODE
    VideoGraphicsArray vga;
#endif

    printf("Initializing Hardware, Stage 2\n");
    drvManager.ActivateAll();

    printf("Initializing Hardware, Stage 3\n");

#ifdef GRAPHICSMODE
    vga.SetMode(320, 200, 8);
    Window win1(&desktop, 10, 10, 20, 20, 0xA8, 0x00, 0x00);
    desktop.AddChild(&win1);
    Window win2(&desktop, 40, 15, 30, 30, 0x00, 0xA8, 0x00);
    desktop.AddChild(&win2);
#endif

    while (true)
        ; // kernelMain is going ghost mode
}

void initProcess()
{
    printf("Init: Init process is started\n");

    // Part A lifecycle
    const uint32_t numOfProcess = 8;
    pid_t pids[numOfProcess];
    void (*process[])() = {collatz, long_running_program_wrapper, binarySearch, linearSearch,
                           collatz, long_running_program_wrapper, binarySearch, linearSearch};

    /*       // Part B lifecycles

          // The first strategy

          const uint32_t numOfProcess = 10;
          pid_t pids[numOfProcess];
          void (*process[])() = {collatz, collatz, collatz, collatz,
                                 collatz, collatz, collatz, collatz,
                                 collatz, collatz};s

          // Second strategy
          const uint32_t numOfProcess = 6;
          pid_t pids[numOfProcess];
          void (*process[])() = {collatz, long_running_program_wrapper,
                                 collatz, long_running_program_wrapper,
                                 collatz, long_running_program_wrapper};

          // Third strategy
          const uint32_t numOfProcess = 12;
          pid_t pids[numOfProcess];
          void (*process[])() = {collatz, long_running_program_wrapper, binarySearch, linearSearch,
                                 collatz, long_running_program_wrapper, binarySearch, linearSearch,
                                 collatz, long_running_program_wrapper, binarySearch, linearSearch};

          // Dynamic Priority Strategy
          const uint32_t numOfProcess = 12;
          pid_t pids[numOfProcess];
          void (*process[])() = {collatz, long_running_program_wrapper, binarySearch, linearSearch};
          // The Collatz program is initially placed in the ready queue with a priority lower than the other programs. */

    // Part C lifecycles */

    /*     // Random Process Spawning with Interactive Input Handling Strategy
        const uint32_t numOfProcess = 10;
        pid_t pids[numOfProcess];
        void (*process[])() = {interruptAwaitingProcess, interruptAwaitingProcess, interruptAwaitingProcess,
                               interruptAwaitingProcess, interruptAwaitingProcess, interruptAwaitingProcess,
                               interruptAwaitingProcess, interruptAwaitingProcess, interruptAwaitingProcess,
                               interruptAwaitingProcess}; */
    /*
        // Interactive Input Priority Strategy
        const uint32_t numOfProcess = 12;
        pid_t pids[numOfProcess];
        void (*process[])() = {collatz, long_running_program_wrapper, binarySearch, linearSearch,
                               collatz, long_running_program_wrapper, binarySearch, linearSearch,
                               collatz, long_running_program_wrapper, binarySearch, linearSearch}; */

    printf("Init: Forking processes is started\n");
    for (uint32_t i = 0; i < numOfProcess; ++i)
        pids[i] = fork(process[i]);
    printf("Init: Forking processes is finished\n");

    printf("Init: Waiting for all child processes to finish\n");
    for (uint32_t i = 0; i < numOfProcess; ++i)
        waitpid(pids[i]);
    printf("Init: All child processes are finished\n");

    exit(exit_success);
}

void binarySearch()
{
    uint32_t elements[] = {10, 20, 30, 50, 60, 100, 110, 130, 150, 170};

    uint32_t left = 0, right = 9, middle;
    uint32_t searchKey = 110;
    int32_t foundIndex = -1;

    while (left <= right)
    {
        middle = (left + right) / 2;
        uint32_t currentElement = elements[middle];

        if (currentElement == searchKey)
        {
            foundIndex = middle;
            break;
        }
        else if (currentElement < searchKey)
        {
            left = middle + 1;
        }
        else
        {
            right = middle - 1;
        }
    }
    int32_t results[] = {foundIndex};
    print("Binary Search Output: %d\n", 1, results);
    exit(exit_success);
}

void linearSearch()
{
    uint32_t values[] = {10, 20, 80, 30, 60, 50, 110, 100, 130, 170};
    uint32_t target = 175;
    int32_t resultIndex = -1;

    for (uint32_t pos = 0; pos < 10; ++pos)
    {
        if (values[pos] == target)
        {
            resultIndex = pos;
            break;
        }
    }

    int32_t output[] = {resultIndex};
    print("Linear Search Output: %d\n", 1, output);

    exit(exit_success);
}

void collatz()
{
    int32_t result[1];

    for (uint32_t number = 6; number < 9; ++number)
    {
        result[0] = number;
        print("Starting Collatz Sequence for %d: ", 1, result);
        uint32_t sequence = number;

        while (sequence != 1)
        {
            result[0] = sequence;
            print("%d, ", 1, result);

            if (sequence % 2 == 0)
            {
                sequence /= 2;
            }
            else
            {
                sequence = sequence * 3 + 1;
            }
        }

        result[0] = 1;
        print("%d\n", 1, result);
    }

    exit(exit_success);
}

int32_t long_running_program(int32_t n)
{
    int32_t result = 0;
    for (uint32_t i = 0; i < n; i++)
    {
        for (uint32_t j = 0; j < n; j++)
        {
            result += i * j;
        }
    }
    return result;
    exit(exit_success);
}

void idleProcess()
{
    printf("Idle process is started...\n");
    while (1)
        ;
}

void idleProcess2()
{
    printf("Idle process 2 is started...\n");
    while (1)
        ;
}

void interruptAwaitingProcess()
{
    printf("Interrupt awaiting process is started...\n");
    while (1)
    {
        // Wait for a keyboard interrupt
        if (myos::drivers::KeyboardDriver::returnFromKeyboardDriver)
        {
            printf("Keybord interrup is detected by the process. The process is terminated\n");
            myos::drivers::KeyboardDriver::returnFromKeyboardDriver = false;
            exit(exit_success);
        }

        // Wait for a mouse interrupt
        if (myos::drivers::MouseDriver::returnFromMouseDriver)
        {
            printf("Mouse interrup is detected by the process. The process is terminated\n");
            myos::drivers::MouseDriver::returnFromMouseDriver = false;
            exit(exit_success);
        }
    }
}
