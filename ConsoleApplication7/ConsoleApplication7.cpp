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
#include <array>
#include <type_traits>
#include <utility>    // for std::declval

using namespace std;
using namespace std::chrono;

struct Person {
    string name, surname;
    array<int, 5> homework{};
    int exam{};
    double finalAvg{};

    Person() = default;

    Person(string n, string s, array<int, 5> hw, int ex)
        : name(move(n)), surname(move(s)), homework(hw), exam(ex)
    {
        double avgHW = accumulate(hw.begin(), hw.end(), 0.0) / hw.size();
        finalAvg = 0.4 * avgHW + 0.6 * exam;
    }
};

//
// Safe reserve helper for C++14: detect reserve() with SFINAE + tag-dispatch (MSVC/C++14 friendly)
//
template<typename T>
class has_reserve {
private:
    template<typename U>
    static auto test(int) -> decltype(std::declval<U&>().reserve(size_t{}), std::true_type{});
    template<typename>
    static std::false_type test(...);
public:
    static constexpr bool value = decltype(test<T>(0))::value;
};

template<typename C>
void reserve_if_possible_impl(C& c, size_t n, std::true_type) { c.reserve(n); }

template<typename C>
void reserve_if_possible_impl(C&, size_t, std::false_type) { /* no-op */ }

template<typename C>
void reserve_if_possible(C& c, size_t n)
{
    using tag = std::integral_constant<bool, has_reserve<C>::value>;
    reserve_if_possible_impl(c, n, tag{});
}

//
// ----------- FILE GENERATION -----------
//
void generateFile(const string& filename, long long count)
{
    auto start = high_resolution_clock::now();

    ofstream file(filename);
    ios::sync_with_stdio(false);

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 10);

    file << "Name Surname HW1 HW2 HW3 HW4 HW5 Exam\n";

    for (long long i = 1; i <= count; ++i)
    {
        file << "Name" << i << " Surname" << i;
        for (int j = 0; j < 5; j++)
            file << " " << dist(gen);
        file << " " << dist(gen) << "\n";
    }

    auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - start).count();
    cout << "Generated " << filename << " in " << elapsed / 1000.0 << " seconds.\n";
}

//
// ----------- LOADING STUDENTS INTO A CONTAINER -----------
//
template<typename Container>
Container loadStudents(const string& filename)
{
    ifstream in(filename);
    string header;
    getline(in, header);

    Container students;

    // try to reserve a sensible starting guess if supported by Container
    reserve_if_possible(students, 100000);

    string name, surname;
    while (in >> name >> surname)
    {
        array<int, 5> hw{};
        for (int i = 0; i < 5; i++)
            in >> hw[i];
        int ex;
        in >> ex;

        students.emplace_back(name, surname, hw, ex);
    }

    return students;
}

//
// ----------- STRATEGY 1: COPY INTO TWO CONTAINERS -----------
//
template<typename Container>
void strategy1_split(const string& filename)
{
    cout << "Loading " << filename << " into " << typeid(Container).name() << "...\n";
    auto loadStart = high_resolution_clock::now();

    Container students = loadStudents<Container>(filename);

    auto loadTime = duration_cast<milliseconds>(high_resolution_clock::now() - loadStart).count();

    //
    // Strategy 1 Split: copy to two new containers
    //
    auto splitStart = high_resolution_clock::now();

    Container passed, failed;

    reserve_if_possible(passed, students.size());
    reserve_if_possible(failed, students.size());

    for (const auto& s : students)
    {
        if (s.finalAvg >= 5.0)
            passed.push_back(s);   // COPY
        else
            failed.push_back(s);   // COPY
    }

    auto splitTime = duration_cast<milliseconds>(high_resolution_clock::now() - splitStart).count();

    cout << "  Load time:  " << loadTime / 1000.0 << " s\n";
    cout << "  Split time: " << splitTime / 1000.0 << " s\n";
}

//
// ----------- MAIN BENCHMARK DRIVER -----------
//
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    vector<pair<string, long long>> files = {
        {"Students10k.txt",   10000},
        {"Students100k.txt", 100000},
        {"Students1m.txt",  1000000},
        {"Students10m.txt",10000000}
    };

    cout << "\n=== Generating files ===\n";
    for (auto& f : files)
        generateFile(f.first, f.second);

    cout << "\n=== Strategy 1: Copy-based Split ===\n";

    for (auto& f : files)
    {
        cout << "\n--- " << f.first << " ---\n";
        strategy1_split<vector<Person>>(f.first);
        strategy1_split<list<Person>>(f.first);
        strategy1_split<deque<Person>>(f.first);
    }

    cout << "\nDone.\n";
    return 0;
}