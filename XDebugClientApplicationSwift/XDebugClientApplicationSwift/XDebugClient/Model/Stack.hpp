//
//  Stack.hpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/9/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#ifndef Stack_hpp
#define Stack_hpp

#include <vector>
#include "StackLineItem.hpp"

namespace pugi
{
    class xml_node;
}

class Stack
{
public:
    void addToStackFromXMLNode(const pugi::xml_node node);
    const StackLineItem* getItemAtIndex(int i);
    int getStackSize();
    ~Stack();
private:
    std::vector<StackLineItem*> stack;
    void addToStack(StackLineItem *item);
};

#endif /* Stack_hpp */
