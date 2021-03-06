/**
 * @file waypoint_type.cpp
 * @brief
 *
 * @author Levi Armstrong
 * @date June 15, 2020
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

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <typeindex>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_command_language/waypoint_type.h>
#include <tesseract_command_language/null_waypoint.h>
#include <tesseract_command_language/cartesian_waypoint.h>
#include <tesseract_command_language/joint_waypoint.h>
#include <tesseract_command_language/state_waypoint.h>

namespace tesseract_planning
{
bool isCartesianWaypoint(const Waypoint& waypoint)
{
  return (waypoint.getType() == std::type_index(typeid(CartesianWaypoint)));
}

bool isJointWaypoint(const Waypoint& waypoint)
{
  return (waypoint.getType() == std::type_index(typeid(JointWaypoint)));
}

bool isStateWaypoint(const Waypoint& waypoint)
{
  return (waypoint.getType() == std::type_index(typeid(StateWaypoint)));
}

bool isNullWaypoint(const Waypoint& waypoint) { return (waypoint.getType() == std::type_index(typeid(NullWaypoint))); }

}  // namespace tesseract_planning
