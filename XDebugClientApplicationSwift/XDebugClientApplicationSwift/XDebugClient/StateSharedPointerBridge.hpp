//
//  SharedPointerBridge.hpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/11/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#ifndef SharedPointerBridge_hpp
#define SharedPointerBridge_hpp

#include <stdio.h>
#include <string>

class BreakpointState;

class StateSharedPointerBridge
{
public:
    StateSharedPointerBridge(void* shared_ptr);
    void* getVariablesPointer(int stack_index);

    void* getStackPointer();
    
    std::string getSourceCode(const std::string &filename);
    std::string getSourceCodeFilename(int stack_index);
    int getLineNumber(int stack_index);

    std::string getOutputStream(int stream_index);
    
private:
    BreakpointState* getState(int history_index);
    BreakpointState* state;
};

#endif /* SharedPointerBridge_hpp */
