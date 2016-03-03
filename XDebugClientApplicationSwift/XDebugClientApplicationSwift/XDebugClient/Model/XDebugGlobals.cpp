//
//  XDebugGlobals.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/1/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#include "XDebugGlobals.hpp"
#include "XDebugSession.hpp"

const bool XDebugGlobals::shouldAcceptSession(bool is_breakpoint,string const idekey)
{
    int count_sessions = getXDebugOpenSessions()->countSessions();
    if (count_sessions)
    {
        return false;
    }
    return true;
    /*
    bool result = false;
    this->lock.lock();
    bool idekey_match = idekey.compare(this->required_ide_key);
    if ((this->require_idekey && idekey_match) || !this->require_idekey)
    {
        if (this->break_on_next) {
            result = true;
        }
        if (is_breakpoint)
        {
            result = true;
        }
    }
    this->lock.unlock();
    return result;
     */
}

XDebugGlobals* getGlobals()
{
    static XDebugGlobals globals;
    return &globals;
}

void XDebugGlobals::acceptMessage(DebuggerActions action, int value, std::string str_value)
{
    this->lock.lock();
    switch (action) {
            
        default:
            break;
    }
    this->lock.unlock();
}

void XDebugGlobals::acceptMessage(DebuggerActions action, int value)
{
    this->lock.lock();
    switch (action) {
            
        default:
            break;
    }
    this->lock.unlock();
}

const int XDebugGlobals::getIntValue(DebuggerActions message)
{
    int result = 0;
    this->lock.lock();
    switch (message) {
        case debugger_variable_context_depth:
            result = this->context_depth;
            break;
        default:
            break;
    }
    this->lock.unlock();
    return result;
}