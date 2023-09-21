#include "Linker.h"

class GitFetchProcess {
public:
    GitFetchProcess(const std::string& folderPath) : folderPath_(folderPath) {}

    bool Start() {
        pid_t childPid = fork(); // Create a new child process

        if (childPid == -1) {
            std::cerr << "Failed to fork a new process." << std::endl;
            return false;
        }

        if (childPid == 0) {
            std::string fetchCommand = "cd \"" + folderPath_ + "\" && git fetch origin";
            int execResult = execlp("sh", "sh", "-c", fetchCommand.c_str(), nullptr);

            std::cerr << "Failed to execute the command in the child process. Error code: " << execResult << std::endl;
            exit(1);
        } else {
            int status;
            waitpid(childPid, &status, 0);

            if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                return true;
            } else {
                std::cerr << "Child process failed to execute the command." << std::endl;
                return false;
            }
        }
    }

    bool WaitForCompletion(int timeoutSeconds) {
        int pid = -1;
        int status = 0;
        int result = 0;

        pid = fork();

        if (pid == -1) {
            std::cerr << "Failed to fork." << '\n';
            return false;
        }

        if (pid == 0) {
            // Child process
            std::string fetchCommand = "cd \"" + folderPath_ + "\" && git fetch origin";
            result = std::system(fetchCommand.c_str());
            exit(result);
        } else {
            // Parent process
            int waitResult = 0;
            int timeoutCounter = 0;

            do {
                waitResult = waitpid(pid, &status, WNOHANG);
                if (waitResult == -1) {
                    std::cerr << "Waiting for process failed." << '\n';
                    return false;
                }

                if (waitResult == 0) {
                    // Process is still running
                    sleep(1);
                    timeoutCounter++;
                } else {
                    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                        return true;
                    } else {
                        std::cerr << "Process did not complete successfully." << '\n';
                        return false;
                    }
                }
            } while (timeoutCounter < timeoutSeconds);

            // Timeout reached
            std::cerr << "Process did not complete within the specified timeout." << '\n';
            return false;
        }
    }

private:
    std::string folderPath_;
};

int Run() {
    std::cout << "Enter the folder path containing Git repositories: ";
    std::string folderPath;
    std::cin >> folderPath;

    GitFetchProcess gitProcess(folderPath);

    if (gitProcess.Start()) {
        std::cout << "Process started successfully." << '\n';
        std::cout << "Process ID (PID) on Linux: " << getpid() << '\n';


        // Wait for process to complete or forcefully terminate after 1 minute
        if (gitProcess.WaitForCompletion(60)) {
            std::cout << "Process completed successfully." << '\n';
        } else {
            std::cout << "Process did not complete within the specified timeout." << '\n';
        }
    } else {
        std::cerr << "Failed to start the process." << '\n';
    }

    return 0;
}
