#include <cassert>
#include <cmath>
#include <cstdint>

#include <iostream>
#include <ranges>
#include <vector>

constexpr int32_t cnt_2(const int32_t n) {
  int r{};
  while ((1 << r) < n + r + 1) {
    ++r;
  }
  return r;
}

constexpr bool is_2(int32_t x) { return (x > 0) && (x & (x - 1)) == 0; }

std::vector<int32_t> code_message(const std::vector<int32_t>& data) {
  size_t n = data.size();
  int32_t rest = cnt_2(n);
  int32_t csn = n + rest;

  std::vector<int32_t> cs(csn + 1, 0);
  int32_t cbn = 0;

  for (int32_t i = 1, j = 0; i <= csn; ++i) {
    if (!is_2(i)) {
      cs[i] = data[j];
      ++j;
    }
  }

  for (int32_t r = 0; r < rest; ++r) {
    int32_t par = 0;
    int32_t p = 1 << r;

    for (int32_t i = 1; i <= csn; ++i) {
      if (i & p)
        par ^= cs[i];
    }
    cs[p] = par;
  }

  return {cs.begin() + 1, cs.end()};
}

int32_t decode_message(const std::vector<int32_t>& coded_data) {
  size_t n = coded_data.size();

  int32_t cnt = 0;
  while ((1 << cnt) <= n) {
    ++cnt;
  }

  int32_t error_pos = 0;

  for (int32_t r = 0; r < cnt; ++r) {
    const int32_t p = 1 << r;

    int32_t par = 0;

    for (int32_t pos = 1; pos <= n; ++pos) {
      if (pos == p)
        continue;

      if (pos & p) {
        par ^= coded_data[pos - 1];
      }
    }

    if (par != coded_data[p - 1]) {
      error_pos |= p;
    }
  }

  return error_pos;
}

std::vector<int> send_splited_message(const std::vector<int>& ms,
                                      int block_size = 4) {
  int n = ms.size();
  std::vector<int> errors(std::ceil(ms.size() / 4.0), 0);

  int error_i = 0;
  for (auto chunk : ms | std::views::chunk(block_size)) {

    auto cm = code_message(chunk | std::ranges::to<std::vector<int>>());

    cm[2] = 0;

    errors[error_i++] = decode_message(cm);
  }

  return errors;
}

int32_t main() {
  std::vector<int32_t> v = {1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1};
  // auto ms = code_message(v);
  //
  // std::cout << "Coded message:\n";
  // for (auto a : ms) {
  //   std::cout << a << ' ';
  // }
  // std::cout << '\n';
  //
  // std::vector<int32_t> m{0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1};
  //
  // std::cout << "Error index: " << decode_message(m) << '\n';

  auto errors = send_splited_message(v);

  for (int a : errors) {
    std::cout << a << ' ';
  }
}
