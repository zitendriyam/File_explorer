#include <iostream>
#include <filesystem>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

namespace fs = std::filesystem;
using std::cin;
using std::cout;
using std::endl;
using std::string;

void listFiles(const string &path) {
    try {
        cout << "Contents of " << path << ":\n";
        for (auto &entry : fs::directory_iterator(path))
            cout << (entry.is_directory() ? "[D] " : "[F] ") << entry.path().filename().string() << '\n';
    } catch (const fs::filesystem_error &e) {
        cout << "Error listing directory: " << e.what() << '\n';
    }
}

void changeDirectory(const string &path) {
    try {
        fs::current_path(path);
        cout << "Changed directory to: " << fs::current_path().string() << '\n';
    } catch (const fs::filesystem_error &e) {
        cout << "Change directory error: " << e.what() << '\n';
    }
}

void copyFile(const string &src, const string &dest) {
    try {
        fs::copy_file(src, dest, fs::copy_options::overwrite_existing);
        cout << "File copied successfully.\n";
    } catch (const fs::filesystem_error &e) {
        cout << "Copy error: " << e.what() << '\n';
    }
}

void renameFile(const string &oldp, const string &newp) {
    try {
        fs::rename(oldp, newp);
        cout << "Renamed/moved successfully.\n";
    } catch (const fs::filesystem_error &e) {
        cout << "Rename error: " << e.what() << '\n';
    }
}

void createAndWriteFile(const string &path, const string &content) {
    std::ofstream ofs(path, std::ios::binary);
    if (!ofs) {
        cout << "Failed to open file for writing: " << path << '\n';
        return;
    }
    ofs << content;
    ofs.close();
    cout << "File written: " << path << '\n';
}

void removeFile(const string &path) {
    try {
        if (fs::remove(path))
            cout << "Removed: " << path << '\n';
        else
            cout << "File not found or could not remove: " << path << '\n';
    } catch (const fs::filesystem_error &e) {
        cout << "Remove error: " << e.what() << '\n';
    }
}

void searchFile(const string &start, const string &name) {
    try {
        bool found = false;
        for (auto &p : fs::recursive_directory_iterator(start)) {
            if (p.path().filename() == name) {
                cout << "Found: " << p.path().string() << '\n';
                found = true;
            }
        }
        if (!found) cout << "No matches found under " << start << '\n';
    } catch (const fs::filesystem_error &e) {
        cout << "Search error: " << e.what() << '\n';
    }
}

void showPermissions(const string &path) {
    struct stat info;
    if (stat(path.c_str(), &info) == 0) {
        cout << "Owner: "
             << ((info.st_mode & S_IRUSR) ? "r" : "-")
             << ((info.st_mode & S_IWUSR) ? "w" : "-")
             << ((info.st_mode & S_IXUSR) ? "x" : "-") << '\n';
        cout << "Group: "
             << ((info.st_mode & S_IRGRP) ? "r" : "-")
             << ((info.st_mode & S_IWGRP) ? "w" : "-")
             << ((info.st_mode & S_IXGRP) ? "x" : "-") << '\n';
        cout << "Others: "
             << ((info.st_mode & S_IROTH) ? "r" : "-")
             << ((info.st_mode & S_IWOTH) ? "w" : "-")
             << ((info.st_mode & S_IXOTH) ? "x" : "-") << '\n';
    } else {
        perror("stat() error");
    }
}

void changePermissions(const string &path, mode_t mode) {
    if (chmod(path.c_str(), mode) == 0)
        cout << "Permissions changed.\n";
    else
        perror("chmod() error");
}

int main() {
    int choice = 0;
    string a, b, content;

    while (true) {
        cout << "\nSimple File Explorer - current: " << fs::current_path().string() << '\n';
        cout << "1) List directory\n2) Change directory\n3) Copy file\n4) Rename/move file\n";
        cout << "5) Create/write file\n6) Delete file\n7) Search file recursively\n8) Show permissions\n9) Change permissions (octal)\n0) Exit\n";
        cout << "Choose: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input. Try again.\n";
            continue;
        }
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                cout << "Path to list: ";
                std::getline(cin, a);
                if (a.empty()) a = ".";
                listFiles(a);
                break;
            case 2:
                cout << "Directory to change to: ";
                std::getline(cin, a);
                if (a.empty()) a = ".";
                changeDirectory(a);
                break;
            case 3:
                cout << "Source file: ";
                std::getline(cin, a);
                cout << "Destination path: ";
                std::getline(cin, b);
                copyFile(a, b);
                break;
            case 4:
                cout << "Old path: ";
                std::getline(cin, a);
                cout << "New path: ";
                std::getline(cin, b);
                renameFile(a, b);
                break;
            case 5:
                cout << "File to create/write: ";
                std::getline(cin, a);
                cout << "Enter content (single line). To add multi-line, use shell tools.\n";
                std::getline(cin, content);
                createAndWriteFile(a, content + "\n");
                break;
            case 6:
                cout << "File to remove: ";
                std::getline(cin, a);
                removeFile(a);
                break;
            case 7:
                cout << "Start directory: ";
                std::getline(cin, a);
                if (a.empty()) a = ".";
                cout << "Filename to search for (exact match): ";
                std::getline(cin, b);
                searchFile(a, b);
                break;
            case 8:
                cout << "Path: ";
                std::getline(cin, a);
                showPermissions(a);
                break;
            case 9: {
                cout << "Path: ";
                std::getline(cin, a);
                cout << "Enter mode in octal (e.g. 0755): ";
                string modeStr;
                std::getline(cin, modeStr);
                try {
                    mode_t mode = static_cast<mode_t>(std::stoul(modeStr, nullptr, 8));
                    changePermissions(a, mode);
                } catch (...) {
                    cout << "Invalid octal mode.\n";
                }
            } break;
