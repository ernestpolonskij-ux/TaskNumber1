#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <random>
#include <chrono>

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
    for (auto& f : files)
        splitFile(f.first);

    cout << "\nAll tasks completed.\n";
    return 0;
}