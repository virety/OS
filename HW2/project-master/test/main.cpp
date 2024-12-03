#include <iostream>
#include "background_runner.h"

int main() {
    const char* program = "notepad.exe";  // Для примера, запускаем Блокнот
    const char* args[] = { program, NULL };

    int exit_code = run_in_background(program, args);
    std::cout << "Program exited with code: " << exit_code << std::endl;

    return 0;
}
