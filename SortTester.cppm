module;

#include <functional>
#include <iterator>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <ranges>

export module SortTester;

import ArrayGenerator;
import sorting;

static constexpr std::size_t SEED = 69'420'52'228'1337;

static constexpr std::size_t BENCH_MIN = 500;
static constexpr std::size_t BENCH_MAX = 100'000;
static constexpr std::size_t BENCH_STEP = 100;
static constexpr std::size_t BENCH_REPS = 10;

export enum class Generator {
    Random,
    Descending,
    Semisorted,
};

export class SortTester {
   public:
    using V = std::int64_t;
    using D = std::vector<V>;
    using I = D::iterator;
    using BenchResult = std::vector<std::pair<std::size_t, std::int64_t>>;

   private:
    ArrayGenerator<V> gen;
    std::function<void(I, I)> sort = sorting::merge_sort<I, I>;
    Generator gen_type = Generator::Random;

   public:
    constexpr SortTester() : gen(0, 6000, BENCH_MAX, SEED) {
    }

    constexpr auto set_sort(std::function<void(I, I)> sort_f) -> SortTester& {
        sort = sort_f;
        return *this;
    }

    constexpr auto set_generator(Generator gen_type_k) -> SortTester& {
        gen_type = gen_type_k;
        return *this;
    }

    constexpr auto bench() -> BenchResult {
        auto d = BenchResult{};
        d.reserve((BENCH_MAX - BENCH_MIN + 1) / BENCH_STEP);
        for (std::size_t s = BENCH_MIN; s <= BENCH_MAX; s += BENCH_STEP) {
            auto msecs = std::int64_t{0};
            for (auto _ : std::views::iota(0uz, BENCH_REPS)) {
                auto v = generate(s);
                msecs += bench_once(v, sort);
            }
            d.emplace_back(s, msecs / static_cast<std::int64_t>(BENCH_REPS));
        }
        return d;
    }

   private:
    constexpr auto generate(std::size_t s) -> D {
        switch (gen_type) {
            case Generator::Random:
                return gen.get_random(s);
            case Generator::Descending:
                return gen.get_descending(s);
            case Generator::Semisorted:
                return gen.get_semisorted(s);
        }
    }

    constexpr auto bench_once(std::vector<V>& v, std::invocable<I, I> auto sort) const -> std::int64_t {
        auto start = std::chrono::high_resolution_clock::now();
        sort(std::begin(v), std::end(v));
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
    }
};
