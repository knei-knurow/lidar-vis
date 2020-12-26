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
  CloudGrabber() : status_(true){};
  virtual ~CloudGrabber(){};
  virtual bool read(Cloud& cloud) = 0;
  virtual bool get_status() const { return status_; }

 protected:
  bool status_;
};

class CloudFileGrabber : public CloudGrabber {
 public:
  CloudFileGrabber(const std::string& filename, float rot_angle_ = 0.0f);
  virtual bool read(Cloud& cloud);

 private:
  std::string filename_;
  float rot_angle_;
  Cloud cloud_;
};

class CloudFileSeriesGrabber : public CloudGrabber {
 public:
  CloudFileSeriesGrabber(const std::string& filename);
  virtual bool read(Cloud& cloud);

 private:
  bool open();

  std::string filename_;
  std::ifstream file_;
  size_t clouds_cnt_;
  std::chrono::steady_clock::time_point next_cloud_time_;
};
