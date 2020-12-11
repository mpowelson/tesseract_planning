﻿/**
 * @file raster_taskflow.h
 * @brief Plans raster paths
 *
 * @author Matthew Powelson
 * @date July 15, 2020
 * @version TODO
 * @bug No known bugs
 *
 * @copyright Copyright (c) 2020, Southwest Research Institute
 *
 * @par License
 * Software License Agreement (Apache License)
 * @par
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * @par
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef TESSERACT_PROCESS_MANAGERS_RASTER_TASKFLOW_H
#define TESSERACT_PROCESS_MANAGERS_RASTER_TASKFLOW_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <functional>
#include <vector>
#include <thread>
#include <taskflow/taskflow.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_process_managers/core/taskflow_generator.h>

namespace tesseract_planning
{
/**
 * @brief This class provides a taskflow for a raster process.
 *
 * Given a ProcessInput in the correct format, it handles the creation of the process dependencies and uses Taskflow to
 * execute them efficiently in a parallel based on those dependencies.
 *
 * The required format is below.
 * for skipping of rasters without replanning. This logic must be handled in the execute process.
 *
 * Composite
 * {
 *   Composite - from start
 *   Composite - Raster segment
 *   Composite - Transitions
 *   Composite - Raster segment
 *   Composite - Transitions
 *   Composite - Raster segment
 *   Composite - to end
 * }
 */
class RasterTaskflow : public TaskflowGenerator
{
public:
  using UPtr = std::unique_ptr<RasterTaskflow>;

  RasterTaskflow(TaskflowGenerator::UPtr freespace_taskflow_generator,
                 TaskflowGenerator::UPtr transition_taskflow_generator,
                 TaskflowGenerator::UPtr raster_taskflow_generator,
                 std::string name = "RasterTaskflow");

  ~RasterTaskflow() override = default;
  RasterTaskflow(const RasterTaskflow&) = delete;
  RasterTaskflow& operator=(const RasterTaskflow&) = delete;
  RasterTaskflow(RasterTaskflow&&) = delete;
  RasterTaskflow& operator=(RasterTaskflow&&) = delete;

  const std::string& getName() const override;

  TaskflowContainer generateTaskflow(ProcessInput input,
                                     std::function<void()> done_cb,
                                     std::function<void()> error_cb) override;

private:
  TaskflowGenerator::UPtr freespace_taskflow_generator_;
  TaskflowGenerator::UPtr transition_taskflow_generator_;
  TaskflowGenerator::UPtr raster_taskflow_generator_;
  std::string name_;

  /**
   * @brief Checks that the ProcessInput is in the correct format.
   * @param input ProcessInput to be checked
   * @return True if in the correct format
   */
  bool checkProcessInput(const ProcessInput& input) const;
};

}  // namespace tesseract_planning

#endif
