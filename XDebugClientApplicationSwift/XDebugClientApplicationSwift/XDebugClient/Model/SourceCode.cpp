//
//  SourceCode.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/3/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#include "SourceCode.hpp"
#include <iostream>

SourceCode::SourceCode(const std::string &filename, const std::string &code)
{
    this->filename = filename;
    this->code = code;
}

const std::string SourceCode::getCodeString()
{
    return this->code;
}

const bool SourceCode::isFilename(const std::string &filename)
{
    if (this->filename.compare(filename) == 0)
    {
        return true;
    } else {
        return false;
    }
}

SourceCode::~SourceCode()
{
    std::cout << "\nSource Code Object destroyed\n";
}