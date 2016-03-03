//
//  SharedPointerBridge.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/11/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#include "StateSharedPointerBridge.hpp"
#include "Model/Variable.hpp"
#include "Model/BreakpointState.hpp"
#include "XDebugSession.hpp"

StateSharedPointerBridge::StateSharedPointerBridge(void* shared_ptr)
{
    std::shared_ptr<BreakpointState>* a = (std::shared_ptr<BreakpointState>*)shared_ptr;
    BreakpointState* c = a->get();
    if (c)
    {
        this->state = c;
    } else {
        // throw error here
        std::cout << "\nError 46\n";
        throw "Error - pointer is 0 or has been reset";
    }
}

void* StateSharedPointerBridge::getVariablesPointer(int stack_index)
{
    return &this->state->context_variables;
}

void* StateSharedPointerBridge::getStackPointer()
{
    return &this->state->stack;
}

std::string StateSharedPointerBridge::getSourceCodeFilename(int stack_index)
{
    int stack_size = this->state->stack.getStackSize();
    if (stack_index < stack_size)
    {
        const StackLineItem* stackLine = this->state->stack.getItemAtIndex(stack_index);
        if (stackLine)
        {
            std::string filename = stackLine->filename;
            return filename;
        }
    }
    return "";
}

std::string StateSharedPointerBridge::getSourceCode(const std::string &filename)
{
    std::shared_ptr<XDebugSession> session = this->state->session_ptr.lock();
    if (session)
    {
        if (session->hasSourceForFile(filename))
        {
            std::shared_ptr<SourceCode> sourceCode = session->getSourceForFile(filename);
            return sourceCode->getCodeString();
        }
    }
    return "";
}

int StateSharedPointerBridge::getLineNumber(int stack_index)
{
    int stack_size = this->state->stack.getStackSize();
    if (stack_index < stack_size)
    {
        const StackLineItem* stackLine = this->state->stack.getItemAtIndex(stack_index);
        if (stackLine)
        {
            return stackLine->line_number;
        }
    }
    return 0;
}

std::string StateSharedPointerBridge::getOutputStream(int stream_index)
{
    // stream 1 == stdout
    // stream 2 == stdin
    return "";
}

