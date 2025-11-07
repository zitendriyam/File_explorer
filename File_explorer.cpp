

#include <iostream>
#include <filesystem>
#include <fstream>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

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
        cout << "Error: " << e.what() << '\n';
    }
}

void changeDirectory(const string &path) {
    if (chdir(path.c_str()) == 0) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)))
            cout << "Changed directory to: " << cwd << '\n';
    } else {
        perror("chdir() error");
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
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) strcpy(cwd, ".");

    while (true) {
        cout << "\nSimple File Explorer - current: " << cwd << '\n';
        cout << "1) List directory\n2) Change directory\n3) Copy file\n4) Rename/move file\n";
        cout << "5) Create/write file\n6) Delete file\n7) Search file recursively\n8) Show permissions\n9) Change permissions (octal)\n0) Exit\n";
        cout << "Choose: ";
        if (!(cin >> choice)) { cin.clear(); cin.ignore(10000, '\n'); continue; }

        switch (choice) {
            case 1:
                cout << "Path to list: ";
                cin >> a;
                listFiles(a);
                break;
            case 2:
                cout << "Directory to change to: ";
                cin >> a;
                changeDirectory(a);
                if (getcwd(cwd, sizeof(cwd)) == nullptr) strcpy(cwd, ".");
                break;
            case 3:
                cout << "Source file: ";
                cin >> a;
                cout << "Destination path: ";
                cin >> b;
                copyFile(a, b);
                break;
            case 4:
                cout << "Old path: ";
                cin >> a;
                cout << "New path: ";
                cin >> b;
                renameFile(a, b);
                break;
            case 5:
                cout << "File to create/write: ";
                cin >> a;
                cin.ignore(); // consume newline
                cout << "Enter content (single line). To add multi-line easily, use echo/printf in shell.\n";
                std::getline(cin, content);
                createAndWriteFile(a, content + "\n");
                break;
            case 6:
                cout << "File to remove: ";
                cin >> a;
                removeFile(a);
                break;
            case 7:
                cout << "Start directory: ";
                cin >> a;
                cout << "Filename to search for (exact match): ";
                cin >> b;
                searchFile(a, b);
                break;
            case 8:
                cout << "Path: ";
                cin >> a;
                showPermissions(a);
                break;
            case 9: {
                cout << "Path: ";
                cin >> a;
                cout << "Enter mode in octal (e.g. 0755): ";
                string modeStr;
                cin >> modeStr;
                mode_t mode = static_cast<mode_t>(std::stoul(modeStr, nullptr, 8));
                changePermissions(a, mode);
            } break;
            case 0:
                cout << "Bye!\n";
                return 0;
            default:
                cout << "Invalid choice\n";
        }
    }
    return 0;
}
