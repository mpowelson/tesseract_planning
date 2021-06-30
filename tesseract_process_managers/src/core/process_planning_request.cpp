/**
 * @file process_planning_request.cpp
 * @brief Process Info
 *
 * @author Matthew Powelson
 * @date June 30. 2021
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
#include <boost/serialization/unordered_map.hpp>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_process_managers/core/process_planning_request.h>

namespace tesseract_planning
{
bool ProcessPlanningRequest::operator==(const ProcessPlanningRequest& rhs) const
{
  bool equal = true;
  equal &= name == rhs.name;
  equal &= instructions == rhs.instructions;
  equal &= seed == rhs.seed;
  /// @todo Add equality operator for environment state
//  equal &= *env_state == *rhs.env_state;
  equal &= commands == rhs.commands;
  equal &= profile == rhs.profile;
  equal &= plan_profile_remapping == rhs.plan_profile_remapping;
  equal &= composite_profile_remapping == rhs.composite_profile_remapping;
  return equal;
}
bool ProcessPlanningRequest::operator!=(const ProcessPlanningRequest& rhs) const { return !operator==(rhs); }

template <class Archive>
void ProcessPlanningRequest::serialize(Archive& ar, const unsigned int /*version*/)
{
  ar& boost::serialization::make_nvp("name", name);
  ar& boost::serialization::make_nvp("instructions", instructions);
  ar& boost::serialization::make_nvp("seed", seed);
  /// @todo Add serialization for EnvState
//  ar& boost::serialization::make_nvp("env_state", env_state);
  /// @todo Add serialization for Commands
//  ar& boost::serialization::make_nvp("commands", commands);
  ar& boost::serialization::make_nvp("profile", profile);
  ar& boost::serialization::make_nvp("plan_profile_remapping", plan_profile_remapping);
  ar& boost::serialization::make_nvp("composite_profile_remapping", composite_profile_remapping);
}
}  // namespace tesseract_planning

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
template void tesseract_planning::ProcessPlanningRequest::serialize(boost::archive::xml_oarchive& ar, const unsigned int version);
template void tesseract_planning::ProcessPlanningRequest::serialize(boost::archive::xml_iarchive& ar, const unsigned int version);
