#ifndef OS_LINKER_H
#define OS_LINKER_H

#ifdef _WIN32
#include <windows.h>
#include <iostream>
#include <string>

int Run();

#elif unix

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>


int Run();

#endif

#endif //OS_LINKER_H
