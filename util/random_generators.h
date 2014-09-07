#ifndef RANDOM_GENERATORS_H
#define RANDOM_GENERATORS_H

#include <random>

struct RandomIntGenerator {
  RandomIntGenerator(int low, int high) {
    dist = std::uniform_int_distribution<int>(low, high);
    std::random_device rd;
    gen = std::default_random_engine(rd());
  }
  int get_random_value() { return dist(gen); }
  int operator()() { return dist(gen); }

 private:
  std::uniform_int_distribution<int> dist;
  std::default_random_engine gen;
};

struct RandomDoubleGenerator {
  RandomDoubleGenerator(double low, double high) {
    dist = std::uniform_real_distribution<double>(low, high);
    std::random_device rd;
    gen = std::default_random_engine(rd());
  }
  double get_random_value() { return dist(gen); }
  double operator()() { return dist(gen); }

 private:
  std::uniform_real_distribution<double> dist;
  std::default_random_engine gen;
};

struct RandomFloatGenerator {
  RandomFloatGenerator(float low, float high) {
    dist = std::uniform_real_distribution<float>(low, high);
    std::random_device rd;
    gen = std::default_random_engine(rd());
  }
  float get_random_value() { return dist(gen); }
  float operator()() { return dist(gen); }

 private:
  std::uniform_real_distribution<float> dist;
  std::default_random_engine gen;
};

#endif
