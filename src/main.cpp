#include <iostream> //Provides std::cout for writing text to standard output.

enum class Action      //making action type explicit
{
    MoveLeft,
    MoveRight,
    Wait
};

// Represents the environment in which our agent operates.

class Environment
{
private:
    int position = 0;    // The environment's internal state.
    int  max_position = 3;

public:
    int observe() const
    {
        return position;    // Give the agent the current state as an observation.
    }

    bool act(Action action)
    {
        int movement = 0;

        if (action == Action::MoveLeft)    //Translating action into an environmental effect; env owns the interpretation of actions
        {
           movement = -1;
        }
        else if (action == Action::MoveRight)
        {
           movement = 1;
        }
        else if (action == Action::Wait)
        {
           movement = 0;
        }

        int new_position = position + movement; // Introducing a constrain in the environment as an obstacle to the goal evaluation of the agent.

        if (new_position < 0 || new_position > max_position)
        {
          return false;
        }

        position = new_position;

        std::cout << "Position: "
                  << position
                  << '\n';

        return true;
    }
};


// Represents the agent that observes the environment and selects actions.
class Agent
{
private:
    int goal = 3;   //agent's goal

public:
    // Receives an observation and decides what action should be taken.
    Action decide(int observation) const
    {
        if (observation < goal)    //agent now has current state + goal + decision policy; agent to select from action space
        {
            return Action::MoveRight;    // changeing action from simple integers to something concrete || !(If nothing special is observed, choose action 1)
        }

        return Action::Wait;        // choose wait and not an integer || !(Otherwise choose action 0)
    }
    bool IsgoalReached(int observation) const          //added a goal evaluation function to make the decision making explicit
    {
           return observation >= goal; //independently goal reached state; termination
    }
};


int main()
{
    Environment environment;
    Agent agent;

    // the for loop has inbuilt default termination mechanism, the limit 100 protects it against any bug that can make it run indefinitely

    for (int step = 0; step < 100; ++step)  //creating for loop for continuous iteration & changes to the env state via agent
    {
        int observation = environment.observe();

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

    return 0;
}
