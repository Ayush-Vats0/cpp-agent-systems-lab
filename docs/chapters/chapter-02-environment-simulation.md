# Chapter 02 — Environment Simulation

## 1. Overview

This chapter introduces a more explicit Environment interface for the 3×3 GridWorld.

The implementation develops the separation between the Agent and the Environment by introducing a distinction between three operations:

- `observe()` — retrieve the Environment's current state.
- `tryMove()` — simulate a possible movement without committing it.
- `act()` — execute a movement and update the Environment's actual state.

The central learning objective is to distinguish between a hypothetical state transition and an actual state transition.

This distinction becomes important when an agent needs to examine possible actions without physically changing the environment during every evaluation.

## 2. Learning Objectives

The implementation introduces the following concepts:

- The Environment as the owner of the actual world state.
- Observation through a dedicated interface.
- Hypothetical movement simulation.
- Boundary validation.
- Obstacle validation.
- Separation between candidate-state calculation and transition validity.
- The difference between simulation and execution.
- The use of C++ structures and reference parameters to return calculated state.

The chapter also preserves an intermediate implementation milestone that exposed an action-semantics inconsistency. That historical issue is documented rather than silently removed from the archived source.

## 3. Environment Configuration

The implementation continues to use the original **3×3 GridWorld**.

The environment contains:

- Starting position: `(0, 0)`
- Goal position: `(2, 2)`
- Obstacle position: `(1, 1)`

The position is represented by the `Position` structure:

```cpp
struct Position
{
    int horizontal;
    int vertical;
};
```

A `Position` is a type. Individual coordinates are represented by objects or variables of that type.

The Environment maintains the actual position of the agent.

## 4. Environment Interface

The implementation introduces three important operations.

### 4.1 `observe()`

Conceptually:

```cpp
Position observe() const
{
    return position;
}
```

The purpose of `observe()` is to provide the caller with the Environment's current state.

It does not modify the Environment.

The returned position represents the actual state at the time of observation.

### 4.2 `tryMove()`

The conceptual interface is:

```cpp
bool tryMove(
    Position state,
    Action action,
    Position& next_state
);
```

The function receives:

- A state from which to evaluate a movement.
- An action to evaluate.
- A reference to a `Position` object in which to return the candidate state.

The function calculates the hypothetical result of applying the action.

Its responsibilities include:

1. Initialize the candidate state from the supplied state.
2. Interpret the requested action.
3. Calculate the candidate position.
4. Check the GridWorld boundaries.
5. Check whether the candidate position is blocked by an obstacle.
6. Return whether the transition is valid.

The Boolean return value indicates validity.

The `next_state` reference carries the calculated candidate position.

These are two separate pieces of information.

A candidate position can be calculated even when the movement is invalid.

### 4.3 `act()`

The purpose of `act()` is different.

It performs an actual environment transition.

Conceptually, the operation:

1. Starts from the Environment's real position.
2. Calculates the requested movement.
3. Validates the candidate.
4. Commits the new position when the transition is accepted.

The important distinction is that `act()` changes the Environment's state, whereas `tryMove()` does not.

## 5. Simulation Versus Execution

The central distinction can be summarized as follows.

| Operation | Input state | Calculates candidate | Changes actual state |
|---|---|---|---|
| `observe()` | Environment state | No | No |
| `tryMove()` | Supplied hypothetical state | Yes | No |
| `act()` | Environment state | Yes | Yes, when accepted |

The conceptual flow is:

```text
                 Environment
                      |
                      |
                  observe()
                      |
                      v
                Actual State
                      |
                      |
               tryMove(state,
                 action, result)
                      |
                      v
               Candidate State
                      |
                      v
              Validity Result
                      |
                      v
             No state committed
```

By contrast, actual execution follows:

```text
             Environment State
                      |
                      v
                 act(action)
                      |
                      v
              Calculate Candidate
                      |
                      v
                  Validate
                      |
                      v
                Commit State
                      |
                      v
              Updated Environment
```

The distinction is fundamental:

**Simulation evaluates a possibility. Execution changes the actual environment.**

## 6. Runtime Experiment

The implementation was tested after the existing reactive agent loop had reached its goal.

The recorded output was:

```text
Position: (1, 0)
Position: (2, 0)
Position: (2, 1)
Position: (2, 2)

Goal Reached!

Real position before tests: (2, 2)

MoveRight valid: 0

Hypothetical result: (2, 3)

Real position after test: (2, 2)
```

The actual Environment position before the test was:

```text
(2, 2)
```

The experiment then evaluated a hypothetical `MoveRight` action.

The calculated candidate was:

```text
(2, 3)
```

However, the vertical coordinate `3` lies outside the valid 3×3 grid.

Therefore, the transition was invalid.

The Boolean result was printed as `0`, representing `false`.

Most importantly, the actual Environment position remained:

```text
(2, 2)
```

The hypothetical calculation did not commit the candidate position.

## 7. What the Experiment Demonstrated

The experiment established several important points.

### 7.1 Hypothetical Evaluation Does Not Require State Mutation

The Environment can evaluate a possible movement without physically moving the agent.

This is useful when multiple possible actions need to be considered from the same state.

### 7.2 Candidate State and Validity Are Independent

The candidate result `(2, 3)` was calculated even though the transition was invalid.

Therefore:

- The candidate position describes the result of applying the requested movement.
- The Boolean result describes whether that movement is permitted.

A caller must check validity before treating the candidate as an executable transition.

### 7.3 The Actual Environment State Remains Authoritative

The real position before and after the hypothetical test remained `(2, 2)`.

This demonstrates why simulation and execution need separate semantics.

## 8. Historical Implementation Qualification

This chapter preserves the Environment Simulation milestone recovered from Git commit:

```text
50b614c
```

The archived implementation introduced `tryMove()` alongside `act()`, but it also contained an action-semantics inconsistency.

That inconsistency was identified during the subsequent development of planning and action execution.
It was later corrected in the implementation history, culminating in the changes recorded in commit:

```text
70c77f3
```

The Chapter 02 source is intentionally preserved as a historical snapshot.

It should therefore be understood as an intermediate engineering milestone, not as the final corrected Environment implementation.

The repository preserves this distinction so that the development history remains visible and the earlier implementation can be examined in its original context.

## 9. Why This Design Matters for Search

The separation between `tryMove()` and `act()` provides an important foundation for search algorithms.

Consider a search process evaluating several possible actions from one state:

```text
                  Current State
                       |
          +------------+------------+
          |            |            |
          v            v            v
      MoveRight     MoveDown      MoveLeft
          |            |            |
          v            v            v
      Candidate A  Candidate B  Candidate C
          |            |            |
          +------------+------------+
                       |
                Evaluate Results
```

The search process can examine candidate states without repeatedly changing the Environment's actual position.

This allows planning to reason about possible transitions separately from physical execution.

At this stage, however, this chapter introduces the simulation interface and its underlying concepts. It does not yet implement the search algorithm itself.

## 10. Verification and Platform Qualification

The chapter is preserved as a historical implementation milestone.

The repository's source verification was performed in a Linux environment using GNU G++ with C++17.

Native Windows compilation and execution have not been verified.

The historical action-semantics inconsistency should also be kept in mind when interpreting the archived source.
The later corrected implementation is preserved in the repository's subsequent development history.

## 11. Chapter Outcome

Chapter 02 establishes a more explicit Environment interaction model:

- `observe()` retrieves the actual state.
- `tryMove()` calculates a hypothetical candidate and reports validity.
- `act()` performs an actual state transition.

The most important distinction is:

**A simulated transition is not an executed transition.**

This separation provides the foundation for evaluating possible actions without mutating the real environment,
which is essential for the planning and search implementation introduced in the next chapter.

The chapter is complete as a historical implementation milestone.
