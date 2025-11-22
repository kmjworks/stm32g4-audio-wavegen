#include <gtest/gtest.h>
#include <fmt/core.h>
#include <structures/RBuffer.hpp>

template <typename T>
testing::AssertionResult HasExpectedAmountOfSpaceLeft(const RBuffer<T>& buf, const size_t expected) {
    const size_t left = buf.getSpaceLeft();
    if (left == expected) {
        return testing::AssertionSuccess();
    } else {
        return testing::AssertionFailure() << fmt::format("Expected {} elements, but got {}\n", expected, left);
    }
} 


TEST(RingBufferTest, BasicSpaceLeftTest) {
    constexpr size_t elements = 20;
    RBuffer<uint8_t> buf(elements);
    EXPECT_EQ(buf.getSpaceLeft(), elements);
}

TEST(RingBufferTest, PushToBufferAndCheckSpaceLeft) {
    constexpr size_t elements = 20;
    RBuffer<uint8_t> buf(elements);
    const std::vector<uint8_t> vec = {200, 123, 176, 22, 35};
    const size_t expectedSizeAvailableAfterPush = elements - vec.size();

    buf.push(vec.data(), vec.size());
    EXPECT_TRUE(HasExpectedAmountOfSpaceLeft(buf, expectedSizeAvailableAfterPush));
}

TEST(RingBufferTest, PeekFromBufferAndCheckValue) {
    RBuffer<uint32_t> buffer(2);
    constexpr uint32_t pushedValue = 34213;
    buffer.push(&pushedValue, 1);

    uint32_t valuePeeked = 0;
    bool peekSuccess = buffer.peek(valuePeeked); 

    EXPECT_TRUE(peekSuccess);
    EXPECT_EQ(valuePeeked, pushedValue);
}