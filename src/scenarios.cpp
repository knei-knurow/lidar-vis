#include "scenarios.h"

RecordSeriesScenario::RecordSeriesScenario() = default;

bool RecordSeriesScenario::update(Cloud& cloud) {
  return true;
}

ScreenshotSeriesScenario::ScreenshotSeriesScenario(std::function<bool()>& screenshot_fn) {
  screenshot_fn_ = screenshot_fn;
}

bool ScreenshotSeriesScenario::update(Cloud& cloud) {
  return screenshot_fn_();
}
