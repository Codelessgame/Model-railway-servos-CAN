
template <typename T, size_t N>
constexpr size_t arrayLength(const T (&)[N]) noexcept {
    return N;
}