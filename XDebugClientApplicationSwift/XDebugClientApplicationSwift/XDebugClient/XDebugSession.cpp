//
//  XDebugSession.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/1/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#include "XDebugSession.hpp"
#include "Model/DebuggerCommand.cpp"
#include <iostream>
#include "XDebugClientBridge.hpp"

OpenedSessions* getXDebugOpenSessions()
{
    static OpenedSessions opened_sessions;
    return &opened_sessions;
}

XDebugSession::~XDebugSession()
{
    state = 0;
    if (state_shared_ptr)
    {
        state_shared_ptr->destroyRecursively(); //  there may be another reference to the current object
        state_shared_ptr.reset();
    }
    std::cout << "\n\nXDebugSession destroyed\n\n" << std::endl;
    requireDisplayUpdate();
}

XDebugSession::XDebugSession(const int session_id)
{
    this->session_id = session_id;
}


void XDebugSession::setNewBreakpoint(std::shared_ptr<BreakpointState> new_ptr)
{
    breakpoint_lock.lock();
    if (this->state_shared_ptr && this->state)
    {
        this->state->setNext(this->state_shared_ptr,new_ptr);
    }
    this->state_shared_ptr = new_ptr;
    BreakpointState *a = this->state_shared_ptr.get();
    this->state = a;
    breakpoint_lock.unlock();
}

std::shared_ptr<BreakpointState> XDebugSession::getCurrentBreakpoint()
{
    breakpoint_lock.lock();
    std::shared_ptr<BreakpointState> ptr = this->state_shared_ptr;
    breakpoint_lock.unlock();
    return ptr;
}


void XDebugSession::acceptMessage(const DebuggerActions action, const int num_arg)
{
    switch (action) {
        case debugger_action_step_in:
            this->sendCommand("step_into -iii");
            break;
        case debugger_action_step_out:
            this->sendCommand("step_out -iii");
            break;
        case debugger_action_step_over:
            this->sendCommand("step_over -iii");
            break;
        case debugger_action_stop:
            this->sendCommand("stop -iii");
            break;
        case debugger_action_run:
            this->sendCommand("run -iii");
            break;
        case debugger_action_change_variables_depth:
            break;
            
            default:
                break;
    }
}

void XDebugSession::acceptMessageWithString(const DebuggerActions action, const int num_arg, const std::string string_arg)
{
}


void XDebugSession::sendCommand(const std::string cmd_str)
{
    std::shared_ptr<XDebugSocket> socket_p = socket.lock();
    if (socket_p)
    {
        socket_p->sendCommand(cmd_str,true);
    }
}


const bool XDebugSession::hasSourceForFile(const std::string &filename)
{
    this->source_code_lock.lock();
    bool result = false;
    for (int i = 0;i<this->source_code_archive.size();i++)
    {
        std::shared_ptr<SourceCode> ptr_i = this->source_code_archive[i];
        SourceCode* ptr2 = ptr_i.get();
        if (ptr2->isFilename(filename))
        {
            result = true;
            break;
        }
    }
    this->source_code_lock.unlock();
    return result;
}

std::shared_ptr<SourceCode> XDebugSession::getSourceForFile(const std::string &filename)
{
    this->source_code_lock.lock();
    std::shared_ptr<SourceCode> ptr_i;
    for (int i = 0;i<this->source_code_archive.size();i++)
    {
        ptr_i = this->source_code_archive[i];
        SourceCode* ptr2 = ptr_i.get();
        if (ptr2->isFilename(filename))
        {
            break;
        }
    }
    this->source_code_lock.unlock();
    return ptr_i;
}

void XDebugSession::addSourceCode(std::shared_ptr<SourceCode> s)
{
    this->source_code_lock.lock();
    this->source_code_archive.push_back(s);
    this->source_code_lock.unlock();
}


void XDebugSession::setXDebugSocket(std::weak_ptr<XDebugSocket>a)
{
    this->socket = a;
}












    std::shared_ptr<XDebugSession> OpenedSessions::getSessionById(const int id)
    {
        if (enable_multisession)
        {
            this->lock.lock();
            std::map<int, std::weak_ptr<XDebugSession>>::iterator it;
            it = this->current_sessions.find(id);
            std::weak_ptr<XDebugSession> a = it->second;
            std::shared_ptr<XDebugSession> ptr = a.lock();
            this->lock.unlock();
            return ptr;
        } else {
            this->lock.lock();
            std::shared_ptr<XDebugSession> ptr = this->current_session.lock();
            this->lock.unlock();
            return ptr;
        }
    }
    const bool OpenedSessions::sessionExists(const int id)
    {
        int count;
        if (enable_multisession)
        {
            this->lock.lock();
            count = (int)this->current_sessions.count(id);
            this->lock.unlock();
        } else {
            count = this->countSessions();
        }
        return count > 0;
    }
void OpenedSessions::removeSession(const int session_id)
    {
        this->lock.lock();
        if (enable_multisession)
        {
            this->current_sessions.erase(session_id);
        } else {
            this->current_session.reset(); // not needed on weak pointer
        }
        this->lock.unlock();
    }

const bool OpenedSessions::addSession(const int session_id,std::shared_ptr<XDebugSession> session)
{
    if (enable_multisession)
    {
        this->lock.lock();
        std::weak_ptr<XDebugSession> weak_ptr = session;
        int count = (int)this->current_sessions.count(session_id);
        if (count > 0 || session_id == 0)
        {
            this->lock.unlock();
            return false;
        }
        std::map<int, std::weak_ptr<XDebugSession>>::iterator it;
        this->current_sessions.insert(it,std::pair<int,std::weak_ptr<XDebugSession>>((int)session_id,weak_ptr));
        this->lock.unlock();
        return true;
    } else {
        this->lock.lock();
        std::shared_ptr<XDebugSession> ptr = this->current_session.lock();
        if (ptr)
        {
            this->lock.unlock();
            return false;
        }
        this->current_session = session;
        this->lock.unlock();
        return true;
    }
}


const int OpenedSessions::countSessions()
{
    this->lock.lock();
    int count;
    if (this->enable_multisession)
    {
        count = (int)this->current_sessions.size();
    } else {
        std::shared_ptr<XDebugSession> ptr = this->current_session.lock();
        if (ptr)
        {
            count = 1;
        } else {
            count = 0;
        }
    }
    this->lock.unlock();
    return count;
}


