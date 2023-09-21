#include "Linker.h"

int Run() {
    std::string folderPath;
    int priority;

    std::cout << "Enter the folder path: ";
    std::cin >> folderPath;

    std::cout << "Enter the process priority (-20 to 19): ";
    std::cin >> priority;

    if (priority < -20 || priority > 19) {
        std::cerr << "Invalid priority value. Priority must be between -20 and 19." << std::endl;
        return 1;
    }

    int pid = fork();  // Створюємо дочірній процес

    if (pid == -1) {
        std::cerr << "Failed to fork a child process" << std::endl;
        return 1;
    } else if (pid == 0) {
        // Дочірній процес
        std::string gitCommand = "git fetch origin";
        std::string shellCommand = "cd " + folderPath + " && " + gitCommand;

        // Встановлюємо новий пріоритет процесу
        if (nice(priority) == -1) {
            std::cerr << "Failed to set process priority" << std::endl;
            return 1;
        }

        // Запускаємо Git-команду
        int result = system(shellCommand.c_str());

        if (result == 0) {
            std::cout << "Git fetch process completed" << std::endl;
        } else {
            std::cerr << "Failed to run Git fetch process" << std::endl;
        }

        return 0;
    } else {
        // Батьківський процес
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            std::cout << "Child process exited with status: " << exitStatus << std::endl;
        } else {
            std::cerr << "Child process did not exit normally" << std::endl;
        }
    }

    return 0;
}
