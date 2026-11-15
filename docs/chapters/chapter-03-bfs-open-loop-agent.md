# Chapter 03 — BFS Open-Loop Agent

## 1. Overview

Chapter 03 introduces search-based planning into the GridWorld agent.

The earlier chapters established a reactive agent and an environment capable of validating hypothetical transitions and executing actual actions.
This chapter builds on that foundation by introducing **Breadth-First Search (BFS)** as a systematic method for finding a route from a starting state to a goal.

The implementation evolves from selecting individual actions reactively to computing a complete route before execution.

The resulting system follows this sequence:

**Observe → Plan → Reconstruct Path → Convert Path to Actions → Execute**

This is the project's first complete search-based, planner-driven, open-loop agent.

## 2. Learning Objectives

The primary objectives of this chapter are:

- Understand state-space search in a constrained environment.
- Apply Breadth-First Search to find a route through an unweighted graph.
- Understand the role of a frontier and visited-state tracking.
- Prevent repeated exploration of previously discovered states.
- Record predecessor relationships for path reconstruction.
- Distinguish the search process from the final solution path.
- Separate planning responsibilities from agent and environment responsibilities.
- Convert a state path into an executable action sequence.
- Understand the characteristics and limitations of open-loop execution.
- Distinguish route length, route cost and computational complexity.

## 3. Environment Configuration

The implementation continues using the **3×3 GridWorld** established in the earlier chapters.

| Property | Configuration |
|---|---|
| Grid dimensions | 3×3 |
| Valid horizontal coordinates | 0–2 |
| Valid vertical coordinates | 0–2 |
| Starting position | (0,0) |
| Goal position | (2,2) |
| Obstacle | (1,1) |
| Movement model | Four-directional |
| Movement cost | 1 per transition |

The environment is represented as:

```text
(0,0)       (1,0)       (2,0)
 Start

(0,1)       (1,1)       (2,1)
             Obstacle

(0,2)       (1,2)       (2,2)
                         Goal
```

The obstacle at `(1,1)` prevents direct movement through the centre of the grid.

The environment continues to enforce its movement constraints. BFS uses the environment's transition-validation mechanism when examining possible successor states.

## 4. Why Breadth-First Search?

The GridWorld can be modelled as a graph:

- Each valid position represents a state or vertex.
- Each legal movement represents an edge or transition.
- The starting position is the initial state.
- The destination is the goal state.

BFS explores this graph in layers, using a First-In, First-Out (FIFO) frontier.

It is suitable for this environment because every movement has the same cost.

Under this condition, BFS finds a route with the minimum number of transitions, provided the graph is explored correctly.

BFS does not guarantee the minimum total cost when transitions have different weights.

## 5. BFS Search Process

The implemented search follows these steps:

1. Insert the starting state into the frontier.
2. Remove the next state from the front of the frontier.
3. Examine its valid neighboring states.
4. Ignore states that have already been discovered.
5. Mark each newly discovered state as visited before inserting it into the frontier.
6. Record the predecessor of each newly discovered state.
7. Continue until the goal is discovered or the reachable frontier is exhausted.
8. Reconstruct the route using the recorded predecessor relationships.
9. Reverse the reconstructed route to obtain start-to-goal order.

### 5.1 Frontier

The frontier contains discovered states waiting to be explored.

Because BFS uses FIFO ordering, states are processed in the order in which they enter the frontier.

The frontier does not store complete paths or branches. It stores states awaiting exploration.

### 5.2 Visited-State Tracking

The environment contains reversible movements. Without visited-state tracking, the search could repeatedly revisit states.

For example:

```text
(0,0) → (1,0) → (0,0) → (1,0) → ...
```

The implementation marks a state as visited when it is first discovered, before adding it to the frontier.

This prevents duplicate discovery and unnecessary repeated exploration.

### 5.3 Parent Relationships

When a state is discovered, the search records the state from which it was reached.

For the verified route, the relationships are:

```text
(1,0) → (0,0)
(2,0) → (1,0)
(2,1) → (2,0)
(2,2) → (2,1)
```

These relationships form the predecessor chain needed to reconstruct the route.

## 6. Search Exploration Versus the Final Route

An important distinction established during this chapter is:

**BFS exploration is not the same as the final solution path.**

The search explores alternative reachable states while determining a route.

The recorded exploration sequence was:

```text
(0,0)
(1,0)
(0,1)
(2,0)
(0,2)
(2,1)
```

These are states explored during the search, not the final route.

The final route is:

```text
(0,0)
    ↓
(1,0)
    ↓
(2,0)
    ↓
(2,1)
    ↓
(2,2)
```

The search process may examine states that do not appear in the final solution.

This distinction is important when analysing search algorithms: the explored state space and the returned solution are different things.

## 7. Path Reconstruction

After the goal is discovered, the predecessor relationships provide the information required to reconstruct the route.

Starting at the goal, the implementation follows the parent relationship backward:

```text
(2,2)
  ↓
(2,1)
  ↓
(2,0)
  ↓
(1,0)
  ↓
(0,0)
```

This produces the route in goal-to-start order.

The sequence is then reversed to obtain the required start-to-goal route:

```text
(0,0)
(1,0)
(2,0)
(2,1)
(2,2)
```

The parent relationships already contain the reconstruction information. A second graph search is not required.

## 8. Planner Abstraction

Initially, the BFS logic was located inside `main()`.

It was subsequently extracted into a dedicated `Planner` abstraction.

The Planner owns:

- BFS search.
- Frontier management.
- Visited-state tracking.
- Parent relationships.
- Goal detection.
- Path reconstruction.
- Conversion of a position path into actions.

This separates the responsibilities of the major system components.

| Component | Responsibility |
|---|---|
| Environment | Owns actual state, environmental constraints, observation, hypothetical transition validation and action execution. |
| Agent | Owns the objective and participates in executing the resulting plan. |
| Planner | Owns the search process, route reconstruction and path-to-action conversion. |
| Main orchestration | Coordinates the overall program flow. |

The architectural distinction is:

**Agent ≠ Planner ≠ Environment**

The Agent has a planning capability, but the search algorithm does not need to reside inside the Agent itself.

The Planner provides a separate responsibility that can be used by the Agent.

## 9. Path-to-Action Conversion

The Planner produces a sequence of positions.

The Agent requires an action sequence that can be executed by the Environment.

The `pathToActions()` operation performs this conversion by examining consecutive positions in the reconstructed route.

The verified route:

```text
(0,0)
(1,0)
(2,0)
(2,1)
(2,2)
```

becomes:

```text
MoveRight
MoveRight
MoveDown
MoveDown
```

Each action represents the transition between two consecutive states:

```text
path[i] ── action[i] ──→ path[i+1]
```

The route contains five states and four transitions. Therefore, the resulting action sequence contains four actions.

## 10. Open-Loop Planning and Execution

Chapter 03 establishes an open-loop execution model.

The overall process is:

```text
Environment
     ↓
Observe starting state
     ↓
Planner
     ↓
BFS search
     ↓
Position path
     ↓
Path-to-action conversion
     ↓
Complete action sequence
     ↓
Agent
     ↓
Execute planned actions
     ↓
Environment
```

The complete route is calculated before execution begins.

The Agent then executes the planned action sequence without performing a fresh observation-and-evaluation cycle after every individual action.

This means execution relies on the assumption that the environment continues to behave as expected.

The distinction is:

**Open-loop:**

```text
Observe → Plan → Execute complete plan
```

**Closed-loop:**

```text
Plan → Act → Observe → Evaluate
                   ↓
             Continue or Replan
```

The open-loop implementation is a meaningful planning milestone, but it does not yet provide continuous feedback-based execution verification.

## 11. Verified Results

### 11.1 Final Route

The verified route from the starting position to the goal is:

```text
(0,0) → (1,0) → (2,0) → (2,1) → (2,2)
```

### 11.2 Action Sequence

```text
MoveRight
MoveRight
MoveDown
MoveDown
```

### 11.3 Route Metrics

| Metric | Result |
|---|---:|
| States in the route | 5 |
| Number of transitions | 4 |
| Cost per transition | 1 |
| Total route cost | 4 |

The total route cost is:

\[
4 \times 1 = 4
\]

### 11.4 Verification History

The completed BFS/open-loop implementation was recovered from verified Git commit:

`70c77f3`

The implementation compiled and executed successfully.

Chapter 04 was subsequently created by copying this known-good Chapter 03 implementation and compiling it unchanged. Its initial output matched the Chapter 03 baseline.

This preserved the open-loop implementation as a distinct engineering milestone.

## 12. Route Cost Versus Computational Complexity

This chapter also establishes an important distinction between three measurements.

| Concept | Meaning | GridWorld result |
|---|---|---:|
| Route length | Number of transitions in the selected route | 4 |
| Route cost | Sum of the transition costs | 4 |
| Computational complexity | How the algorithm's work scales with input size | Analysed separately |

For a graph represented using adjacency lists, BFS has the standard time complexity:

\[
O(V+E)
\]

where:

- \(V\) is the number of vertices or states.
- \(E\) is the number of edges or transitions.

Its usual auxiliary space complexity is:

\[
O(V)
\]

for the frontier, visited-state tracking and parent information, excluding graph storage.

In a four-directional grid, each state has at most four neighbors. Consequently, the number of edges grows proportionally to the number of states:

\[
E=O(V)
\]

Therefore, BFS on this grid has linear time growth with respect to the number of states:

\[
O(V)
\]

For a square grid with side length \(L\), the number of states is \(V=L^2\), so the corresponding expression is:

\[
O(L^2)
\]

These expressions describe algorithmic growth, not the cost of the selected route.

## 13. Limitations

The implementation demonstrates a successful BFS-based open-loop agent, but several limitations remain.

1. **Equal transition costs:** BFS minimizes the number of transitions. It does not necessarily minimize total cost when transitions have different weights.
2. **Precomputed execution:** The complete route is calculated before execution.
3. **No continuous execution verification:** The Agent does not compare the observed environment state with the expected state after every individual action.
4. **No feedback-driven replanning:** An unexpected execution result does not trigger the closed-loop recovery process in this implementation.
5. **Environment assumptions:** The planned route depends on the environment remaining sufficiently consistent with the conditions used during planning.

These limitations define the boundary of the open-loop implementation.

## 14. Chapter Outcome

Chapter 03 completed the transition from reactive movement to search-based planning.

The implementation established:

- BFS-based route discovery.
- FIFO frontier-based exploration.
- Visited-state tracking.
- Parent-based path reconstruction.
- Separation of explored states from the final solution path.
- A dedicated Planner abstraction.
- Conversion of state paths into executable actions.
- Complete open-loop plan execution.
- A verified route through the 3×3 GridWorld.

The resulting architecture provides a reusable planning foundation while making the limitations of open-loop execution explicit.

**Chapter 03 establishes that an agent can calculate a complete route before acting. It also establishes why calculating a route and reliably executing it**
**are separate engineering concerns.**
