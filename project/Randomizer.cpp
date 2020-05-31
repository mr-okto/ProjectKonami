#include <algorithm>
#include "Randomizer.hpp"


Randomizer::Randomizer()
    : engine_m(std::random_device{}()) {}

std::string Randomizer::get_string(unsigned len) {
  static std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
  static std::uniform_int_distribution<std::string::size_type> pick(0, alphabet.size() - 1);
  std::string result(len, ' ');
  std::generate_n(result.begin(), len, [&]() { return alphabet[pick(engine_m)]; });
  return result;
}

int Randomizer::get_number(int min, int max) {
  std::uniform_int_distribution<int> pick(min, max);
  return pick(engine_m);
}

Randomizer &Randomizer::get_instance() {
  static Randomizer randomizer;
  return randomizer;
}
