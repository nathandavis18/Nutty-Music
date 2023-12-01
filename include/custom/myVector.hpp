#ifndef MYVEC
#define MYVEC
#include "myIterator.hpp"
#include "myReverseIterator.hpp"
#include <vector> //Only used for comparing my vector to an std::vector
#include <algorithm>
namespace custom {

    template <typename T>
    class myVector {
    public:
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        std::allocator<value_type> allocator;
        static_assert(std::is_same_v<value_type, decltype(allocator)::value_type>);
        using traits = std::allocator_traits<decltype(allocator)>;

        myVector() : m_capacity(0), buffer(nullptr), m_finish(nullptr) {
            //traits::rebind_alloc<value_type> alloc_ = allocator;
        } //Default Constructor

        myVector(const myVector& vec) : m_capacity(vec.capacity()), buffer(traits::allocate(allocator, vec.size())) {
            //traits::rebind_alloc<value_type> alloc_ = allocator;
            m_finish = buffer;
            for (int i = 0; i < vec.size(); ++i) {
                traits::construct(allocator, m_finish++, *(vec.buffer + i));
            }
        } //Copy Constructor

        myVector(myVector&& moveVec) noexcept : m_capacity(std::exchange(moveVec.m_capacity, 0)),
                                                buffer(std::exchange(moveVec.buffer, nullptr)), 
                                                m_finish(std::exchange(moveVec.m_finish, nullptr)) {
            //traits::rebind_alloc<value_type> alloc_ = allocator;
        } //Move constructor

        myVector(const size_t capacity) : m_capacity(capacity), buffer(traits::allocate(allocator, capacity)) {
            //traits::rebind_alloc<value_type> alloc_ = allocator;
            for (myIterator<value_type> it = begin(); it != end(); ++it) {
                std::uninitialized_fill(begin(), end(), typename std::iterator_traits<myIterator<value_type>>::value_type());
            }
            m_finish = buffer + capacity;
        } //Capacity constructor 

        //Initializer List Constructor
        myVector(std::initializer_list<value_type> il) : m_capacity(il.end() - il.begin()), buffer(traits::allocate(allocator, il.size())) {
            //traits::rebind_alloc<value_type> alloc_ = allocator;
            auto it = il.begin();
            while(it != il.end()) {
                traits::construct(allocator, m_finish++, *it++);
            }
        }

        ~myVector() { //Destructor
            destroyObjects(buffer, m_finish);
            traits::deallocate(allocator, buffer, m_capacity);
        }

        myVector& operator=(const myVector& cpy) noexcept { //Copy assignment
            m_capacity = cpy.capacity();
            buffer = traits::allocate(allocator, m_capacity);
            m_finish = buffer;
            for (int i = 0; i < cpy.size(); ++i) {
                traits::construct(allocator, m_finish++, *(cpy.buffer + i));
            }
            return *this;
        }

        myVector& operator=(myVector&& moveVec) noexcept { //Move assignment
            if (this == &moveVec) return *this; //Moving something into itself doesn't make sense

            m_capacity = std::exchange(moveVec.m_capacity, 0);
            buffer = std::exchange(moveVec.buffer, nullptr);
            m_finish = std::exchange(moveVec.m_finish, nullptr);
            return *this;
        }

        myVector& operator=(std::initializer_list<value_type> il) noexcept { //Initializer List assignment
            m_capacity = il.end() - il.begin();
            buffer = traits::allocate(allocator, m_capacity);
            m_finish = buffer;
            auto it = il.begin();
            while(it != il.end()) {
                traits::construct(allocator, m_finish++, *it++);
            }
            return *this;
        }

        [[nodiscard]] bool operator==(const myVector& b) noexcept { //Tests if two custom vectors are equal
            if (size() != b.size()) return false;
            for (int i = 0; i < size(); ++i) {
                if ((*this)[i] != b[i]) return false;
            }
            return true;
        }
        [[nodiscard]] bool operator==(const std::vector<T>& b) noexcept { //Tests if a std::vector and myVector are equal
            if (size() != b.size()) return false;
            for (int i = 0; i < size(); ++i) {
                if ((*this)[i] != b[i]) return false;
            }
            return true;
        }

        [[nodiscard]] reference operator[](size_t index) noexcept { //Returns a read/write value at given index, no exception handling
            return *(buffer + index);
        }

        [[nodiscard]] const_reference operator[](size_t index) const noexcept { //Returns a read-only value at the given index, no exception handling
            return *(buffer + index);
        }

        [[nodiscard]] reference at(int index) { //Returns a read/write value at the given index
            try {
                return *(buffer + index);
            }
            catch (std::out_of_range exception) {
                throw exception = "Invalid Index";
            }
        }

        [[nodiscard]] const_reference at(int index) const { //Returns a read-only value at the given index
            try {
                return *(buffer + index);
            }
            catch (std::out_of_range exception) {
                throw exception = "Invalid Index";
            }
        }

        void push_back(const_reference data) { //Adds a new value to the back of the vector. 
            if (size() == m_capacity) { //If the new size > capacity, allocate more space
                realloc();
            }
            traits::construct(allocator, m_finish++, data);
        }

        void push_back(T&& data) { //Adds a moved value to the back of the vector
            emplace_back(std::move(data));
        }

        void push_front(const_reference data) { //Adds the new value to the front of the vector, after moving everything forward one position
            if (size() == m_capacity) { //Allocates more space if needed
                realloc();
            }
            move_forward(begin(), end());
            traits::construct(allocator, buffer, data);
            ++m_finish;
        }
        void push_front(T&& data) { //Adds the new value to the front of the vector, after moving everything forward one position
            emplace(begin(), std::move(data));
        }

        void pop_back() noexcept { //Removes the last element and decreases size() of the vector
            traits::destroy(allocator, --m_finish); //Calls the destructor on the object if required
        }
        void pop_front() noexcept { //Removes the first element, moves everything forward, and decreases size()
            traits::destroy(allocator, buffer);
            --m_finish;
            move_backward(begin(), end());
        }

        void reserve(size_t capacity) {
            if (capacity <= m_capacity) return; //Reserve doesn't shrink capacity, only makes it grow
            realloc(capacity);
        }

        void resize(size_t newSize) {
            if (newSize < size()) {
                while (m_finish != buffer + newSize) {
                    traits::destroy(allocator, --m_finish);
                }
            }
            else if (newSize > m_capacity) {
                realloc(newSize);
            }
            std::uninitialized_fill(begin() + size(), begin() + newSize, typename std::iterator_traits<myIterator<value_type>>::value_type());
            m_finish = buffer + newSize;
        }

        void reverse() noexcept {
            if (begin() == end() || begin() + 1 == end()) return; //Vectors of size 0 and 1 are already reversed

            auto fIt = begin(), bIt = end() - 1;
            while (fIt < bIt) {
                std::iter_swap(fIt, bIt);
                ++fIt; --bIt;
            }
        }

        void shrink_to_fit() { //Deallocates space such that the vector will have size == capacity
            if (m_capacity == size()) return; //Already properly shrunk

            size_t tmpCapacity = size();
            pointer newBuffer = traits::allocate(allocator, tmpCapacity);
            pointer newFinish = newBuffer;
            for (int i = 0; i < size(); ++i) {
                traits::construct(allocator, newFinish++, std::move_if_noexcept(*(buffer + i)));
            }
            std::swap(m_capacity, tmpCapacity);
            std::swap(buffer, newBuffer);
            std::swap(m_finish, newFinish);
            destroyObjects(newBuffer, newFinish);
            traits::deallocate(newBuffer, tmpCapacity);
        }

        void swap(myVector& v) noexcept { //Swaps two vectors
            std::swap(m_capacity, v.m_capacity);
            std::swap(m_finish, v.m_finish);
            std::swap(buffer, v.buffer);
        }

        void clear() noexcept { //Removes all the data from a vector. Doesn't affect capacity
            while (m_finish > buffer) {
                traits::destroy(allocator, --m_finish);
            }
        }

        template<class Iter>
        void insert(Iter it, const_reference value) {
            if (size() == m_capacity) {
                size_t offset = it - begin(); //Get the iterator's offset so we don't lose it after reallocation
                realloc();
                it = begin() + offset; //Set the iterator back to the correct offset, since data may have been moved to a new location after reallocation.
            }
            move_forward(it, end());
            traits::construct(allocator, buffer + (it - begin()), value);
            ++m_finish;
        }

        template<class Iter, class... Args>
        void emplace(Iter it, Args&&... args) {
            if (size() == m_capacity) {
                size_t offset = it - begin();
                realloc();
                it = begin() + offset;
            }
            move_forward(it, end());
            traits::construct(allocator, buffer + (it - begin()), std::forward<Args>(args)...);
            ++m_finish;
        }
        template<class... Args>
        void emplace_back(Args&&... args) {
            if (size() == m_capacity) {
                realloc();
            }
            traits::construct(allocator, m_finish++, std::forward<Args>(args)...);
        }

        bool find(const_reference value_to_find) {
            for (auto it = begin(); it != end(); ++it) {
                if (*it == value_to_find) return true;
            }
            return false;
        }

        reference front() noexcept { return *begin(); } //Returns a read/write value for the object at the front of the vector
        const_reference front() const noexcept { return *cbegin(); } //Returns a read-only value for the object at the front of the vector
        reference back() noexcept { return *(begin() + (size() - 1)); } //Returns a read/write value for the object at the back of the vector
        const_reference back() const noexcept { return *(begin() + (size() - 1)); } //Returns a read-only value for the object at the back of the vector

        myIterator<value_type> begin() noexcept { //Returns a random-access iterator pointing to the front of the vector
            return myIterator<value_type>(buffer);
        }

        const myIterator<value_type> cbegin() const noexcept { //Returns a conset random-access iterator to the first element in the vector
            return myIterator<value_type>(buffer);
        }

        myIterator<value_type> end() noexcept { //Returns a random-access iterator pointing just beyond the vector
            return myIterator<value_type>(m_finish);
        }

        const myIterator<value_type> cend() const noexcept { //Returns a const random-access iterator just beyond the vector
            return myIterator<value_type>(m_finish);
        }

        myReverseIterator<value_type> rbegin() noexcept { //Returns a random-access reverse iterator pointing to the last element in the vector
            return myReverseIterator<value_type>(m_finish - 1);
        }

        const myReverseIterator<value_type> crbegin() const noexcept { //Returns a const random-access reverse iterator pointing to the last element in the vector
            return myReverseIterator<value_type>(m_finish - 1);
        }

        myReverseIterator<value_type> rend() noexcept { //Returns a random-access reverse iterator pointing in front of the vector
            return myReverseIterator<value_type>(buffer - 1);
        }

        const myReverseIterator<value_type> crend() const noexcept { //Returns a const random-access reverse iterator pointing in front of the vector
            return myReverseIterator<value_type>(buffer - 1);
        }

        [[nodiscard]] bool isEmpty() const noexcept { return begin() == end(); }

        const size_t size() const noexcept { return size_t(m_finish - buffer); }

        const size_t capacity() const noexcept { return m_capacity; }

    private:
        pointer buffer;
        size_t m_capacity;

        pointer m_finish;

        const size_t newCapacity() const noexcept { return m_capacity * 3 / 2 + 1; }

        void destroyObjects(pointer& buff, pointer& finish) noexcept {
            for (int i = 0; i < finish - buff; ++i) {
                traits::destroy(allocator, buff + i);
            }
        }
        void realloc(size_t capacity = 0) {
            pointer newFinish, newBuffer;
            size_t tmpCapacity;
            try {
                tmpCapacity = capacity == 0 ? newCapacity() : capacity;
                newBuffer = traits::allocate(allocator, tmpCapacity);
                newFinish = newBuffer;
                for (size_t i = 0; i < size(); ++i) {
                    traits::construct(allocator, newBuffer + i, std::move_if_noexcept(*(buffer + i)));
                    ++newFinish;
                }
                std::swap(m_capacity, tmpCapacity);
                std::swap(buffer, newBuffer);
                std::swap(m_finish, newFinish);

                destroyObjects(newBuffer, newFinish);
                traits::deallocate(allocator, newBuffer, tmpCapacity);
            }
            catch (...) {
                destroyObjects(newBuffer, newFinish);
                traits::deallocate(allocator, newBuffer, tmpCapacity);
                throw;
            }
        }
        template<class Iter>
        void move_forward(Iter start, Iter end) noexcept {
            if (start == end) return;

            while (end != start) {
                *end = *(end - 1);
                --end;
            }
        }
        template<class Iter>
        void move_backward(Iter start, Iter end) noexcept {
            if (start == end) return;

            while (start != end) {
                *start = *(start + 1);
                ++start;
            }
        }
    };
}
#endif //MYVEC