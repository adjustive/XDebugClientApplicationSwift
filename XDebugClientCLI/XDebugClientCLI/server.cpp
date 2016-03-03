//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "XDebugSession.cpp"
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <fstream>

using boost::asio::ip::tcp;



class server
{
public:
    server(boost::asio::io_service& io_service, short port)
    : io_service_(io_service),
    acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        XDebugSession* new_session = new XDebugSession(io_service_);
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    }
    
    void handle_accept(XDebugSession* new_session,
                       const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
            new_session = new XDebugSession(io_service_);
            acceptor_.async_accept(new_session->socket(),
                                   boost::bind(&server::handle_accept, this, new_session,
                                               boost::asio::placeholders::error));
        }
        else
        {
            delete new_session;
        }
    }
    
private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};

