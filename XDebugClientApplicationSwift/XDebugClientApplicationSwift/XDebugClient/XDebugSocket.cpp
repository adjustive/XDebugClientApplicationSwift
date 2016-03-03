//
//  XDebugSocket.cpp
//  XDebugClientCLI
//
//  Created by Elijah on 12/25/15.
//  Copyright © 2015 elijah. All rights reserved.
//

//#include "XDebugSocket.hpp"
#include "XDebugSocket.hpp"
#include "Model/DebuggerCommand.cpp"
#include <sstream>
#include <string>
#include <queue>
#include <mutex>
#include "XDebugClientBridge.hpp"
#include "c_utility_functions.hpp"
#include <map>
#include <iostream>
#include <boost/algorithm/string.hpp> // include Boost, a C++ library
#include "Model/XDebugGlobals.hpp"
#include "Model/Stack.hpp"



// todo - add "states" to start breakpoint and end breakpoint which will appear as debugger commands
//
//
// todo - find a way to manage variables on the stack -  add them to a "map", but then requires special
//          memory management for the first item
//          - it can be done like this: the variables are deleted from the map and held only as a reference
//            in Breakpoint State
//
//


XDebugSocket::XDebugSocket(boost::asio::io_service& io_service) : XDebugSocketBase(io_service)
{
    //this->testCommandQueue();
    //XDebugSocket(boost::asio::io_service& io_service) : XDebugSocketBase(io_service)
    this->self_pointer = std::shared_ptr<XDebugSocket>(this);
    this->is_ready_to_send = true;
    queue = new std::queue<DebuggerCommand*>();
}

XDebugSocket::~XDebugSocket()
{
    while (this->queue->size())
    {
        DebuggerCommand* cmd = this->queue->front();
        delete cmd;
        this->queue->pop();
    }
    delete queue;
}


bool XDebugSocket::processStringRead(const char* string, int length)
{
    //std::string string = std::string(s);
    std::istringstream input = std::istringstream(string);
    
    bool update_display = false;
    try {
        pugi::xml_document doc;
        pugi::xml_parse_result result = doc.load_string(string);
        if (!result)
        {
            std::cout << "XML [] parsed with errors, attr value: [" << doc.child("node").attribute("attr").value() << "]\n";
            std::cout << "Error description: " << result.description() << "\n";
            std::cout << "Error offset: " << result.offset << " (error at [..." << result.offset << "]\n\n";
            return false;
        }
        pugi::xml_node firstElement = doc.document_element();
        pugi::xml_attribute statusAttr = firstElement.attribute("status");
        if (statusAttr)
        {
            const char* status = statusAttr.value();
            if (status && strcmp(status, "stopping") == 0)
            {
                return false;
            }
        }
        ParseResponse parse_response = this->processXMLPropertyTree(firstElement,string);
        if (parse_response.is_response && parse_response.response_id >= this->current_write_index)
        {
            this->queue_lock.lock();
            this->is_ready_to_send = true;
        } else {
            // the case of notifications
            // and anything else not requested
            // XML should be parsed in processXMLPropertyTree
            update_display = parse_response.should_update_display;
        }
        if (parse_response.printToLog)
        {
            std::cout << "\n" << string << "\n";
        }
        this->doSendNext(true);// will unlock, but not lock
    } catch (...)
    {
        std::cout << "Error 42 XDebugSocket.cpp";
        return false;
    }
    // Update the display outside of the try block
    if (update_display)
    {
        requireDisplayUpdate(); // does very little, shouldn't crash (sets a bool variable read by a timer)
    }
    return true;
}

void iterateXMLAttributes()
{
 //   pugi::xml_attribute_iterator ait = pt.
    //for (; ait != it->attributes_end(); ++ait)
   // {
   //    std::cout << " " << ait->name() << "=" << ait->value();
   // }
}

ParseResponse XDebugSocket::processXMLPropertyTree(const pugi::xml_node &firstNode, const char * string)
{
    // check init app_id to see if it is not a duplicate of one running
    // and in that case to terminate the session
    
    //ptree::const_iterator end = pt.end();
    //for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
    //ptree::const_iterator it = pt.begin();
    
    const char * tag = firstNode.name();
    //std::cout << it->first << ": " << it->second.get_value<std::string>() << std::endl;
    //std::cout << it->second;
    if (std::strcmp(tag, "init") == 0)
    {
        return this->processXMLInitTag(firstNode,string);
    } else if (std::strcmp(tag, "response") == 0)
    {
        return this->processXMLResponseTag(firstNode,string);
    } else if (std::strcmp(tag, "stream") == 0)
    {
        return this->processXMLStreamTag(firstNode,string);
    }
      //  break;
    //}
    return ParseResponse();
}

ParseResponse XDebugSocket::processXMLStreamTag(const pugi::xml_node &node, const char * string)
{
    ParseResponse response;
    response.is_response = false;
    
    const char* stream_name = node.attribute("type").value();
    const char* stream_encoded_value = node.first_child().value();
    if (this->session_ptr)
    {
        response.should_update_display = true;
        if (std::strcmp(stream_name, "stdout") == 0)
        {
            this->session->stdout_data.appendStringBase64(stream_encoded_value);
        } else if(std::strcmp(stream_name, "stderr") == 0)
        {
            this->session->stderr_data.appendStringBase64(stream_encoded_value);
        }
    }
    // <stream xmlns="urn:debugger_protocol_v1" xmlns:xdebug="http://xdebug.org/dbgp/xdebug" type="stdout" encoding="base64"><![CDATA[MTEx]]></stream>
    return response;
}


ParseResponse XDebugSocket::processXMLResponseTag(const pugi::xml_node &node, const char * string)
{
    ParseResponse response = ParseResponse();
    response.is_response = true;
    const char* t_id_str = node.attribute("transaction_id").value();
    const char* status   = node.attribute("status").value();
    const char* test     = node.attribute("sadfdsf").value();
    const char* command  = node.attribute("command").value();
    response.response_id = std::stoi(t_id_str);
    if (this->is_expecting_breakpoint && status && strcmp(status, "break") == 0)
    {
        this->handleXMLBreakpoint(node,string);
        response.printToLog = true;
    }
    if (command)
    {
        // COMMANDS NOT ON A BREAKPOINT
        if (std::strcmp(command,"source") == 0)
        {
            const char* value_encoding = node.attribute("encoding").value();
            const pugi::xml_node value_node = node.first_child();
            if (value_node && strcmp(value_encoding, "base64") == 0)
            {
                const char* valuenode_type = value_node.name();
                if (!*valuenode_type)
                {
                    const char* source_base64 = value_node.value();
                    try {
                        std::string source_decoded = decode64(source_base64);
                        SourceCode* s = new SourceCode(this->last_requested_filename,source_decoded);
                        std::shared_ptr<SourceCode> s_ptr = std::shared_ptr<SourceCode>(s);
                        this->session_ptr->addSourceCode(s_ptr);
                    } catch (...) {
                        std::cout << "\bSource error 145\n";
                    }
                }
            }
        }
        // end commands not on a breakpoint
    }
    // COMMANDS FOR A BREAKPOINT
    // a breakpoint should always exist for these commands, but might not if there is an error
    if (command && this->new_breakpoint)
    {
        if(strcmp(command,"stack_get") == 0)
        {
            // PARSE STACK_GET
            this->new_breakpoint->stack.addToStackFromXMLNode(node);
            response.printToLog = true;

            // need to get here at least one source
            // add to top of the queue if needed
            int stack_size = this->new_breakpoint->stack.getStackSize();
            if (stack_size >= 1)
            {
                const StackLineItem* line_i = this->new_breakpoint->stack.getItemAtIndex(0);
                if (line_i)
                {
                    std::string filename_i = line_i->filename;
                    if (!this->session_ptr->hasSourceForFile(filename_i))
                    {
                        this->last_requested_filename = filename_i;
                        this->addToTopOfQueue("source -iii -f "+ filename_i);
                    }
                }
            }
        } else if(strcmp(command,"context_get") == 0)
        {
            this->new_breakpoint->context_variables.appendNodeChildren(node);
            
        } else if(strcmp(command,"eval") == 0)
        {
            // sofar, there is only one eval command
            // if there are more, then they must be tracked somehow
            
            for (pugi::xml_node child_i = node.first_child(); child_i; child_i = child_i.next_sibling())
            {
                // should be only one property tag
                const char* tag_name_i = child_i.name();
                if (strcmp(tag_name_i, "property")==0)
                {
                    Variable* php_error_arr = this->new_breakpoint->context_variables.appendEmptyVariable();
                    php_error_arr->setName("PHP LAST ERROR");
                    php_error_arr->setType("array");
                    php_error_arr->appendNodeChildren(child_i);
                }
            }
        }
    } else {
        std::cout << "\n\n\nError 202\n\n";
    }
    return response;
}

void XDebugSocket::handleXMLBreakpoint(const pugi::xml_node &node, const char * string)
{
    this->is_expecting_breakpoint = false;
    /* GET THE FILENAME */
    std::string filename;
    for (pugi::xml_node child_node = node.child("xdebug:message"); child_node; child_node = child_node.next_sibling("xdebug:message"))
    {
        pugi::xml_attribute attribute_i = child_node.attribute("filename");
        if (attribute_i)
        {
            filename = attribute_i.value();
        }
        //std::cout << "Attribute " << child_node.attribute("filename").value();
    }
    this->requestState(filename, 10);
}

void XDebugSocket::requestState(std::string &filename,int line_number)
{
    // node contains the filename
    if (!this->session_id)
    {
        std::cout << "\nError 137\n";
        return; // invalid
    }
    if (!this->session_started)
    {
        bool should_break = getGlobals()->shouldAcceptSession(true,this->idekey);
        if (should_break)
        {
            // init session and continue to debug
            bool was_started = this->beginSession();
            if (!was_started)
            {
                return; // issue of session already started, not a bug
            }
        } else {
            // end processing
            return;
        }
    }
    
    /* SET NEW BREAKPOINT STATE */
    new_breakpoint = getNewBreakPointState();
    new_breakpoint->stdout_length = this->session_ptr->stdout_data.getLength();
    new_breakpoint->stderr_length = this->session_ptr->stderr_data.getLength();
    
    /* CHECK FOR SOUCE FILE */
    bool has_source = this->session_ptr->hasSourceForFile(filename);
    
    /* RUN COMMANDS TO GET INFO: stack_get, context_get */
    DebuggerCommand cmd;
    
    
    if (!has_source)
    {
        this->last_requested_filename = filename;
        this->sendCommand("source -iii -f "+ filename, false);
    }
    
    std::string error_get_last_b64 = encode64("error_get_last()");
    //std::string get_line_number = encode64("__LINE__");
    this->sendCommand("context_get -iii -c 1", false);
    this->sendCommand("context_get -iii -c 0", false);
    this->sendCommand("eval -iii -- "+error_get_last_b64, false);
    //this->sendCommand("eval -iii -- "+get_line_number, false);
    this->sendCommand("stack_get -iii", false);
    this->sendInternalAction(internal_action_accept_user_commands);
    this->sendInternalAction(internal_action_finalize_breakpoint);
    
}

ParseResponse XDebugSocket::processXMLInitTag(const pugi::xml_node &node,const char * string)
{
    ParseResponse response =  ParseResponse();
    response.printToLog = true;
    std::string idekey = node.attribute("idekey").value();
    std::string appid  = node.attribute("appid").value();
    std::string filename  = node.attribute("fileuri").value();
    int app_id_int = std::stoi(appid);
    //std::string fileuri = node.attribute("fileuri").value();
    bool should_break = getGlobals()->shouldAcceptSession(false,idekey);
    this->idekey = idekey; // in case to break later
    this->session_id = app_id_int;
    if (should_break)
    {
        bool was_started = this->beginSession();
        if (was_started)
        {
            this->sendCommand("feature_set -iii -n max_children -v 2000", false);
            this->sendCommand("feature_set -iii -n max_depth -v 5", false);
            this->requestState(filename, 0);
        }
    }
    return response;
}

bool XDebugSocket::beginSession()
{
    std::shared_ptr<XDebugSession> session = std::shared_ptr<XDebugSession>(new XDebugSession(this->session_id));
    bool was_started = getXDebugOpenSessions()->addSession(this->session_id, session);
    if (was_started)
    {
        this->session_started = true;
        this->session_ptr = session;
        this->session = session.get();
        this->session_ptr->setXDebugSocket(this->self_pointer);
        this->sendCommand("stdout -iii -c 1", false);
        this->sendCommand("stderr -iii -c 1", false);
        this->sendCommand("context_names -iii",false);
        return true;
    }
    return false;
}

void XDebugSocket::sendCommand(std::string cmd_str, bool is_user_cmd)
{
    DebuggerCommand cmd = DebuggerCommand();
    cmd.command = cmd_str;
    cmd.is_user_cmd = is_user_cmd;
    this->addToQueue(cmd);
}


void XDebugSocket::sendInternalAction(DebugInternalQueueAction action)
{
    DebuggerCommand cmd = DebuggerCommand();
    cmd.is_action = true;
    cmd.action = action;
    this->addToQueue(cmd);
}

void XDebugSocket::addToQueue(DebuggerCommand &item)
{
    this->queue_lock.lock();
    if (this->block_user_commands && item.is_user_cmd)
    {
        this->queue_lock.unlock();
        return;
    }
    if (item.is_user_cmd)
    {
        this->block_user_commands = true;
    }
    DebuggerCommand* cmd_copy = new DebuggerCommand(item);
    this->queue->push(cmd_copy);
    this->queue_lock.unlock();
    this->doSendNext(false);
}

void XDebugSocket::addToTopOfQueue(std::string command)
{
    this->queue_lock.lock();
    std::queue<DebuggerCommand*>* queue2 = new std::queue<DebuggerCommand*>();
    DebuggerCommand* cmd_n = new DebuggerCommand();
    cmd_n->command = command;
    queue2->push(cmd_n);
    while (this->queue->size())
    {
        DebuggerCommand* cmd_i = this->queue->front();
        this->queue->pop();
        queue2->push(cmd_i);
    }
    delete this->queue;
    this->queue = queue2;
    this->queue_lock.unlock();
    this->doSendNext(false);
}

void XDebugSocket::testCommandQueue()
{
    /*
    DebuggerCommand a;
    
    
    
    a = DebuggerCommand();
    a.command = "run -iii";
    queue.push(a);

    return;
    
    // first set causes execution to continue
    queue = std::queue<DebuggerCommand>();
    a = DebuggerCommand();
    a.command = "context_names -iii";
    this->addToQueue(a);
    //"<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<response xmlns=\"urn:debugger_protocol_v1\" xmlns:xdebug=\"http://xdebug.org/dbgp/xdebug\" command=\"context_names\" transaction_id=\"1\"><context name=\"Locals\" id=\"0\"></context><context name=\"Superglobals\" id=\"1\"></context><context name=\"User defined constants\" id=\"2\"></context></response>"
    //queue.push(a);
    
    // then it should break on xdebug_break line
    // in that case, it is needed to get the stack variables, and the stack trace
    a = DebuggerCommand();
    a.command = "context_get -d 2 -c 1 -i 2";
    this->addToQueue(a);
    
    
    std::string b64 = "KGVjaG8gIkhlbGxvIGZyb20gRGVidWcgQXBwbGljYXRpb24iKQ=="; // ZWNobyAiSGVsbG8gZnJvbSBEZWJ1ZyBBcHBsaWNhdGlvbiI7
    std::string cmd = "eval -i 3 -- "+b64;
    a = DebuggerCommand();
    a.command = cmd; //
    //this->addToQueue(&a);
    
    
    b64 ="aXNzZXQoJF9TRVJWRVJbJ1JFUVVFU1RfVVJJJ10pID8gJF9TRVJWRVJbJ1JFUVVFU1RfVVJJJ10gOiAoaXNzZXQoJGFyZ3YpID8gaW1wbG9kZSgnLCcsJGFyZ3YpOicnKQ=="; //isset($_SERVER['REQUEST_URI']) ? $_SERVER['REQUEST_URI'] : (isset($argv) ? implode(',',$argv):'')
    b64 = "YWJjKCk=";
    b64 = "cHJpbnRfcigxMTEp";
    b64 = "cHJpbnRfcigiV2VsY29tZSB0byB0aGUgRGVidWdnZXIgQXBwbGljYXRpb24hIik=";
    //b64 = encode64("file:///tmp/2.php");
    cmd = "eval -i 4 -- "+b64;
    a = DebuggerCommand();
    a.command = cmd; //
    this->addToQueue(&a);
    
    a = DebuggerCommand();
    a.command = "source -i 5 -f file:///tmp/2.php";
    queue.push(a);
    */
}

void XDebugSocket::doSendNext(bool is_locked)
{
    if (!is_locked)
    {
        this->queue_lock.lock();
    }
    bool continue_loop = true;
    while (continue_loop)
    {
        continue_loop = false;
        if (!this->is_ready_to_send || this->queue->size() == 0)
        {
            this->queue_lock.unlock();
            return;
        }
        DebuggerCommand* current_command_object = this->queue->front();
        this->queue->pop();
        if (!current_command_object->is_action)
        {
            /* DO WRITE OPERATIONS HERE */
            std::string current_command = current_command_object->command;
            this->is_ready_to_send = false;
            this->current_write_index++;
            if (this->current_write_index == 22)
            {
                //std::cout << "____";
            }
            std::string number = std::to_string(this->current_write_index);
            boost::replace_all(current_command, " -iii", " -i "+number);
            std::cout << "\n"+current_command+"\n";
            this->write(&current_command);
            /* FINISH WRITE OPERATION */
            continue_loop = false;
        } else {
            /* HANDLE INTERNAL ACTIONS */
            if (current_command_object->action == internal_action_finalize_breakpoint)
            {
                //  SET THE NEW BREAKPOINT FOR THE DISPLAY
                // context_get is always the last command run and sets the breakpoint state
                // if the context should be updated on the same breakpoint, first set new_breakpoint to a copy
                // and empty variables for it (or just get the entire stack again)
                this->new_breakpoint->session_ptr = this->session_ptr;
                this->session->setNewBreakpoint(this->new_breakpoint);
                this->new_breakpoint.reset();
                this->is_expecting_breakpoint = true;
                requireDisplayUpdate();
            }
            if (current_command_object->action == internal_action_accept_user_commands)
            {
                this->block_user_commands = false;
            }
            /* END INTERNAL ACTIONS */
            continue_loop = true;
        }
        delete current_command_object;
    }
    this->queue_lock.unlock(); // don't unlock until write sent, otherwise socket might be closed
                                // how is that possible?
    
    // todo - add "states" to start breakpoint and end breakpoint which will appear as debugger commands
    //
    //
    //
}

std::shared_ptr<XDebugSocketBase> XDebugSocket::getBaseSharedPointer()
{
    return this->self_pointer;
}


void XDebugSocket::finish()
{
    this->queue_lock.lock();
    this->is_ready_to_send = false;
    this->queue_lock.unlock();
    std::cout << "\n Called XDebugSocket::finish\n";
    this->new_breakpoint.reset();
    getXDebugOpenSessions()->removeSession(this->session_id);
    this->self_pointer.reset();
    XDebugSocketBase::finish();
}