#include "cloud-grabbers.h"

CloudFileGrabber::CloudFileGrabber(const std::string& filename, float rot_angle) {
  filename_ = filename;
  rot_angle_ = rot_angle;
  status_ = std::ifstream(filename_).good();
  if (!status_) {
    std::cerr << "error: file does not contain a valid cloud" << std::endl;
    status_ = false;
  }
}

bool CloudFileGrabber::read(Cloud& cloud) {
  if (!status_)
    return false;

  if (cloud_.size == 0) {
    std::ifstream file(filename_);
    while (file) {
      std::string line;
      std::getline(file, line);
      if (line.empty() || line[0] == '#')
        continue;
      PointCyl pt_cyl;
      std::stringstream sline(line);
      sline >> pt_cyl.angle >> pt_cyl.dist;

      if (pt_cyl.dist > cloud_.max) {
        cloud_.max = pt_cyl.dist;
        cloud_.max_idx = cloud_.size;
      }
      if (pt_cyl.dist < cloud_.min && pt_cyl.dist > 0) {
        cloud_.min = pt_cyl.dist;
        cloud_.min_idx = cloud_.size;
      }

      cloud_.size++;
      cloud_.pts_cyl.push_back(pt_cyl);
      cloud_.pts_cart.push_back(pt_cyl.to_cart());
    }

    if (cloud_.size == 0) {
      std::cerr << "error: file does not contain a valid cloud" << std::endl;
      status_ = false;
    }
    cloud = cloud_;
  } else if (rot_angle_ != 0.0f) {
    for (int i = 0; i < cloud.size; i++) {
      cloud.pts_cyl[i].angle += rot_angle_;
      if (cloud.pts_cyl[i].angle >= 360.0f) {
        cloud.pts_cyl[i].angle -= 360.0f;
      } else if (cloud.pts_cyl[i].angle < 0.0f) {
        cloud.pts_cyl[i].angle += 360.0f;
      }
      cloud.pts_cart[i] = cloud.pts_cyl[i].to_cart();
    }
  }

  return status_;
}

//
//	CloudFileSeriesGrabber
//
CloudFileSeriesGrabber::CloudFileSeriesGrabber(const std::string& filename) {
  filename_ = filename;
  clouds_cnt_ = 0;
  file_.open(filename_);
  open();
  next_cloud_time_ = std::chrono::steady_clock::now();
}

bool CloudFileSeriesGrabber::read(Cloud& cloud) {
  if (!status_)
    return false;

  if (std::chrono::steady_clock::now() < next_cloud_time_)
    return true;

  if (cloud.size == 0)
    next_cloud_time_ = std::chrono::steady_clock::now();

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
    sline >> pt_cyl.angle >> pt_cyl.dist;

    if (pt_cyl.dist > cloud.max) {
      cloud.max = pt_cyl.dist;
      cloud.max_idx = cloud.size;
    }
    if (pt_cyl.dist < cloud.min && pt_cyl.dist > 0) {
      cloud.min = pt_cyl.dist;
      cloud.min_idx = cloud.size;
    }

    cloud.size++;
    cloud.pts_cyl.push_back(pt_cyl);
    cloud.pts_cart.push_back(pt_cyl.to_cart());
  }

  if (cloud.size == 0 && clouds_cnt_ == 0) {
    std::cerr << "error: file does not contain a valid cloud" << std::endl;
    status_ = false;
  }

  clouds_cnt_++;

  if (cloud.size == 0) {
    std::cerr << "Cloud series end." << std::endl;
    next_cloud_time_ += std::chrono::milliseconds(1000);
    file_.clear();
    file_.seekg(0);
    status_ = open();
  }

  return status_;
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
    status_ = false;
  }
  return status_;
}
