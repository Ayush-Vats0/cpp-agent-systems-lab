# Chapter 04 — Closed-Loop Agent

## 1. Overview

Chapter 04 evolves the search-based open-loop agent into a closed-loop agent capable of observing the environment after execution,
evaluating the actual outcome, detecting unexpected state changes, and generating a replacement plan.

Chapter 03 established that an Agent could calculate a complete route and execute its corresponding action sequence.
However, that approach relied on the assumption that the environment would behave as expected throughout execution.

This chapter addresses that limitation.

The Agent now executes one action at a time and evaluates the resulting environment state against its predicted state.
If the two states differ, the current plan is no longer trusted. The Agent uses the actual observed state as the starting point for replanning.

The resulting control loop is:

**Observe → Plan → Act → Observe → Evaluate → Continue or Replan**

This is the first implementation in the project that demonstrates feedback-driven adaptation to an unexpected environment state.

## 2. Learning Objectives

The primary computer science and agent-system objectives are:

- Understand the difference between open-loop and closed-loop execution.
- Establish the Agent as the owner of execution progress.
- Track the current plan and the next action to execute.
- Predict the expected state before performing an action.
- Observe the actual environment state after execution.
- Evaluate expected state against observed state.
- Detect divergence between predicted and actual outcomes.
- Invalidate a plan when its assumptions are no longer reliable.
- Replan from the actual observed state rather than the original starting state.
- Distinguish successful planning, an already-reached goal, and planning failure.
- Resume execution using a replacement plan.
- Stop safely when replanning cannot produce a valid route.

## 3. Environment Configuration

Chapter 04 retains the **3×3 GridWorld** used in Chapters 01–03.

| Property | Configuration |
|---|---|
| Grid dimensions | 3×3 |
| Starting position | (0,0) |
| Goal position | (2,2) |
| Normal obstacle | (1,1) |
| Movement model | Four-directional |
| Movement cost | 1 per transition |

The normal environment configuration remains unchanged.

The controlled disturbance experiments temporarily introduced unexpected states and additional obstacles. These were test conditions,
not permanent changes to the environment.

After testing, the artificial disturbance and failure-mode modifications were removed.

## 4. From Open-Loop to Closed-Loop Execution

### 4.1 Chapter 03: Open-Loop Execution

The earlier implementation followed this pattern:

```text
Observe
   ↓
Plan complete route
   ↓
Generate action sequence
   ↓
Execute complete plan
```

The Agent assumed that the planned transitions would occur as expected.

There was no continuous feedback mechanism to determine whether the environment had actually reached the state predicted by the plan after each action.

### 4.2 Chapter 04: Closed-Loop Execution

The new implementation introduces feedback:

```text
Observe
   ↓
Plan
   ↓
Select one action
   ↓
Predict expected state
   ↓
Execute action
   ↓
Observe actual state
   ↓
Evaluate expected vs observed
   ↓
Continue or detect mismatch
```

The Agent no longer treats the original plan as unquestionable.

Instead, it checks whether the environment's actual behavior remains consistent with the predicted transition.

## 5. Agent-Owned Execution State

A major architectural change was moving execution progress into the Agent.

The Agent owns:

- The current action plan.
- The index of the next action.
- Retrieval of the next action.
- Advancement through the action sequence.
- Replacement of the current plan.
- Resetting execution progress when a replacement plan is loaded.

The resulting execution model is:

```text
Agent
 ├── Current plan
 ├── Current action index
 ├── Next-action selection
 ├── Execution
 ├── Observation
 └── Evaluation
```

This is important because the Agent, rather than the main program's orchestration logic, controls its own execution progress.

### 5.1 Plan Replacement

The Agent's plan-loading operation replaces the existing action sequence and resets the action index to the beginning of the new sequence.

Conceptually:

```text
Old plan
   ↓
Invalidated
   ↓
New plan loaded
   ↓
Action index reset
   ↓
Execution resumes
```

Resetting the index is essential. Without it, the Agent could load a new plan but continue from an index belonging to the previous plan.

## 6. Expected State and Actual State

The closed-loop mechanism depends on distinguishing two states.

**Expected state**

The state predicted by applying the planned action to the current state using the Environment's transition rules.

**Observed state**

The state actually reported by the Environment after the action has been executed.

The process is:

```text
Current observed state
        │
        ├───────────────┐
        ↓               │
Hypothetical            │
transition              │
        ↓               │
Expected state          │
                        │
Current state           │
        ↓               │
Actual execution        │
        ↓               │
Environment             │
        ↓               │
Observed state          │
                        │
        └──────┬────────┘
               ↓
       Compare both states
```

The hypothetical transition and actual execution use the same environmental movement rules.

This preserves the distinction established in Chapter 02:

- `tryMove()` evaluates a hypothetical transition.
- `act()` attempts an actual state-changing action.

A prediction is not proof of execution success. The Agent must observe the resulting environment state.

## 7. State Evaluation and Mismatch Detection

After execution, the Agent compares the expected and observed positions.

Both coordinates must match for the evaluation to succeed.

For example:

| Expected state | Observed state | Evaluation |
|---|---|---|
| (1,0) | (1,0) | Match |
| (1,0) | (0,0) | Mismatch |
| (1,0) | (0,2) | Mismatch |

When the states match, the Agent can advance to the next action.

When they differ, the Agent identifies that the current plan may no longer be valid.

This is the transition from merely executing actions to verifying their outcomes.

## 8. Controlled Mismatch Experiment

To verify that mismatch detection was functional, a deterministic, testing-only disturbance was temporarily introduced.

The normal first action was:

```text
(0,0) → (1,0)
```

The Agent expected:

```text
Expected Position: (1, 0)
```

The test disturbance then changed the environment's reported state to:

```text
Observed Position: (0, 0)
```

The Agent detected:

```text
State mismatch.
Plan may no longer be valid.
```

This experiment demonstrated that the evaluation mechanism was not simply confirming successful execution.
It could also identify divergence between the predicted outcome and the actual observed state.

The artificial disturbance was subsequently removed. It is not part of the final environment implementation.

## 9. Planner Control and PlanningResult

The chapter introduced a higher-level planning interface.

Instead of having the Agent depend directly on the BFS implementation, the planning request is made through `Planner::plan()`.

The Planner internally uses BFS for the current implementation.

This separates the Agent's requirement for a plan from the particular search algorithm used to produce it.

### 9.1 PlanningResult

A planning operation must communicate more than a route.

An empty route alone is ambiguous: it could mean that the Agent is already at the goal, or that no route exists.

To distinguish these outcomes, the Planner returns a `PlanningResult` containing:

- A planning status.
- A position path.

### 9.2 PlanningStatus

Three outcomes were established.

| Status | Meaning | Agent response |
|---|---|---|
| Success | A valid route was found. | Convert the route into actions, load the new plan and continue. |
| AlreadyAtGoal | The supplied starting state is already the objective. | Stop; no movement is required. |
| Failure | No valid route could be found. | Stop rather than continue with an invalid plan. |

This result contract makes the Planner's outcome explicit.

It also prevents the Agent from interpreting every empty path as the same condition.

## 10. Replanning from the Actual Observed State

The most important closed-loop recovery principle established in this chapter is:

**Replanning must begin from the state the Environment actually reached, not from the state the Agent expected to reach.**

When a mismatch occurs, the Agent already has the observed state.

That state becomes the starting point for the next planning request.

The replanning pipeline is:

```text
Mismatch detected
       ↓
Invalidate current plan
       ↓
Use actual observed state
       ↓
Planner::plan()
       ↓
PlanningResult
       ↓
 ┌─────────────┬────────────────┐
 │             │                │
Success    AlreadyAtGoal      Failure
 │             │                │
 ↓             ↓                ↓
Generate      Stop             Stop
actions
 │
 ↓
Load replacement plan
 │
 ↓
Reset action index
 │
 ↓
Resume execution
```

The Agent requests a new plan. It does not need to know that BFS is the underlying search algorithm.

This maintains the separation between execution control and planning.

## 11. Replanning Experiments

Three outcomes were tested to verify the replanning decision process.

### 11.1 Successful Replanning

The initial route was:

```text
(0,0) → (1,0) → (2,0) → (2,1) → (2,2)
```

After the first action, the Agent expected:

```text
(1,0)
```

The controlled disturbance instead produced:

```text
(0,2)
```

The Agent detected the mismatch and requested a new plan starting from `(0,2)`.

The Planner generated:

```text
(0,2)
   ↓
(1,2)
   ↓
(2,2)
```

The replacement action sequence was:

```text
MoveRight
MoveRight
```

The Agent loaded the replacement plan, reset its action index and resumed execution.

It successfully reached the original goal `(2,2)`.

This experiment demonstrated genuine adaptation: the Agent did not simply restart the original action sequence.
It generated a new route from the actual observed state.

### 11.2 Already-at-Goal Replanning

A separate experiment tested the case where the Agent's observed state during replanning was already the goal.

The Planner returned:

`AlreadyAtGoal`

The Agent stopped because no additional movement was necessary.

This verified that replanning correctly distinguishes task completion from an ordinary route search.

### 11.3 Replanning Failure

A temporary failure-mode experiment introduced additional obstacles that made the goal unreachable.

The Agent detected a mismatch:

| State | Position |
|---|---|
| Expected | (1,0) |
| Observed | (2,0) |

The Planner attempted to find a route from `(2,0)`.

BFS explored the reachable states but could not reach the goal.

The result was:

`Failure`

The Agent stopped rather than continuing with an invalid plan.

This verified that a failed replanning operation does not cause the Agent to execute stale actions.

After these experiments, the artificial disturbance and failure-mode changes were reverted.

## 12. Final Clean Execution

After removing the temporary testing modifications, the clean implementation was executed again.

The BFS exploration output was:

```text
BFS exploring: (0, 0)
BFS exploring: (1, 0)
BFS exploring: (0, 1)
BFS exploring: (2, 0)
BFS exploring: (0, 2)
BFS exploring: (2, 1)
```

The planned path was:

```text
(0, 0)
(1, 0)
(2, 0)
(2, 1)
(2, 2)
```

The generated actions were:

```text
MoveRight
MoveRight
MoveDown
MoveDown
```

The execution results were:

| Executed action | Resulting position |
|---|---|
| MoveRight | (1,0) |
| MoveRight | (2,0) |
| MoveDown | (2,1) |
| MoveDown | (2,2) |

At every step, the expected position matched the observed position.

The clean implementation therefore completed the original route successfully after the temporary test mechanisms had been removed.

## 13. Architectural Responsibilities

The chapter reinforces the separation of four major responsibilities.

| Component | Responsibility |
|---|---|
| Environment | Owns actual state, constraints, transition rules, hypothetical simulation and actual execution. |
| Agent | Owns the objective, current plan, execution progress, observation, evaluation and execution control. |
| Planner | Receives planning requests, coordinates the planning process and returns a planning result. |
| Search algorithm | Performs the underlying state-space search. The current implementation uses BFS. |

The intended relationship is:

```text
                 AGENT
                   │
           Planning request
                   │
                   ▼
                PLANNER
                   │
             Planning control
                   │
                   ▼
                  BFS
                   │
              Position path
                   │
                   ▼
            Action conversion
                   │
              Action plan
                   │
                   ▼
                 AGENT
                   │
             Execute action
                   │
                   ▼
              ENVIRONMENT
                   │
             Actual state
                   │
                   ▼
             Observation
                   │
                   ▼
             Evaluation
                   │
          Continue or replan
```

One architectural concern was also identified: the earlier Planner implementation used Agent goal-evaluation functionality.
If the Agent directly depends on the Planner, this can create a bidirectional dependency.

The underlying requirement is that the Planner needs goal-evaluation capability, not necessarily the entire Agent.
This concern was documented for future architectural refinement rather than being treated as an already completed refactoring.

## 14. Limitations and Boundaries

The completed implementation demonstrates a functional closed-loop planning and recovery cycle, but its scope remains deliberately limited.

- The environment is a small, deterministic 3×3 GridWorld.
- BFS remains the underlying search algorithm.
- The experiments use controlled disturbances rather than a continuously changing external environment.
- The tests demonstrate individual recovery cases, not repeated or indefinite disturbance handling.
- Dynamic obstacles and changing goals were not implemented.
- A broader policy for repeated unsuccessful replanning was not established.
- The Planner–Agent dependency concern was identified but not fully refactored.

These boundaries are important when interpreting the results. The implementation demonstrates feedback-driven replanning,
but it is not a general-purpose autonomous navigation or recovery system.

## 15. Verification

The Chapter 04 implementation was compiled and executed successfully.

The standard verification command, run from the repository root, was:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/04-closed-loop-agent/main.cpp -o agent
```

The completed work includes:

- Normal closed-loop execution.
- Expected-versus-observed state evaluation.
- Controlled mismatch detection.
- Successful replanning from an actual observed state.
- Replacement-plan loading and execution resumption.
- Already-at-goal handling.
- Unreachable-goal handling.
- Restoration and successful execution of the clean implementation.

The native Windows build and execution environment has not been verified. The documented successful verification is based on the Linux GNU G++ environment.

## 16. Chapter Outcome

Chapter 04 establishes the first complete feedback-driven control loop in the GridWorld project.

The Agent has evolved from executing a precomputed route to monitoring the actual environment and responding when reality diverges from its prediction.

The principal outcomes are:

- Agent-owned execution state.
- One-action-at-a-time execution.
- Expected-state prediction.
- Actual-state observation.
- State evaluation and mismatch detection.
- Plan invalidation.
- Replanning from the actual observed state.
- Explicit planning outcomes.
- Replacement-plan loading and execution resumption.
- Safe stopping when the goal is already reached or replanning fails.

The central engineering principle is:

**A plan is not proof that the environment will behave as expected. An agent must observe actual outcomes, evaluate them, and revise its plan when necessary.**

Chapter 04 therefore marks the transition from a planner-driven open-loop program to a feedback-driven agent capable of detecting and recovering from controlled
environmental divergence.
