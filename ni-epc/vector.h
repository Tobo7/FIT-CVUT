#ifndef EPC_VECTOR_H
#define EPC_VECTOR_H

#include <cstdlib>
#include <memory>
#include <algorithm>
#include <utility>

namespace epc
{
    template <typename T, size_t N>
    class vector
    {
        size_t capacity_;
        size_t size_;
        T* data_;
        alignas(T) std::byte buffer_ [N * sizeof(T)];

    public:
        vector() noexcept {
            capacity_ = N;
            size_ = 0;
            data_ = ptr_buffer();
        }

        vector(const vector& source) {
            if(source.is_short()) {
                data_ = ptr_buffer();
                for (int i = 0; i < source.size_; ++i) {
                    new (data_ + i) T (source[i]);
                }
            } else {
                data_ = static_cast<T*> (operator new (source.capacity_ * sizeof (T)));
                try {
                    std::uninitialized_copy_n(source.data_, source.size_, data_);
                } catch (...) {
                    operator delete(data_);
                    throw;
                }
            }

            capacity_ = source.capacity_;
            size_ = source.size_;

        }

        vector& operator=(const vector& source) {
            vector<T, N> tmp(source);
            swap(tmp);
            return *this;

        }

        vector(vector&& source) {
            capacity_ = N;
            size_ = 0;
            data_ = ptr_buffer();
            swap(source);
        }

        vector& operator=(vector&& source) {
            clear();
            swap(source);
            return *this;
        }

        ~vector() {
            clear();
            if(!is_short()) {
                operator delete (data_);
            }
        }

        T* data() {
            return data_;
        }

        const T* data() const {
            return data_;
        }

        T& operator[](size_t i) {
            return data_[i];
        }

        const T& operator[](size_t i) const {
            return data_[i];
        }

        void push_back(const T& val) {
            emplace_back(val);
        }

        void push_back(T&& val) {
            emplace_back(std::move(val));
        }

        template <typename... Ts>
        void emplace_back(Ts&&... val) {
            if(size_ == capacity_) {
                reserve(capacity_ * 2);
            }

            new (data_ + size_) T (std::forward<Ts>(val)...);
            size_++;
        }

        void pop_back() {
            data_[size_-1].~T();
            size_--;
        }

        void clear() {
            std::destroy_n(data_, size_);
            size_ = 0;
        }

        void reserve(size_t newCapacity) {
            if(newCapacity <= capacity_) {
                return;
            }

            T * newData = static_cast<T*> (operator new (newCapacity * sizeof(T)));

            try {
                std::uninitialized_move_n(data_, size_, newData);
                std::destroy_n(data_, size_);
                if(!is_short()) {
                    operator delete (data_);
                }
            } catch (...) {
                operator delete (newData);
                throw;
            }

            data_ = newData;
            capacity_ = newCapacity;

        }

        size_t capacity() const {
            return capacity_;
        }

        size_t size() const {
            return size_;
        }

        void swap(vector& other) {

            if(!other.is_short() && !is_short()) {
                std::swap(data_, other.data_);
            } else if(other.is_short() && is_short()) {
                vector & smaller = (size_ < other.size_ ? *this : other );
                vector & larger = (size_ > other.size_ ? *this : other );

                const size_t difference = larger.size_ - smaller.size_;
                std::swap_ranges(data_, data_ + smaller.size_, other.data_);
                std::uninitialized_move_n(larger.data_ + smaller.size_, difference, smaller.data_ + smaller.size_);
                std::destroy_n(larger.data_ + smaller.size_, difference);
            } else {
                vector & sml = (is_short() ? *this : other );
                vector & lrg = (!is_short() ? *this : other );

                std::uninitialized_move_n(sml.ptr_buffer(), sml.size_, lrg.ptr_buffer());
                std::destroy_n(sml.ptr_buffer(), sml.size_);
                std::swap(data_, other.data_);
                lrg.data_ = lrg.ptr_buffer();
            }

            std::swap(size_, other.size_);
            std::swap(capacity_, other.capacity_);
        }

        bool is_short() const {
            return data_ == reinterpret_cast<const T*>(buffer_);
        }

        T * ptr_buffer() {
            return reinterpret_cast<T*>(buffer_);
        }

    };
}

#endif
