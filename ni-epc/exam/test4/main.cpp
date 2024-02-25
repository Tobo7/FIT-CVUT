#include <string>
#include <memory>
#include <vector>
#include <optional>
#include <iostream> 
#include <memory> 
#include <cmath>

//STATIC_ASSERT
template <typename T>
class A {
    static_assert(!std::floating_point<T>, "ERR");
T a;
public:
A(T a):a(a) {}

};


//CONCEPTS

template <typename T>
concept not_floating_point = !std::floating_point<T>;

template <not_floating_point T>
bool equal_c(const T a, const T b) { return a == b; }


template <not_floating_point T>
class B {
T a;
public:
B(T a):a(a) {}

};


template <typename T> struct is_floating_point { static const bool value = false; };
template<> struct is_floating_point< float > { static const bool value = true; };
template<> struct is_floating_point< double > { static const bool value = true; };
template<> struct is_floating_point< long double > { static const bool value = true; };


template <bool B> struct enable_if { };
template <> struct enable_if<true> { using type = void; };

//SFINAE

template <typename T>
bool equal(const T a, const T b, typename enable_if<!is_floating_point<T>::value>::type* = nullptr) { return a == b; }

template <typename T>
bool equal(const T a, const T b, typename enable_if<is_floating_point<T>::value>::type* = nullptr) { return abs(a - b) < 0.001; } 



int main() 
{ 
    equal(1,2);
    equal(1.0,2.0);
    B(1);
    

    return 0; 
}