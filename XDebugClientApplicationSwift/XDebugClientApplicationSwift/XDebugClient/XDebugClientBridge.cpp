//
//  XDebugClientBridge.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 12/29/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#include "XDebugClientBridge.hpp"
#include "XDebugSession.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include "server.cpp"


#import <vector>
#import <mutex>

static XDebugClientBridge xdebug_bridge_instance;

using std::string;
using std::vector;

XDebugClientBridge* getXDebugClientInstance()
{
    return &xdebug_bridge_instance;
}

XDebugClientBridge::XDebugClientBridge()
{
    this->current_server_reference = 0;
}


void XDebugClientBridge::doSomething()
{
    // test function only
    //std::cout << "\nLine 17 of XDebugClientManger.cpp (C++)\n\n";
    //requireDisplayUpdate();
}


void XDebugClientBridge::stopServer()
{
    std::shared_ptr<boost::asio::io_service> io_service = this->current_server_reference;
    if (io_service)
    {
        io_service->stop(); // will update reference after it stops --- this->current_server_reference = 0;
    } else {
        this->server_lock.lock();
        this->should_restart_server = false;
        this->server_lock.unlock();
    }
}

void XDebugClientBridge::restartServer()
{
    this->server_lock.lock();
    this->should_restart_server = true;
    this->server_lock.unlock();
    this->stopServer();
}


void XDebugClientBridge::runServer()
{
    this->server_lock.lock();
    this->should_restart_server = false;
    this->server_lock.unlock();
    
    bool should_restart_loop = true;
    while (should_restart_loop) {
        should_restart_loop = false;
        
        std::shared_ptr<boost::asio::io_service> io_service = std::shared_ptr<boost::asio::io_service>(new boost::asio::io_service());
        using boost::asio::ip::tcp;
    
        short port2 = 9001;
        tcp::endpoint endpoint_all   = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port2);
        //tcp::endpoint endpoint_local = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"),port2);
        tcp::endpoint* endpoint = &endpoint_all;
        //endpoint = &endpoint_local;
        try
        {
            this->server_lock.lock();
            this->current_server_reference = io_service;
            this->server_lock.unlock();
            
            server s(*io_service.get(), std::atoi("9001"),*endpoint);
            //server s(io_service, std::atoi("9000"));
            
            io_service->run();
        }
        catch (std::exception& e)
        {
            this->removeServerReference();
            current_server_reference = 0;
            std::cerr << "Server Exception: " << e.what() << "\n";
            std::string text = e.what();
            serverExited(&text);
            return;
        } catch (const char * e)
        {
            this->removeServerReference();
            current_server_reference = 0;
            std::string text = std::string(e);
            std::cerr << "Server Exception: " << text << "\n";
            serverExited(&text);
            return;
        } catch (...)
        {
            this->removeServerReference();
            current_server_reference = 0;
            std::cerr << "Server Exception: unknown\n";
            std::string text = "Unknown Server Exceptiont";
            serverExited(&text);
            return;
        }
        this->removeServerReference();
        this->server_lock.lock();
        if (this->should_restart_server)
        {
            should_restart_loop = true;
            this->should_restart_server = false;
            std::cout << "\nServer will restart\n" << std::endl;
        }
        this->server_lock.unlock();
        
    } // end should restart loop
    serverExited(0);
}

void XDebugClientBridge::removeServerReference()
{
    this->server_lock.lock();
    current_server_reference = 0;
    this->server_lock.unlock();
}

const std::string XDebugClientBridge::getFunctionNameForStack(int session_id, int index)
{
    return "main()";
}

const std::string XDebugClientBridge::getFilenameForStack(int session_id, int index)
{
    return "/var/filename";
}

const int XDebugClientBridge::getLineNumberForStack(int session_id, int index)
{
    return 1;
}

const int XDebugClientBridge::getStackSize()
{
    return 1;
}

const std::vector<int> XDebugClientBridge::getCurrentSessionsList()
{
    vector<int> stack = std::vector<int>();
    return stack;
}


const bool XDebugClientBridge::acceptMessage(int session_id, DebuggerActions action, int num_arg)
{
    std::shared_ptr<XDebugSession> session = this->getSession(session_id);
    if (session)
    {
        session->acceptMessage(action, num_arg);
        return true;
    } else {
        return false;
    }
}

const bool XDebugClientBridge::acceptMessageWithString(int session_id, DebuggerActions action, int num_arg, std::string string_arg)
{
    std::shared_ptr<XDebugSession> session = this->getSession(session_id);
    if (session)
    {
        session->acceptMessageWithString(action, num_arg,string_arg);
        return true;
    } else {
        return false;
    }
}

std::shared_ptr<XDebugSession> XDebugClientBridge::getSession(int session_id)
{
    OpenedSessions* sessions = getXDebugOpenSessions();
    return sessions->getSessionById(session_id);
}


void* XDebugClientBridge::getBreakpointStateWithLock(int session_id,int history_index)
{
    // should count forwards
    // gui must store the index of the lastest history session always
    // and determine if in history mode or not
    // should allow for a -1 index to account for no history state
    std::shared_ptr<XDebugSession> session = getXDebugOpenSessions()->getSessionById(session_id);
    if (session)
    {
        std::shared_ptr<BreakpointState> state = session->getCurrentBreakpoint();
        
        std::shared_ptr<BreakpointState>* a = (std::shared_ptr<BreakpointState>*)malloc(sizeof(std::shared_ptr<BreakpointState>));
        *a = state;
        return a;
    }
    return 0;
}

void XDebugClientBridge::removeSessionLock(void* ptr)
{
    if (ptr)
    {
        std::shared_ptr<BreakpointState>* a = (std::shared_ptr<BreakpointState>*)ptr;
        a->~shared_ptr();
        free(a);
    }
}

bool XDebugClientBridge::compareBreakpointSharedPointers(void* ptr1, void* ptr2)
{
    /*
     This function does not work as it is named, as it returns false when there are no pointers
     But that works for it's use case(s) sofar
     */
    if (!ptr1 && !ptr2)
    {
        return false;
    }
    if (!ptr1 || !ptr2)
    {
        return true; // one false, one true (so different)
    }
    std::shared_ptr<BreakpointState>* shared1 = (std::shared_ptr<BreakpointState>*)ptr1;
    std::shared_ptr<BreakpointState>* shared2 = (std::shared_ptr<BreakpointState>*)ptr1;
    BreakpointState* real1 = shared1->get();
    BreakpointState* real2 = shared2->get();
    if (real1 == real2)
    {
        return true;
    } else {
        return false;
    }
}




