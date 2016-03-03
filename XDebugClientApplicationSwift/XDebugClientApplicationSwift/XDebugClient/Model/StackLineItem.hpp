//
//  StackLineItem.h
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/9/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#ifndef StackLineItem_h
#define StackLineItem_h

#include <string>

namespace pugi
{
    class xml_node;
}


class StackLineItem
{
public:
    const std::string getFormattedString() const;
    std::string filename;
    std::string where;
    int line_number;
    int level;
    void initFromXMLNode(const pugi::xml_node node);
};

#endif /* StackLineItem_h */
