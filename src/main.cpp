#include <iostream> //Provides std::cout for writing text to standard output.
#include <queue> //Provides std::queue for creatig queues.
#include <vector> //helps storing values dynamically that are found by the bfs
#include <algorithm> //to reverse the path provided by the recorder

struct Position    //adding a structure to simulate planning in the agent and take the whole system to a 2D world
{
    int horizontal;
    int vertical;
};

enum class Action      //making action type explicit
{
    MoveLeft,
    MoveRight,
    MoveUp,
    MoveDown,
    Wait
};

// Represents the environment in which our agent operates.

class Environment
{
private:
    Position position {0, 0};    // The environment's internal state.

public:
    Position observe() const
    {
        return position;    // Give the agent the current state as an observation.
    }

    bool tryMove(Position state, Action action, Position& next_state) //simulator to understand possibility by the agent
    {

         next_state = state;

          //to  translate action into hypothetical movement

         if (action == Action::MoveLeft)
         {
             next_state.vertical--;
         }
         else if (action == Action::MoveRight)
         {
             next_state.vertical++;
         }
         else if (action == Action::MoveUp)
         {
             next_state.horizontal--;
         }
         else if (action == Action::MoveDown)
         {
             next_state.horizontal++;
         }
         else if (action == Action::Wait)
         {
             // No change.
         }

        //obstacle validation

        if (next_state.horizontal < 0 ||
            next_state.horizontal > 2 ||
            next_state.vertical < 0 ||
            next_state.vertical > 2)
         {

             return false;

         }

       if (next_state.horizontal == 1 &&
           next_state.vertical == 1)
         {
             return false;
         }

       return true;

    }

    bool act(Action action)
    {
        Position new_position = position;

        if (action == Action::MoveLeft)    //Translating action into an environmental effect; env owns the interpretation of actions
        {
           new_position.horizontal--;
        }
        else if (action == Action::MoveRight)
        {
           new_position.horizontal++;
        }
        else if (action == Action::MoveUp)
        {
           new_position.vertical--;
        }
        else if (action == Action::MoveDown)
        {
           new_position.vertical++;
        }
        else if (action == Action::Wait)
        {
                // nothing
        }

        if (new_position.horizontal < 0 || new_position.horizontal > 2 || new_position.vertical < 0 || new_position.vertical > 2)  //added the validation check for the obstacle
        {
          return false;
        }

        if (new_position.horizontal == 1 && new_position.vertical == 1)     //introducting an obstacle to the environment so that the bot cant take invalid actions
        {
          return false;
        }

        position = new_position;

        std::cout << "Position: ("
          << position.horizontal
          << ", "
          << position.vertical
          << ")\n";

        return true;
    }
};

// Represents the agent that observes the environment and selects actions.
class Agent
{
private:
    Position goal {2, 2};   //agent's goal

public:
    // Receives an observation and decides what action should be taken.
    Action decide(Position observation) const
    {
        if (observation.horizontal < goal.horizontal)    //agent now can choose from multiple actions from action space; agent now has current state + goal + decision policy; agent to select from action space
        {
            return Action::MoveRight;    // changeing action from simple integers to something concrete || !(If nothing special is observed, choose action 1)
        }
        else if (observation.horizontal > goal.horizontal)
        {
            return Action::MoveLeft;
        }
        else if (observation.vertical < goal.vertical)
        {
            return Action::MoveDown;
        }
        else if (observation.vertical > goal.vertical)
        {
            return Action::MoveUp;
        }

            return Action::Wait;        // choose wait and not a position || !(Otherwise choose action)
    }

    bool isGoal(Position state)   //added a goal
    {
             return state.horizontal == 2 &&
             state.vertical == 2;
    }

    bool IsgoalReached(Position observation) const          //added a goal evaluation function to make the decision making explicit
    {
           return observation.horizontal == goal.horizontal && observation.vertical == goal.vertical; //independently goal reached state; termination
    }

    bool execute(Environment& environment, Action action)
    {
        return environment.act(action);

    }

  };

class Planner  //BFS gets its own implementation component separately, away from main()
{
public:

         std::vector<Position> bfs(Environment& environment, Agent& agent, Position start)
         {

                     std::queue<Position> frontier;

                     bool visited[3][3] = {};

                     Position parent[3][3];

                     visited[start.horizontal][start.vertical] = true;

                     frontier.push(start);

                     bool goal_found = false;

                     Position goal;

                     while (!frontier.empty() && !goal_found)
                     {

                           Position current = frontier.front();
                           frontier.pop();

                           std::cout << "BFS exploring: ("
                                     << current.horizontal << ", "
                                     << current.vertical << ")\n";

                           std::vector<Action> actions =
                           {

                                Action::MoveLeft,
                                Action::MoveRight,
                                Action::MoveUp,
                                Action::MoveDown,
                                Action::Wait
                           };

                           for (Action action : actions)
                           {

                                Position next_state;

                                bool valid = environment.tryMove(current,action,next_state);

                                if (valid && !visited[next_state.horizontal][next_state.vertical])
                                {
                                    visited[next_state.horizontal][next_state.vertical] = true;

                                    parent[next_state.horizontal][next_state.vertical] = current;

                                    if (agent.isGoal(next_state))
                                    {

                                        goal = next_state;

                                        goal_found = true;
                                        break;
                                    }

                                    frontier.push(next_state);
                                }
                            }
                     }

                     std::vector<Position> path;

                     if (goal_found)
                     {

                         Position current = goal;

                         while (current.horizontal != start.horizontal || current.vertical != start.vertical)
                         {

                                path.push_back(current);

                                current = parent[current.horizontal][current.vertical];
                         }

                         path.push_back(start);

                         std::reverse(path.begin(), path.end());
                     }

        return path;
    }

    std::vector<Action> pathToActions(const std::vector<Position>& path) //introducing a function for planner to handle path to execution planning as well
    {
        std::vector<Action> actions;

            for (std::size_t i = 0; i + 1 < path.size(); ++i)
                {
                    Position current = path[i];

                    Position next = path[i + 1];

                    if (next.vertical > current.vertical)
                    {

                       actions.push_back(Action::MoveRight);

                    }
                    else if (next.vertical < current.vertical)
                    {

                       actions.push_back(Action::MoveLeft);

                    }
                    else if (next.horizontal > current.horizontal)
                    {

                       actions.push_back(Action::MoveDown);

                    }
                    else if (next.horizontal < current.horizontal)
                    {

                        actions.push_back(Action::MoveUp);

                    }
               }

               return actions;

     }
};


int main()

    {

    Environment environment;

    Agent agent;

    Planner planner;

    Position start = environment.observe();

    std::vector<Position> path = planner.bfs(environment, agent, start); //main() becomes orchestrator

    std::cout << "Planned Path:\n";

    for (const Position& position : path)
    {
        std::cout << "("
                  << position.horizontal
                  << ", "
                  << position.vertical
                  << ")\n";
    }

    std::vector<Action> actions = planner.pathToActions(path);  //testing planner method for the action plan derived by the bfs

    for (Action action : actions) //orchestrating the execution  of the planned action
    {

        bool success = agent.execute(environment, action);

        if (!success)
        {

           std::cout << "Execution failed.\n";
           break;

        }

    }

  /*  std::cout << "Planned Actions:\n";

    for (Action action : actions)
    {
        if (action == Action::MoveLeft)

            std::cout << "MoveLeft\n";

        else if (action == Action::MoveRight)

            std::cout << "MoveRight\n";

        else if (action == Action::MoveUp)

            std::cout << "MoveUp\n";

        else if (action == Action::MoveDown)

            std::cout << "MoveDown\n";

        else

            std::cout << "Wait\n";

     }*/ //testing block to test the planner's action plan for the path derived by the bfs

  /*  // the for loop has inbuilt default termination mechanism, the limit 100 protects it against any bug that can make it run indefinitely

    for (int step = 0; step < 100; ++step)  //creating for loop for continuous iteration & changes to the env state via agent
    {
        Position observation = environment.observe();

        if (agent.IsgoalReached(observation))
        {
           std::cout << "Goal Reached!\n";          //goal evaluation break
           break;
        }

        Action action = agent.decide(observation);   //changing int to conrete definable action in this iteration

        bool action_sucess = environment.act(action);    //changing the orchestration of the agent's actions to deal with an environmental constrain

        if (!action_sucess)
        {
             std::cout << "Action failed, Goal cannot be reached.\n";
             break;
        }

    }*/ //old reactive architecture.

  /*  std::queue<Position> frontier; //creating a queue object

    bool visited[3][3] = {}; //creating a primitive visited array that can track the bfs search discovery.

    Position parent[3][3]; //adding a recorder for the bfs

    Position start = environment.observe();

    visited[start.horizontal][start.vertical] = true;

    frontier.push(start);

    bool goal_found = false;

    while(!frontier.empty() && !goal_found)  //putting bfs to action
    {

       Position current = frontier.front();

       frontier.pop();

       std::cout << "BFS exploring: ("
                 << current.horizontal << ", "
                 << current.vertical << ")\n";

       std::vector<Action> actions =
       {
         Action::MoveLeft,
         Action::MoveRight,
         Action::MoveUp,
         Action::MoveDown,
         Action::Wait
      };

     for (Action action : actions)
     {

         Position next_state;

         bool valid = environment.tryMove(current, action, next_state);

        if (valid && !visited[next_state.horizontal][next_state.vertical])
        {

            visited[next_state.horizontal][next_state.vertical] = true;

            parent[next_state.horizontal][next_state.vertical] = current;

            if (agent.isGoal(next_state))
            {

               goal_found = true;
               break;
            }

            frontier.push(next_state);
        }

      }

    }


   Position goal{2, 2};  //adding path reconstruction for the bfs

   std::vector<Position> path;

   Position current = goal;

   while (current.horizontal != start.horizontal || current.vertical != start.vertical)
   {

          path.push_back(current);

          current = parent[current.horizontal][current.vertical];
   }

   path.push_back(start);

   std::reverse(path.begin(), path.end()); 

   std::cout << "BFS Path:\n";

   for (const Position& position : path)
   {

         std::cout << "("
                   << position.horizontal
                   << ", "
                   << position.vertical
                   << ")\n";
   }*/ //Old architecture decision of putting bfs inside the main()
           return 0;
}
