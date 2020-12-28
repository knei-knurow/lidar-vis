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

/**
 * Scenario is a set of actions which are executed just after grabbing the cloud data,
 * but before its visualization by the GUI.
 */
class Scenario {
 public:
  Scenario() = default;
  virtual ~Scenario() = default;
  virtual bool update(Cloud& cloud) = 0;
  virtual ScenarioType get_type() const = 0;
};

class RecordSeriesScenario : public Scenario {
 public:
  RecordSeriesScenario();
  bool update(Cloud& cloud) override;
  inline ScenarioType get_type() const override { return ScenarioType::RECORD_SERIES; }
};

// Very slow
class ScreenshotSeriesScenario : public Scenario {
 public:
  explicit ScreenshotSeriesScenario(std::function<bool()>& screenshot_fn);
  bool update(Cloud& cloud) override;
  inline ScenarioType get_type() const override { return ScenarioType::SCREENSHOT_SERIES; }

 private:
  std::function<bool()> screenshot_fn_;
};
