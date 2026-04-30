#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <psapi.h>

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "psapi.lib")

using namespace std;

// ---------------- DECOY FILES ----------------
vector<string> decoyFiles = {
    "Important\\passwords.txt",
    "Important\\bank_info.txt",
    "Important\\employee_records.txt"
};

// ---------------- TIME ----------------
string getTime() {
    time_t now = time(0);
    char buffer[26];
    ctime_s(buffer, sizeof(buffer), &now);
    buffer[24] = '\0';
    return string(buffer);
}

// ---------------- LOGGING ----------------
void logEvent(const string& msg) {
    ofstream log("log.txt", ios::app);
    log << "[" << getTime() << "] " << msg << endl;
}

// ---------------- ALERT ----------------
void triggerAlert(const string& message) {
    MessageBoxA(NULL, message.c_str(), "DFIR Honeypot Alert", MB_OK | MB_ICONWARNING);
}

// ---------------- DIRECTORY ----------------
void createDirectory() {
    CreateDirectoryA("Important", NULL);
}

// ---------------- DECOY FILES ----------------
void createDecoys() {
    for (const auto& file : decoyFiles) {
        ofstream f(file);

        if (file.find("passwords.txt") != string::npos) {
            f << "Admin: admin\nPassword: P@ssw0rd123!\n";
        }
        else if (file.find("bank_info.txt") != string::npos) {
            f << "Bank: First National Bank\nAccount: 123456789\nRouting: 987654321\n";
        }
        else if (file.find("employee_records.txt") != string::npos) {
            f << "Employee: John Doe\nID: EMP-4421\nSalary: CONFIDENTIAL\n";
        }

        f.close();
    }
}

// ---------------- FOREGROUND PROCESS ----------------
string getForegroundProcess() {
    HWND hwnd = GetForegroundWindow();
    DWORD pid;
    GetWindowThreadProcessId(hwnd, &pid);

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);

    if (hProcess) {
        char name[MAX_PATH];
        GetModuleBaseNameA(hProcess, NULL, name, MAX_PATH);
        CloseHandle(hProcess);
        return string(name);
    }

    return "UNKNOWN_PROCESS";
}

// 🔥 FIXED SYS MON FILE EVENT (REAL ATTRIBUTION)
// 🔥 FIXED SYS MON PROCESS ATTRIBUTION (RELIABLE)

string getSysmonProcessEvent() {

    string cmd =
        "powershell -Command \"Get-WinEvent -LogName 'Microsoft-Windows-Sysmon/Operational' "
        "| Where-Object { $_.Id -eq 1 } "
        "| Select-Object -First 1 -ExpandProperty Message\"";

    FILE* pipe = _popen(cmd.c_str(), "r");
    if (!pipe) return "SYSMON_ERROR";

    char buffer[1024];
    string output;

    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }

    _pclose(pipe);

    if (output.empty())
        return "NO_SYSMON_MATCH";

    return output;
}

// ---------------- SYS MON REGISTRY EVENTS (UNCHANGED) ----------------
string getSysmonRegistryEvent() {

    FILE* pipe = _popen(
        "powershell -Command \"Get-WinEvent -LogName 'Microsoft-Windows-Sysmon/Operational' "
        "| Where-Object { $_.Id -eq 13 -and $_.Message -like '*VPN_Backup*' } "
        "| Select-Object -First 1 -ExpandProperty Message\"",
        "r"
    );

    if (!pipe) return "SYSMON_REGISTRY_ERROR";

    char buffer[512];
    string output;

    while (fgets(buffer, sizeof(buffer), pipe)) {
        output += buffer;
    }

    _pclose(pipe);

    if (output.empty())
        return "NO_REGISTRY_MATCH";

    return output;
}

// ---------------- REGISTRY WATCHER (UNCHANGED) ----------------
DWORD WINAPI watchRegistry(LPVOID lpParam) {

    HKEY hKey;

    if (RegOpenKeyExA(
        HKEY_CURRENT_USER,
        "Software\\CorpSecurity\\VPN_Backup",
        0,
        KEY_NOTIFY,
        &hKey) != ERROR_SUCCESS) {

        cout << "Registry monitor failed\n";
        return 1;
    }

    cout << "Registry monitoring active...\n";

    while (true) {

        LONG result = RegNotifyChangeKeyValue(
            hKey,
            TRUE,
            REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_CHANGE_NAME,
            NULL,
            FALSE
        );

        if (result == ERROR_SUCCESS) {

            Sleep(1000); // allow Sysmon to log

            string sysmon = getSysmonRegistryEvent();

            string alert =
                "[ALERT] REGISTRY HONEYPOT TRIGGERED!\n"
                "Key: HKCU\\Software\\CorpSecurity\\VPN_Backup\n\n"
                "Sysmon Details:\n" + sysmon;

            cout << alert << endl;
            logEvent(alert);
            triggerAlert("Registry Honeypot Triggered!");
        }
    }
    RegCloseKey(hKey);
    return 0;
}

// ---------------- MAIN ----------------
int main() {

    createDirectory();
    createDecoys();

    // registry honeypot (UNCHANGED)
    HKEY hKey;
    RegCreateKeyExA(
        HKEY_CURRENT_USER,
        "Software\\CorpSecurity\\VPN_Backup",
        0,
        NULL,
        0,
        KEY_WRITE,
        NULL,
        &hKey,
        NULL
    );

    RegSetValueExA(
        hKey,
        "AdminBackupToken",
        0,
        REG_SZ,
        (const BYTE*)"TOKEN-ADMIN-8834-SECRET-DO-NOT-USE",
        strlen("TOKEN-ADMIN-8834-SECRET-DO-NOT-USE") + 1
    );

    RegCloseKey(hKey);

    logEvent("Registry honeypot created");

    CreateThread(NULL, 0, watchRegistry, NULL, 0, NULL);

    HANDLE hDir = CreateFileA(
        "Important",
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    cout << "DFIR Honeypot Active - Monitoring...\n";

    char buffer[1024];
    DWORD bytes;

    while (true) {

        if (ReadDirectoryChangesW(
            hDir,
            buffer,
            sizeof(buffer),
            FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME |
            FILE_NOTIFY_CHANGE_LAST_WRITE |
            FILE_NOTIFY_CHANGE_SIZE,
            &bytes,
            NULL,
            NULL
        )) {

            FILE_NOTIFY_INFORMATION* info = (FILE_NOTIFY_INFORMATION*)buffer;

            wstring wfile(info->FileName, info->FileNameLength / 2);
            string fileStr(wfile.begin(), wfile.end());

            string action;

            switch (info->Action) {
            case FILE_ACTION_ADDED: action = "CREATED"; break;
            case FILE_ACTION_REMOVED: action = "DELETED"; break;
            case FILE_ACTION_MODIFIED: action = "MODIFIED"; break;
            default: action = "OTHER"; break;
            }

            string process = getForegroundProcess();


            Sleep(750); // 🔥 give Sysmon time to write event

            string sysmon = getSysmonProcessEvent();

            string baseLog =
                action + " | File: " + fileStr +
                " | Process: " + process;

            cout << baseLog << endl;
            logEvent(baseLog);

            bool isDecoy = false;
            for (auto& d : decoyFiles) {
                if (d == "Important\\" + fileStr) {
                    isDecoy = true;
                    break;
                }
            }

            if (isDecoy) {

                string alert =
                    "[ALERT] FILE DECOY TRIGGERED | " + baseLog +
                    "\n\nSysmon Attribution:\n" + sysmon;

                cout << alert << endl;
                logEvent(alert);

                triggerAlert("File Honeypot Triggered!");
            }
        }
    }

    return 0;
}

