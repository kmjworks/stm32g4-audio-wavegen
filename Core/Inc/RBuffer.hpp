#pragma once
#include <algorithm>
#include <cstring>
#include <memory>

template<typename T> class RBuffer {
    public:
        explicit RBuffer(size_t size) : size(size), begin(0), end(0), wrapOnOverflow(false) {
            buffer = std::make_unique<T>(size);
        }

        RBuffer(const RBuffer<T>& rb) {
            this(rb.size);
            begin = rb.begin;
            end = rb.end;

            buffer = std::move(rb.buffer);
        }

        ~RBuffer() = default;
        
        size_t push(const T* data, size_t sz) {
            sz = std::min(sz, getNumberOfElementsInBuffer());

            if (sz == 0) {
                return sz;
            }

            const size_t firstWriteChunk = std::min(sz, size - end);
            std::memcpy(buffer + end, data, firstWriteChunk * sizeof(T));
            end = (end + firstWriteChunk) % size;

            if (firstWriteChunk < sz) {
                const size_t secondWriteCunk = sz - firstWriteChunk;
                std::memcpy(buffer + end, data + firstWriteChunk,secondWriteCunk * sizeof(T));
                end = (end + secondWriteCunk) % size;
            }

            if (begin == end) {
                wrapOnOverflow = true;
            }

            return sz;
        }

        size_t pop(T* dest, size_t sz) {
            sz = std::min(sz, getNumberOfElementsInBuffer());

            if (sz == 0) {
                return sz;
            }

            if (wrapOnOverflow) {
                wrapOnOverflow = false; 
            }

            const size_t firstReadChunk = std::min(sz, size - begin);
            std::memcpy(dest, buffer + begin, firstReadChunk * sizeof(T));
            begin = (begin + firstReadChunk) % size;

            if (firstReadChunk < sz) {
                const size_t secondWriteChunk = sz - firstReadChunk;
                std::memcpy(dest, firstReadChunk, buffer + begin, secondWriteChunk * sizeof(T));
                begin = (begin + secondWriteChunk) % size;
            } 

            return sz;
        }

        size_t getNumberOfElementsInBuffer() {
            if (end == begin) {
                return wrapOnOverflow ? size : 0;
            } else if (end > begin) {
                return (end-begin);
            } else {
                return size + end - begin;
            }
        }

        size_t getSpaceLeft() {
            return size - getNumberOfElementsInBuffer();
        }

    private:
        std::unique_ptr<T> buffer;
        size_t size;
        size_t begin;
        size_t end;

        bool wrapOnOverflow;
};
