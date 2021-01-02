#include "gui.h"

GUI::GUI(const GUISettings& settings) {
  settings_ = settings;
  settings_.origin_x = settings_.width / 2;
  settings_.origin_y = settings_.height / 2;

  sf::ContextSettings window_settings;
  window_settings.antialiasingLevel = settings_.antialiasing;

  // Load from a font file on disk
  if (!settings_.font.loadFromFile("arial.ttf")) {
    std::clog << "lidar-vis: error loading font file" << std::endl;
  } else {
    std::clog << "loaded font" << std::endl;
  }

  window_.create(sf::VideoMode(settings_.width, settings_.height), "Lidar",
                 sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize, window_settings);

  for (int i = 0; i < int(StatusKey::COUNT); i++) {
    status_keys_[i] = false;
  }

  screenshots_cnt_ = 0;
}

bool GUI::update(const Cloud& cloud) {
  if (!settings_.running) {
    window_.close();
    return false;
  }

  handle_input(cloud);
  window_.clear(settings_.color_background);

  if (settings_.render_grid) {
    render_grid();
  }

  if (cloud.size > 0) {
    if (settings_.autoscale) {
      auto new_scale = calc_scale(cloud.max_distance);
      if (new_scale >= settings_.scale * 1.1 || new_scale <= settings_.scale * 0.9)
        settings_.scale = new_scale;
    }

    if (settings_.render_grid) {
      render_cloud_bars(cloud);
    }

    render_front_line(cloud.points_cart.front().x, cloud.points_cart.front().y);

    if (settings_.points_display_mode == GUISettings::DOTS_LINES) {
      render_connected_cloud(cloud);
    } else if (settings_.points_display_mode == GUISettings::DOTS) {
      render_cloud(cloud);
    } else if (settings_.points_display_mode == GUISettings::LINES) {
      render_connected_cloud(cloud, 1.0, false);
    }
  }

  render_text(cloud);

  render_point(0, 0, Color::Red);

  window_.display();
  window_.setTitle("Scale: 1mm -> " + std::to_string(settings_.scale) + "px");
  sf::sleep(sf::milliseconds(settings_.sleep_time_ms));
  return true;
}

void GUI::handle_input(const Cloud& cloud) {
  sf::Event event;
  while (window_.pollEvent(event)) {
    if (event.type == sf::Event::Resized) {
      settings_.width = event.size.width;
      settings_.height = event.size.height;
      settings_.origin_x = event.size.width / 2;
      settings_.origin_y = event.size.height / 2;
      sf::FloatRect visible_area(0, 0, event.size.width, event.size.height);
      window_.setView(sf::View(visible_area));
      std::cout << "Window resized: " << event.size.width << "x" << event.size.height << std::endl;
    }
    if (event.type == sf::Event::Closed)
      settings_.running = false;
    if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::Up)
        status_keys_[int(StatusKey::UP)] = true;
      if (event.key.code == sf::Keyboard::Down)
        status_keys_[int(StatusKey::DOWN)] = true;
      if (event.key.code == sf::Keyboard::Left)
        status_keys_[int(StatusKey::LEFT)] = true;
      if (event.key.code == sf::Keyboard::Right)
        status_keys_[int(StatusKey::RIGHT)] = true;

      if (event.key.code == sf::Keyboard::S)
        save_screenshot();

      if (event.key.code == sf::Keyboard::R)
        settings_.render_mouse_ray = !settings_.render_mouse_ray;
      if (event.key.code == sf::Keyboard::C)
        settings_.colormap =
            GUISettings::Colormap((settings_.colormap + 1) % GUISettings::COLORMAP_COUNT);
      if (event.key.code == sf::Keyboard::M)
        settings_.points_display_mode = GUISettings::PointsDispayMode(
            (settings_.points_display_mode + 1) % GUISettings::PTS_DISPLAY_MODE_COUNT);
    }

    if (event.type == sf::Event::MouseWheelScrolled) {
      settings_.autoscale = false;
      if (event.mouseWheelScroll.delta > 0)
        settings_.scale *= 1.25;
      else if (event.mouseWheelScroll.delta < 0)
        settings_.scale *= 0.8;
    } else if (event.type == sf::Event::MouseButtonPressed) {
      if (event.mouseButton.button == sf::Mouse::Middle) {
        settings_.autoscale = true;
        settings_.origin_y = settings_.height / 2;
        settings_.origin_x = settings_.width / 2;
      }
    }

    if (event.type == sf::Event::KeyReleased) {
      if (event.key.code == sf::Keyboard::Up)
        status_keys_[int(StatusKey::UP)] = false;
      if (event.key.code == sf::Keyboard::Down)
        status_keys_[int(StatusKey::DOWN)] = false;
      if (event.key.code == sf::Keyboard::Left)
        status_keys_[int(StatusKey::LEFT)] = false;
      if (event.key.code == sf::Keyboard::Right)
        status_keys_[int(StatusKey::RIGHT)] = false;
    }
  }

  if (status_keys_[int(StatusKey::UP)])
    settings_.origin_y += 25;
  if (status_keys_[int(StatusKey::DOWN)])
    settings_.origin_y -= 25;
  if (status_keys_[int(StatusKey::LEFT)])
    settings_.origin_x += 25;
  if (status_keys_[int(StatusKey::RIGHT)])
    settings_.origin_x -= 25;
}

void GUI::render_grid() {
  for (int i = 0; i <= 30; i++) {
    auto circle = sf::CircleShape(1000 * i * settings_.scale, 64);
    circle.setOrigin(circle.getRadius() - 1, circle.getRadius() - 1);
    circle.setPosition(settings_.origin_x, settings_.origin_y);
    circle.setFillColor(Color::Transparent);
    circle.setOutlineColor(settings_.color_grid);
    if (i % 10 == 0)
      circle.setOutlineThickness(4);
    else
      circle.setOutlineThickness(1);
    window_.draw(circle);
  }
}

void GUI::render_cloud_bars(const Cloud& cloud) {
  if (cloud.size == 0)
    return;

  unsigned max_width = 80;
  for (int j = 0; j < settings_.height; j++) {
    float dist = cloud.points_cyl[size_t(j * cloud.size / settings_.height)].distance;

    int width = int(std::round(dist / cloud.max_distance * max_width));

    Color color;
    if (settings_.colormap == GUISettings::FROM_ANGLE) {
      color = calc_color_from_angle(float(j * cloud.size / settings_.height) / float(cloud.size));
    } else if (settings_.colormap == GUISettings::FROM_DIST) {
      color =
          calc_color_from_dist(cloud.points_cyl[size_t(j * cloud.size / settings_.height)].distance,
                               cloud.max_distance, 1.0);
    }

    auto rect = sf::RectangleShape(sf::Vector2f(width, 1));
    rect.setPosition(0, j);
    rect.setFillColor(color);
    window_.draw(rect);
  }
}

void GUI::render_cloud(const Cloud& cloud, float lightness) {
  if (cloud.size == 0)
    return;

  for (int i = 0; i < cloud.size; i++) {
    if (cloud.points_cyl[i].distance == 0)
      continue;

    Color color;
    if (settings_.colormap == GUISettings::FROM_ANGLE) {
      color = calc_color_from_angle(float(i) / float(cloud.size), lightness);
    } else if (settings_.colormap == GUISettings::FROM_DIST) {
      color = calc_color_from_dist(cloud.points_cyl[i].distance, cloud.max_distance, lightness);
    }
    render_point(cloud.points_cart[i].x, cloud.points_cart[i].y, color);
  }
}

void GUI::render_connected_cloud(const Cloud& cloud, float lightness, bool render_points) {
  if (cloud.size == 0)
    return;

  sf::VertexArray vertex_arr(sf::PrimitiveType::LinesStrip);
  for (int i = 0; i < cloud.size; i++) {
    if (cloud.points_cyl[i].distance == 0)
      continue;

    Color color;
    if (settings_.colormap == GUISettings::FROM_ANGLE) {
      color = calc_color_from_angle(float(i) / float(cloud.size), lightness);
    } else if (settings_.colormap == GUISettings::FROM_DIST) {
      color = calc_color_from_dist(cloud.points_cyl[i].distance, cloud.max_distance, lightness);
    }

    if (render_points) {
      render_point(cloud.points_cart[i].x, cloud.points_cart[i].y, color);
    }

    color.a = 128;
    sf::Vertex vertex(sf::Vector2f(cloud.points_cart[i].x * settings_.scale + settings_.origin_x,
                                   cloud.points_cart[i].y * settings_.scale + settings_.origin_y),
                      color);
    vertex_arr.append(vertex);
  }
  vertex_arr.append(vertex_arr[0]);
  window_.draw(vertex_arr);
}

void GUI::render_point(int x, int y, const Color& color) {
  auto pt = sf::CircleShape(settings_.bold_mode ? 5 : 2);

  pt.setOrigin(1, 1);
  pt.setPosition(x * settings_.scale + settings_.origin_x,
                 y * settings_.scale + settings_.origin_y);
  pt.setFillColor(color);
  window_.draw(pt);
}

void GUI::render_front_line(int x, int y) {
  sf::Vertex line[] = {
      sf::Vertex(sf::Vector2f(settings_.origin_x, settings_.origin_y), settings_.color_grid),
      sf::Vertex(sf::Vector2f(x * settings_.scale + settings_.origin_x,
                              y * settings_.scale + settings_.origin_y),
                 settings_.color_grid)};
  window_.draw(line, 2, sf::Lines);
}

void GUI::render_text(const Cloud& cloud) {
  std::string msg = "cloud " + std::to_string(cloud.index);
  sf::Text text(msg, settings_.font, 48);
  text.setPosition(settings_.width - (settings_.width / 4), 0);
  text.setFillColor(sf::Color::White);

  window_.draw(text);
}

bool GUI::save_screenshot() {
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%d.%m.%Y-%H.%M.%S");

  auto filename = settings_.output_dir + "/screenshot-" + std::to_string(++screenshots_cnt_) + "-" +
                  oss.str() + ".png";

  sf::Texture texture;
  texture.create(settings_.width, settings_.height);
  texture.update(window_);

  if (!texture.copyToImage().saveToFile(filename)) {
    std::cerr << "error: unable to save screenshot" << std::endl;
    return false;
  } else {
    std::cout << "screenshot saved: " << filename << std::endl;
    return true;
  }
}

float GUI::calc_scale(float max_dist) const {
  return float(settings_.height) * 0.7f / max_dist;
}

Color GUI::calc_color_from_angle(float v, float lightness) {
  Color c0, c1;
  if (v >= 0 && v <= 0.33f) {
    c0 = Color(0, 255, 255);
    c1 = Color(255, 0, 255);
  } else if (v <= 0.66f) {
    v -= 0.33f;
    c0 = Color(255, 255, 0);
    c1 = Color(0, 255, 255);
  } else if (v <= 1.0f) {
    v -= 0.66f;
    c0 = Color(255, 0, 255);
    c1 = Color(255, 255, 0);
  } else {
    return Color(255, 255, 255);
  }

  c0.r *= float(v) / 0.34f;
  c0.g *= float(v) / 0.34f;
  c0.b *= float(v) / 0.34f;
  c1.r *= 1.0f - float(v) / 0.34f;
  c1.g *= 1.0f - float(v) / 0.34f;
  c1.b *= 1.0f - float(v) / 0.34f;

  return Color(float(c0.r + c1.r) * lightness, float(c0.g + c1.g) * lightness,
               float(c0.b + c1.b) * lightness, 255);
}

Color GUI::calc_color_from_dist(float dist, float max, float lightness) {
  float v = dist / max;
  Color c0, c1;
  if (v >= 0 && v <= 0.33f) {
    c0 = Color(255, 255, 0);
    c1 = Color(255, 0, 0);
  } else if (v <= 0.66f) {
    v -= 0.33f;
    c0 = Color(0, 255, 0);
    c1 = Color(255, 255, 0);
  } else if (v <= 1.0f) {
    v -= 0.66f;
    c0 = Color(0, 0, 255);
    c1 = Color(0, 255, 0);
  } else {
    return Color(255, 255, 255);
  }

  c0.r *= float(v) / 0.34f;
  c0.g *= float(v) / 0.34f;
  c0.b *= float(v) / 0.34f;
  c1.r *= 1.0f - float(v) / 0.34f;
  c1.g *= 1.0f - float(v) / 0.34f;
  c1.b *= 1.0f - float(v) / 0.34f;

  return Color(float(c0.r + c1.r) * lightness, float(c0.g + c1.g) * lightness,
               float(c0.b + c1.b) * lightness, 255);
}
