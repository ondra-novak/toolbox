#pragma once
#ifndef uuid_84348b5a_c15c_4ca0_9bb7_13e56d657771
#define uuid_84348b5a_c15c_4ca0_9bb7_13e56d657771

#include <span>


///Defines fix size vector allocated in runtime
/**
@tparam T item type

Just like vector, but cannot resize, it has fixed size, the code is simplier
 */
template<typename T>
class FixSizeVector: public std::span<T> {
public:

    constexpr FixSizeVector() = default;
    constexpr FixSizeVector(std::size_t cnt):std::span<T>(cnt?new T[cnt]:nullptr, cnt) {}
    constexpr ~FixSizeVector() {
        T *ptr = this->data();
        delete [] ptr;
    }

    constexpr FixSizeVector(const FixSizeVector &other)
        :FixSizeVector(other._size())
    {
        std::copy(other.begin(), other.end(), this->begin());
    }
    constexpr FixSizeVector &operator=(const FixSizeVector &other) {
        if (this != &other) {
            std::destroy_at(this);
            std::construct_at(this, other);
        }
        return *this;
    }
    constexpr FixSizeVector(FixSizeVector &&other):std::span<T>(std::move(other)) {
        other.std::span<T>::operator=(std::span<T>());
    }


    constexpr FixSizeVector &operator=(FixSizeVector &&other) {
        if (this != &other) {
            std::destroy_at(this);
            std::construct_at(this, std::move(other));
        }
        return *this;
    }
    


};

#endif