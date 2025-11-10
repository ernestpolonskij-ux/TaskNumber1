#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <random>
#include <chrono>
#include <list>
#include <deque>
#include <typeinfo>

using namespace std;
using namespace std::chrono;

struct Person {
    string name, surname;
    vector<int> homework;
    int exam;
    double finalAvg;

    Person(string n, string s, vector<int> hw, int ex)
        : name(n), surname(s), homework(hw), exam(ex)
    {
        double avgHW = accumulate(homework.begin(), homework.end(), 0.0) / homework.size();
        finalAvg = 0.4 * avgHW + 0.6 * exam;
    }
};

template <typename Container>
void splitUsingContainer(const string& filename) {
    auto start = chrono::high_resolution_clock::now();

    ifstream in(filename);
    if (!in) {
        cerr << "Cannot open: " << filename << "\n";
        return;
    }

    string passedFile = "passed_" + filename;
    string failedFile = "failed_" + filename;

    ofstream passed(passedFile), failed(failedFile);

    string header;
    getline(in, header);
    passed << header << "\n";
    failed << header << "\n";

    Container students;

    string name, surname;
    while (in >> name >> surname) {
        vector<int> hw(5);
        for (int i = 0; i < 5; i++) in >> hw[i];
        int ex; in >> ex;

        students.emplace_back(name, surname, hw, ex);
    }

    // Partition: move to two new containers
    Container passedGroup, failedGroup;

    for (auto& s : students) {
        if (s.finalAvg >= 5.0)
            passedGroup.push_back(move(s));
        else
            failedGroup.push_back(move(s));
    }

    // Write results
    auto writeGroup = [&](Container& grp, ofstream& file) {
        for (auto& s : grp) {
            file << s.name << " " << s.surname;
            for (int h : s.homework) file << " " << h;
            file << " " << s.exam << "\n";
        }
        };

    writeGroup(passedGroup, passed);
    writeGroup(failedGroup, failed);

    auto end = chrono::high_resolution_clock::now();
    cout << "Processed " << filename
        << " using container <" << typeid(Container).name() << "> in "
        << chrono::duration_cast<chrono::milliseconds>(end - start).count() / 1000.0
        << " seconds.\n";
}

void generateFile(const string& filename, long long count) {
    auto start = high_resolution_clock::now();

    ofstream file(filename);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 10);

    file << "Name Surname HW1 HW2 HW3 HW4 HW5 Exam\n";

    for (long long i = 1; i <= count; ++i) {
        file << "Name" << i << " Surname" << i;
        for (int j = 0; j < 5; j++) file << " " << dist(gen);
        file << " " << dist(gen) << "\n";
    }

    file.close();

    auto end = high_resolution_clock::now();
    cout << "Generated " << filename << " in "
        << duration_cast<milliseconds>(end - start).count() / 1000.0
        << " seconds.\n";
}

void splitFile(const string& filename) {
    auto start = high_resolution_clock::now();

    ifstream in(filename);
    string passedFile = "passed_" + filename;
    string failedFile = "failed_" + filename;

    ofstream passed(passedFile), failed(failedFile);

    string header;
    getline(in, header);
    passed << header << "\n";
    failed << header << "\n";

    string name, surname;
    while (in >> name >> surname) {
        vector<int> hw(5);
        for (int i = 0; i < 5; i++) in >> hw[i];
        int ex; in >> ex;

        double avgHW = accumulate(hw.begin(), hw.end(), 0.0) / hw.size();
        double finalGrade = 0.4 * avgHW + 0.6 * ex;

        ostream& out = (finalGrade >= 5.0) ? passed : failed;
        out << name << " " << surname;
        for (int h : hw) out << " " << h;
        out << " " << ex << "\n";
    }

    auto end = high_resolution_clock::now();
    cout << "Split " << filename << " in "
        << duration_cast<milliseconds>(end - start).count() / 1000.0
        << " seconds.\n";
}

int main() {
    vector<pair<string, long long>> files = {
        {"Students10k.txt",     10000},
        {"Students100k.txt",   100000},
        {"Students1m.txt",    1000000},
        {"Students10m.txt",  10000000}
    };

    cout << "Generating files...\n";
    for (auto& f : files)
        generateFile(f.first, f.second);

    cout << "\nSplitting files...\n";

    for (auto& f : files) {
        cout << "\n--- " << f.first << " ---\n";

        // vector (original behavior)
        splitFile(f.first);

        // list
        splitUsingContainer<list<Person>>(f.first);

        // deque
        splitUsingContainer<deque<Person>>(f.first);
    }

    cout << "\nAll tasks completed.\n";
    return 0;
}