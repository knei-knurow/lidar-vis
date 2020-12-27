#pragma once
#include "cloud-grabbers.h"
#include "gui.h"
#include "scenarios.h"

class App {
 public:
  explicit App(std::vector<std::string>& args);
  ~App();

  int run();
  static void print_help();

 private:
  static bool check_arg(std::vector<std::string>& all_args,
                 const std::string& short_arg,
                 const std::string& long_arg);

  static std::string get_arg_value(std::vector<std::string>& all_args,
                            const std::string& short_arg,
                            const std::string& long_arg,
                            const std::string& default_value = "");

  bool parse_args(std::vector<std::string>& args);

  bool running_;
  std::unique_ptr<CloudGrabber> cloud_grabber_;
  std::unique_ptr<Scenario> scenario_;
  std::unique_ptr<GUI> gui_;
};
