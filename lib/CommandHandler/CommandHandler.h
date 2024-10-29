#pragma once
#include <cstdint>

class CommandHandler{
public:
    // create an enum for command return values
    enum CommandStatus{
        SUCCESS,
        FAILURE,
        INVALID
    };

    // create a typedef for a command callback function which takes an array pointer and an array size as an argument and returns a CommandStatus
    typedef CommandStatus (*CommandCallback)(uint32_t * command, uint32_t commandSize);

    CommandHandler();
    ~CommandHandler() = default;

    /**
     * @brief Register a command callback function to a command ID
     * @param commandID The command ID to register the callback to
     * @param callback The callback function to register
     * @return true if the callback was registered successfully, false otherwise
     */
    bool RegisterCommand(uint32_t commandID, CommandCallback callback);

    /**
     * @brief Remove a command callback function from a command ID
     * @param commandID The command ID to remove the callback from
     * @return true if the callback was removed successfully, false otherwise
     */
    bool RemoveCommand(uint32_t commandID);

    /**
     * @brief Process a command
     * @param command The command to process
     */
    CommandStatus ProcessCommand(uint32_t * command, uint32_t commandSize);

private:
    static constexpr uint32_t MAX_COMMAND_SIZE{10};

    // an array of command callbacks
    CommandCallback commandCallbacks[MAX_COMMAND_SIZE];
    // an array of command callback IDs
    uint32_t commandCallbackIDs[MAX_COMMAND_SIZE];
};