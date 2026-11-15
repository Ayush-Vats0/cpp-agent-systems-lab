# Architecture Overview

## 1. Purpose

This document describes the architectural evolution of the GridWorld agent system across Chapters 01–05.

The project begins with a simple goal-directed reactive agent and progressively introduces an explicit environment model, graph-based planning,
closed-loop execution, and cost-aware pathfinding.

The main architectural components are:

- **Agent** — represents the goal-directed decision-making component.
- **Environment** — represents the world in which the Agent operates and maintains the actual environment state.
- **Planner** — computes a route from a starting state to a goal using a search algorithm.

The separation between these responsibilities is central to the project's development.

This is a learning and experimentation codebase. The architecture should be understood in the context of the implementations in this repository,
not as a claim that the project is a complete production agent framework.

## 2. Architectural Evolution

GridWorld progression: Chapters 01–04 use the original 3×3 GridWorld. Chapter 05 extends the environment to 5×5 to support more meaningful weighted-terrain
and Dijkstra experiments. The larger grid is a Chapter 05 extension, not the environment used throughout the earlier chapters.

### Chapter 01 — Reactive Agent

The initial implementation establishes the basic idea of a goal-directed agent.

The Agent observes the current situation, evaluates its objective, and selects an action. The environment provides the conditions under which the action takes place.

The initial interaction can be represented as:

```text
OBSERVE
   |
   v
EVALUATE OBJECTIVE
   |
   v
SELECT ACTION
   |
   v
EXECUTE ACTION
   |
   v
CHECK PROGRESS
```

At this stage, the emphasis is on the basic relationship between an Agent, its objective, and the environment.

There is no separate path-planning component yet. The Agent's behavior is based on the immediate situation rather than a computed route through a search space.

### Chapter 02 — Environment Simulation

The next step makes the environment an explicit component of the implementation.

The project develops a two-dimensional GridWorld. Positions, movement, boundaries, and obstacles become concrete parts of the simulated world.

The Environment is responsible for maintaining its actual state and enforcing movement constraints.

Conceptually, the interaction becomes:

```text
             AGENT
               |
               | observes
               v
          ENVIRONMENT
               |
               | provides
               v
          CURRENT STATE
               |
               | Agent selects action
               v
          ENVIRONMENT
               |
               | validates and applies
               v
           NEW STATE
```

This separation establishes an important boundary:

- The Agent can request an action.
- The Environment determines whether that action is legal.
- The Environment determines the resulting state.

The Agent's intention does not, by itself, change the world.

### Chapter 03 — BFS Open-Loop Agent

Chapter 03 introduces a dedicated planning responsibility.

Instead of selecting only an immediate action, the system can search for a route from the starting position to the goal.

Breadth-First Search (BFS) is used to explore the GridWorld as a graph.

The planning process maintains the structures required for graph traversal and path reconstruction:

- A frontier of states awaiting exploration.
- A visited structure to avoid repeatedly exploring states.
- Parent information to reconstruct the route after reaching the goal.

The resulting architecture separates route computation from route execution:

```text
             AGENT
               |
               | supplies planning objective
               v
            PLANNER
               |
               | searches GridWorld
               v
             BFS
               |
               | reconstructs
               v
              PATH
               |
               | supplied for execution
               v
             AGENT
               |
               v
          ENVIRONMENT
```

The resulting behavior is open-loop:

```text
OBSERVE
   |
   v
PLAN COMPLETE ROUTE
   |
   v
EXECUTE ROUTE
   |
   v
COMPLETE
```

The route is computed before execution. The execution process therefore relies on the assumptions made during planning remaining sufficiently valid.

This is useful for demonstrating graph search and path reconstruction, but it does not yet provide the same feedback-driven recovery behavior as the
later closed-loop implementation.

### Chapter 04 — Closed-Loop Agent

Chapter 04 introduces a major architectural change: execution is no longer treated as an uninterrupted sequence based solely on the original plan.

The Agent executes one step, observes the Environment again, and evaluates whether the observed result matches the expected result.

The resulting control loop is:

```text
             GOAL
               |
               v
            OBSERVE
               |
               v
             PLAN
               |
               v
          SELECT ACTION
               |
               v
           EXECUTE STEP
               |
               v
            OBSERVE
               |
               v
         VERIFY EXPECTED
         AGAINST OBSERVED
               |
          +----+----+
          |         |
        MATCH     MISMATCH
          |         |
          v         v
       CONTINUE   REPLAN
          |         |
          +----<----+
```

The Agent now has to reason about two different things:

1. **Expected state** — the state anticipated after executing an action.
2. **Observed state** — the state actually reported by the Environment after execution.

A mismatch indicates that the observed result differs from the expected result. It is information that the current execution assumptions may no longer be valid.

The system can then use the actual observed position as the starting point for a new planning request.

The chapter also distinguishes planning outcomes. A successful plan can be executed, an already-reached goal requires no further movement,
and a failed planning request must not be treated as a valid route.

The implementation and its experiments examine conditions such as:

- The Agent already being at the goal.
- Planner failure.
- Execution mismatch.
- Fault injection and replanning.
- Recovery through a new planning request.

This is the key architectural transition from planning a route to controlling execution through observation and feedback.

### Chapter 05 — Planning with Dijkstra

Chapter 05 extends the planning capability by introducing cost-aware pathfinding.

BFS remains available, but Dijkstra's algorithm is integrated into the existing Planner.

The Planner now uses a minimum-priority queue, a distance table, and a parent table to identify a minimum-cost route under the environment's
nonnegative transition-cost model.

The implementation includes:

- `QueueEntry`, containing a position and its associated cost.
- `CompareCost`, which provides minimum-cost priority ordering.
- A distance table for the best-known cost to each position.
- A parent table for path reconstruction.
- Cost relaxation when a cheaper route is discovered.
- Lazy priority-queue insertion.
- Stale-entry detection.
- Goal detection after stale-entry filtering.

The current implementation uses `std::priority_queue` with lazy insertion. It does not use an indexed binary heap or a decrease-key operation.

The GridWorld's destination-entry cost model is used in the weighted terrain comparison. The cost of entering a destination position is
considered when evaluating a transition.

This allows the project to demonstrate why the route with the fewest movements is not necessarily the route with the lowest total cost.

The stale-entry experiment uses a separate transition-dependent cost model to demonstrate how a previously queued,
more expensive entry can become outdated after a cheaper route is discovered. This distinction matters because the standard destination-entry cost model does not,
by itself, produce that same stale-entry scenario.

The Planner's responsibility remains route computation. The Agent and Environment retain their separate roles in the broader execution architecture.

## 3. Component Responsibilities

### 3.1 Agent

The Agent represents the goal-directed decision-making component.

Across the chapters, its responsibilities develop from immediate action selection into coordinating planning and execution.

Its responsibilities include:

- Maintaining the objective.
- Evaluating whether the objective has been reached.
- Working with the current observed state.
- Requesting a plan when planning is required.
- Using the planning result to guide action.
- Comparing expected and observed outcomes in the closed-loop implementation.
- Continuing or requesting replanning based on the result.

The Agent does not independently determine whether an environment action is legal or what state change actually occurred.

### 3.2 Environment

The Environment represents the simulated world and maintains its actual state.

Its responsibilities include:

- Maintaining the current GridWorld state.
- Providing observations.
- Representing the grid and its movement constraints.
- Enforcing boundary and obstacle rules.
- Applying valid actions.
- Providing transition-cost information for cost-aware planning.

The Environment is the authority on the actual simulated state.

A route calculated by the Planner is a prediction of a possible sequence of states. It is not proof that those states have already been reached.

### 3.3 Planner

The Planner is responsible for computing a route from a starting position to a goal.

Its responsibilities include:

- Exploring reachable states.
- Applying the selected search algorithm.
- Tracking the information required for search.
- Determining whether a goal can be reached.
- Reconstructing a route when a solution is found.

The project develops this responsibility through BFS and subsequently Dijkstra.

BFS is retained as a separate planning option. Dijkstra adds cost-aware route selection.

The Planner does not replace the Environment as the authority on actual state, nor does route computation alone guarantee successful execution.

## 4. Overall Architecture

The architecture can be summarized as three cooperating components:

```text
                  +----------------------+
                  |         AGENT        |
                  |                      |
                  |  Objective           |
                  |  Decision-making     |
                  |  Execution control   |
                  |  Outcome evaluation  |
                  +----------+-----------+
                             |
                 planning request / result
                             |
                             v
                  +----------------------+
                  |        PLANNER       |
                  |                      |
                  |  BFS                 |
                  |  Dijkstra            |
                  |  Search data         |
                  |  Path reconstruction |
                  +----------+-----------+
                             |
                    grid / cost information
                             |
                             v
                  +----------------------+
                  |      ENVIRONMENT     |
                  |                      |
                  |  Actual state        |
                  |  Observation         |
                  |  Movement validation |
                  |  State transitions   |
                  |  Transition costs    |
                  +----------------------+
```

This diagram represents the conceptual responsibility boundaries developed by the project.
It is not intended to imply that every interaction shown is implemented as a separate interface or API.

The important distinction is that the components have different responsibilities:

| Component | Primary responsibility |
|---|---|
| Agent | Objective-directed decisions and execution control |
| Planner | Route computation and path reconstruction |
| Environment | Actual world state, observations, and action enforcement |

## 5. Open-Loop and Closed-Loop Execution

The difference between open-loop and closed-loop execution is fundamental to the project's architecture.

| Aspect | Open-loop | Closed-loop |
|---|---|---|
| Planning | Complete route planned before execution | Route planned, then execution is monitored |
| Execution | Route executed as a sequence | Actions executed step by step |
| Observation | Primarily used before planning | Repeated during execution |
| Verification | No equivalent continuous feedback loop | Expected and observed outcomes compared |
| Mismatch handling | Original plan may no longer be valid | Mismatch can trigger replanning |
| Recovery | Not the focus of the open-loop implementation | Explicitly explored through experiments |

A plan should be treated as conditional on the assumptions used to calculate it.

The closed-loop implementation makes this practical: execution provides evidence about whether the assumptions remain valid.

## 6. Architectural Boundaries and Design Observations

The development of these chapters establishes several important boundaries.

**Planning is not execution.** A computed route describes intended movement. It does not establish that the movement has occurred.

**The Agent does not own the Environment's actual state.** It acts based on observations and must evaluate the results of its actions.

**The Environment enforces its own constraints.** An action request does not override boundaries, obstacles, or movement rules.

**A planning failure is not a valid route.** The Agent must distinguish a successful planning result from an already-reached goal or a failure to find a route.

**A plan can become invalid.** Closed-loop execution allows the Agent to detect divergence and request a new plan from the observed state.

**The search algorithm and the broader control loop are different concerns.** BFS and Dijkstra determine routes using different optimization criteria.
The Agent's responsibility to coordinate execution and evaluate outcomes remains distinct from the algorithm used to compute the route.

## 7. Scope and Limitations

The architecture documented here is deliberately limited to the implemented GridWorld system.

It demonstrates:

- Goal-directed reactive behavior.
- Explicit environment simulation.
- Graph-based route planning.
- Open-loop and closed-loop execution.
- Expected-versus-observed state comparison.
- Replanning after execution mismatch.
- Unweighted and cost-aware search.
- Controlled experiments involving planning and execution edge cases.

It does not claim to provide:

- A general-purpose production agent runtime.
- A distributed execution architecture.
- LLM integration or tool orchestration.
- A generalized plugin or policy framework.
- Formal verification of the complete system.
- A production-grade interface or deployment model.

These boundaries are important when evaluating what the repository actually demonstrates.

## 8. Conclusion

The project's architectural progression is:

```text
REACTIVE AGENT
      |
      v
EXPLICIT ENVIRONMENT
      |
      v
GRAPH-BASED PLANNING
      |
      v
CLOSED-LOOP EXECUTION
      |
      v
COST-AWARE PLANNING
```

The main result is not simply the addition of more search algorithms.

It is the progressive separation of decision-making, world-state management, route computation, and execution feedback.

That separation provides a foundation for understanding how an autonomous system can plan, act, observe the consequences, detect divergence, and revise its behavior.
