//
//  XDebugHistoryItem.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/1/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#include "XDebugHistoryItem.hpp"
#include <string>

class XDebugHistoryItem
{
    int utime;
    std::string request_uri;
    bool was_accepted;
};