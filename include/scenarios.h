#pragma once
#include <chrono>
#include <functional>
#include <string>
#include <vector>
#include "cloud.h"

enum class ScenarioType {
  IDLE,
  RECORD_SERIES,
  SCREENSHOT_SERIES,
};

class Scenario {
 public:
  Scenario() : status_(true){};
  virtual ~Scenario() {}
  virtual bool update(Cloud& cloud) = 0;
  inline virtual bool get_status() const { return status_; }
  inline virtual ScenarioType get_type() const = 0;

 protected:
  bool status_;
};

class RecordSeriesScenario : public Scenario {
 public:
  RecordSeriesScenario();
  virtual bool update(Cloud& cloud);
  inline virtual ScenarioType get_type() const { return ScenarioType::RECORD_SERIES; }
};

// Very slow
class ScreenshotSeriesScenario : public Scenario {
 public:
  ScreenshotSeriesScenario(std::function<bool()> screenshot_fn);
  virtual bool update(Cloud& cloud);
  inline virtual ScenarioType get_type() const { return ScenarioType::SCREENSHOT_SERIES; }

 private:
  std::function<bool()> screenshot_fn_;
};
