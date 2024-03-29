#include "../include/cloud-grabbers.h"

SingleCloudGrabber::SingleCloudGrabber(float rot_angle) {
    rot_angle_ = rot_angle;
    ok_ = std::cin.good();
    if (!ok_) {
        std::cerr << "lidar-vis: error: file does not contain a valid cloud"
                  << std::endl;
        ok_ = false;
    }
}

bool SingleCloudGrabber::read(Cloud& cloud) {
    if (!ok_) {
        return false;
    }

    if (cloud_.size == 0) {
        while (std::cin) {
            std::string line;
            std::getline(std::cin, line);

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

            if (point_cyl.distance < cloud_.min_distance &&
                point_cyl.distance > 0) {
                cloud_.min_distance = point_cyl.distance;
                cloud_.min_distance_index = cloud_.size;
            }

            cloud_.size++;
            cloud_.points_cyl.push_back(point_cyl);
            cloud_.points_cart.push_back(point_cyl.to_cart());
        }

        if (cloud_.size == 0) {
            std::cerr << "lidar-vis: error: file does not contain a valid cloud"
                      << std::endl;
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

CloudSeriesGrabber::CloudSeriesGrabber(bool ignore_sleep) {
    cloud_count_ = 0;
    ignore_sleep_ = ignore_sleep;
    open();
    next_cloud_time_ = std::chrono::steady_clock::now();
}

bool CloudSeriesGrabber::read(Cloud& cloud) {
    if (!ok_) {
        return false;
    }

    if (!ignore_sleep_ && std::chrono::steady_clock::now() < next_cloud_time_) {
        std::this_thread::sleep_until(next_cloud_time_);
    }

    if (!ignore_sleep_ && cloud.size == 0) {
        next_cloud_time_ = std::chrono::steady_clock::now();
    }

    cloud = Cloud();
    while (std::cin.good()) {
        std::string line;
        std::getline(std::cin, line);
        PointCyl pt_cyl;
        std::stringstream sline(line);

        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line[0] == '!') {
            long long delay_ms = 0;
            char __;
            sline >> __ >> cloud.index >> delay_ms;
            if (!ignore_sleep_)
                next_cloud_time_ = std::chrono::steady_clock::now() +
                                   std::chrono::milliseconds(delay_ms);
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
        std::cerr << "lidar-vis: error: file does not contain a valid cloud"
                  << std::endl;
        ok_ = false;
    }

    cloud_count_++;

    if (cloud.size == 0) {
        std::cerr << "lidar-vis: cloud series end" << std::endl;
        if (!ignore_sleep_)
            next_cloud_time_ += std::chrono::milliseconds(1000);
        std::cin.clear();
        std::cin.seekg(0);
        ok_ = open();
    }

    return ok_;
}

bool CloudSeriesGrabber::open() {
    while (std::cin.good()) {
        std::string line;
        std::getline(std::cin, line);
        if (line.empty() || line[0] == '!') {
            break;
        }
    }

    if (!std::cin.good()) {
        std::cerr
            << "lidar-vis: error: file does not contain a valid cloud series"
            << std::endl;
        ok_ = false;
    }

    return ok_;
}
