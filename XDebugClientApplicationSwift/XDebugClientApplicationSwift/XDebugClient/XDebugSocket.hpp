//
//  XDebugSocket.hpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 12/31/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#ifndef XDebugSocket_h
#define XDebugSocket_h

//
//  XDebugSocket.cpp
//  XDebugClientCLI
//
//  Created by Elijah on 12/25/15.
//  Copyright © 2015 elijah. All rights reserved.
//

//#include "XDebugSocket.hpp"
#include "XDebugSocketBase.hpp"
#include "Model/DebuggerCommand.cpp"
#include <string>
#include <mutex>
#include <queue>
//#include <boost/property_tree/xml_parser.hpp>
//#include <boost/property_tree/ptree.hpp>
#include "pugixml.hpp"
#include "Model/BreakpointState.hpp"


//using boost::property_tree::ptree;

typedef struct {
    bool is_response;
    bool should_update_display;
    int response_id;
    bool printToLog;
} ParseResponse;

class XDebugSession;

class XDebugSocket: public XDebugSocketBase
{
public:
    XDebugSocket(boost::asio::io_service& io_service);
    void addToQueue(DebuggerCommand &item);
    void sendCommand(std::string cmd_str, bool is_user_cmd);
    ~XDebugSocket();
private:
    void sendInternalAction(DebugInternalQueueAction action);
    void addToTopOfQueue(std::string command);
    bool processStringRead(const char* s, int length) override;
    ParseResponse processXMLPropertyTree(const pugi::xml_node &node, const char * string);
    ParseResponse processXMLResponseTag(const pugi::xml_node &node, const char * string);
    ParseResponse processXMLInitTag(const pugi::xml_node &node, const char * string);
    ParseResponse processXMLStreamTag(const pugi::xml_node &node, const char * string);
    void handleXMLBreakpoint(const pugi::xml_node &node, const char * string);
    void testCommandQueue();
    void doSendNext(bool is_locked);
    virtual void finish() override;
    bool beginSession();
    void requestState(std::string &filename,int line_number);
    std::shared_ptr<XDebugSocketBase> getBaseSharedPointer() override;
    
    std::queue<DebuggerCommand*>* queue;
    std::mutex queue_lock;
    bool is_ready_to_send = false;
    bool session_started = false;
    bool block_user_commands = true;
    bool is_expecting_breakpoint = false;
    XDebugSession* session; // check session
    std::shared_ptr<XDebugSession> session_ptr;
    std::shared_ptr<XDebugSocket> self_pointer;
    std::string idekey;
    int current_write_index = -1; // increment before first write to begin at 0
    int session_id = 0;
    std::shared_ptr<BreakpointState> new_breakpoint; // reset and call this->session_ptr->setNewBreakpoint(new_breakpoint);
    std::string last_requested_filename; // for source request
};


#endif /* XDebugSocket_h */
