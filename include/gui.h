#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "cloud.h"

typedef sf::Color Color;

struct GUISettings {
    enum Colormap {
        FROM_ANGLE,
        FROM_DIST,

        COLORMAP_COUNT,
    };

    enum PointsDispayMode {
        DOTS_LINES,
        DOTS,
        LINES,

        PTS_DISPLAY_MODE_COUNT,
    };

    bool running = true;
    unsigned width = 1280;
    unsigned height = 720;
    unsigned int origin_x = width / 2;
    unsigned origin_y = height / 2;
    float scale = 0.2;
    bool autoscale = true;
    Colormap colormap = FROM_ANGLE;
    PointsDispayMode points_display_mode = DOTS_LINES;
    sf::Font font;
    float sleep_time_ms = 1000.0f / 30.0f;

    Color color_background = Color(16, 16, 24);
    Color color_grid = Color(40, 40, 50);

    bool render_mouse_ray = true;
    bool render_grid = true;
    bool bold_mode = false;
    unsigned antialiasing = 8;

    std::string output_dir = ".";
};

/**
 * GUI is responsible for the visual layer of the application and interacting
 * with the user.
 */
class GUI {
   public:
    explicit GUI(const GUISettings& settings);
    virtual bool update(const Cloud& cloud);

    inline GUISettings settings() const { return settings_; }
    inline GUISettings& settings() { return settings_; }

    bool save_screenshot();
    bool save_cloud(const Cloud& cloud);

   private:
    enum class StatusKey {
        UP,
        RIGHT,
        DOWN,
        LEFT,
        COUNT,
    };

    void handle_input(const Cloud& cloud);
    void render_grid();
    void render_cloud_bars(const Cloud& cloud);
    void render_cloud(const Cloud& cloud, float lightness = 1.0f);
    void render_connected_cloud(const Cloud& cloud,
                                float lightness = 1.0f,
                                bool render_points = true);
    void render_point(int x, int y, const Color& color);
    void render_front_line(int x, int y);
    void render_text(const Cloud& cloud);

    float calc_scale(float max_dist) const;
    static Color calc_color_from_angle(float v, float lightness = 1.0f);
    static Color calc_color_from_dist(float dist,
                                      float max,
                                      float lightness = 1.0f);

    GUISettings settings_;
    sf::RenderWindow window_;
    bool status_keys_[int(StatusKey::COUNT)];
    size_t screenshots_cnt_;
};
