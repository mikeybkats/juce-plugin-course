namespace ws::detail {
template <typename T, size_t Size>
class StridedQueue {
public:
  void setStride(size_t newStride) { stride = newStride; }

  [[nodiscard]] size_t size() const noexcept { return stridedElements.size(); }

  T& front() noexcept { return stridedElements.front(); }

  T& at(size_t index) { return stridedElements.at(index); }

  void pushBack(std::span<const T> buffer) {
    const auto toBeAdded = newElementsCount(buffer.size());

    if (stridedElements.size() <= toBeAdded) {
      // stridedElements will be completely overwritten
      const auto bufferEndIndex = elementIndex + (toBeAdded - 1u) * stride;

      for (const auto i : std::views::iota(0u, toBeAdded)) {
        *(stridedElements.rbegin() + i) = buffer[bufferEndIndex - i * stride];
      }

      // current buffer's last element is "-1st" with respect to the next buffer
      elementIndex = (bufferEndIndex + stride - 1u) % stride;
      return;
    }

    std::rotate(stridedElements.begin(), stridedElements.begin() + toBeAdded,
                stridedElements.end());

    const auto beginIndex = stridedElements.size() - toBeAdded;

    for (const auto i : std::views::iota(0u, toBeAdded)) {
      jassert((beginIndex + i) < stridedElements.size());
      jassert(elementIndex < buffer.size());

      stridedElements.at(beginIndex + i) = buffer[elementIndex];
      elementIndex += stride;
    }

    // current buffer's last element is "-1st" with respect to the next buffer
    elementIndex = (elementIndex - 1u) % stride;
  }

  void pushBackZeros(size_t zerosCount) {
    // reset elementIndex; the order changed
    elementIndex = 0u;

    const auto toBeAdded = newElementsCount(zerosCount);
    if (toBeAdded < stridedElements.size()) {
      std::rotate(stridedElements.begin(), stridedElements.begin() + toBeAdded,
                  stridedElements.end());
    }
    const auto beginIndex =
        std::max(0, static_cast<int>(stridedElements.size()) -
                        static_cast<int>(toBeAdded));
    std::fill(stridedElements.begin() + beginIndex, stridedElements.end(), 0.f);
  }

private:
  [[nodiscard]] size_t newElementsCount(size_t sampleCount) const noexcept {
    const auto lowerBound = sampleCount / stride;

    if (lowerBound * stride + elementIndex < sampleCount) {
      return lowerBound + 1u;
    }

    return lowerBound;
  }

  std::array<T, Size> stridedElements{};
  size_t elementIndex{0u};
  size_t stride{1u};
};
}  // namespace ws::detail
