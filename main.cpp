#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>
#include <cctype>
#include <iomanip>

using namespace std;

// ===========================
// DATA STRUCTURES
// ===========================

class Student {
public:
    string name;
    string indexNumber;

    Student(const string &n, const string &i) : name(n), indexNumber(i) {}

    void display() const {
        cout << "  Name: " << name << " | Index: " << indexNumber << endl;
    }
};

class AttendanceSession {
public:
    string courseCode;
    string date;
    string startTime;
    int duration;

    AttendanceSession(const string &c, const string &d, const string &s, int dur)
        : courseCode(c), date(d), startTime(s), duration(dur) {}

    void displaySession() const {
        cout << "  Course: " << courseCode << " | Date: " << date
             << " | Time: " << startTime << " | Duration: " << duration << "h" << endl;
    }
};

class AttendanceRecord {
public:
    string studentIndex;
    string courseCode;
    string status;

    AttendanceRecord(const string &s, const string &c, const string &st)
        : studentIndex(s), courseCode(c), status(st) {}
};

// Global containers
vector<Student> students;
vector<AttendanceSession> sessions;
vector<AttendanceRecord> attendanceRecords;

// ===========================
// UTILITY FUNCTIONS
// ===========================

void trim(string &s) {
    s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
    s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), s.end());
}

void clearInputBuffer() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void printSection(const string &title) {
    cout << "\n" << string(50, '=') << endl;
    cout << "  " << title << endl;
    cout << string(50, '=') << endl;
}

void printMenu() {
    printSection("STUDENT ATTENDANCE SYSTEM");
    cout << "  1.  Register Student\n";
    cout << "  2.  View All Students\n";
    cout << "  3.  Search Student\n";
    cout << "  4.  Create Attendance Session\n";
    cout << "  5.  Mark Attendance\n";
    cout << "  6.  Update Attendance Record\n";
    cout << "  7.  View Session Attendance\n";
    cout << "  8.  Display Attendance Summary\n";
    cout << "  9.  Export to Excel (CSV)\n";
    cout << "  10. Exit\n";
    cout << string(50, '-') << endl;
    cout << "  Enter your choice: ";
}

// ===========================
// FILE OPERATIONS
// ===========================

void loadStudents() {
    ifstream file("students.txt");
    if (!file.is_open()) {
        return; // File doesn't exist yet, that's fine
    }

    string line;
    while (getline(file, line)) {
        trim(line);
        if (line.empty()) continue;

        // Remove trailing carriage return
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        trim(line);

        // Parse CSV line
        size_t commaPos = line.find(',');
        if (commaPos == string::npos) continue;

        string name = line.substr(0, commaPos);
        string index = line.substr(commaPos + 1);
        trim(name);
        trim(index);

        if (!name.empty() && !index.empty()) {
            students.emplace_back(name, index);
        }
    }
    file.close();
}

void saveStudentToFile(const string &name, const string &index) {
    ofstream file("students.txt", ios::app);
    if (file.is_open()) {
        file << name << "," << index << endl;
        file.close();
    } else {
        cerr << "  ERROR: Could not write to students.txt\n";
    }
}

void exportToCSV() {
    if (attendanceRecords.empty()) {
        cout << "  No attendance records to export.\n";
        return;
    }

    ofstream file("Attendance_Report.csv");
    if (!file.is_open()) {
        cout << "  ERROR: Failed to create Attendance_Report.csv\n";
        return;
    }

    // Write header
    file << "Student Index,Course Code,Status" << endl;

    // Write data
    for (const auto &record : attendanceRecords) {
        file << record.studentIndex << ","
             << record.courseCode << ","
             << record.status << endl;
    }

    file.close();
    cout << "\n  SUCCESS! Data exported to 'Attendance_Report.csv'\n";
    cout << "  Open the file in Microsoft Excel or any spreadsheet application.\n";
}

// ===========================
// STUDENT MANAGEMENT
// ===========================

void registerStudent() {
    clearInputBuffer();
    cout << "\n--- Register New Student ---\n";

    cout << "Enter student name: ";
    string name;
    getline(cin, name);
    trim(name);

    cout << "Enter index number: ";
    string index;
    getline(cin, index);
    trim(index);

    if (name.empty() || index.empty()) {
        cout << "  ERROR: Name and index cannot be empty.\n";
        return;
    }

    // Check for duplicate index
    for (const auto &s : students) {
        if (s.indexNumber == index) {
            cout << "  ERROR: Student with this index already exists.\n";
            return;
        }
    }

    students.emplace_back(name, index);
    saveStudentToFile(name, index);

    cout << "\n  SUCCESS: Student registered!\n";
    cout << "  " << name << " (" << index << ")\n";
}

void viewStudents() {
    printSection("ALL REGISTERED STUDENTS");

    if (students.empty()) {
        cout << "  No students registered yet.\n";
        return;
    }

    cout << "\n  Total students: " << students.size() << "\n\n";
    for (size_t i = 0; i < students.size(); i++) {
        cout << "  [" << (i + 1) << "] ";
        students[i].display();
    }
}

void searchStudent() {
    cout << "\n--- Search Student ---\n";
    cout << "Enter index number to search: ";
    string index;
    cin >> index;

    for (const auto &s : students) {
        if (s.indexNumber == index) {
            cout << "\n  FOUND:\n";
            s.display();
            return;
        }
    }
    cout << "  Student not found.\n";
}

// ===========================
// SESSION MANAGEMENT
// ===========================

void createSession() {
    clearInputBuffer();
    cout << "\n--- Create Attendance Session ---\n";

    cout << "Enter course code: ";
    string course;
    getline(cin, course);
    trim(course);

    cout << "Enter date (YYYY-MM-DD): ";
    string date;
    getline(cin, date);
    trim(date);

    cout << "Enter start time (HH:MM): ";
    string time;
    getline(cin, time);
    trim(time);

    cout << "Enter duration (in hours): ";
    int duration;
    cin >> duration;

    sessions.emplace_back(course, date, time, duration);

    // Save session to file
    string filename = "session_" + course + "_" + date + ".txt";
    ofstream file(filename, ios::app);
    if (file.is_open()) {
        file << course << "," << date << "," << time << "," << duration << endl;
        file.close();
    }

    cout << "\n  SUCCESS: Session created!\n";
}

// ===========================
// ATTENDANCE MARKING
// ===========================

void markAttendance() {
    if (students.empty()) {
        cout << "  ERROR: No students registered. Register students first.\n";
        return;
    }

    if (sessions.empty()) {
        cout << "  ERROR: No sessions created. Create a session first.\n";
        return;
    }

    printSection("MARK ATTENDANCE");

    cout << "\n  Available Sessions:\n";
    for (size_t i = 0; i < sessions.size(); i++) {
        cout << "\n  [" << (i + 1) << "]\n";
        sessions[i].displaySession();
    }

    cout << "\n  Select session number: ";
    int sessionChoice;
    cin >> sessionChoice;

    if (sessionChoice < 1 || sessionChoice > (int)sessions.size()) {
        cout << "  Invalid selection.\n";
        return;
    }

    const auto &selectedSession = sessions[sessionChoice - 1];

    clearInputBuffer();
    cout << "\n  Marking attendance for course: " << selectedSession.courseCode << endl;
    cout << "  (P=Present, A=Absent, L=Late)\n";
    cout << string(50, '-') << endl;

    for (const auto &student : students) {
        cout << "\n  " << student.name << " (" << student.indexNumber << ") - Status: ";
        char status;
        cin >> status;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        string statusStr;
        if (status == 'P' || status == 'p') {
            statusStr = "Present";
        } else if (status == 'A' || status == 'a') {
            statusStr = "Absent";
        } else if (status == 'L' || status == 'l') {
            statusStr = "Late";
        } else {
            statusStr = "Absent"; // Default to absent
        }

        attendanceRecords.emplace_back(student.indexNumber, selectedSession.courseCode, statusStr);
    }

    cout << "\n  SUCCESS: Attendance marked for " << students.size() << " students.\n";
}

void updateAttendance() {
    if (attendanceRecords.empty()) {
        cout << "  No attendance records to update.\n";
        return;
    }

    printSection("UPDATE ATTENDANCE");

    cout << "\n  Attendance Records:\n";
    for (size_t i = 0; i < attendanceRecords.size(); i++) {
        cout << "  [" << (i + 1) << "] "
             << attendanceRecords[i].studentIndex << " - "
             << attendanceRecords[i].courseCode << " - "
             << attendanceRecords[i].status << endl;
    }

    cout << "\n  Select record to update (1-" << attendanceRecords.size() << "): ";
    int choice;
    cin >> choice;

    if (choice < 1 || choice > (int)attendanceRecords.size()) {
        cout << "  Invalid selection.\n";
        return;
    }

    cout << "  New status (P/A/L): ";
    char newStatus;
    cin >> newStatus;

    string statusStr;
    if (newStatus == 'P' || newStatus == 'p') {
        statusStr = "Present";
    } else if (newStatus == 'A' || newStatus == 'a') {
        statusStr = "Absent";
    } else if (newStatus == 'L' || newStatus == 'l') {
        statusStr = "Late";
    } else {
        statusStr = "Absent";
    }

    attendanceRecords[choice - 1].status = statusStr;
    cout << "  SUCCESS: Record updated.\n";
}

// ===========================
// REPORTING
// ===========================

void viewSessionAttendance() {
    if (attendanceRecords.empty()) {
        cout << "  No attendance records available.\n";
        return;
    }

    clearInputBuffer();
    cout << "\n--- View Session Attendance ---\n";
    cout << "Enter course code: ";
    string courseCode;
    getline(cin, courseCode);
    trim(courseCode);

    printSection("ATTENDANCE FOR " + courseCode);

    int count = 0;
    for (const auto &record : attendanceRecords) {
        if (record.courseCode == courseCode) {
            cout << "  " << record.studentIndex << " : " << record.status << endl;
            count++;
        }
    }

    if (count == 0) {
        cout << "  No records found for course: " << courseCode << endl;
    }
}

void attendanceSummary() {
    if (attendanceRecords.empty()) {
        cout << "  No attendance records to summarize.\n";
        return;
    }

    printSection("ATTENDANCE SUMMARY");

    int presentCount = 0, absentCount = 0, lateCount = 0;

    for (const auto &record : attendanceRecords) {
        if (record.status == "Present") {
            presentCount++;
        } else if (record.status == "Absent") {
            absentCount++;
        } else if (record.status == "Late") {
            lateCount++;
        }
    }

    cout << "\n  Total Records: " << attendanceRecords.size() << endl;
    cout << "\n  Present: " << presentCount << endl;
    cout << "  Absent:  " << absentCount << endl;
    cout << "  Late:    " << lateCount << endl;

    if (attendanceRecords.size() > 0) {
        double presentPercentage = (presentCount * 100.0) / attendanceRecords.size();
        cout << "\n  Attendance Rate: " << fixed << setprecision(2) << presentPercentage << "%" << endl;
    }
}

// ===========================
// MAIN PROGRAM
// ===========================

int main() {
    loadStudents();

    int choice;
    bool running = true;

    while (running) {
        printMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                registerStudent();
                break;
            case 2:
                viewStudents();
                break;
            case 3:
                searchStudent();
                break;
            case 4:
                createSession();
                break;
            case 5:
                markAttendance();
                break;
            case 6:
                updateAttendance();
                break;
            case 7:
                viewSessionAttendance();
                break;
            case 8:
                attendanceSummary();
                break;
            case 9:
                exportToCSV();
                break;
            case 10:
                running = false;
                cout << "\n  Thank you for using Student Attendance System!\n";
                cout << string(50, '=') << "\n";
                break;
            default:
                cout << "  Invalid choice. Please try again.\n";
        }
    }

    return 0;
}