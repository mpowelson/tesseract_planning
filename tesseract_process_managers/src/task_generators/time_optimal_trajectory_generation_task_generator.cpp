/**
 * @file time_optimal_trajectory_generation_task_generator.h
 * @brief Perform TOTG
 *
 * @author Levi Armstrong
 * @author Matthew Powelson
 * @date January 22, 2021
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
#include <console_bridge/console.h>
TESSERACT_COMMON_IGNORE_WARNINGS_POP

#include <tesseract_motion_planners/planner_utils.h>
#include <tesseract_process_managers/task_generators/time_optimal_trajectory_generation_task_generator.h>
#include <tesseract_command_language/composite_instruction.h>
#include <tesseract_command_language/move_instruction.h>
#include <tesseract_command_language/utils/filter_functions.h>
#include <tesseract_command_language/utils/flatten_utils.h>
#include <tesseract_command_language/utils/utils.h>
#include <tesseract_time_parameterization/time_optimal_trajectory_generation.h>
#include <tesseract_time_parameterization/utils.h>

namespace tesseract_planning
{
TimeOptimalTrajectoryGenerationProfile::TimeOptimalTrajectoryGenerationProfile(double max_velocity_scaling_factor,
                                                                               double max_acceleration_scaling_factor,
                                                                               double path_tolerance,
                                                                               double resample_dt,
                                                                               double min_angle_change)
  : max_velocity_scaling_factor(max_velocity_scaling_factor)
  , max_acceleration_scaling_factor(max_acceleration_scaling_factor)
  , path_tolerance(path_tolerance)
  , resample_dt(resample_dt)
  , min_angle_change(min_angle_change)
{
}

TimeOptimalTrajectoryGenerationTaskGenerator::TimeOptimalTrajectoryGenerationTaskGenerator(std::string name)
  : TaskGenerator(std::move(name))
{
  // Register default profile
  composite_profiles["DEFAULT"] = std::make_shared<TimeOptimalTrajectoryGenerationProfile>();
}

int TimeOptimalTrajectoryGenerationTaskGenerator::conditionalProcess(TaskInput input, std::size_t unique_id) const
{
  if (input.isAborted())
    return 0;

  auto info = std::make_shared<TimeOptimalTrajectoryGenerationTaskInfo>(unique_id, name_);
  info->return_value = 0;
  input.addTaskInfo(info);

  // --------------------
  // Check that inputs are valid
  // --------------------
  Instruction* input_results = input.getResults();
  if (!isCompositeInstruction(*input_results))
  {
    CONSOLE_BRIDGE_logError("Input results to TOTG must be a composite instruction");
    return 0;
  }

  auto* ci = input_results->cast<CompositeInstruction>();
  const ManipulatorInfo& manip_info = ci->getManipulatorInfo();
  const auto fwd_kin = input.env->getManipulatorManager()->getFwdKinematicSolver(manip_info.manipulator);

  // Get Composite Profile
  std::string profile = ci->getProfile();
  profile = getProfileString(profile, name_, input.composite_profile_remapping);
  auto cur_composite_profile =
      getProfile<TimeOptimalTrajectoryGenerationProfile>(profile,
                                                         composite_profiles,
                                                         std::make_shared<TimeOptimalTrajectoryGenerationProfile>(),
                                                         ci->getProfileOverride(name_));

  // Create data structures for checking for plan profile overrides
  auto flattened = flatten(*ci, moveFilter);
  if (flattened.empty())
  {
    CONSOLE_BRIDGE_logWarn("TOTG found no MoveInstructions to process");
    info->return_value = 1;
    return 1;
  }

  double velocity_scaling_factor = cur_composite_profile->max_velocity_scaling_factor;
  double acceleration_scaling_factor = cur_composite_profile->max_acceleration_scaling_factor;

  // Loop over all sub-composites
  bool use_move_profile = false;
  std::vector<double> scaling_factors(ci->size(), velocity_scaling_factor);
  for (std::size_t idx = 0; idx < ci->size(); idx++)
  {
    const auto mi = ci->at(idx).cast_const<CompositeInstruction>();
    profile = mi->getProfile();

    // Check for remapping of the plan profile
    std::string remap = getProfileString(profile, name_, input.plan_profile_remapping);
    auto cur_move_profile = getProfile<TimeOptimalTrajectoryGenerationProfile>(
        remap, composite_profiles, nullptr, mi->getProfileOverride(name_));

    // If there is a move profile associated with it, override the parameters
    if (cur_move_profile)
    {
      use_move_profile = true;
      scaling_factors[idx] = cur_move_profile->max_velocity_scaling_factor;
    }
  }

  if (use_move_profile)
  {
    // Set these to 1 so we can do the scaling after the fact
    velocity_scaling_factor = 1;
    acceleration_scaling_factor = 1;
  }

  // Solve using parameters
  TimeOptimalTrajectoryGeneration solver(cur_composite_profile->path_tolerance,
                                         cur_composite_profile->resample_dt,
                                         cur_composite_profile->min_angle_change);

  // Copy the Composite before passing in because it will get flattened and resampled
  CompositeInstruction resampled(*ci);
  if (!solver.computeTimeStamps(resampled,
                                fwd_kin->getLimits().velocity_limits,
                                fwd_kin->getLimits().acceleration_limits,
                                velocity_scaling_factor,
                                acceleration_scaling_factor))
  {
    CONSOLE_BRIDGE_logInform("Failed to perform TOTG for process input: %s!", input_results->getDescription().c_str());
    return 0;
  }

  // Unflatten
  if (cur_composite_profile->unflatten)
  {
    CompositeInstruction unflattened = unflatten(resampled, *ci, cur_composite_profile->path_tolerance + 1e-6);

    // Rescale
    if (use_move_profile)
    {
      RescaleTimings(unflattened, scaling_factors);
    }

    for(std::size_t idx = 0; idx < ci->size(); idx++)
      (*ci)[idx] = unflattened[idx];
  }
  else
  {
    if (use_move_profile)
      CONSOLE_BRIDGE_logWarn("TOTG Move Profile specified but unflatten is not set in the composite profile. Move "
                             "Profile will be ignored");

    for(std::size_t idx = 0; idx < ci->size(); idx++)
      (*ci)[idx] = resampled[idx];
  }

  CONSOLE_BRIDGE_logDebug("TOTG succeeded");
  info->return_value = 1;
  return 1;
}

void TimeOptimalTrajectoryGenerationTaskGenerator::process(TaskInput input, std::size_t unique_id) const
{
  conditionalProcess(input, unique_id);
}

CompositeInstruction
TimeOptimalTrajectoryGenerationTaskGenerator::unflatten(const CompositeInstruction& flattened_input,
                                                        const CompositeInstruction& pattern,
                                                        double tolerance) const
{
  CompositeInstruction unflattened(pattern);
  for (auto& instr : unflattened)
    instr.cast<CompositeInstruction>()->clear();

  Eigen::VectorXd last_pt_in_input = getJointPosition(pattern.at(0).cast_const<CompositeInstruction>()->back().cast_const<MoveInstruction>()->getWaypoint());

  // Loop over the flattened composite adding the instructions to the appropriate subcomposite
  for (std::size_t resample_idx = 0, original_idx = 0; resample_idx < flattened_input.size(); resample_idx++)
  {
    // Add flattened point to the subcomposite
    unflattened[original_idx].cast<CompositeInstruction>()->push_back(flattened_input.at(resample_idx));

    // If all joints are within the tolerance, then this point hopefully corresponds
    if (isMoveInstruction(flattened_input.at(resample_idx)))
    {
      Eigen::VectorXd current_pt =
          getJointPosition(flattened_input.at(resample_idx).cast_const<MoveInstruction>()->getWaypoint());
      bool increment = true;
      for (Eigen::Index jnt = 0; jnt < current_pt.size(); jnt++)
        increment &= std::abs(last_pt_in_input[jnt] - current_pt[jnt]) < tolerance;

      // If it corresponds, then start adding points to the subcomposite
      if (increment)
      {
        if(original_idx < pattern.size() - 1)  // Keep from incrementing too far at the end of the last composite
          original_idx++;
        unflattened[original_idx].cast<CompositeInstruction>()->clear();
        last_pt_in_input = getJointPosition(pattern.at(original_idx).cast_const<CompositeInstruction>()->back().cast_const<MoveInstruction>()->getWaypoint());
      }
    }
  }
  return unflattened;
}

TimeOptimalTrajectoryGenerationTaskInfo::TimeOptimalTrajectoryGenerationTaskInfo(std::size_t unique_id,
                                                                                 std::string name)
  : TaskInfo(unique_id, std::move(name))
{
}
}  // namespace tesseract_planning
