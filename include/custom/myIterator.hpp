#ifndef MYITER
#define MYITER
#include <iterator>
#include <cstddef>
namespace custom{
    template <typename T>
    class myIterator{
    public:
        using value_type = T;
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        myIterator(T* p) : ptr(p) {}
        T& operator*() const { return *ptr; } //Dereference operator
        T* operator->() { return ptr; }
        myIterator& operator++() { ptr++; return *this; } //Increment operators
        myIterator operator++(int) { myIterator tmp = *this; ++(*this); return tmp; }
        myIterator& operator--() { ptr--; return *this; } //Decrement operators 
        myIterator operator--(int) { myIterator tmp = *this; --(*this); return tmp; }
        myIterator& operator+=(const difference_type m) { ptr += m; return *this; }
        myIterator& operator-=(const difference_type m) { ptr -= m; return *this; }        

        constexpr myIterator operator-(const difference_type m) const noexcept { return myIterator(ptr - m); }
        constexpr myIterator operator+(const difference_type m) const noexcept { return myIterator(ptr + m); }
        friend auto operator-(const myIterator& lhs, const myIterator& rhs) { return (lhs.ptr - rhs.ptr); }
        friend auto operator<=>( const myIterator&, const myIterator& ) = default;
    private:
        T* ptr;
    };
}
#endif //MYITER