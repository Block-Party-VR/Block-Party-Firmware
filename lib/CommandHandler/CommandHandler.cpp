#include "CommandHandler.h"

CommandHandler::CommandHandler(){
    for(uint32_t i = 0; i < MAX_COMMAND_SIZE; i++){
        commandCallbacks[i] = nullptr;
        commandCallbackIDs[i] = 0;
    }
}

bool CommandHandler::RegisterCommand(uint32_t commandID, CommandCallback callback){
    for(uint32_t i = 0; i < MAX_COMMAND_SIZE; i++){
        if(commandCallbacks[i] == nullptr){
            commandCallbacks[i] = callback;
            commandCallbackIDs[i] = commandID;
            return true;
        }
    }
    return false;
}

bool CommandHandler::RemoveCommand(uint32_t commandID){
    for(uint32_t i = 0; i < MAX_COMMAND_SIZE; i++){
        if(commandCallbackIDs[i] == commandID){
            commandCallbacks[i] = nullptr;
            commandCallbackIDs[i] = -1;
            return true;
        }
    }
    return false;
}

CommandHandler::CommandStatus CommandHandler::ProcessCommand(uint32_t * command, uint32_t commandSize){
    uint32_t commandID{command[0]};
    // get a pointer to the second element in the array because the first element is the command ID
    uint32_t * args{&(command[1])};

    for(uint32_t i = 0; i < MAX_COMMAND_SIZE; i++){
        if(commandCallbacks[i] != nullptr && commandCallbackIDs[i] == commandID){
            return commandCallbacks[i](args, commandSize-1);
        }
    }
    return CommandStatus::INVALID;
}