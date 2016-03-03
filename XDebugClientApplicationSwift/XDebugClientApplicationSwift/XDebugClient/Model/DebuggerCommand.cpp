//
//  DebuggerCommand.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 12/30/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#ifndef DebuggerCommand_hpp
#define DebuggerCommand_hpp

#include <stdio.h>
#include <string>

typedef enum {
    internal_action_finalize_breakpoint,
    internal_action_accept_user_commands
} DebugInternalQueueAction;

class DebuggerCommand
{
public:
    std::string command;
    bool is_user_cmd = false;
    bool is_action = false;
    DebugInternalQueueAction action;
};

#endif