#include <cassert>
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

std::vector<int32_t> code(const std::vector<int32_t>& data) {
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

// return error pos in 1-index
int32_t auto_fix_message(std::vector<int32_t>& coded_data) {
  size_t n = coded_data.size();

  int32_t cnt = cnt_2(n) - 1;

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

  if (error_pos != 0)
    coded_data[error_pos - 1] = !coded_data[error_pos - 1];

  return error_pos;
}


std::vector<int> decode(const std::vector<int>& coded_data) {
  int n = coded_data.size();
  int cnt2 = cnt_2(n) - 1;
  std::vector<int> data;
  data.reserve(n - cnt2);

  for (int i = 0; i < n; ++i) {
    const int pos = i + 1; 

    if (is_2(pos))
      continue;

    data.emplace_back(coded_data[i]);
  }

  return data;
}


// emulate sinding message
std::vector<int> send_splited_message(const std::vector<int>& ms,
                                      int block_size = 4) {
  int n = ms.size();
  std::vector<int> received_message;
  received_message.reserve(n);

  for (auto chunk : ms | std::views::chunk(block_size)) {

    auto cm = code(chunk | std::ranges::to<std::vector<int>>());

    int error = auto_fix_message(cm);

    auto received_part = decode(cm);

    received_message.insert(received_message.end(), received_part.begin(), received_part.end());
  }

  return received_message;
}

int32_t main() {
  std::vector<int32_t> v = {1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1};

  for (auto a : v) {
    std::cout << a << ' ';
  }
  std::cout << '\n';

  auto ms = code(v);
  auto res = send_splited_message(v);

  for (auto a : res) {
    std::cout << a << ' ';
  }
  std::cout << '\n';

#if 0
  std::vector<int32_t> m{0, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1};

  std::cout << "Error index: " << auto_fix_message(m) << '\n';

  for (int a : m) {
    std::cout << a << ' ';
  }
  std::cout << '\n';

  auto decoded = decode(m);

  for (int a : v) {
    std::cout << a << ' ';
  }
  std::cout << '\n';
  for (int a : decoded) {
    std::cout << a << ' ';
  }
#endif
}
