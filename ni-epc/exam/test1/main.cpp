#include <iostream>
#include <type_traits>
#include <memory>

template <typename ...Ts> size_t number_of_ints(Ts...) {
    return (std::is_same_v<Ts, int> + ... + 0);
}

template <unsigned int T, unsigned int P>
struct pow { static const int value = T * pow<T, P-1>::value;};

template <unsigned int T>
struct pow<T, 0> { static const int value = 1;};

int main() {
  // std::cout << number_of_ints(1, "text", 'c', 4, 2L, 2.0, 50) << std::endl;
   // std::cout << number_of_ints() << std::endl;

   std::cout << pow<2,4>::value << std::endl;


   std::unique_ptr<std::string> p = std::make_unique<std::string>("ahoj");
}