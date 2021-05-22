/**
 * @file online_planning_server.cpp
 * @brief
 *
 * @author Matthew Powelson
 * @date July 15. 2020
 * @version TODO
 * @bug No known bugs
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

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <console_bridge/console.h>
#include <fstream>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_online_planning/online_planning_server.h>

namespace tesseract_planning
{

  OnlinePlanningServer::setManipulatorGoal(std::string manipulator, ProcessPlanningRequest request)
  {
    // Fill out OptimizationStateInfo

    // Create Tasks
    /**
    *                           I think Taskflow has a parfor?
    *                          |-----> optimization_callbacks_[0]  ----|
    *  stepOptimizationTaskFn -|-----> optimization_callbacks_[1]  ----|--> if(info.revision == revision_)
    *            ^             |-----> optimization_callbacks_[n]  ----|         |
    *            |                                                               |
    *            ----------------------------------------------------------------|
    *
    *
    *
    *  // On another call to setManipulatorGoal another loop gets kicked off with updatd info where info.revision == revision_
    *
    *
    *
    */




  }

}  // namespace tesseract_planning
