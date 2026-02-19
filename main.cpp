#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

// ================= STUDENT CLASS =================
class Student {
private:
    string name;
    string indexNumber;

public:
    Student(string n = "", string i = "") {
        name = n;
        indexNumber = i;
    }

    string getName() const { return name; }
    string getIndexNumber() const { return indexNumber; }

    void display() const {
        cout << left << setw(25) << name
             << setw(15) << indexNumber << endl;
    }

    string toFileString() const {
        return name + "," + indexNumber;
    }
};

// ================= GLOBAL DATA =================
vector<Student> students;

// ================= FILE FUNCTIONS =================
void loadStudents() {
    ifstream file("students.txt");
    if (!file) return;

    string line;
    while (getline(file, line)) {
        size_t pos = line.find(",");
        if (pos != string::npos) {
            string name = line.substr(0, pos);
            string index = line.substr(pos + 1);
            students.push_back(Student(name, index));
        }
    }
    file.close();
}

void saveStudents() {
    ofstream file("students.txt");
    for (const Student &s : students) {
        file << s.toFileString() << endl;
    }
    file.close();
}

// ================= STUDENT MANAGEMENT =================
void registerStudent() {
    string name, index;

    cout << "\nEnter Student Name: ";
    cin.ignore();
    getline(cin, name);

    cout << "Enter Index Number: ";
    getline(cin, index);

    students.push_back(Student(name, index));
    saveStudents();

    cout << "Student Registered Successfully!\n";
}

void viewStudents() {
    if (students.empty()) {
        cout << "\nNo students registered.\n";
        return;
    }

    cout << "\nRegistered Students:\n";
    cout << left << setw(25) << "Name"
         << setw(15) << "Index Number" << endl;
    cout << "-------------------------------------------\n";

    for (const Student &s : students) {
        s.display();
    }
}

void searchStudent() {
    string index;
    cout << "\nEnter Index Number to Search: ";
    cin >> index;

    for (const Student &s : students) {
        if (s.getIndexNumber() == index) {
            cout << "\nStudent Found:\n";
            s.display();
            return;
        }
    }

    cout << "Student Not Found.\n";
}

// ================= CREATE ATTENDANCE SESSION =================
void createSession() {
    if (students.empty()) {
        cout << "\nNo students registered.\n";
        return;
    }

    string courseCode, date, startTime;
    int duration;

    cout << "\nEnter Course Code: ";
    cin >> courseCode;

    cout << "Enter Date (YYYY_MM_DD): ";
    cin >> date;

    cout << "Enter Start Time: ";
    cin >> startTime;

    cout << "Enter Duration (minutes): ";
    cin >> duration;

    string filename = "session_" + courseCode + "_" + date + ".txt";
    ofstream file(filename);

    file << "Course: " << courseCode << endl;
    file << "Date: " << date << endl;
    file << "Start Time: " << startTime << endl;
    file << "Duration: " << duration << " mins\n\n";

    int present = 0, absent = 0, late = 0;

    for (const Student &s : students) {
        int choice;
        cout << "\nMark attendance for "
             << s.getName() << " ("
             << s.getIndexNumber() << ")\n";
        cout << "1. Present\n2. Absent\n3. Late\nChoice: ";
        cin >> choice;

        string status;

        if (choice == 1) {
            status = "Present";
            present++;
        }
        else if (choice == 2) {
            status = "Absent";
            absent++;
        }
        else {
            status = "Late";
            late++;
        }

        file << s.getName() << ","
             << s.getIndexNumber() << ","
             << status << endl;
    }

    file << "\nSummary:\n";
    file << "Present: " << present << endl;
    file << "Absent: " << absent << endl;
    file << "Late: " << late << endl;

    file.close();

    cout << "\nSession Saved Successfully!\n";
}

// ================= VIEW ALL SESSIONS =================
void viewSessionReport() {
    vector<string> sessionFiles;

    // Scan current directory
    for (const auto &entry : fs::directory_iterator(".")) {
        string filename = entry.path().filename().string();

        if (filename.find("session_") == 0 &&
            filename.find(".txt") != string::npos) {
            sessionFiles.push_back(filename);
        }
    }

    if (sessionFiles.empty()) {
        cout << "\nNo session files found.\n";
        return;
    }

    cout << "\nAvailable Sessions:\n";
    for (int i = 0; i < sessionFiles.size(); i++) {
        cout << i + 1 << ". "
             << sessionFiles[i] << endl;
    }

    int choice;
    cout << "\nSelect session number to view: ";
    cin >> choice;

    if (choice < 1 || choice > sessionFiles.size()) {
        cout << "Invalid selection.\n";
        return;
    }

    ifstream file(sessionFiles[choice - 1]);
    if (!file) {
        cout << "Error opening file.\n";
        return;
    }

    string line;
    cout << "\n===== SESSION REPORT =====\n";
    while (getline(file, line)) {
        cout << line << endl;
    }

    file.close();
}

// ================= MENU =================
void menu() {
    cout << "\n===== DIGITAL ATTENDANCE SYSTEM =====\n";
    cout << "1. Register Student\n";
    cout << "2. View Students\n";
    cout << "3. Search Student\n";
    cout << "4. Create Attendance Session\n";
    cout << "5. View All Sessions\n";
    cout << "6. Exit\n";
    cout << "Choose option: ";
}

// ================= MAIN =================
int main() {
    loadStudents();

    int choice;

    do {
        menu();
        cin >> choice;

        switch (choice) {
            case 1: registerStudent(); break;
            case 2: viewStudents(); break;
            case 3: searchStudent(); break;
            case 4: createSession(); break;
            case 5: viewSessionReport(); break;
            case 6: cout << "Exiting program...\n"; break;
            default: cout << "Invalid option.\n";
        }

    } while (choice != 6);

    return 0;
}
