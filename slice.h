//
// Created by ByteDance on 2024/2/5.
//
#include <cassert>
#include <cstddef>
#include <cstring>
#include <string>


#ifndef DATAFRAME_SLICE_H
#define DATAFRAME_SLICE_H

class  Slice {
        public:
        // Create an empty slice.
        Slice() : data_(""), size_(0) {}

        // Create a slice that refers to d[0,n-1].
        Slice(const char* d, size_t n) : data_(d), size_(n) {}

        // Create a slice that refers to the contents of "s"
        Slice(const std::string& s) : data_(s.data()), size_(s.size()) {}

        // Create a slice that refers to s[0,strlen(s)-1]
        Slice(const char* s) : data_(s), size_(strlen(s)) {}

        // Intentionally copyable.
        Slice(const Slice&) = default;
        Slice& operator=(const Slice&) = default;

        // Return a pointer to the beginning of the referenced data
        const char* data() const { return data_; }

        // Return the length (in bytes) of the referenced data
        size_t size() const { return size_; }

        // Return true iff the length of the referenced data is zero
        bool empty() const { return size_ == 0; }

        // Return the ith byte in the referenced data.
        // REQUIRES: n < size()
        char operator[](size_t n) const {
            assert(n < size());
            return data_[n];
        }

        // Change this slice to refer to an empty array
        void clear() {
            data_ = "";
            size_ = 0;
        }

        // Return a string that contains the copy of the referenced data.
        std::string ToString() const { return std::string(data_, size_); }


        private:
        const char* data_;
        size_t size_;
};

#endif //DATAFRAME_SLICE_H
