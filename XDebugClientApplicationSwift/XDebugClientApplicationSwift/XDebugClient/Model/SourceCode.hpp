//
//  SourceCode.hpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/3/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#ifndef SourceCode_hpp
#define SourceCode_hpp

#include <stdio.h>
#include <string>

class SourceCode
{
public:
    SourceCode(const std::string &filename, const std::string &code);
    const std::string getCodeString();
    const bool isFilename(const std::string &filename);
    ~SourceCode();
private:
    std::string code;
    std::string filename;
};

#endif /* SourceCode_hpp */
