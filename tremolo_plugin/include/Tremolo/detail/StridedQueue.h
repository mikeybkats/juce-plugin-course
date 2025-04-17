namespace ws::detail {
template <typename T, size_t Size>
class StridedQueue {
public:
  void setStride(size_t newStride) { stride = newStride; }

  [[nodiscard]] size_t size() const noexcept { return stridedElements.size(); }

  T& front() noexcept { return stridedElements.front(); }

  T& at(size_t index) { return stridedElements.at(index); }

  void pushBack(juce::Span<const T> buffer) {
    const auto toBeAdded = newElementsCount(buffer.size());

    if (stridedElements.size() <= toBeAdded) {
      // stridedElements will be completely overwritten
      for (const auto i : std::views::iota(0u, toBeAdded)) {
        *(stridedElements.end() - i - 1u) =
            buffer[buffer.size() - i * stride - 1u];
      }
      elementIndex = stride - 1u;
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

    elementIndex %= stride;
  }

  void pushBackZeros(size_t zerosCount) {
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
