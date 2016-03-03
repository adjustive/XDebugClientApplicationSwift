//
//  BreakpointState.hpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/9/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#ifndef BreakpointState_h
#define BreakpointState_h

#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <set>
#include "Stack.hpp"
#include "Variable.hpp"

class XDebugSession;
class BreakpointState;
std::shared_ptr<BreakpointState> getNewBreakPointState();

class BreakpointState // will require a shared pointer to access
{
public:
    Stack stack;
    Variable context_variables;
    
    int history_index = 0;
    int stdout_length;
    int stderr_length;
    
    
    void setNext(std::shared_ptr<BreakpointState> prev_shared_ptr,std::shared_ptr<BreakpointState> new_shared_ptr);
    std::shared_ptr<BreakpointState> getNext();
    std::shared_ptr<BreakpointState> getPrevious();
    void destroyRecursively();
    ~BreakpointState();
    void didExpandVariableCallback(std::string* fullname);
    void didContractVariableCallback(std::string* fullname);
    bool valueWasExpanded(std::string* fullname) const;
    void parseExpansionStates();
    std::weak_ptr<XDebugSession> session_ptr;
private:
    BreakpointState();
    friend std::shared_ptr<BreakpointState> getNewBreakPointState();
    std::shared_ptr<BreakpointState> previousState;
    std::weak_ptr<BreakpointState> nextState;
    std::mutex lock; // lock for next/previous
    std::set<std::string> expanded_values;
    
};


#endif /* BreakpointState_h */
