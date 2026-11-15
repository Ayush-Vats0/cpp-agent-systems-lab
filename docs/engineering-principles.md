# Engineering Principles

## 1. Purpose

This document records the engineering principles demonstrated and examined during the development of Chapters 01–05.

The objective is to explain not only what the system does, but also why its responsibilities, algorithms, validation mechanisms,
and experiments are structured in particular ways.

The principles are grounded in the GridWorld implementations and the experiments conducted during development.
They are not claims that the repository implements every aspect of a production-grade autonomous system.

## 2. Separation of Responsibilities

A system becomes easier to reason about when different responsibilities have clear boundaries.

The project uses three principal components:

| Component | Responsibility |
|---|---|
| Agent | Objective evaluation, decision-making, and execution coordination |
| Environment | Actual state, observations, movement validation, and state transitions |
| Planner | Search, route computation, and path reconstruction |

This separation avoids treating the Agent, the world, and the planning algorithm as one indivisible unit.

For example:

- The Agent requests a route.
- The Planner computes a route.
- The Environment determines whether an action is valid and applies it.

A planning algorithm should not need to own the Environment's actual state. Similarly, the Environment should not need to decide which search
algorithm the Agent should use.

**Engineering principle:** Keep responsibilities distinct so that changes to one concern do not unnecessarily alter unrelated parts of the system.

## 3. The Environment Is the Authority on Actual State

The Agent can observe the Environment, predict the result of an action, and request a state transition. It cannot unilaterally establish what actually happened.

The Environment maintains the actual simulated state and enforces its movement rules.

This distinction is visible in the difference between:

- `tryMove()` — evaluates a hypothetical movement without changing the actual Environment state.
- `act()` — applies an action to the Environment.

This distinction is important because evaluating a possible transition and executing that transition are different operations.

A valid prediction does not prove that an action was executed. Likewise, a planned position does not prove that the Agent has reached it.

**Engineering principle:** Keep authoritative state ownership separate from decision-making and prediction.

## 4. Planning Is Not Execution

A plan is a computed sequence of intended states or actions.

It is not evidence that the sequence has been executed successfully.

The open-loop implementation demonstrates this distinction by computing a complete route before executing it.

The closed-loop implementation goes further by checking the result of individual actions against the expected state.

The distinction can be summarized as:

```text
PLANNING
   |
   v
INTENDED ROUTE
   |
   v
EXECUTION
   |
   v
ACTUAL ENVIRONMENT STATE
```

The intended route and actual execution must not be treated as interchangeable.

**Engineering principle:** Maintain a clear distinction between what a system intends to do and what it has actually done.

## 5. Feedback and Verification

Chapter 04 demonstrates why autonomous execution benefits from feedback.

Before an action is executed, the Agent can determine an expected next state. After execution, it observes the Environment and obtains the actual state.

The two values can then be compared.

```text
CURRENT OBSERVATION
       |
       v
EXPECTED NEXT STATE
       |
       v
EXECUTE ACTION
       |
       v
OBSERVE ENVIRONMENT
       |
       v
ACTUAL NEXT STATE
       |
       v
COMPARE
   /       \
MATCH    MISMATCH
  |          |
  v          v
CONTINUE   REPLAN
```

A mismatch is not automatically proof of a defect in the Agent. It can indicate an unexpected transition, an execution problem,
an environmental change, or an incorrect assumption.

The important requirement is that the system recognizes the divergence rather than blindly continuing with an assumption that is no longer valid.

**Engineering principle:** Verify important state transitions using observed results rather than relying exclusively on predictions.

## 6. Replanning and Recovery

A route is valid only under the conditions and assumptions used to compute it.

If execution produces an unexpected state, continuing with the original route may no longer be appropriate.

The closed-loop implementation demonstrates a recovery approach:

1. Detect a mismatch.
2. Obtain the actual observed position.
3. Use that position as the new planning starting point.
4. Request a new route.
5. Continue only when a usable planning result is available.

This is a controlled response to divergence.

The experiments involving fault injection, mismatch handling, and planner failure help examine the difference between detecting a problem and
successfully recovering from it.

A failed planning request must not be treated as a valid route.

**Engineering principle:** Recovery should be based on the actual observed state, not on an outdated expected state.

## 7. Choose Algorithms According to Their Optimization Objective

Different algorithms solve different optimization problems.

The project retains BFS and introduces Dijkstra because the distinction between minimizing the number of transitions and minimizing total route cost is important.

| Property | BFS | Dijkstra |
|---|---|---|
| Frontier | FIFO queue | Minimum-priority queue |
| Optimization objective | Minimum number of edges | Minimum total path cost |
| Cost assumptions | Equal transition costs for minimum-cost interpretation | Nonnegative transition costs |
| Main search bookkeeping | Visited and parent information | Distance, parent, and priority queue |
| Can a previously discovered route improve? | Not in ordinary unweighted BFS | Yes |

The weighted terrain experiment demonstrates why this distinction matters.

In Chapter 05, the original 3×3 GridWorld was extended to 5×5 to create a more useful environment for weighted-route comparisons.

In the 5×5 weighted terrain experiment:

- BFS selected a four-move route with total cost 31.
- Dijkstra selected a six-move route with total cost 6.

The route with fewer movements was not the route with the lowest cost.

**Engineering principle:** Select an algorithm based on the problem's actual objective and assumptions, not simply because it is familiar or commonly used.

## 8. Respect Algorithmic Preconditions

An algorithm's guarantees depend on its assumptions.

Dijkstra's algorithm requires nonnegative transition costs. The implementation and experiments examine nonnegative costs, including zero-cost transitions.

Negative transition costs are outside the correctness guarantee of the implemented Dijkstra approach.

The zero-cost experiment also demonstrates why relaxation must be implemented carefully.

The implementation uses strict improvement:

```cpp
if (candidate < distance[next])
{
    distance[next] = candidate;
}
```

Replacing `<` with `<=` in the zero-cost-cycle experiment caused repeated equal-cost queue insertions. The experiment was stopped, and strict improvement was restored.

This illustrates that a small comparison change can alter termination behavior.

**Engineering principle:** Preserve algorithmic preconditions and invariants when implementing or modifying an algorithm.

## 9. Keep Search State and Queue State Consistent

The Dijkstra implementation uses a lazy priority-queue approach.

When a cheaper route to a position is found, a new queue entry is inserted. The previous entry is not explicitly removed.

As a result, the queue can contain outdated entries.

The distance table remains the authoritative record of the best-known cost.

The stale-entry check is:

```cpp
if (cost != distance[current])
{
    continue;
}
```

An outdated entry is discarded when it no longer matches the authoritative distance.

The separate stale-entry experiment demonstrates this behavior using a transition-dependent cost model.

This approach avoids implementing an indexed heap and decrease-key operation in the current version.

**Engineering principle:** When using duplicated or cached state, define which representation is authoritative and how outdated representations are detected.

## 10. Handle Failure and Boundary Conditions Explicitly

A successful pathfinding demonstration is not sufficient evidence of correct behavior.

The experiments also examine situations in which the normal execution path is unavailable or unnecessary.

Examples include:

- The Agent already being at the goal.
- The goal being unreachable.
- A planning request failing.
- An execution mismatch.
- A previously queued priority entry becoming stale.
- Zero-cost transitions and cycles.

The unreachable-goal experiment is particularly important.

When the priority queue is exhausted without reaching the goal, Dijkstra reports that the goal is unreachable and returns an empty path.
The experiment intentionally exits with a non-zero status.

This is an expected failure scenario, not an unexpected program crash.

**Engineering principle:** Define and examine failure behavior, not just successful execution.

## 11. Validate Behavior Through Controlled Experiments

Experiments should isolate the behavior being investigated.

The GridWorld's development history matters when interpreting these experiments:

- Chapters 01–04 used a 3×3 GridWorld.
- Chapter 05 extended the GridWorld to 5×5 for more meaningful weighted-terrain and Dijkstra experiments.
- Chapter 05 also included separate configurations for specific algorithmic edge cases.

The Chapter 05 experiments use deliberately constructed environments to examine particular properties.

Examples:

| Experiment | Property examined |
|---|---|
| Weighted terrain comparison | BFS versus Dijkstra optimization objectives |
| Stale priority queue | Detection of outdated queue entries |
| Zero-cost cycle | Strict relaxation and termination |
| Unreachable goal | Queue exhaustion and failure reporting |
| Standalone Dijkstra run | Independent execution on a separate GridWorld configuration |

The stale-entry experiment deliberately uses a transition-dependent cost model. It should not be confused with the normal destination-entry cost
model used by the weighted terrain comparison.

Controlled experiments help prevent conclusions from being drawn from examples that do not actually demonstrate the property under investigation.

**Engineering principle:** Construct experiments that isolate a claim and produce observable evidence for it.

## 12. Verify Results, Not Just Compilation

Compilation establishes that source code satisfies the compiler's requirements. It does not establish that
an algorithm is correct or that the program behaves as intended.

The project uses compilation from the repository root with explicit C++17, warning flags, and the shared-header include path:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic -Iinclude chapters/01-reactive-agent/main.cpp -o agent
```

Runtime experiments then examine actual behavior.

For pathfinding, this includes checking:

- The returned route.
- The number of transitions.
- The cumulative route cost.
- Parent-based path reconstruction.
- Whether the route is executable under the Environment's movement rules.
- Whether failure cases are reported appropriately.

The weighted terrain experiment is an example of why route cost must be calculated rather than inferred from the route's appearance.

**Engineering principle:** Treat compilation, execution, and behavioral verification as separate forms of evidence.

## 13. Preserve Experimental History

The repository retains separate chapter implementations and distinct experiment programs.

This is intentional.

A learning repository should make it possible to examine how an implementation developed and how individual concepts were tested.

Replacing every earlier implementation with only the latest version would remove useful evidence of that development.

The experiment directories preserve focused demonstrations without requiring every experiment to become part of the primary implementation.

**Engineering principle:** Preserve meaningful experimental history when it contributes to understanding, reproducibility, or technical explanation.

## 14. Avoid Premature Optimization

The current Dijkstra implementation uses `std::priority_queue` with lazy insertion.

An indexed binary heap and decrease-key operation are possible alternatives, but they introduce additional implementation responsibilities, including:

- Heap storage.
- Position-to-index mapping.
- Heap invariants.
- Index updates during swaps.
- Sift-up and sift-down operations.
- Decrease-key and extract-min behavior.

The current implementation is sufficient to demonstrate Dijkstra's behavior and the lazy priority-queue approach.

An alternative data structure should be introduced when there is a concrete learning or performance reason to compare it, not simply because it is more sophisticated.

The same reasoning applies to future search extensions such as heuristic-guided planning.

**Engineering principle:** Prefer a correct, understandable implementation before introducing optimization complexity without a demonstrated need.

## 15. Distinguish Implemented Features from Conceptual Lessons

Technical documentation must distinguish what the code actually implements from what the development process teaches conceptually.

For this repository:

| Category | Meaning |
|---|---|
| Implemented | Present in the source code |
| Experimentally verified | Executed in a controlled experiment with the expected observed behavior |
| Conceptually examined | Studied and reasoned about, but not necessarily implemented as a system feature |
| Future extension | Identified as possible additional work, not part of the current implementation |

The historical and implementation scope should also remain clear:

- Chapters 01–04 use the original 3×3 GridWorld.
- Chapter 05 extends the environment to 5×5 for its weighted terrain and Dijkstra work.
- BFS and Dijkstra are implemented.
- Stale-entry handling and zero-cost behavior have dedicated experiments.
- The broader idea of selecting planning algorithms dynamically is a conceptual architectural direction, not a completed general-purpose algorithm-selection framework.
- An indexed binary heap, Manhattan distance heuristic, and A* are future extensions, not completed features.

**Engineering principle:** Make technical claims proportional to the implementation and evidence available.

## 16. Conclusion

The first five chapters establish a set of practical engineering lessons:

1. Separate decision-making, environment management, and planning.
2. Keep actual state under the authority of the Environment.
3. Distinguish a plan from its execution.
4. Verify important transitions using observed results.
5. Replan from the actual state after divergence.
6. Select algorithms according to their optimization objectives and assumptions.
7. Preserve invariants and define failure behavior.
8. Use controlled experiments to test specific claims.
9. Verify runtime behavior in addition to compilation.
10. Preserve useful implementation history and avoid unsupported claims.

The GridWorld's progression from 3×3 in Chapters 01–04 to 5×5 in Chapter 05 also illustrates an important experimental principle: modify the
environment when a specific experiment requires it, rather than retroactively treating the expanded environment as though it had existed throughout the project.

Together, these principles provide a disciplined foundation for building and evaluating more complex autonomous systems.
