# Chapter 05 — Dijkstra for GridWorld

## 1. Overview

Chapter 05 extends the existing GridWorld planning system from unweighted search to weighted shortest-path search.

The earlier chapters established a reactive agent, an environment simulation, BFS-based planning and closed-loop execution.
This chapter introduces Dijkstra's algorithm to address a limitation of BFS: BFS minimizes the number of transitions,
but it does not necessarily minimize total path cost when transitions have different costs.

Dijkstra's algorithm was integrated into the existing `Planner` while preserving the responsibilities of the `Agent` and `Environment`.

The chapter also includes controlled experiments covering weighted route selection, lazy priority-queue behaviour,
stale entries, zero-cost transitions and unreachable goals.

## 2. Learning Objectives

The objectives of this chapter were to:

- Understand why BFS is insufficient for general weighted pathfinding.
- Integrate Dijkstra's algorithm into the existing planning architecture.
- Use a minimum-priority queue to process states by accumulated path cost.
- Maintain best-known distances and predecessor relationships.
- Apply cost relaxation and reject outdated queue entries.
- Reconstruct a minimum-cost path.
- Verify Dijkstra's behaviour using deterministic experiments.
- Examine zero-cost transitions and unreachable goals.
- Preserve the distinction between planning and environment execution.

## 3. Environment Evolution

Chapters 01–04 used a 3×3 GridWorld.

Chapter 05 expanded the environment to 5×5 to provide sufficient space for controlled weighted-terrain experiments.

The 5×5 environment uses:

- Grid dimensions: 5×5.
- Valid horizontal coordinates: 0–4.
- Valid vertical coordinates: 0–4.
- Four directional movement actions.
- Obstacle validation.
- Destination-entry transition costs for the weighted-terrain experiment.

The Environment remains responsible for movement constraints and transition costs. The Planner consumes these rules when evaluating candidate routes.

Both `tryMove()` and `act()` enforce the same grid boundaries and obstacle restrictions.

## 4. Why Dijkstra Was Introduced

BFS explores states in increasing order of the number of transitions from the starting state.

This is appropriate when all transitions have equal cost.

When transition costs differ, the route with the fewest movements may not be the least expensive route.

The distinction is:

| Algorithm | Optimization objective |
|---|---|
| BFS | Minimum number of transitions in an unweighted graph |
| Dijkstra | Minimum accumulated path cost with non-negative edge costs |

Dijkstra therefore provides a different planning objective without requiring a replacement of the existing BFS implementation.

## 5. Dijkstra Implementation

Dijkstra was integrated into the existing `Planner`.

The implementation uses the following components:

### 5.1 Priority queue

A minimum-priority queue selects the candidate state with the lowest accumulated cost.

The implementation uses:

- `QueueEntry`, containing a position and its associated cost.
- `CompareCost`, which provides minimum-cost ordering.
- `std::priority_queue` with a custom comparator.

The implementation uses lazy insertion: when a better route to a state is discovered,
a new queue entry is inserted rather than searching for and updating an existing entry.

### 5.2 Distance table

The distance table records the best-known accumulated cost for each state.

It is initialized using an infinity sentinel:

`INF = std::numeric_limits<int>::max()`

The sentinel represents a state for which no route has yet been discovered. It is not an actual route cost.

### 5.3 Parent table

The parent table records the predecessor state used to reach a state through its currently best-known route.

It supports reconstruction of the final path after the goal has been reached.

The parent table stores predecessor positions, not queue entries or accumulated costs.

### 5.4 Cost relaxation

For a transition from state \(u\) to state \(v\), the candidate accumulated cost is:

\[
\text{candidate}=d(u)+w(u,v)
\]

where:

- \(d(u)\) is the best-known cost to reach \(u\).
- \(w(u,v)\) is the transition cost from \(u\) to \(v\).

The implementation updates the distance and predecessor only when the candidate is strictly better:

`candidate < distance[v]`

An improved candidate is also inserted into the priority queue.

### 5.5 Stale-entry filtering

Because the implementation uses lazy insertion, multiple entries for the same state may coexist in the priority queue.

The distance table remains authoritative. A queue entry is stale when its recorded cost no longer matches the best-known distance.

The implementation checks for stale entries before processing a state.

The goal is accepted only when it is removed from the priority queue as a valid minimum-cost entry.
Merely discovering the goal is not sufficient to establish that its route is optimal.

### 5.6 Path reconstruction and integration

After reaching the goal through a valid minimum-cost entry, the Planner reconstructs the route using the parent table.

The reconstructed path is then used by the existing planning and execution pipeline.

BFS remains available, and `Planner::plan()` delegates to Dijkstra in the integrated implementation.

## 6. Experiment 1 — Weighted Terrain: BFS versus Dijkstra

### Objective

Verify that BFS and Dijkstra can select different routes when the environment assigns different costs to entering destination cells.

### Environment configuration

| Property | Value |
|---|---|
| Grid | 5×5 |
| Start | `(0,2)` |
| Goal | `(4,2)` |
| Obstacle | `(1,1)` |
| Normal destination-entry cost | 1 |
| Expensive cells | `(1,2)`, `(2,2)`, `(3,2)` |
| Entry cost of expensive cells | 10 |

The starting cell incurs no entry cost. The cost of a route is calculated by summing the entry costs of its destination cells.

### BFS result

BFS selected the route with the fewest movements:

`(0,2) → (1,2) → (2,2) → (3,2) → (4,2)`

- Movements: 4
- Total cost: \(10+10+10+1=31\)

### Dijkstra result

Dijkstra selected a route with more movements but a lower accumulated cost:

`(0,2) → (0,3) → (1,3) → (2,3) → (3,3) → (4,3) → (4,2)`

- Movements: 6
- Total cost: \(1+1+1+1+1+1=6\)

### Results

| Metric | BFS | Dijkstra |
|---|---:|---:|
| Movements | 4 | 6 |
| Total route cost | 31 | 6 |

The reduction in route cost was:

\[
\frac{31-6}{31}\times100
=80.65\%
\]

### Verification

Both searches were performed against the same environment before execution.

The Dijkstra path was selected for the existing execution pipeline.

The execution verification confirmed:

- Six actions were generated.
- All six actions executed successfully.
- Every observed position matched its expected position.
- The Agent reached `(4,2)`.

Neither search modified the Environment's actual position during planning.

### Conclusion

This experiment demonstrated the difference between minimizing movement count and minimizing accumulated route cost.

BFS selected the route with fewer movements. Dijkstra selected the more expensive-terrain-avoiding route, even though it required two additional movements.

This was a controlled, executed comparison rather than a theoretical illustration.

## 7. Experiment 2 — Lazy Priority Queue and Stale Entries

### Objective

Verify that the lazy priority-queue implementation can retain multiple entries for one state and correctly reject an outdated entry.

### Experimental model

This experiment used a controlled transition-dependent cost model.

It is important to distinguish this from the destination-entry cost model used in Experiment 1.
Here, the cost is associated with a particular transition, not solely with the destination cell.

### Topology

The experiment used five positions:

| Label | Position |
|---|---|
| S | `(0,2)` |
| P | `(1,2)` |
| B | `(0,3)` |
| A | `(1,3)` |
| G | `(2,3)` |

The available transitions were:

| Transition | Cost |
|---|---:|
| S → P | 1 |
| S → B | 2 |
| P → A | 10 |
| B → A | 1 |
| A → G | 20 |

Both routes from S converge at A before continuing to G.

### Observed distance updates

The first route discovered A at cost 11:

\[
1+10=11
\]

The alternative route subsequently improved the best-known cost to A:

\[
2+1=3
\]

The distance table therefore recorded:

`A: INF → 11`

`A: 11 → 3`

The priority queue contained two entries for A:

- `(A, 3)`
- `(A, 11)`

The entry with cost 3 was processed first. When the outdated entry with cost 11 was subsequently removed,
the stale-entry check rejected it because the recorded best-known distance was already 3.

### What the experiment demonstrated

- Multiple entries for the same position can coexist in a lazy priority queue.
- The distance table stores the authoritative best-known cost.
- The priority queue stores candidate entries, which can become outdated.
- An outdated entry can be removed from the queue without processing its outgoing transitions.
- Stale-entry filtering avoids the need to locate and remove old entries immediately.

### Important cost-model distinction

The original destination-only terrain model could not produce this particular stale-entry scenario.

If the cost of entering a cell is fixed, every incoming transition to that cell has the same cost.
The experiment therefore used transition-dependent costs to create two routes to the same state with different accumulated costs.

This distinction is intentional and preserves the validity of both experiments.

## 8. Stale-Entry Guard

The experiment also examined two stale-entry comparisons:

- `cost != distance[current]`
- `cost > distance[current]`

Both worked under the implementation's assumptions:

- Transition costs are non-negative.
- Relaxation uses strict improvement (`<`).
- Queue entries are inserted only when a better distance is discovered.

The `>` comparison is sufficient under these assumptions because an inserted queue entry cannot have a cost below the current best-known distance for that state.

The important invariant is that an outdated, more expensive entry must not be expanded.

## 9. Experiment 3 — Zero-Cost Transitions

### Objective

Verify that Dijkstra can handle zero-cost transitions and examine why strict relaxation matters in the presence of zero-cost cycles.

### Environment configuration

The experiment used a restricted four-cell configuration:

| Label | Position |
|---|---|
| S | `(0,2)` |
| A | `(1,2)` |
| B | `(1,3)` |
| G | `(2,3)` |

The relevant entry costs were:

- Entering A: 0
- Entering B: 0
- Entering G: 1
- Starting cell S: no entry cost

The resulting route was:

`S → A → B → G`

Its total cost was:

\[
0+0+1=1
\]

### Strict relaxation

The implementation uses strict improvement:

`candidate < distance[next]`

This allows a zero-cost transition to be considered without repeatedly accepting an equal-cost route as a new improvement.

### Controlled comparison

During the experiment, the strict comparison was temporarily changed from `<` to `<=`.

With equal-cost updates accepted, the search repeatedly inserted entries around the zero-cost cycle.
The run did not terminate normally, and it was stopped manually with Ctrl+C.

The strict `<` condition was restored.

### Conclusion

The experiment verified that:

- Dijkstra can operate with zero-cost transitions.
- Distinct states can be connected by a zero-cost transition.
- Equal-cost alternatives do not need to replace an existing best-known route.
- Strict relaxation prevents repeated equal-cost updates from continually generating new queue entries around a zero-cost cycle.

Dijkstra supports non-negative edge costs, including zero. Negative edge costs are outside its standard correctness guarantee.

## 10. Experiment 4 — Unreachable Goal

### Objective

Verify that Dijkstra handles an unreachable goal by exhausting the available search frontier and returning no path.

### Environment configuration

The goal remained a valid in-grid position:

`Goal: (4,2)`

However, traversability was restricted to four reachable positions:

- `(0,2)`
- `(1,2)`
- `(0,3)`
- `(1,3)`

The goal was not connected to this reachable region.

### Observed result

Dijkstra explored the reachable states.

After all reachable candidates had been processed, the priority queue became empty without reaching the goal.

The Planner returned an empty path.

The test program exited with status 1 to indicate that no route had been found. This was the expected result for this experiment, not an unexpected runtime failure.

### Conclusion

The experiment verified that an unreachable goal does not produce a fabricated or invalid route.

Queue exhaustion provides the failure condition when no valid route to the goal exists.

## 11. Dijkstra Preconditions and Correctness Boundaries

The experiments reinforce several important conditions for using Dijkstra correctly.

### Non-negative transition costs

Dijkstra's standard correctness guarantee requires non-negative edge costs.

Zero-cost transitions are valid. Negative transition costs are outside the algorithm's guarantee.

### Best-known distance

The distance table must represent the best-known cost to each state.

A newly discovered route replaces the recorded distance only when it is strictly cheaper.

### Valid minimum queue entry

A state must be processed using a valid queue entry. Stale entries must be rejected before further expansion or goal termination.

### Predecessor consistency

The parent table must reflect the route associated with the currently best-known distance.

### Planning and execution separation

Finding a path does not execute it. The Environment remains responsible for actual movement, while the Agent follows the resulting plan
through the existing execution process.

These conditions describe the assumptions and invariants on which the implementation's behaviour depends.
The experiments provide evidence for the tested cases; they are not a formal proof of every possible execution.

## 12. Implementation and Verification Summary

The final implementation retains the existing GridWorld architecture.

| Component | Responsibility |
|---|---|
| Environment | Actual state, movement validation, obstacles and transition costs |
| Agent | Objective and execution responsibilities |
| Planner | BFS, Dijkstra, search bookkeeping and path reconstruction |

The Dijkstra implementation includes:

- Minimum-cost priority queue.
- Queue entries containing position and cost.
- Custom minimum-priority comparator.
- Best-known distance table.
- Parent table.
- Cost relaxation.
- Lazy insertion.
- Stale-entry filtering.
- Valid minimum-priority goal detection.
- Legal-neighbour exploration.
- Parent-based path reconstruction.
- Integration into the existing Planner interface.

The chapter's source programs were compiled from the repository root using GNU G++ with C++17, the shared-header include path,
and the following warning-enabled command:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude <source.cpp> -o agent
```

The restructuring and verification review covered 14 unique source programs:

| Verification result | Count |
|---|---:|
| Source programs compiled | 14 |
| Programs exiting with status 0 | 13 |
| Expected unreachable-goal exit status 1 | 1 |
| Unexpected runtime failures | 0 |

The unreachable-goal program's non-zero exit was intentional and is documented in Experiment 4.

The verified compilation and execution environment was Linux using GNU G++. Native Windows compilation and execution have not been independently tested or verified.

## 13. Limitations and Future Extensions

The current implementation uses a lazy `std::priority_queue` approach. It does not implement an indexed binary heap or decrease-key operation.

The following are possible future extensions to this repository:

- An indexed binary heap with state-to-index tracking and decrease-key.
- Manhattan distance as a heuristic for grid-based search.
- A* search using accumulated path cost and heuristic estimates.

These are future extensions, not completed or verified features of the current implementation.

The current chapter focuses on establishing and testing Dijkstra's behaviour before introducing additional search optimizations.

## 14. Chapter Outcome

Chapter 05 completed the transition from unweighted BFS planning to weighted shortest-path planning.

The implementation was integrated into the existing GridWorld architecture, and its behaviour was examined through deterministic experiments.

The completed experiments demonstrated:

1. BFS and Dijkstra can select different routes when transition costs differ.
2. Dijkstra can reduce total route cost even when the selected route contains more movements.
3. Lazy priority queues can retain outdated entries while the distance table remains authoritative.
4. Stale entries can be rejected before their outgoing transitions are processed.
5. Zero-cost transitions are supported when strict relaxation is used.
6. An unreachable goal results in queue exhaustion and an empty path rather than an invalid route.
7. Planning remains separate from actual environment execution.

With this chapter, the repository's five-chapter GridWorld learning sequence is complete.

The result is a progressively developed agent system: from reactive behaviour and environment simulation to BFS planning, closed-loop execution and weighted
shortest-path search.
