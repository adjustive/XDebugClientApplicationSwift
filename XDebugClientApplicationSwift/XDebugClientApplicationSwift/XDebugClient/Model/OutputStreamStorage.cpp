//
//  OutputStreamStorage.cpp
//  XDebugClientApplicationSwift
//
//  Created by Elijah on 1/1/16.
//  Copyright © 2016 elijah. All rights reserved.
//

#include "OutputStreamStorage.hpp"
#include "c_utility_functions.hpp"
#include <iostream>

void OutputStreamStorage::appendStringBase64(const char * base64)
{
    try {
        std::string decoded = decode64(base64);
        this->appendString(decoded);
    } catch (...)
    {
        std::cout << "\nError 19\n";
        return;
    }
}

const std::string OutputStreamStorage::getContents()
{
    this->lock.lock();
    const std::string str = this->string;
    this->lock.unlock();
    return str;
}
const int OutputStreamStorage::getLength()
{
    this->lock.lock();
    const int len = (int)this->string.length();
    this->lock.unlock();
    return len;
}
void OutputStreamStorage::appendString(std::string const &input)
{
    this->lock.lock();
    this->string.append(input);
    this->lock.unlock();
}

void OutputStreamStorage::appendString(const char &input)
{
    this->lock.lock();
    this->string.append(&input);
    this->lock.unlock();
}