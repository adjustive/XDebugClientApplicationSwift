//
//  XDebugSession.hpp
//  XDebugClientCLI
//
//  Created by Elijah on 12/25/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#ifndef XDebugSession_hpp
#define XDebugSession_hpp


#include "XDebugSessionBase.hpp"



class XDebugSession: public XDebugSessionBase
{
public:
    XDebugSession(boost::asio::io_service& io_service);
};


#endif /* XDebugSession_hpp */

