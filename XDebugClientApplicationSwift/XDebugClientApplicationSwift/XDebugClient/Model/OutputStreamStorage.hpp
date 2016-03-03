//
//  OutputStreamStorage.hpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/1/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#ifndef OutputStreamStorage_hpp
#define OutputStreamStorage_hpp

#include <stdio.h>
#include <string>
#include <mutex>

class OutputStreamStorage
{
public:
    const std::string getContents();
    const int getLength();
    void appendString(std::string const &input);
    void appendString(const char &input);
    void appendStringBase64(const char * base64);
private:
    std::string string;
    std::mutex lock;
};

#endif /* OutputStreamStorage_hpp */
