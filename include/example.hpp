// Copyright 2021 Ko71k <rassmagin.wolf@mail.ru>

#ifndef INCLUDE_EXAMPLE_HPP_
#define INCLUDE_EXAMPLE_HPP_

#include <picosha2.h>
#include <signal.h>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <mutex>
#include <nlohmann/json.hpp>
#include <random>
#include <string>
#include <thread>

#include <boost/log/trivial.hpp>



namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;

using json = nlohmann::json;

struct result {
  size_t timestamp = 0;
  std::string hash;
  std::string data;
};

bool check_hash(const std::string& str);

void saveInfo(json&, const result&);

void start_threads(size_t, json&);

result generate_hash(size_t);

[[noreturn]] void main_work(json&, size_t);

#endif  // INCLUDE_EXAMPLE_HPP_
