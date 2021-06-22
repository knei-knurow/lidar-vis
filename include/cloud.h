#pragma once
#include <cmath>
#include <vector>

enum class CoordSystem {
    CART,
    CYL,
};

struct PointCart {
    int x = 0;
    int y = 0;
};

struct PointCyl {
    float angle = 0;
    float distance = 0;

    PointCart to_cart() const;
    PointCart to_cart(float scale, float origin_x, float origin_y) const;
};

struct Cloud {
    std::vector<PointCyl> points_cyl;
    std::vector<PointCart> points_cart;
    float min_distance = std::numeric_limits<float>::max();
    float max_distance = 0;
    size_t max_distance_index = -1;
    size_t min_distance_index = -1;
    size_t size = 0;
    int index = 0;
};
