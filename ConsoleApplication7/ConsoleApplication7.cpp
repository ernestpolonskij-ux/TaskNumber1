#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <random>

using namespace std;

class Person {
private:
    string name;
    string surname;
    vector<int> homework;
    int exam;
    double finalAvg;
    double finalMed;

public:
    // Default constructor
    Person() : exam(0), finalAvg(0.0), finalMed(0.0) {}

    // Parameterized constructor
    Person(const string& n, const string& s, const vector<int>& hw, int ex)
        : name(n), surname(s), homework(hw), exam(ex), finalAvg(0.0), finalMed(0.0) {
    }

    // Copy constructor (Rule of Three)
    Person(const Person& other) {
        name = other.name;
        surname = other.surname;
        homework = other.homework;
        exam = other.exam;
        finalAvg = other.finalAvg;
        finalMed = other.finalMed;
    }

    // Assignment operator (Rule of Three)
    Person& operator=(const Person& other) {
        if (this != &other) {
            name = other.name;
            surname = other.surname;
            homework = other.homework;
            exam = other.exam;
            finalAvg = other.finalAvg;
            finalMed = other.finalMed;
        }
        return *this;
    }

    // Destructor (Rule of Three)
    ~Person() {}

    // Methods to calculate final grades
    void calculateFinals() {
        if (homework.empty()) {
            finalAvg = finalMed = 0;
            return;
        }

        // Average
        double avgHW = accumulate(homework.begin(), homework.end(), 0.0) / homework.size();
        finalAvg = 0.4 * avgHW + 0.6 * exam;

        // Median
        vector<int> sorted = homework;
        sort(sorted.begin(), sorted.end());
        double med;
        size_t mid = sorted.size() / 2;
        if (sorted.size() % 2 == 0)
            med = (sorted[mid - 1] + sorted[mid]) / 2.0;
        else
            med = sorted[mid];
        finalMed = 0.4 * med + 0.6 * exam;
    }

    // Generate random grades
    static Person generateRandom(const string& n, const string& s, int hwCount = 5) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dist(1, 10);

        vector<int> hw(hwCount);
        for (int& h : hw) h = dist(gen);
        int ex = dist(gen);

        Person p(n, s, hw, ex);
        p.calculateFinals();
        return p;
    }

    // Overloaded output operator <<
    friend ostream& operator<<(ostream& out, const Person& p) {
        out << left << setw(12) << p.name
            << left << setw(15) << p.surname
            << setw(12) << fixed << setprecision(2) << p.finalAvg
            << " | " << setw(10) << fixed << setprecision(2) << p.finalMed;
        return out;
    }

    // File write
    void writeToFile(ofstream& file) const {
        file << name << " " << surname;
        for (int g : homework) file << " " << g;
        file << " " << exam << "\n";
    }

    // File read
    static vector<Person> readFromFile(const string& filename) {
        ifstream file(filename);
        vector<Person> people;
        if (!file) {
            cerr << "Error: Cannot open file.\n";
            return people;
        }

        string name, surname;
        while (file >> name >> surname) {
            vector<int> hw;
            int grade;
            for (int i = 0; i < 5; ++i) { // assuming 5 homework grades
                if (!(file >> grade)) break;
                hw.push_back(grade);
            }
            int exam;
            if (!(file >> exam)) break;

            Person p(name, surname, hw, exam);
            p.calculateFinals();
            people.push_back(p);
        }

        return people;
    }
};

void generateFile(const string& filename, int studentCount = 10) {
    ofstream file(filename);
    if (!file) {
        cerr << "Error creating file.\n";
        return;
    }

    file << "Name Surname HW1 HW2 HW3 HW4 HW5 Exam\n";
    for (int i = 1; i <= studentCount; ++i) {
        string name = "Name" + to_string(i);
        string surname = "Surname" + to_string(i);
        Person p = Person::generateRandom(name, surname);
        p.writeToFile(file);
    }

    cout << "File '" << filename << "' generated successfully.\n";
}

int main() {
    string filename = "Students.txt";

    cout << "1 - Generate random data file\n";
    cout << "2 - Read file and display results\n";
    cout << "Choose: ";
    int choice;
    cin >> choice;

    if (choice == 1) {
        int n;
        cout << "How many students to generate? ";
        cin >> n;
        generateFile(filename, n);
    }
    else if (choice == 2) {
        vector<Person> students = Person::readFromFile(filename);

        cout << left << setw(12) << "Name"
            << setw(15) << "Surname"
            << setw(12) << "Final (Avg.)"
            << " | " << setw(10) << "Final (Med.)" << endl;
        cout << string(60, '-') << endl;

        for (const auto& s : students)
            cout << s << endl;
    }
    else {
        cout << "Invalid choice.\n";
    }

    return 0;
}