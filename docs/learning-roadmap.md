# Learning Roadmap

## C++ Agent Systems: Foundations & Experiments

This repository documents five chapters of an engineering-focused learning journey into autonomous agents, environment interaction, planning,
search algorithms, and closed-loop execution.

The implementation language is **C++17**.

The objective is to develop a practical understanding of how agents perceive an environment, make decisions, plan actions, execute them,
and respond when actual execution differs from expectations.

This is an engineering-oriented learning project. It emphasizes implementation, experimentation, debugging, architectural reasoning,
and verification rather than treating algorithms as isolated theoretical exercises.

---

## 1. Training Objectives

The foundational stage focuses on developing the following capabilities:

- Understanding the relationship between an agent and its environment.
- Designing state representations and action interfaces.
- Implementing reactive and goal-directed behavior.
- Distinguishing reactive decision-making from explicit planning.
- Understanding graph traversal and shortest-path algorithms.
- Implementing closed-loop execution and replanning.
- Handling execution failures and unexpected state changes.
- Evaluating algorithmic behavior through controlled experiments.
- Applying C++17 and basic software engineering practices to agent development.

The GridWorld environment provides a controlled setting in which these concepts can be implemented and examined.

## 2. Curriculum

### Chapter 01 — Reactive Agent

**Status:** Completed

Introduces the fundamental agent–environment interaction model.

Topics:
- Agent and environment responsibilities.
- Observation and action.
- Reactive decision-making.
- Goal representation and goal evaluation.
- Basic agent execution loop.

**Learning outcome:** Understand how an agent observes its environment, selects an action, and influences the environment.

### Chapter 02 — Environment Simulation

**Status:** Completed

Develops the environment as an explicit software component rather than treating it as a passive representation of state.

Topics:
- Environment state.
- Position representation.
- Actions and state transitions.
- Movement validation.
- Obstacles and boundaries.
- Observation and action interfaces.

**Learning outcome:** Implement a controlled environment with explicit state-transition rules and a defined interaction interface.

### Chapter 03 — BFS Open-Loop Agent

**Status:** Completed

Introduces explicit path planning using Breadth-First Search (BFS).

Topics:
- Grid representation as a graph.
- Search frontier and visited states.
- Parent tracking.
- Path reconstruction.
- Converting planned paths into actions.
- Open-loop execution.

**Learning outcome:** Implement an agent that computes a path toward a goal before executing the planned sequence.

### Chapter 04 — Closed-Loop Agent

**Status:** Completed

Extends the planning architecture with observation-driven execution and recovery.

Topics:
- Closed-loop execution.
- Predicted versus observed state.
- Execution mismatch detection.
- Replanning.
- Planner failure handling.
- Already-at-goal handling.
- Fault injection and recovery-path validation.

**Experiments:**
- Mismatch handling.
- Planner failure.
- Replanning when already at the goal.
- Closed-loop recovery through fault injection.
- Fault-injection-based replanning architecture validation.

**Learning outcome:** Understand why a planned action sequence cannot be assumed to execute exactly as expected,
and implement a system that detects discrepancies and responds through replanning.

### Chapter 05 — Planning and Search Algorithms: Foundations to Dijkstra

**Status:** Experiments completed; documentation and publication preparation in progress.

Develops cost-aware path planning and examines the behavior of Dijkstra's shortest-path algorithm.

Topics:
- Weighted terrain.
- BFS versus Dijkstra.
- Priority queues.
- Distance tracking.
- Parent-based path reconstruction.
- Relaxation.
- Stale priority queue entries.
- Zero-cost transitions and cycles.
- Unreachable goals.
- Algorithmic correctness and complexity.

**Experiments:**
- Standalone Dijkstra execution on a new grid.
- Weighted-terrain BFS and Dijkstra comparison.
- Stale priority queue entry handling.
- Zero-cost transitions and cycle behavior.
- Unreachable-goal handling.

**Learning outcome:** Understand the difference between minimum-hop and minimum-cost path planning,
implement Dijkstra, and investigate important edge cases affecting its correctness and termination.

---

## 3. Future Extensions

The planning and search study can be extended with additional algorithmic and data-structure work:

- Indexed binary heap and indexed-value/decrease-key operations.
- Manhattan distance as a heuristic.
- A* search algorithm.

These topics are planned extensions and are not represented as completed features of the current implementation.

## 4. Engineering Approach

The training follows an implementation-driven approach:

1. Establish a concept or engineering requirement.
2. Design the relevant component or behavior.
3. Implement it in C++17.
4. Compile and execute the implementation.
5. Examine observed behavior.
6. Introduce controlled experiments where necessary.
7. Investigate failures and edge cases.
8. Record the results and engineering conclusions.

The experiments are retained as evidence of the learning and validation process.

## 5. Repository Scope

This repository contains:

- Chapters 01–05 source implementations.
- Chapter 04 closed-loop execution experiments.
- Chapter 05 Dijkstra experiments.
- Chapter-specific technical documentation.
- Overall architecture and engineering principles.
- Build and execution instructions.

It is not intended to be a complete production agent framework.
Its purpose is to demonstrate the engineering foundations and experiments developed during these five chapters.

---

**Language:** C++17
**Primary environment:** GridWorld
**Compiler:** GNU G++
**Compilation standard:** `-std=c++17 -Wall -Wextra -pedantic -Iinclude`
