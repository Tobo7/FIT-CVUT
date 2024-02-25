#include <iostream>


struct X{
    void f() { std::cout << 'A' << std::endl;}
    void g();
};

void X::g() {  std::cout << 'B' << std::endl; }