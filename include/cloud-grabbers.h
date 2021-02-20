#pragma once
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "cloud.h"

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
  explicit SingleCloudGrabber(float rot_angle_ = 0.0f);

  /**
   * Read reads from stdin and assign the result to its cloud parameter.
   * @param cloud Pointer to the cloud to which the read data will be put.
   * @return true if reading was successful, false otherwise
   */
  bool read(Cloud& cloud) override;

 private:
  float rot_angle_;
  Cloud cloud_;
};

class CloudSeriesGrabber : public CloudGrabber {
 public:
  explicit CloudSeriesGrabber();
  bool read(Cloud& cloud) override;

 private:
  bool open();

  size_t cloud_count_;
  std::chrono::steady_clock::time_point next_cloud_time_;
};

class PointStreamGrabber : public CloudGrabber {
public:
	explicit PointStreamGrabber();
	bool read(Cloud& cloud) override;

	long long get_last_timestamp() const;
private:
	bool process_line(const std::string& line, Cloud& cloud);

	long long last_timestamp_;
};