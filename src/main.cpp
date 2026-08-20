#include <iostream> //Provides std::cout for writing text to standard output.
#include <queue> //Provides std::queue for creatig queues.

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
    bool IsgoalReached(Position observation) const          //added a goal evaluation function to make the decision making explicit
    { 
           return observation.horizontal == goal.horizontal && observation.vertical == goal.vertical; //independently goal reached state; termination
    }
  };

int main()

    {

    Environment environment;
    Agent agent;

    // the for loop has inbuilt default termination mechanism, the limit 100 protects it against any bug that can make it run indefinitely

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

    }

    std::queue<Position> frontier; //creating a queue object

    bool visited[3][3] = {}; //creating a primitive visited array that can track the bfs search discovery.

    Position start = environment.observe();

    visited[start.horizontal][start.vertical] = true;

    frontier.push(start);

    Position current = frontier.front();

    frontier.pop();

           return 0;
}
