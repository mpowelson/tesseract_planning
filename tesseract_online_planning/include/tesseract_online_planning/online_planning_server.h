/**
 * @file online_planning_server.h
 * @brief
 *
 * @author Matthew Powelson
 * @date May 22, 2021
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
#ifndef TESSERACT_ONLINE_PLANNING_ONLINE_PLANNING_SERVER_H
#define TESSERACT_ONLINE_PLANNING_ONLINE_PLANNING_SERVER_H

#include <tesseract_common/macros.h>
TESSERACT_COMMON_IGNORE_WARNINGS_PUSH
#include <taskflow/taskflow.hpp>
#include <memory>
#include <functional>
#include <vector>

#include <trajopt_ifopt/trajopt_ifopt.h>
#include <tesseract_process_managers/core/process_planning_server.h>


TESSERACT_COMMON_IGNORE_WARNINGS_POP


namespace tesseract_planning
{


enum class OptimizationState
{
  OPTIMIZING,
  FIXED
};

/// @brief Supposed to keep track of the state the environment is in for a single SQP iteration
/// The idea is that other threads can be adjusting things, setting up new goals, etc while one SQP iteration is happening.
///
struct OptimizationStateInfo
{
  unsigned revision;
  tesseract_environment::Environment::Ptr environment;
};


class OnlinePlanningServer
{
public:
  OnlinePlanningServer();

  // ---------- User Interface ------------------

  /// @todo: Actually I think maybe this should go elsewhere. The online server just does the SQP and the passing in of seeds can be elsewhere
  void setProcessPlanner(ProcessPlanningServer server);

  /// @brief Runs through on demand planner
  void setManipulatorGoal(std::string manipulator, ProcessPlanningRequest request);

  /// @brief Toggle optimization timer
  bool startOnlineUpdates();

  /// @brief Toggle optimization timer
  bool stopOnlineUpdates();

  /// @brief Toggle state of a specific maninpulator
  void setManipulatorState(std::string, OptimizationState state);



  // ----------- Execution Interface --------------------

  /// @brief Gets the execution feedback and turns off pieces of the optimization that don't apply
  void updateExecutedSteps();

  /// @brief Get trajectory to send to execution for splicing
  CompositeInstruction getUpdatedTraj(std::string manipulator);


private:

  /// @brief Update SQP costs/cnts and set
  bool setManipulatorGoal();




  /// @brief Runs one step of the SQP usign the latest environment, goals, and seed
  int stepOptimizationTaskFn();

  std::vector<std::function<int()>> optimization_callbacks_;


  /// @brief Maybe not necessary if included in trajopt
  bool runTimeParameterization();


  /// Used to determine if the OptimizationStateInfo is up to date
  unsigned revision_;



  /// @brief Key: manipulator name, Value: Optimization variables associated with it
  std::unordered_map<std::string, std::vector<ifopt::VariableSet>> optimization_variables_;

  std::vector<OptimizationStateInfo> optimization_state_infos;

    std::shared_ptr<tf::Executor> executor_;

};
}  // namespace tesseract_planning
#endif  // tesseract_online_planning_TASKFLOW_CONTAINER_H
