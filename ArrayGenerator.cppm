module;

#include <algorithm>
#include <concepts>
#include <iterator>
#include <random>

export module ArrayGenerator;

export template <std::integral T>
class ArrayGenerator {
   private:
    using C = std::vector<T>;

    C random;
    C descending;
    C semisorted;
    std::mt19937 gen;

   public:
    constexpr ArrayGenerator(T min, T max, std::size_t max_size, std::size_t seed) {
        gen = std::mt19937(seed);
        auto dis = std::uniform_int_distribution<T>(min, max);

        random = C{};
        random.reserve(max_size);
        for (std::size_t i = 0; i < max_size; i++) {
            random.push_back(dis(gen));
        }

        auto sorted = random;
        std::sort(std::begin(sorted), std::end(sorted));

        descending = sorted;
        std::reverse(std::begin(descending), std::end(descending));

        semisorted = sorted;
        auto dis_i = std::uniform_int_distribution<std::size_t>(0uz, max_size - 1);
        auto shuffles = max_size / 100;
        for (std::size_t i = 0; i < shuffles; i++) {
            std::swap(semisorted[dis_i(gen)], semisorted[dis_i(gen)]);
        }
    }

    constexpr auto get_random(std::size_t size) -> C {
        return get_slice(size, random);
    }

    constexpr auto get_descending(std::size_t size) -> C {
        return get_slice(size, descending);
    }

    constexpr auto get_semisorted(std::size_t size) -> C {
        return get_slice(size, semisorted);
    }

   private:
    constexpr auto get_slice(std::size_t size, const C& cont) -> C {
        auto ssize = std::min(static_cast<std::ptrdiff_t>(size), std::ssize(cont));
        auto dis = std::uniform_int_distribution<std::ptrdiff_t>(0z, std::ssize(cont) - ssize);
        auto offset = dis(gen);
        return {std::begin(cont) + offset, std::begin(cont) + offset + ssize};
    }
};
