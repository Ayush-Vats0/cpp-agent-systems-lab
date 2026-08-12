#include <iostream> //Provides std::cout for writing text to standard output.

// Represents the environment in which our agent operates.

class Environment
{
private:
    int position = 0;    // The environment's internal state.

public:
    int observe() const
    {
        return position;    // Give the agent the current state as an observation.
    }

    void act(int action)
    {
        position += action; // Apply the agent's action to the environment.

        std::cout << "Position: "
                  << position
                  << '\n';
    }
};


// Represents the agent that observes the environment and selects actions.
class Agent
{
public:
    // Receives an observation and decides what action should be taken.
    int decide(int observation) const
    {
        if (observation == 0)
        {
            return 1;    // If nothing special is observed, choose action 1.
        }

        return 0;        // Otherwise choose action 0.
    }
};


int main()
{
    Environment environment;   // Creates an Environment object.
    Agent agent;                // Creates an Agent object.

    int observation = environment.observe();
    // The agent obtains information from its environment.

    int action = agent.decide(observation);
    // The agent processes the observation and selects an action.

    environment.act(action);
    // The selected action is sent back to the environment.

    return 0;
    // Returning 0 conventionally indicates successful program termination.
}
