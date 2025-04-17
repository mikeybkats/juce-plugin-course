namespace ws::detail {
template <size_t Size>
class StridedQueue {
public:
  StridedQueue() {
    lfoSamplesToPlot.resize(Size, 0.f);
  }

  void setStride(int newStride) {
    jassert( newStride > 0);

    stride = newStride;
  }

  [[nodiscard]] size_t size() const noexcept {
    return lfoSamplesToPlot.size();
  }

  float& front() noexcept {
    return lfoSamplesToPlot.front();
  }

  float& at(size_t index) {
    return lfoSamplesToPlot.at(index);
  }

  void pushBack(const juce::AudioBuffer<float>& buffer) {
    for (; sampleIndex < buffer.getNumSamples(); sampleIndex += stride) {
      const auto sample = buffer.getSample(0, sampleIndex);
      lfoSamplesToPlot.pop_front();
      lfoSamplesToPlot.push_back(sample);
    }
    sampleIndex %= stride;
  }

  void pushBackZeros(int zerosCount) {
    for (; sampleIndex < zerosCount; sampleIndex += stride) {
      lfoSamplesToPlot.pop_front();
      lfoSamplesToPlot.push_back(0.f);
    }
    sampleIndex %= stride;
  }
  
private:
  std::deque<float> lfoSamplesToPlot;
  int sampleIndex{0};
  int stride{1};
};
}
