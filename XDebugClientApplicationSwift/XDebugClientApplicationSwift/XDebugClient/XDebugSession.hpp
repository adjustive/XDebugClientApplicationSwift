//
//  XDebugSession.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 12/31/15.
//  Copyright © 2015 elijah. All rights reserved.
//


#ifndef XDebugSession_h
#define XDebugSession_h

#include "Model/BreakpointState.hpp"
#include <map>
#include <mutex>
#include <iterator>
#include "XDebugSocket.hpp"
#include "DebuggerActions.h"
#include <string>
#include "OutputStreamStorage.hpp"
#include "Model/SourceCode.hpp"
#include <vector>
#include "Model/Variable.hpp"


class XDebugSession
{
public:
    XDebugSession(const int session_id);
    ~XDebugSession();
    int session_id;
    void acceptMessage(const DebuggerActions action, const int num_arg);
    void acceptMessageWithString(const DebuggerActions action, const int num_arg, const std::string string_arg);
    void sendCommand(const std::string cmd);
    void setNewBreakpoint(std::shared_ptr<BreakpointState> new_ptr);
    OutputStreamStorage stdout_data; // thread safe
    OutputStreamStorage stderr_data; // thread safe
    const bool hasSourceForFile(const std::string &filename);
    void addSourceCode(std::shared_ptr<SourceCode> s);
    std::shared_ptr<SourceCode> getSourceForFile(const std::string &filename); // not clear why shared ptr is needed
    Variable context_variables;
    std::shared_ptr<BreakpointState> getCurrentBreakpoint();
    void setXDebugSocket(std::weak_ptr<XDebugSocket>ptr);
private:
    BreakpointState* state;
    std::shared_ptr<BreakpointState> state_shared_ptr;
    std::mutex breakpoint_lock;
    std::weak_ptr<XDebugSocket> socket;
    std::mutex source_code_lock;
    std::vector<std::shared_ptr<SourceCode>> source_code_archive;
};


// multisesion is disabled, just returns one object
class OpenedSessions
{
public:
    std::shared_ptr<XDebugSession> getSessionById(const int id);
    const bool sessionExists(const int id);
    void removeSession(const int session_id);
    const bool addSession(const int session_id,std::shared_ptr<XDebugSession> session);
    const int countSessions();
private:
    std::map<int, std::weak_ptr<XDebugSession>> current_sessions;
    std::mutex lock;
    std::weak_ptr<XDebugSession> current_session;
    bool enable_multisession;
};


OpenedSessions* getXDebugOpenSessions();


#endif



