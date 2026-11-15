# C++ Agent Systems: Foundations & Experiments

A C++17 engineering project exploring the foundations of autonomous agent systems through a progressively developed GridWorld environment.

The repository follows the development of an agent from basic reactive behaviour to explicit path planning, closed-loop execution, recovery,
and cost-aware shortest-path search.

The emphasis is on implementation, architectural separation, controlled experimentation, debugging, and verification.

## Project Overview

The project uses a two-dimensional GridWorld to examine how an agent interacts with an environment, makes decisions, plans routes, executes actions,
and responds when actual execution differs from expectations.

The architecture is progressively developed around three principal components:

| Component | Responsibility |
|---|---|
| **Agent** | Represents the goal-directed decision-making and execution component. |
| **Environment** | Maintains the actual world state and enforces movement constraints and transition rules. |
| **Planner** | Computes routes using search algorithms and reconstructs paths. |

The project demonstrates the importance of separating decision-making, environment-state management, planning, and execution feedback.

## Chapter Progression

### Chapter 01 — Reactive Agent

Introduces the fundamental agent–environment interaction model.

Key concepts:
- Goal-directed behaviour.
- Observation and action.
- Reactive decision-making.
- Agent and environment responsibilities.
- Basic execution loop.

### Chapter 02 — Environment Simulation

Develops an explicit two-dimensional environment with defined state and movement rules.

Key concepts:
- Grid and position representation.
- Environment state.
- Movement validation.
- Obstacles and boundaries.
- Observation and action interfaces.
- Hypothetical movement validation through `tryMove()`.

### Chapter 03 — BFS Open-Loop Agent

Introduces graph-based path planning using Breadth-First Search (BFS).

Key concepts:
- Grid representation as a graph.
- Search frontier and visited-state tracking.
- Parent relationships.
- Path reconstruction.
- Path-to-action conversion.
- Open-loop execution.

### Chapter 04 — Closed-Loop Agent

Extends the system with observation-driven execution and recovery.

Key concepts:
- One-action-at-a-time execution.
- Expected versus observed state.
- Execution mismatch detection.
- Plan invalidation.
- Replanning from the actual environment state.
- Explicit planning outcomes.
- Recovery and failure handling.

Experiments:
- Already-at-goal handling.
- Closed-loop recovery.
- Fault-injection replanning.
- Mismatch handling.
- Planner failure handling.

### Chapter 05 — Dijkstra for GridWorld

Introduces cost-aware path planning using Dijkstra's shortest-path algorithm.

Key concepts:
- Weighted terrain.
- BFS versus Dijkstra.
- Minimum-priority queues.
- Best-known distance tracking.
- Cost relaxation.
- Parent-based path reconstruction.
- Lazy priority-queue insertion.
- Stale-entry filtering.
- Zero-cost transitions and cycles.
- Unreachable-goal handling.

The Chapter 05 environment expands from the earlier 3×3 GridWorld to a 5×5 grid for more meaningful weighted-terrain experiments.

Experiments:
- Weighted-terrain BFS and Dijkstra comparison.
- Stale priority-queue entry handling.
- Standalone Dijkstra execution on an expanded grid.
- Unreachable-goal handling.
- Zero-cost transition and cycle handling.

## Repository Structure

```text
.
├── chapters/
│   ├── 01-reactive-agent/
│   │   └── main.cpp
│   │
│   ├── 02-environment-simulation/
│   │   └── main.cpp
│   │
│   ├── 03-bfs-open-loop-agent/
│   │   └── main.cpp
│   │
│   ├── 04-closed-loop-agent/
│   │   ├── main.cpp
│   │   └── experiments/
│   │       ├── already-at-goal/
│   │       ├── closed-loop-recovery/
│   │       ├── fault-injection-replanning/
│   │       ├── mismatch-handling/
│   │       └── planner-failure/
│   │
│   └── 05-dijkstra/
│       ├── main.cpp
│       └── experiments/
│           ├── stale-priority-queue/
│           ├── standalone-new-grid/
│           ├── unreachable-goal/
│           └── zero-cost-cycle/
│
├── docs/
│   ├── architecture-overview.md
│   ├── build-and-run.md
│   ├── engineering-principles.md
│   ├── learning-roadmap.md
│   └── chapters/
│       ├── chapter-01-reactive-agent.md
│       ├── chapter-02-environment-simulation.md
│       ├── chapter-03-bfs-open-loop-agent.md
│       ├── chapter-04-closed-loop-agent.md
│       └── chapter-05-dijkstra.md
│
├── include/
│   └── project_banner.hpp
├── .gitignore
└── README.md
```

The repository contains **14 independent C++ programs**. Each source file has its own `main()` function and is compiled separately.

## Requirements

- C++17-compatible compiler.
- GNU G++ recommended.
- Terminal or command-line environment.
- Visual Studio Code is optional.

The project uses standard C++ facilities and does not require external libraries, a database, or a network connection during program execution.

## Build and Run

Clone the repository:

```bash
git clone git@github.com:Ayush-Vats0/cpp-agent-systems-lab.git
cd cd cpp-agent-systems-lab
```

Compile an individual program from the repository root:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude \
chapters/01-reactive-agent/main.cpp \
-o chapters/01-reactive-agent/agent
```

Run it:

```bash
./chapters/01-reactive-agent/agent
```

The `-Iinclude` option makes the shared project header available to the compiler.

Each program can be compiled and executed independently. Do not compile all source files into a single executable because they contain separate entry points.

For complete instructions, including all 14 compilation commands, Windows and Linux environments, WSL setup, experiment execution, and troubleshooting, see:

**[Build and Run Guide](docs/build-and-run.md)**

## Shared Project Header

The repository provides a shared header:

`include/project_banner.hpp`

It defines `printProjectBanner()`, which displays the project identification and collaboration attribution at program startup.

All 14 programs include this header and invoke the function at the beginning of `main()`.

## Documentation

The `docs/` directory contains the technical documentation for the implemented system:

- **Architecture Overview** — architectural evolution and component responsibilities.
- **Engineering Principles** — design principles established through implementation and experimentation.
- **Learning Roadmap** — chapter progression, objectives, and future extensions.
- **Build and Run Guide** — compilation, execution, environment setup, and troubleshooting.
- **Chapter Documentation** — detailed implementation notes, experiments, results, and limitations for Chapters 01–05.

## Engineering Approach

The project follows an implementation-driven workflow:

1. Establish a concept or engineering requirement.
2. Design the relevant component or behaviour.
3. Implement it in C++17.
4. Compile and execute the implementation.
5. Examine observed behaviour.
6. Introduce controlled experiments where necessary.
7. Investigate failures and edge cases.
8. Record results and engineering conclusions.

Experiments are retained to make the implementation decisions and observed behaviour inspectable.

## Scope and Limitations

This repository demonstrates foundational agent-system concepts in a controlled GridWorld environment.

It is not a general-purpose production agent framework.

The current implementation does not claim to provide:
- A distributed execution architecture.
- LLM integration or tool orchestration.
- A generalized plugin or policy framework.
- Formal verification of the complete system.
- A production deployment model.

Potential future search extensions include indexed binary heaps, decrease-key operations, Manhattan-distance heuristics, and A* search.
These are planned extensions and are not represented as completed features.

## Verification

The repository's 14 programs were compiled and executed in a Linux environment using GNU G++ and the C++17 standard.

The verification covered the main chapter implementations and the Chapter 04 and Chapter 05 experiments.

The unreachable-goal experiment intentionally returns a non-zero exit status. This is an expected result documented in the corresponding chapter documentation,
not an unexpected runtime failure.

Native Windows compilation and execution have not been independently verified.

## Author and Collaboration

- **Author:** Ayush Vats
- **GitHub:** [Ayush-Vats0](https://github.com/Ayush-Vats0)
- **Contact:** 90127732+Ayush-Vats0@users.noreply.github.com

**AI assistance:** ChatGPT (GPT-6)

**Collaboration model:** Human-directed, AI-assisted engineering involving design discussions, technical guidance, implementation support, debugging, and review.

This is an independent project and is not an official OpenAI product, endorsement, or partnership.

---

- **Language:** C++17
- **Environment:** GridWorld
- **Primary compiler:** GNU G++
- **Implemented chapters:** 01–05
