//
//  XDebugSessionBase.hpp
//  XDebugClientCLI
//
//  Created by Elijah on 12/25/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#ifndef XDebugSessionBase_h
#define XDebugSessionBase_h


#include <boost/asio.hpp>


class XDebugSessionBase
{
public:
    XDebugSessionBase(boost::asio::io_service& io_service);
    
    
    void start();
    bool write(const char* data,int length);
    boost::asio::ip::tcp::socket& socket();
    
    
    void processStringRead(const char* s, int length);
};


#endif /* XDebugSessionBase_h */

