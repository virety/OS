#include "background_runner.h"
#include <iostream>
#include <string>
#include <vector>

#ifndef _WIN32  // Для UNIX (POSIX)
int run_in_background(const char* program, const char* args[]) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(program, (char* const*)args);
        perror("exec failed");
        exit(1);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    } else {
        perror("fork failed");
    }
    return -1;
}

#else  // Для Windows
int run_in_background(const char* program, const char* args[]) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Формирование командной строки
    std::string cmd = program;
    for (int i = 1; args[i] != NULL; ++i) {
        cmd += " ";
        cmd += args[i];
    }

    // Преобразование строки в модифицируемый формат для CreateProcess
    std::vector<char> cmdLine(cmd.begin(), cmd.end());
    cmdLine.push_back('\0');  // Завершающий нулевой символ

    // Запуск процесса
    if (!CreateProcessA(
            NULL, 
            cmdLine.data(),   // Передаем строку как изменяемую строку
            NULL, NULL, 
            FALSE, 
            CREATE_NO_WINDOW, // Не создаем консольное окно
            NULL, NULL, 
            &si, &pi)) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        return -1;
    }

    // Ожидание завершения процесса
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    // Закрытие дескрипторов
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return exit_code;
}
#endif
