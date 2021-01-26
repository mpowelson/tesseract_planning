/**
 * @file plan_instruction.h
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
#ifndef TESSERACT_COMMAND_LANGUAGE_PLAN_INSTRUCTION_H
#define TESSERACT_COMMAND_LANGUAGE_PLAN_INSTRUCTION_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <vector>
#include <Eigen/Geometry>
#include <tinyxml2.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_command_language/core/waypoint.h>
#include <tesseract_command_language/constants.h>
#include <tesseract_command_language/instruction_type.h>
#include <tesseract_command_language/types.h>

namespace tesseract_planning
{
enum class PlanInstructionType : int
{
  LINEAR = 0,
  FREESPACE = 1,
  CIRCULAR = 2,
  START = 3
};

class PlanInstruction
{
public:
  PlanInstruction(Waypoint waypoint,
                  PlanInstructionType type,
                  std::string profile = DEFAULT_PROFILE_KEY,
                  ManipulatorInfo manipulator_info = ManipulatorInfo());

  void setWaypoint(Waypoint waypoint);
  Waypoint& getWaypoint();
  const Waypoint& getWaypoint() const;

  void setManipulatorInfo(ManipulatorInfo info);
  const ManipulatorInfo& getManipulatorInfo() const;
  ManipulatorInfo& getManipulatorInfo();

  void setProfile(const std::string& profile);
  const std::string& getProfile() const;

  /** @brief Add a profile object that will override the named profile for a specific task
   * @param task_name Name of the task to which this profile will be applied
   * @param profile ConstPtr to profile of appropriate type for the given task  */
  void addProfileOverride(const std::string& task_name, std::shared_ptr<const void> profile);
  /** @brief Removes a profile override
   * @param task_name Name of task whose profile override should be removed  */
  void removeProfileOverride(const std::string& task_name);
  /** @brief Get profile override for named task
   * @param task_name Name of the task whose profile override should be retrieved
   * @return Profile override if task_name found. Otherwise, nullptr.  */
  const std::shared_ptr<const void>& getProfileOverride(const std::string& task_name) const;
  /** @brief getProfileOverrides
   * @return Copy of the profile override map */
  std::unordered_map<std::string, std::shared_ptr<const void>> getProfileOverrides() const;

  int getType() const;

  const std::string& getDescription() const;

  void setDescription(const std::string& description);

  void print(std::string prefix = "") const;

  PlanInstructionType getPlanType() const;

  void setPlanType(PlanInstructionType type);

  bool isLinear() const;

  bool isFreespace() const;

  bool isCircular() const;

  bool isStart() const;

  tinyxml2::XMLElement* toXML(tinyxml2::XMLDocument& doc) const;

private:
  int type_{ static_cast<int>(InstructionType::PLAN_INSTRUCTION) };

  PlanInstructionType plan_type_;

  /** @brief The assigned waypoint (Cartesian or Joint) */
  Waypoint waypoint_;

  /** @brief The profile used for this plan instruction */
  std::string profile_{ DEFAULT_PROFILE_KEY };

  /** @brief Overrides used to replace named profile specified for a specific task */
  std::unordered_map<std::string, std::shared_ptr<const void>> profile_overrides_;

  /** @brief Contains information about the manipulator associated with this instruction*/
  ManipulatorInfo manipulator_info_;

  /** @brief The description of the instruction */
  std::string description_{ "Tesseract Plan Instruction" };
};

}  // namespace tesseract_planning

#ifdef SWIG
%tesseract_command_language_add_instruction_type(PlanInstruction)
#endif  // SWIG

#endif  // TESSERACT_COMMAND_LANGUAGE_PLAN_INSTRUCTION_H
