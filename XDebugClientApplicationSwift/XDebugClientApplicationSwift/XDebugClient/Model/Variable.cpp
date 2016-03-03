//
//  Variable.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/3/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#include "Variable.hpp"
#include "pugixml.hpp"
#include "c_utility_functions.hpp"
#include <iostream>
#include "BreakpointState.hpp"

void Variable::setRootNode()
{
    this->is_expanded = true;
}

void Variable::initWithXMLNode(const pugi::xml_node &node, int depth_from_top)
{
    if (depth_from_top > 50)
    {
        std::cout << "Large loop error 18";
        return;
    }
    const char* tag_name = node.name();
    if(strcmp(tag_name, "property") == 0)
    {
        // init the property
        const char* name = node.attribute("name").value();
        const char* type = node.attribute("type").value();
        const char* fullname = node.attribute("fullname").value();

        // NAME
        if (name && *name)
        {
            this->name = new std::string(name);
        }
        
        // FULLNAME
        if (type && *type)
        {
            this->full_name = new std::string(fullname);
        }

        // TYPE
        if (type && *type)
        {
            // TYPE OR CLASSNAME
            const char* classname = node.attribute("classname").value();
            if (type && strcmp(type, "object") == 0 && classname && classname[0])
            {
                this->value = new std::string(classname);
            }
            // ADJUST UNINITIALIZED STRING OR SET TYPE NAME
            if (strcmp(type, "uninitialized") == 0)
            {
                this->type = new std::string("-");
            } else {
                this->type = new std::string(type);
            }
        }
        
        const char* encoding = node.attribute("encoding").value();
        pugi::xml_node value_node = node.first_child();
        if (!value_node.empty() && !this->value)
        {
            if (strcmp(encoding, "base64") == 0)
            {
                const char* valuenode_type = value_node.name();
                if (valuenode_type && !*valuenode_type)
                {
                    std::string value_base64 = value_node.value();
                    if (this->value)
                    {
                        delete this->value; // shouldn't reach here (value set with classname)
                    }
                    try {
                        this->value = new std::string(decode64(value_base64));
                    } catch (...)
                    {
                        std::cout << "\nBase64 decode error - line 29\n";
                    }
                }
            } else if(encoding[0] == 0)
            {
                this->value = new std::string(value_node.value());
            }
        }
    }
    //bool is_response_tag = strcmp(tag_name, "response") == 0;
    // check if the property has children
    pugi::xml_attribute count_attr = node.attribute("numchildren");
    if (count_attr)
    {
        const char* num_children_char = count_attr.value();
        if (num_children_char && *num_children_char)
        {
            num_children = atoi(num_children_char);
        }
    }
    if (num_children && !node.first_child().empty())
    {
        this->child_elements = new std::vector<Variable*>();
        this->child_elements->reserve(num_children);
        for (pugi::xml_node child_i = node.first_child(); child_i; child_i = child_i.next_sibling())
        {
            Variable* var_i = new Variable();
            var_i->initWithXMLNode(child_i, depth_from_top + 1);
            this->child_elements->push_back(var_i);
        }
        if (num_children != this->child_elements->size())
        {
            std::cout << "\nError 93: Mismatch of element sizes\n";
            num_children = (int)this->child_elements->size();
        }
    }
}

void Variable::appendNodeChildren(const pugi::xml_node &node)
{
    // INIT VECTOR IF NOT ALREADY INIT
    if (!this->child_elements)
    {
        this->child_elements = new std::vector<Variable*>();
    }
    /* COUNT VARIABLES */
    for (pugi::xml_node child_i = node.first_child(); child_i; child_i = child_i.next_sibling())
    {
        num_children++;
    }
    // RESERVE SPACE
    this->child_elements->reserve(num_children);
    // APPEND THE VARIABLE NODES
    for (pugi::xml_node child_i = node.first_child(); child_i; child_i = child_i.next_sibling())
    {
        Variable* var_i = new Variable();
        var_i->initWithXMLNode(child_i, 1);
        this->child_elements->push_back(var_i);
    }
    num_children = (int)this->child_elements->size();
}


Variable* Variable::appendEmptyVariable()
{
    // INIT VECTOR IF NOT ALREADY INIT
    if (!this->child_elements)
    {
        this->child_elements = new std::vector<Variable*>();
    }
    Variable* var_i = new Variable();
    this->child_elements->push_back(var_i);
    num_children = (int)this->child_elements->size();
    return var_i;
}


const bool Variable::hasChildrenAvailible()
{
    return num_children > 0 && child_elements && this->child_elements->size();
}

const std::string Variable::getName()
{
    if (this->name)
    {
        return *this->name;
    }
    return "";
}
const std::string Variable::getValue()
{
    if (this->value) {
        return *this->value;
    }
    return "";
}

const std::string Variable::getValueMaxChars(int max_chars)
{
    if (this->value) {
        return this->value->substr(0,max_chars);
    }
    return "";
}

const std::string Variable::getType()
{
    if (this->type)
    {
        return *this->type;
    }
    return "";
}


void Variable::setName(const std::string &str)
{
    if (this->name)
    {
        delete this->name;
    }
    this->name = new std::string(str);
}
void Variable::setValue(const std::string &str)
{
    if (this->value)
    {
        delete this->value;
    }
    this->value = new std::string(str);
}
void Variable::setType(const std::string str)
{
    if (this->type)
    {
        delete this->type;
    }
    this->type = new std::string(str);
}

std::vector<Variable*>* Variable::getChildElementsReference()
{
    return child_elements;
}

Variable::Variable()
{
    //std::cout << "\nVariable Init" << std::endl;
}

Variable::~Variable()
{
    //std::cout << "\nVariable destroyed" << std::endl;
    if (this->name)
    {
        delete this->name;
    }
    if (this->value)
    {
        delete this->value;
    }
    if (this->type)
    {
        delete this->type;
    }
    if (this->full_name)
    {
        delete this->full_name;
    }
    if (this->child_elements)
    {
        for (std::vector<Variable* >::iterator it = this->child_elements->begin() ; it != this->child_elements->end(); ++it)
        {
            delete (*it);
        }
        delete this->child_elements;
    }
}

void Variable::parseExpansionStates(const BreakpointState* previouState)
{
    if ((this->is_expanded && !this->full_name) || previouState->valueWasExpanded(this->full_name))
    {
        this->is_expanded = true;
        if (this->child_elements)
        {
            for (std::vector<Variable*>::iterator it = this->child_elements->begin() ; it != this->child_elements->end(); ++it)
            {
                (**it).parseExpansionStates(previouState);
            }
        }
    }
}

void Variable::updateExpansionState(BreakpointState* current_state,bool is_expanded)
{
    this->is_expanded = is_expanded;
    if (current_state && this->full_name)
    {
        if (is_expanded)
        {
            current_state->didExpandVariableCallback(this->full_name);
        } else {
            current_state->didContractVariableCallback(this->full_name);
        }
    }
}