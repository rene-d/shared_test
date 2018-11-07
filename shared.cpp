#include <bits/stdc++.h>
#include <boost/chrono.hpp>
#include <cxxabi.h>

using namespace std;


template<typename T>
    using shared_ptr_single = std::__shared_ptr<T, __gnu_cxx::_S_single>;

template<typename T>
    using shared_ptr_mutex = std::__shared_ptr<T, __gnu_cxx::_S_mutex>;

template<typename T>
    using shared_ptr_atomic = std::__shared_ptr<T, __gnu_cxx::_S_atomic>;


class elapsed
{
    typedef boost::chrono::high_resolution_clock hr_clock;
    hr_clock::time_point start_;

public:
    void start()
    {
        start_ = hr_clock::now();
    }

    double operator()() const
    {
        hr_clock::duration e = hr_clock::now() - start_;
        return e.count() * ((double)hr_clock::period::num / hr_clock::period::den);
    }
};


void measure(const string& title, function<void (void)> fn)
{
    elapsed e;

    cout << setw(30) << title << " : ";
    cout.flush();

    e.start();
    fn();
    double d = e();

    cout << fixed << setprecision(6) << d << " s" << endl;
}


std::string demangle(const char* name)
{
    int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
}


template<typename T>
void __attribute__ ((noinline)) test_byref(unsigned long& n, const T& t)
{
    n += *t;
}


template<typename T>
void __attribute__ ((noinline)) test_byval(unsigned long& n, T t)
{
    n += *t;
}


template<typename T>
void test(unsigned long count)
{
    string name = demangle(typeid(T).name());

    cout << endl
         << name << endl;

    measure("byref", [count]() {
        unsigned long n = 0;
        for (auto i = 0; i < count; ++i) {
            T ptr{ new int(1) };
            test_byref(n, ptr);
        }
        assert(count == n);
    });

    measure("byval", [count]() {
        unsigned long n = 0;
        for (auto i = 0; i < count; ++i) {
            T ptr{ new int(1) };
            test_byval(n, ptr);
        }
        assert(count == n);
    });
}


int main(int argc, char *argv[])
{
    cout << "shared test" << endl;

    cout << "flags:";
#ifdef __i386__
    cout << " i386";
#endif
#ifdef __amd64__
    cout << " amd64";
#endif
#ifdef __OPTIMIZE__
    cout << " optimize";
#endif
#ifdef __NO_INLINE__
    cout << " noinline";
#endif
    cout << endl;

    unsigned long count = 40;

    if (argc > 1 && strcmp(argv[1], "bench") == 0) {
        cout << "benchmarking... ";

        count = 0;
        elapsed e;
        do {
            count += 1;
            for (auto i = 0; i < 10000ul; ++i) {
                int *ptr = new int { 1 };
                delete ptr;
            }
        } while (e() < 0.1);

        cout << count << " iterations" << endl;
    }
    else if (argc > 1 && stoi(argv[1]) > 0) {
        count = stoi(argv[1]);
        cout << "using " << count << " iterations" << endl;
    }
    cout << endl;

    count *= 10 * 10000;

    measure("new/delete", [count]() {
        unsigned long n = 0;
        for (auto i = 0; i < count; ++i) {
            int *ptr = new int { 1 };
            test_byval(n, ptr);
            delete ptr;
        }
        assert(count == n);
    });

    measure("unique_ptr", [count]() {
        unsigned long n = 0;
        for (auto i = 0; i < count; ++i) {
            unique_ptr<int> ptr{ new int(1) };
            test_byref(n, ptr);
        }
        assert(count == n);
    });

    measure("shared_ptr (make_shared)", [count]() {
        unsigned long n = 0;
        for (auto i = 0; i < count; ++i) {
            shared_ptr<int> ptr = make_shared<int>(1);
            test_byref(n, ptr);
        }
        assert(count == n);
    });

    measure("shared_ptr (new)", [count]() {
        unsigned long n = 0;
        for (auto i = 0; i < count; ++i) {
            shared_ptr<int> ptr { new int(1) };
            test_byref(n, ptr);
        }
        assert(count == n);
    });

    measure("shared_ptr (make_shared) byval", [count]() {
        unsigned long n = 0;
        for (auto i = 0; i < count; ++i) {
            shared_ptr<int> ptr = make_shared<int>(1);
            test_byval(n, ptr);
        }
        assert(count == n);
    });

    measure("shared_ptr (new) byval", [count]() {
        unsigned long n = 0;
        for (auto i = 0; i < count; ++i) {
            shared_ptr<int> ptr { new int(1) };
            test_byval(n, ptr);
        }
        assert(count == n);
    });

    test<shared_ptr<int>>(count);
    test<shared_ptr_single<int>>(count);
    test<shared_ptr_mutex<int>>(count);
    test<shared_ptr_atomic<int>>(count);

    return 0;
}