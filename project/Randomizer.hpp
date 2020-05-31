#pragma once
#include <random>

class Randomizer {
private:
  Randomizer();
  std::mt19937 engine_m;
 public:
  static Randomizer &get_instance();
  Randomizer(const Randomizer &) = delete;
  void operator=(const Randomizer &) = delete;
  std::string get_string(unsigned len);
  int get_number(int min, int max);
};


