# Chapter 01 — Reactive Agent

## 1. Overview

This chapter introduces a basic reactive agent operating in a small, discrete GridWorld environment.

The implementation establishes the fundamental interaction cycle between an agent and its environment:

**Observe → Decide → Act**

The agent observes its current position, selects an action according to its decision logic, and asks the environment to execute that action.
The environment updates its state, after which the agent can observe the new position.

This is the first GridWorld implementation milestone in the repository. It provides the foundation for the more structured environment,
planning, and execution models introduced in subsequent chapters.

## 2. Learning Objectives

The implementation introduces the following concepts:

- The basic agent–environment interaction model.
- Representation of an agent's position in a two-dimensional grid.
- Representation of movement actions.
- Observation of the current environment state.
- Decision-making based on the current observation.
- Execution of an action through the environment.
- Repeated interaction until the goal is reached.

The primary objective is to understand the basic reactive interaction cycle before introducing explicit planning and search algorithms.

## 3. Environment Configuration

The initial GridWorld is a **3×3 grid**.

The environment contains:

- Starting position: `(0, 0)`
- Goal position: `(2, 2)`
- Obstacle position: `(1, 1)`

The coordinate representation uses two values:

- `horizontal`
- `vertical`

The environment is therefore represented as a two-dimensional state space rather than a single integer position.

The obstacle occupies the central cell and the goal is located at the bottom-right position.

## 4. Basic Architecture

The implementation establishes two principal responsibilities.

### 4.1 Agent

The Agent represents the decision-making entity.

Its responsibility is to:

- Receive information about the current state.
- Determine which action to take.
- Direct its behaviour toward the goal.

The agent's decision logic is separate from the environment's state representation.

### 4.2 Environment

The Environment represents the world in which the agent operates.

It maintains the agent's position and provides the interaction through which the agent observes the current state and executes an action.

The environment is responsible for maintaining the actual position rather than allowing the agent to directly replace the environment's state.

### 4.3 Interaction Model

The basic interaction can be represented as:

```text
       Environment
            |
            | observe()
            v
          Agent
            |
            | decide()
            v
          Action
            |
            | act()
            v
       Environment
            |
            | updated state
            v
          Agent
```

The interaction repeats until the goal is reached.

## 5. Reactive Agent Loop

The central behaviour of the implementation follows this sequence:

1. Observe the current position.
2. Check whether the goal has been reached.
3. If the goal has not been reached, select an action.
4. Send the selected action to the environment.
5. Observe the resulting position.
6. Repeat the process.

The important characteristic is that the agent makes its decision using the current observation.

There is no separate path-search phase in this implementation. The agent does not first calculate a complete route and then execute that route.

Its behaviour is driven by the repeated observation and decision cycle.

## 6. Execution Result

The recorded execution produced the following sequence of positions:

```text
Position: (1, 0)
Position: (2, 0)
Position: (2, 1)
Position: (2, 2)
Goal Reached!
```

The agent successfully reached the goal at `(2, 2)`.

The observed movement sequence was:

```text
(0, 0)
   |
   v
(1, 0)
   |
   v
(2, 0)
   |
   v
(2, 1)
   |
   v
(2, 2)  Goal
```

The route avoids the obstacle at `(1, 1)`.

## 7. What This Implementation Demonstrates

This milestone establishes several foundational ideas.

### 7.1 Observation and Decision Are Distinct

The agent obtains its current state through observation and uses that information to make a decision.

Observation provides information about the environment. Decision-making determines what the agent intends to do with that information.

### 7.2 The Environment Maintains Its Own State

The agent does not need to directly modify its position.

Instead, it requests an action through the environment. The environment performs the state transition.

This separation provides a foundation for more controlled environment interactions.

### 7.3 Actions Change the Environment

An action is not merely a decision or a description of an intended movement. It is an instruction submitted to the environment for execution.

The resulting state is obtained through a subsequent observation.

### 7.4 Reactive Behaviour Is Repeated Behaviour

The implementation does not treat reaching the goal as a one-time calculation.

It repeatedly observes, decides, and acts until the goal condition is satisfied.

This is the basic closed interaction cycle that later implementations develop further.

## 8. Limitations

This implementation is intentionally small and foundational.

Its limitations include:

- It does not implement a general-purpose path-planning algorithm.
- It does not maintain a search frontier or visited-state collection.
- It does not reconstruct a path from predecessor states.
- It does not provide a separate planning component.
- It does not implement an execution-feedback mechanism that compares an expected next state against an observed next state.

The successful execution demonstrates the behaviour of this particular reactive implementation.
It should not be interpreted as evidence that the agent can find a valid route in arbitrary grid configurations.

## 9. Verification and Historical Context

This chapter preserves the historical Reactive Agent implementation recovered from Git commit:

```text
c849a9c
```

The recovered source represents the clean boundary of the initial GridWorld Reactive Agent milestone.

The recorded execution reached the goal and produced the position sequence documented above.

The repository's broader source verification was performed in a Linux environment using GNU G++ with C++17.
Native Windows compilation and execution have not been verified.

## 10. Chapter Outcome

Chapter 01 establishes the basic interaction between an agent and an environment.

The key model is:

**Observe → Decide → Act → Observe Again**

This provides the foundation for the subsequent introduction of more explicit environment behaviour, state-transition validation, planning, and execution feedback.

The chapter is complete as a historical implementation milestone.
