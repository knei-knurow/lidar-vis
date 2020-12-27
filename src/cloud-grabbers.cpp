#include "cloud-grabbers.h"

SingleCloudGrabber::SingleCloudGrabber(std::istream& input_stream, float rot_angle) {
  input_stream_ = input_stream;
  rot_angle_ = rot_angle;
  ok_ = input_stream.good();
  if (!ok_) {
    std::cerr << "error: file does not contain a valid cloud" << std::endl;
    ok_ = false;
  }
}

bool SingleCloudGrabber::read(Cloud& cloud) {
  if (!ok_)
    return false;

  if (cloud_.size == 0) {
    while (input_stream_) {
      std::string line;
      std::getline(input_stream_, line);

      if (line.empty() || line[0] == '#') {
        continue;
      }

      PointCyl point_cyl;
      std::stringstream line_stream(line);
      line_stream >> point_cyl.angle >> point_cyl.distance;

      if (point_cyl.distance > cloud_.max_distance) {
        cloud_.max_distance = point_cyl.distance;
        cloud_.max_distance_index = cloud_.size;
      }

      if (point_cyl.distance < cloud_.min_distance && point_cyl.distance > 0) {
        cloud_.min_distance = point_cyl.distance;
        cloud_.min_distance_index = cloud_.size;
      }

      cloud_.size++;
      cloud_.points_cyl.push_back(point_cyl);
      cloud_.points_cart.push_back(point_cyl.to_cart());
    }

    if (cloud_.size == 0) {
      std::cerr << "error: file does not contain a valid cloud" << std::endl;
      ok_ = false;
    }
    cloud = cloud_;
  } else if (rot_angle_ != 0.0f) {
    for (int i = 0; i < cloud.size; i++) {
      cloud.points_cyl[i].angle += rot_angle_;
      if (cloud.points_cyl[i].angle >= 360.0f) {
        cloud.points_cyl[i].angle -= 360.0f;
      } else if (cloud.points_cyl[i].angle < 0.0f) {
        cloud.points_cyl[i].angle += 360.0f;
      }
      cloud.points_cart[i] = cloud.points_cyl[i].to_cart();
    }
  }

  return ok_;
}

CloudFileSeriesGrabber::CloudFileSeriesGrabber(const std::string& filename) {
  filename_ = filename;
  cloud_count_ = 0;
  file_.open(filename_);
  open();
  next_cloud_time_ = std::chrono::steady_clock::now();
}

bool CloudFileSeriesGrabber::read(Cloud& cloud) {
  if (!ok_) {
    return false;
  }

  if (std::chrono::steady_clock::now() < next_cloud_time_) {
    return true;
  }

  if (cloud.size == 0) {
    next_cloud_time_ = std::chrono::steady_clock::now();
  }

  cloud = Cloud();
  while (file_.good()) {
    std::string line;
    std::getline(file_, line);
    PointCyl pt_cyl;
    std::stringstream sline(line);

    if (line.empty() || line[0] == '#') {
      continue;
    } else if (line[0] == '!') {
      long long _, delay_ms = 0;
      char __;
      sline >> __ >> _ >> delay_ms;
      next_cloud_time_ = std::chrono::steady_clock::now() + std::chrono::milliseconds(delay_ms);
      break;
    }
    sline >> pt_cyl.angle >> pt_cyl.distance;

    if (pt_cyl.distance > cloud.max_distance) {
      cloud.max_distance = pt_cyl.distance;
      cloud.max_distance_index = cloud.size;
    }
    if (pt_cyl.distance < cloud.min_distance && pt_cyl.distance > 0) {
      cloud.min_distance = pt_cyl.distance;
      cloud.min_distance_index = cloud.size;
    }

    cloud.size++;
    cloud.points_cyl.push_back(pt_cyl);
    cloud.points_cart.push_back(pt_cyl.to_cart());
  }

  if (cloud.size == 0 && cloud_count_ == 0) {
    std::cerr << "error: file does not contain a valid cloud" << std::endl;
    ok_ = false;
  }

  cloud_count_++;

  if (cloud.size == 0) {
    std::cerr << "Cloud series end." << std::endl;
    next_cloud_time_ += std::chrono::milliseconds(1000);
    file_.clear();
    file_.seekg(0);
    ok_ = open();
  }

  return ok_;
}

bool CloudFileSeriesGrabber::open() {
  while (file_.good()) {
    std::string line;
    std::getline(file_, line);
    if (line.empty() || line[0] == '!') {
      break;
    }
  }

  if (!file_.good()) {
    std::cerr << "rrror: file does not contain a valid cloud series" << std::endl;
    ok_ = false;
  }
  return ok_;
}
