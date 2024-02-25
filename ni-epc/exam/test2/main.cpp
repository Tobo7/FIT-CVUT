
#include <iostream>
#include <type_traits>
#include <memory>
#include <list>
#include <set>


template <typename ...Ts> size_t number_of_ints(Ts...) {
    return (std::is_same_v<Ts, int> + ... + 0);
}



template <unsigned int T, unsigned int P>
struct pow { static const int value = T * pow<T, P-1>::value;};

template <unsigned int T>
struct pow<T, 0> { static const int value = 1;};



template <typename T> struct is_bool { static const bool value = false;};

template <> struct is_bool<bool> { static const bool value = true;};




template <typename T> bool larger(const T& a, const T& b) {
     std::cout << is_bool<T>::value << std::endl;
     
     return true;
      }
template <> bool larger(const double& a, const double& b) {
    std::cout << "B" << std::endl;
    return false ;
    }



template <unsigned long I> // primary template
struct factorial { static const unsigned long value = I * factorial<I-1>::value; };
template <> struct factorial<0> { static const unsigned long value = 1; };


template <unsigned long T>
inline unsigned long factorial_v = factorial<T>::value;


template <typename T> struct is_list { static const bool value = false;};
template <typename T> struct is_list<std::list<T>> { static const bool value = true;};


int main() {

std::cout << is_list<std::list<int>>::value; // vypíše "1"
std::cout << is_list<std::set<char>>::value;  // vypíše "0"
std::cout << is_list<std::string>::value; // vypíše "0"
std::cout << is_list<double>::value; // vypíše "0"
}