//
//  Stack.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/9/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#include "Stack.hpp"
#include "pugixml.hpp"


void Stack::addToStack(StackLineItem* item)
{
    this->stack.push_back(item);
}


const StackLineItem* Stack::getItemAtIndex(int i)
{
    return this->stack[i];
}

int Stack::getStackSize()
{
    return (int)this->stack.size();
}


void Stack::addToStackFromXMLNode(const pugi::xml_node node)
{
    for (pugi::xml_node child_i = node.first_child(); child_i; child_i = child_i.next_sibling())
    {
        StackLineItem* stack_line = new StackLineItem();
        stack_line->initFromXMLNode(child_i);
        this->addToStack(stack_line);
    }
}

Stack::~Stack()
{
    for (std::vector<StackLineItem*>::iterator it = this->stack.begin() ; it != this->stack.end(); ++it)
    {
        delete (*it);
    }
    
}