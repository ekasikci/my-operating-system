#ifndef _UNIX_API_H_
#define _UNIX_API_H_

#include <common/types.h>
#include <lib/utility.h>
#include <multitasking/Process.h>

using namespace myos::common;

// Enumeration of system call identifiers used in this API.
enum Syscall
{
    sys_exit = 1,
    sys_fork = 2,
    sys_waitpid = 7,
    sys_execve = 11
};

// Enumeration of exit status used in this API.
enum Exit
{
    exit_success = 0,
    exit_failute = 1
};

// fork system call
pid_t fork(void (*entry_point)(void));

// waitpid system call
void waitpid(uint32_t pid);

// execve system call
void execve();

// exit system call
void exit(Exit exit);

#endif