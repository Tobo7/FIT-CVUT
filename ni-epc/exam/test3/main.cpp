#include <string>
#include <memory>
#include <vector>

#include <optional>
#include <iostream> 
#include <memory> 
  
class StringProxy {
private:
    std::string str;

public:
    // Default constructor
    StringProxy() {
        std::cout << "StringProxy default constructor called." << std::endl;
    }

    // Constructor from const char*
    StringProxy(const char* s) : str(s) {
        std::cout << "StringProxy constructor called with const char*: " << s << std::endl;
    }

    // Constructor from std::string
    StringProxy(const std::string& s) : str(s) {
        std::cout << "StringProxy constructor called with std::string: " << s << std::endl;
    }

    // Copy constructor
    StringProxy(const StringProxy& other) : str(other.str) {
        std::cout << "StringProxy copy constructor called." << std::endl;
    }

    // Move constructor
    StringProxy(StringProxy&& other) noexcept : str(std::move(other.str)) {
        std::cout << "StringProxy move constructor called." << std::endl;
    }

    // Assignment operators
    StringProxy& operator=(const StringProxy& other) {
        std::cout << "StringProxy copy assignment called." << std::endl;
        str = other.str;
        return *this;
    }

    StringProxy& operator=(StringProxy&& other) noexcept {
        std::cout << "StringProxy move assignment called." << std::endl;
        str = std::move(other.str);
        return *this;
    }

    // Proxy methods
    size_t size() const {
        return str.size();
    }

    // ... add other methods as needed to mirror std::string functionality

    // Access to the underlying std::string
    const std::string& get() const { return str; }
    std::string& get() { return str; }

    // Operator overloads as needed, e.g., for stream insertion
    friend std::ostream& operator<<(std::ostream& os, const StringProxy& sp) {
        os << sp.str;
        return os;
    }
};

  
int main() 
{ 


// std::unique_ptr<StringProxy> l;

// std::vector<StringProxy> t;
// t.reserve(1);
// t.emplace_back();

    std::vector<std::unique_ptr<StringProxy>> v;

        v.emplace_back(new StringProxy("string"));
    v.reserve(2);
    v.emplace_back();
    v.emplace_back();
v.emplace_back();

    
    return 0; 
}