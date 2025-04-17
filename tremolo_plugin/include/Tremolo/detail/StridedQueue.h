namespace ws::detail {
template <size_t Size>
class StridedQueue {
public:
  StridedQueue() { stridedElements.resize(Size, 0.f); }

  void setStride(int newStride) {
    jassert(newStride > 0);

    stride = newStride;
  }

  [[nodiscard]] size_t size() const noexcept { return stridedElements.size(); }

  float& front() noexcept { return stridedElements.front(); }

  float& at(size_t index) { return stridedElements.at(index); }

  void pushBack(const juce::AudioBuffer<float>& buffer) {
    const auto toBeAdded = newElementsCount(buffer.getNumSamples());

    if (static_cast<int>(stridedElements.size()) <= toBeAdded) {
      // stridedElements will be completely overwritten
      for (const auto i : std::views::iota(0, toBeAdded)) {
        *(stridedElements.end() - i - 1) =
            buffer.getSample(0, buffer.getNumSamples() - i * stride - 1);
      }
      elementIndex = stride - 1;
      return;
    }

    std::rotate(stridedElements.begin(), stridedElements.begin() + toBeAdded,
                stridedElements.end());

    const auto beginIndex =
        std::max(0, static_cast<int>(stridedElements.size()) - toBeAdded);

    for (const auto i : std::views::iota(0, toBeAdded)) {
      jassert(beginIndex + i < static_cast<int>(stridedElements.size()));
      stridedElements.at(static_cast<size_t>(beginIndex + i)) =
          buffer.getSample(0, elementIndex);
      elementIndex += stride;
      jassert(elementIndex < buffer.getNumSamples());
    }

    elementIndex %= stride;
  }

  void pushBackZeros(int zerosCount) {
    const auto toBeAdded = newElementsCount(zerosCount);
    if (toBeAdded < static_cast<int>(stridedElements.size())) {
      std::rotate(stridedElements.begin(), stridedElements.begin() + toBeAdded,
                  stridedElements.end());
    }
    const auto beginIndex =
        std::max(0, static_cast<int>(stridedElements.size()) - toBeAdded);
    std::fill(stridedElements.begin() + beginIndex, stridedElements.end(), 0.f);
  }

private:
  [[nodiscard]] int newElementsCount(int sampleCount) const noexcept {
    const auto lowerBound = sampleCount / stride;

    if (lowerBound * stride + elementIndex < sampleCount) {
      return lowerBound + 1;
    }

    return lowerBound;
  }

  std::deque<float> stridedElements;
  int elementIndex{0};
  int stride{1};
};
}  // namespace ws::detail
