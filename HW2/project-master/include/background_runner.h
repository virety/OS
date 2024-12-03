#ifndef BACKGROUND_RUNNER_H
#define BACKGROUND_RUNNER_H

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

int run_in_background(const char* program, const char* args[]);

#endif
