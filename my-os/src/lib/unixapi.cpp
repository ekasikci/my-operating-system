#include <lib/unixapi.h>

pid_t fork(void (*entry_point)(void))
{
    uint32_t ret = -1;

    __asm__ volatile(
        "movl %0, %%eax;"                 // Load syscall number for fork into eax
        "movl %1, %%ebx;"                 // Load function pointer (entry_point) into ebx
        "int $0x80;"                      // Trigger system call via interrupt
        :                                 // No output operands
        : "r"(sys_fork), "r"(entry_point) // Inputs: syscall number, entry_point function
        : "eax", "ebx");                  // Registers eax and ebx will be modified
    __asm__ volatile("" : "=a"(ret));     // Store return value from eax into ret

    return ret;
}

void waitpid(uint32_t pid)
{
    Process::Status ret = Process::Running;

    while (ret != Process::Terminated)
    {
        __asm__ volatile(
            "movl %0, %%eax;"             // Load syscall number for waitpid into eax
            "movl %1, %%ebx;"             // Load process ID (pid) into ebx
            "int $0x80;"                  // Trigger system call via interrupt
            :                             // No output operands
            : "r"(sys_waitpid), "r"(pid)  // Inputs: syscall number, process ID
            : "eax", "ebx");              // Registers eax and ebx will be modified
        __asm__ volatile("" : "=a"(ret)); // Store process status from eax into ret
    }
}

void execve(const char *path, char *const argv[], char *const envp[])
{
    __asm__ volatile(
        "movl %0, %%eax\n\t"                               // Load syscall number for execve into eax
        "movl %1, %%ebx\n\t"                               // Load path pointer into ebx
        "movl %2, %%ecx\n\t"                               // Load argv pointer into ecx
        "movl %3, %%edx\n\t"                               // Load envp pointer into edx
        "int $0x80"                                        // Trigger system call via interrupt
        :                                                  /* no outputs */
        : "i"(sys_execve), "m"(path), "m"(argv), "m"(envp) // Inputs: syscall number, path, argv, envp
        : "eax", "ebx", "ecx", "edx");                     // Registers eax, ebx, ecx, and edx will be modified
}

void exit(Exit exit)
{
    __asm__ volatile(
        "movl %0, %%eax;"          // Load syscall number for exit into eax
        "movl %1, %%ebx;"          // Load exit status into ebx
        "int $0x80;"               // Trigger system call via interrupt
        :                          // No output operands
        : "r"(sys_exit), "r"(exit) // Inputs: syscall number, exit status
        : "eax", "ebx");           // Registers eax and ebx will be modified
    while (true)                   // Prevent any further execution after exit
        ;
}
