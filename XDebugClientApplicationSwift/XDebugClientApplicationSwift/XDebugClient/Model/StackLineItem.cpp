//
//  StackLineItem.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/9/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#include "StackLineItem.hpp"
#include <boost/algorithm/string.hpp>
#include "pugixml.hpp"


const std::string StackLineItem::getFormattedString() const
{
    std::string filename_formatted = this->filename;
    boost::erase_all(filename_formatted, "file://");
    return this->where + " " + filename_formatted + " " + std::to_string(this->line_number);
}

void StackLineItem::initFromXMLNode(const pugi::xml_node node)
{
    // should be only one property tag
    const char* tag_name_i = node.name();
    if (strcmp(tag_name_i, "stack")==0)
    {
        const char* where = node.attribute("where").value();
        const char* level_char = node.attribute("level").value();
        //char* type  = child_i.attribute("file");
        const char* filename = node.attribute("filename").value();
        const char* lineno_char = node.attribute("lineno").value();
        
        this->filename = std::string(filename);
        this->where = std::string(where);
        this->line_number = std::atoi(lineno_char);
        this->level = std::atoi(level_char);
        
    } else {
        throw "Invaid data in stack_get error 215";
    }
}