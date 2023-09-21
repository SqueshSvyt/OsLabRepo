#include "Linker.h"

class GitFetchProcess {
public:
    GitFetchProcess(const std::wstring& folderPath) : folderPath_(folderPath), hProcess_(nullptr) {}

    bool Start() {
        std::wstring fetchCommand = L"cmd /c \"cd /d \"" + folderPath_ + L"\" && git fetch origin\"";
        STARTUPINFOW si = {sizeof(STARTUPINFO)};
        PROCESS_INFORMATION pi;

        if (CreateProcessW(nullptr, &fetchCommand[0], nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
            hProcess_ = pi.hProcess;
            CloseHandle(pi.hThread);
            return true;
        } else {
            std::wcerr << L"Failed to start the process." << '\n';
            return false;
        }
    }

    bool Stop() {
        if (hProcess_) {
            if (TerminateProcess(hProcess_, 0)) {
                CloseHandle(hProcess_);
                hProcess_ = nullptr;
                return true;
            } else {
                std::wcerr << L"Failed to stop the process." << '\n';
                return false;
            }
        } else {
            std::wcerr << L"No process is running." << '\n';
            return false;
        }
    }

    bool SetPriority(DWORD priority) {
        if (hProcess_) {
            if (SetPriorityClass(hProcess_, priority)) {
                return true;
            } else {
                std::wcerr << L"Failed to set process priority." << '\n';
                return false;
            }
        } else {
            std::wcerr << L"No process is running." << '\n';
            return false;
        }
    }

    bool WaitForCompletion(DWORD timeoutMilliseconds) {
        if (hProcess_) {
            DWORD result = WaitForSingleObject(hProcess_, timeoutMilliseconds);
            if (result == WAIT_OBJECT_0) {
                return true;
            } else if (result == WAIT_TIMEOUT) {
                std::wcerr << L"Process did not complete within the specified timeout." << '\n';
                return false;
            } else {
                std::wcerr << L"Waiting for process failed." << '\n';
                return false;
            }
        } else {
            std::wcerr << L"No process is running." << '\n';
            return false;
        }
    }

    ~GitFetchProcess() {
        if (hProcess_) {
            CloseHandle(hProcess_);
        }
    }

private:
    std::wstring folderPath_;
    HANDLE hProcess_;
};

int Run() {


    std::wcout << L"Enter the folder path containing Git repositories: ";
    std::wstring folderPath;
    std::wcin >> folderPath;

    GitFetchProcess gitProcess(folderPath);

    if (gitProcess.Start()) {
        std::wcout << L"Process started successfully!" << '\n';
        std::cout << "Process ID (PID) on Windows: " << GetCurrentProcessId() << '\n';

        // Змінити пріоритет процесу на нормальний
        // Зчитати і встановити пріоритет
        std::wcout << L"Enter process priority (1=below_normal, 2=normal, 3=above_normal, 4=high, 5=idle): ";
        int priorityInput;
        std::wcin >> priorityInput;

        DWORD priority = 0;

        switch (priorityInput) {
            case 1:
                priority = BELOW_NORMAL_PRIORITY_CLASS;
                break;
            case 2:
                priority = NORMAL_PRIORITY_CLASS;
                break;
            case 3:
                priority = ABOVE_NORMAL_PRIORITY_CLASS;
                break;
            case 4:
                priority = HIGH_PRIORITY_CLASS;
                break;
            case 5:
                priority = IDLE_PRIORITY_CLASS;
                break;
            default:
                std::wcerr << L"Invalid priority input." << std::endl;
                gitProcess.Stop();
                return 1;
        }

        // Змінити пріоритет процесу
        if (gitProcess.SetPriority(priority)) {
            std::wcout << L"Process priority set successfully." << std::endl;
        } else {
            std::wcerr << L"Failed to set process priority." << std::endl;
            gitProcess.Stop();
            return 1;
        }

        // Зачекати на завершення процесу або примусово завершити через 10 секунд
        if (gitProcess.WaitForCompletion(60000)) {
            std::wcout << L"Process completed successfully." << '\n';
        } else {
            std::wcout << L"Process did not complete within the specified timeout." << '\n';
            // Примусово завершити процес
            gitProcess.Stop();
        }
    } else {
        std::wcerr << L"Failed to start the process." << '\n';
    }

    return 0;
}
