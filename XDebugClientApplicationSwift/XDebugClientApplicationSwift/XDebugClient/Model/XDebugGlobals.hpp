//
//  XDebugGlobals.hpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/1/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#ifndef XDebugGlobals_hpp
#define XDebugGlobals_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <mutex>
#include "../DebuggerActions.h"

using std::string;
using std::vector;

class XDebugGlobals
{
public:
    const bool shouldAcceptSession(bool is_breakpoint,string const idekey);
    void acceptMessage(DebuggerActions action, int value, std::string str_value);
    void acceptMessage(DebuggerActions action, int value);
    const int getIntValue(DebuggerActions message);
private:
    string break_on_uri;
    string required_ide_key;
    bool require_idekey;
    bool break_on_next;
    int context_depth;
    std::mutex lock;
};

XDebugGlobals* getGlobals();

#endif /* XDebugGlobals_hpp */

