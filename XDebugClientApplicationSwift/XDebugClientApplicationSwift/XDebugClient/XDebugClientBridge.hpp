//
//  XDebugClientBridge.hpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 12/29/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#ifndef XDebugClientBridge_hpp
#define XDebugClientBridge_hpp

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <mutex>
#import "XDebugSession.hpp"

void requireDisplayUpdate();
void newSessionNativeCallback(int session_id);
void serverException(const char text);
void serverExited(std::string* e_what);
void stopServer();

class XDebugClientBridge
{
public:
    XDebugClientBridge();
    void runServer();
    void stopServer();
    void restartServer();
    void doSomething();
    const int getStackSize();
    const int getLineNumberForStack(int id, int index);
    const std::string getFunctionNameForStack(int session_id, int index);
    const std::string getFilenameForStack(int i, int index);
    const std::vector<int> getCurrentSessionsList();
    const bool acceptMessage(int session_id, DebuggerActions action, int num_arg);
    const bool acceptMessageWithString(int session_id, DebuggerActions action, int num_arg, std::string string_arg);
    void* getBreakpointStateWithLock(int session_id,int history_index);
    void removeSessionLock(void* ptr);
    bool compareBreakpointSharedPointers(void* ptr1, void* ptr2);
    std::shared_ptr<XDebugSession> getSession(int session_id);
private:
    void removeServerReference();
    std::shared_ptr<boost::asio::io_service> current_server_reference;
    bool should_restart_server;
    std::mutex server_lock;
};

XDebugClientBridge* getXDebugClientInstance();


#endif /* XDebugClientBridge_hpp */
