#pragma once

#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/environment.hpp"
#include "godot_cpp/classes/time.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include <cassert>
#include <sstream>
#include <stack>
#include <string>

namespace BehaviourLib {

struct PerformanceLogger
{
  struct PerfData
  {
    std::string name;
    uint64_t timeUsec;
  };

  std::vector<PerfData> CurrentFrameData;
  std::stack<std::string> InProcessFrameData;

  void StartMeasure(const std::string& name) { InProcessFrameData.push(name); }

  void EndMeasure(const std::string& name, uint64_t deltaTimeUsec)
  {
    std::string& measureName = InProcessFrameData.top();
    assert(measureName == name);
    CurrentFrameData.emplace_back(name, deltaTimeUsec);
    InProcessFrameData.pop();
  }

  void Clear()
  {
    CurrentFrameData.clear();
    assert(InProcessFrameData.empty());
  }

  godot::String GetGodotString()
  {
    std::stringstream stream;

    stream << "FPS: " << godot::Engine::get_singleton()->get_frames_per_second()
           << "\n";
    for (auto& perf : CurrentFrameData) {
      stream << perf.name << ": " << perf.timeUsec << "\n";
    }

    godot::StringName result{ stream.str().c_str() };
    return result;
  }

  void Print()
  {
    godot::UtilityFunctions::print("PERFORMANCE MEASUREMENTS:");
    godot::UtilityFunctions::print(GetGodotString());
    godot::UtilityFunctions::print("END PERFORMANCE MEASUREMENTS:");
  }
};

extern PerformanceLogger PerformanceLogger;

struct PerfMeasure
{
  std::string name;
  uint64_t startTimeUsec = 0;

  PerfMeasure(std::string name)
  {
    this->name = std::move(name);
    uint64_t time = godot::Time::get_singleton()->get_ticks_usec();
    startTimeUsec = time;
    PerformanceLogger.StartMeasure(this->name);
  };
  ~PerfMeasure()
  {
    uint64_t endTimeUsec = godot::Time::get_singleton()->get_ticks_usec();
    uint64_t deltaUsec = endTimeUsec - startTimeUsec;
    PerformanceLogger.EndMeasure(this->name, deltaUsec);
  }
};

#define PERF(x) PerfMeasure performance_measure_variable(x);
} // namespace BehaviourLib
