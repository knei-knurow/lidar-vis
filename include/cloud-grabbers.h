#pragma once
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>
#include "cloud.h"

enum class CloudGrabberType {
  FILE,
  FILE_SERIES,
};

class CloudGrabber {
 public:
  CloudGrabber() : ok_(true){};
  virtual ~CloudGrabber() = default;
  virtual bool read(Cloud& cloud) = 0;
  virtual bool is_ok() const { return ok_; }

 protected:
  bool ok_;
};

class SingleCloudGrabber : public CloudGrabber {
 public:
  explicit SingleCloudGrabber(std::istream& input_stream, float rot_angle_ = 0.0f);
  bool read(Cloud& cloud) override;

 private:
  std::istream input_stream_;
  float rot_angle_;
  Cloud cloud_;
};

class CloudFileSeriesGrabber : public CloudGrabber {
 public:
  explicit CloudFileSeriesGrabber(const std::string& filename);
  bool read(Cloud& cloud) override;

 private:
  bool open();

  std::string filename_;
  std::ifstream file_;
  size_t cloud_count_;
  std::chrono::steady_clock::time_point next_cloud_time_;
};
