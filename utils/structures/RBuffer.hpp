#pragma once
#include <algorithm>
#include <memory>

template<typename T> class RBuffer {
    public:
        explicit RBuffer(size_t capacity) : size(capacity + 1), head(0), tail(0) {
            buffer = std::make_unique<T[]>(this->size);
        }

        RBuffer(const RBuffer&) = delete;
        RBuffer& operator=(const RBuffer&) = delete;

        RBuffer(RBuffer&& other) noexcept 
            : buffer(std::move(other.buffer)), size(other.size), head(other.head), tail(other.tail) {
            other.head = 0;
            other.tail = 0;
            other.size = 0;
        }

        ~RBuffer() = default;
        
        size_t push(const T* data, size_t count) {
            if (!data || count == 0) {
                return 0;
            }

            size_t usableCapacity = size - 1;

            if (count > usableCapacity) {
                size_t offset = count - usableCapacity;
                data += offset;
                count = usableCapacity;
                head = 0;
                tail = 0; 
            }

            size_t currentElements = getNumberOfElementsInBuffer();
            size_t freeSpaceLeft = usableCapacity - currentElements;

            size_t bufferTailAdvancement = (count > freeSpaceLeft) ? (count - freeSpaceLeft) : 0;

            const size_t firstChunk = std::min(count, size - head);
            std::copy(data, data + firstChunk, buffer.get() + head);
            head = (head + firstChunk) % size;

            if (firstChunk < count) {
                const size_t secondChunk = count - firstChunk;
                std::copy(data + firstChunk, data + count, buffer.get());
                head = (head + secondChunk) % size;
            }

            if (bufferTailAdvancement > 0) {
                tail = (tail + bufferTailAdvancement) % size;
            }

            return count;
        }

        size_t pop(T* dest, size_t count) {
            if (!dest) return 0;

            const size_t available = getNumberOfElementsInBuffer();
            count = std::min(count, available);

            if (count == 0) return 0;

            const size_t firstChunk = std::min(count, size - tail);
            std::copy(buffer.get() + tail, buffer.get() + tail + firstChunk, dest);
            tail = (tail + firstChunk) % size;

            if (firstChunk < count) {
                const size_t secondChunk = count - firstChunk;
                std::copy(buffer.get(), buffer.get() + secondChunk, dest + firstChunk);
                tail = (tail + secondChunk) % size;
            }

            return count;
        }

        bool peek(T& peekedValue) const {
            if (head == tail) {
                return false;
            }
            peekedValue = buffer[tail];
            return true;
        }

        size_t getNumberOfElementsInBuffer() const {
            size_t currentHead = head;
            size_t currentTail = tail;
            
            if (currentHead >= currentTail) {
                return (currentHead - currentTail);
            } else {
                return (size + currentHead - currentTail);
            }
        }

        size_t getSpaceLeft() const {
            return (size - 1) - getNumberOfElementsInBuffer();
        }

        void reset() {
            head = 0;
            tail = 0;
        }

        T* getRawBuffer() const {
            return buffer.get();
        }


        size_t getInternalSize() const {
            return size;
        }

        void updateHeadUnsafe(size_t newHead) {
            head = newHead % size;
        }

        const T* getReadPtr() const {
            return buffer.get() + tail;
        }

        size_t getContiguousReadLength() const {
            if (head >= tail) {
                return head - tail;
            } else {
                return size - tail;
            }
        }

    private:
        std::unique_ptr<T[]> buffer;
        size_t size;
        volatile size_t head;
        volatile size_t tail;
};