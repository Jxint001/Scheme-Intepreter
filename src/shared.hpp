#ifndef SHARED_PTR_HPP
#define SHARED_PTR_HPP

#include <utility>
//#include <iostream>

template<typename T>
class SharedPtr {
private:
    T*_p;
    int* count;
public:
    //construct and deconstruct
    SharedPtr(T* ptr):_p(ptr){
        if (ptr != nullptr)  count = new int(1);
        //if (_p == nullptr) std::cout << "null" << std::endl;
    }
    SharedPtr():_p(nullptr), count(nullptr){}
    ~SharedPtr() { reset(); }

    //copy constructor
    SharedPtr(const SharedPtr& other):_p(other._p), count(other.count){
        if (count != nullptr)  ++(*count);
    }
    //copy assignment operator
    SharedPtr &operator=(const SharedPtr& other){
        if (this != &other) {
            if (count != nullptr && --(*count) == 0) {
                delete _p;
                delete count;
            }
            _p = other._p;
            count = other.count;
            if (count != nullptr)  ++(*count);
        }
        return *this;
    }

    //move constructor
    SharedPtr(SharedPtr&& other):_p(other._p), count(other.count) {
        other._p = nullptr;
        other.count = nullptr;
    }
    //move assignment operator
    SharedPtr &operator=(SharedPtr&& other) {
        if (this != &other) {
            if (count != nullptr && --(*count) == 0) {
                delete _p;
                delete count;
            }
            _p = other._p;
            other._p = nullptr;
            count = other.count;
            other.count = nullptr;
        }
        return *this;
    }

    //count pointers pointing to the same memory
    int use_count() const {
        if (count != nullptr)  return *count;
        return 0;
    }

    //get()
    T* get() const {
        return _p;
    }

    //dereference operator
    T& operator*() const {
        return *_p;
    }

    //arrow operator
    T* operator->() const {
        return _p;
    }

    //to bool
    operator bool() const {
        return _p != nullptr;
    }

    //length
    std::size_t length() const {
        return _p->length();
    }

    //reset
    void reset() {
        if (count != nullptr && --(*count) == 0) {
            delete count;
            delete _p;
        }
        _p = nullptr;
        count = nullptr;
    }

    //reset to a new pointer
    void reset(T* ptr) {
        if (count != nullptr && --(*count) == 0) {
            delete _p;
            delete count;
        }
        _p = ptr;
        if (ptr != nullptr)  count = new int(1);
        else count = nullptr;
    }
};

template <typename T, typename... Args>
SharedPtr<T> make_shared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#endif