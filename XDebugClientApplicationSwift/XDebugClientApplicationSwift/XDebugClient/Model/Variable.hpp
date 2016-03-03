//
//  Variable.hpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/3/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#ifndef Variable_hpp
#define Variable_hpp

#include <stdio.h>
#include <string>
#include <vector>

namespace pugi
{
    class xml_node;
}

class BreakpointState;

class Variable {
public:
    int num_children = 0;
    bool is_expanded = false;
    const bool hasChildrenAvailible();
    const std::string getName();
    const std::string getValue();
    const std::string getType();
    void setName(const std::string &str);
    void setValue(const std::string &str);
    void setType(const std::string str);
    const std::string getValueMaxChars(int max_chars);
    void initWithXMLNode(const pugi::xml_node &node, int depth_from_top);
    void appendNodeChildren(const pugi::xml_node &node);
    Variable* appendEmptyVariable();
    std::vector<Variable*>* getChildElementsReference();
    void parseExpansionStates(const BreakpointState* previouState);
    void setRootNode();
    void updateExpansionState(BreakpointState* current_state,bool is_expanded);
    ~Variable();
    Variable();
private:
    std::string* name = 0; // are references because they can be null
    std::string* value = 0;
    std::string* type = 0;
    std::string* full_name = 0;
    std::vector<Variable*>* child_elements = 0;
};


#endif /* Variable_hpp */
