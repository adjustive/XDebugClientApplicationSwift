//
//  BreakpointState.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/9/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#include "BreakpointState.hpp"


static std::mutex expansion_lock;

// this function run on the previous state, sending both the previous state and the new state
// prev_shared_ptr should be the same as "this"
// prob should test with == operator on the shared pointer ie. prev_shared_ptr == this
void BreakpointState::setNext(std::shared_ptr<BreakpointState> prev_shared_ptr,std::shared_ptr<BreakpointState> new_shared_ptr)
{
    BreakpointState* ptr_new_state = new_shared_ptr.get();
    if (ptr_new_state)
    {
        ptr_new_state->history_index++;
        ptr_new_state->previousState = prev_shared_ptr;
        ptr_new_state->context_variables.parseExpansionStates(this); // this and current_shared_ptr are "previous"
        this->lock.lock();
        this->nextState = new_shared_ptr;
        this->lock.unlock();
    }
}

std::shared_ptr<BreakpointState> BreakpointState::getNext()
{
    this->lock.lock();
    std::shared_ptr<BreakpointState> a = this->nextState.lock();
    this->lock.unlock();
    return a;
}

std::shared_ptr<BreakpointState> BreakpointState::getPrevious()
{
    this->lock.lock();
    std::shared_ptr<BreakpointState> a = this->nextState.lock();
    this->lock.unlock();
    return a;
}

void BreakpointState::destroyRecursively(){
    this->lock.lock();
    this->previousState.reset();
    this->lock.unlock();
    /*
     this->lock.lock();
     std::shared_ptr<BreakpointState> ptr_copy_prev = this->previousState; // don't destroy inside the lock
     this->lock.unlock();
     BreakpointState* next = this->nextState.get();
     if (next)
     {
     next->lock.lock();
     next->previousState.reset();
     this->nextState.reset();
     next->lock.unlock();
     ptr_copy.reset();
     }
     if (prev)
     {
     prev->destroyRecursively();
     prev->lock.lock();
     this->previousState.reset();
     prev->lock.unlock();
     ptr_copy.reset();
     }
     ptr_copy_prev.reset();
     ptr_copy_next.reset();
     */
}

BreakpointState::~BreakpointState()
{
    this->destroyRecursively();
    std::cout << "\nDeleted Breakpoint State index " << this->history_index << "\n" << std::endl;;
}

BreakpointState::BreakpointState()
{
    this->context_variables.setRootNode();
    //std::cout << "\nNew Breakpoint State index " << this->history_index << "\n";
}

std::shared_ptr<BreakpointState> getNewBreakPointState() // friend function
{
    return std::shared_ptr<BreakpointState>(new BreakpointState());
}


void BreakpointState::didExpandVariableCallback(std::string* fullname)
{
    expansion_lock.lock();
    int count = (int)this->expanded_values.count(*fullname);
    if (!count)
    {
        this->expanded_values.insert(*fullname);
    }
    expansion_lock.unlock();
}

void BreakpointState::didContractVariableCallback(std::string* fullname)
{
    expansion_lock.lock();
    this->expanded_values.erase(*fullname);
    //std::iterator_traits<std::string> a = this->expanded_values.find(fullname);
    //if (a != this->expanded_values.end())
    //{
     //   this->expanded_values.erase(a);
    //}
    expansion_lock.unlock();
}


bool BreakpointState::valueWasExpanded(std::string* fullname) const
{
    if (!fullname)
    {
        return false;
    }
    if (fullname->compare("$autoloader")==0)
    {
        std::cout << "autoloader";
    }
    expansion_lock.lock();
    int count = (int)this->expanded_values.count(*fullname);
    expansion_lock.unlock();
    return count > 0;
}

void BreakpointState::parseExpansionStates()
{
    std::shared_ptr<BreakpointState> previous = this->previousState;
    if (previous)
    {
        
    }
}