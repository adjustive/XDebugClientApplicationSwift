//
//  XDebugSocketBase.cpp
//  XDebugClientCLI
//
//  Created by Elijah on 12/25/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#include "XDebugSocketBase.hpp"
#include <cstdlib>
#include <stdio.h>
#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <fstream>
#include "Model/DebuggerCommand.cpp"

using boost::asio::ip::tcp;

XDebugSocketBase::XDebugSocketBase(boost::asio::io_service& io_service): socket_(io_service)
    {
    };

tcp::socket& XDebugSocketBase::socket()
    {
        return socket_;
    };
    
void XDebugSocketBase::start()
{
    if (!this->pointer_is_init)
    {
        this->pointer_is_init = true;
        this->base_ptr = this->getBaseSharedPointer();
    }
    tcp::socket* socket = &this->socket_;
    boost::asio::ip::tcp::endpoint remote_ep = socket->remote_endpoint();
    boost::asio::ip::address remote_ad = remote_ep.address();
    std::string s = remote_ad.to_string();
    //std::cout << "\nRemote IP: " << s << "\n";
    
        socket_.async_read_some(boost::asio::buffer(data_, max_buffer_size),
                                boost::bind(&XDebugSocketBase::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }
    
void XDebugSocketBase::handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred)
{
        //std::cout << "\n XDebugSocketBase::handle_read\n";
        if (!error)
        {
            bool continue_session = true;
            char* a = boost::asio::buffer_cast<char*>(boost::asio::buffer(data_, bytes_transferred));
            int null_pos = 0;
            for (int i=1;i<10;i++)
            {
                if (a[i]==0)
                {
                    null_pos = i;
                    break;
                }
            }
            if (!null_pos)
            {
                std::cout << "\nError 45555554\n";
                this->finish();
                throw "Received invalid transmission";
                return; // fail silently
            }
            char* buffered_payload = &a[null_pos+1];
            int xdebug_bytes_sent = std::atoi(a) + 1; // sends an additional null byte
            int xdebug_max_bytes_in_buffer = max_buffer_size - null_pos - 1;
            if (xdebug_bytes_sent <= xdebug_max_bytes_in_buffer)
            {
                buffered_payload[xdebug_bytes_sent] = 0; // append null byte
                continue_session = this->processStringRead(buffered_payload, xdebug_bytes_sent);
            } else {
                if (xdebug_bytes_sent > 1000000*100) // 100 mb
                {
                    std::cout << "\nMax number of bytes was exceeded (Message 1)\n";
                    this->finish();
                    std::cout << "\nMax number of bytes was exceeded (Message 2)\n";
                    throw "Max number of bytes exceeded";
                    return; // set a max packet size of 50 MB
                }
                /* BUILD A NEW BUFFER */
                char* new_buffer_data = new char[xdebug_bytes_sent+1];
                int curr_pos = 0;
                memccpy(new_buffer_data, buffered_payload, curr_pos, max_buffer_size);
                /* READ THE REST OF THE STRING INTO THE BUFFER UPTO (x) bytes */
                boost::system::error_code error_i;
                int max_new_buffer_size = (int)(xdebug_bytes_sent - xdebug_max_bytes_in_buffer);
                std::size_t bytes_read_i = 0;
                int loop_protection = 0;
                while(bytes_read_i < max_new_buffer_size)
                {
                    loop_protection++;
                    boost::asio::mutable_buffers_1 new_buffer = boost::asio::buffer(&new_buffer_data[xdebug_max_bytes_in_buffer+bytes_read_i], max_new_buffer_size - bytes_read_i);
                    
                    bytes_read_i += socket_.read_some(new_buffer, error_i);
                    //std::cout << "\nLoop Size (line 81): " << loop_protection << "\n";
                    
                    if (loop_protection >= 100)
                    {
                        std::cout << "\nError 88: buffer issue, possible infinite loop\n";
                        this->finish();
                        return;
                    }
                    
                }
                int bytes_read_total = (int)(xdebug_max_bytes_in_buffer + bytes_read_i);
                if (!error_i && bytes_read_total == xdebug_bytes_sent) // or bytes_read_i == max_new_buffer_size
                {
                    new_buffer_data[xdebug_bytes_sent] = 0; // append null byte just in case
                    continue_session = this->processStringRead(new_buffer_data, xdebug_bytes_sent);
                    delete new_buffer_data;
                } else {
                    delete new_buffer_data;
                    continue_session = true;
                }
                // no code can go here (possible session closure)
            }
            // no code can go here (possible session closure)
            if (!continue_session)
            {
                return this->finish();
            }
        }
        else
        {
            return this->finish();
        }
        this->start(); // continue reading
    }
    
    bool XDebugSocketBase::write(std::string* data)
    {
        boost::system::error_code error;
        boost::asio::const_buffers_1 new_buffer = boost::asio::buffer(data->c_str(), data->length()+1);
        //socket_.write_some(new_buffer, error);
        boost::asio::async_write(socket_,new_buffer,
            boost::bind(&XDebugSocketBase::handle_write, this, boost::asio::placeholders::error));
        if (!error)
        {
           // new_buffer = boost::asio::buffer(&a, 1);
         //   socket_.write_some(new_buffer,error);
        }
        if (error)
        {
            this->finish();
            return false;
        }
        
        return true;
    }
    
    void XDebugSocketBase::handle_write(const boost::system::error_code& error)
    {
        // do not use 2 handle_write operations in one function, as the object may be deleted
        if (error)
        {
            this->finish();
        }
    }
    
    bool XDebugSocketBase::processStringRead(const char* s, int length)
    {
        std::cout << s << "\n";
        return false; // to be overridden
    }
    
    void XDebugSocketBase::finish()
    {
        try {
            this->closeSocket();
        } catch (boost::system::system_error e)
        {
            std::cout << "Caught error in ::finish boost::system::system_error: " << e.what() << std::endl;
        } catch (std::exception e)
        {
            std::cout << "Caught error in ::finish std::exception: " << e.what() << std::endl;
        } catch (...)
        {
            std::cout << "Caught error in ::finish of unknown type" << std::endl;
        }
        if (this->pointer_is_init)
        {
            this->base_ptr.reset();
        }
    }

std::shared_ptr<XDebugSocketBase> XDebugSocketBase::getBaseSharedPointer()
{
    return std::shared_ptr<XDebugSocketBase>(this);
}
    
XDebugSocketBase::~XDebugSocketBase()
    {
        std::cout << "\n\nXDebugSocket Session Object Destroyed\n\n";
    }

void XDebugSocketBase::closeSocket()
{
    this->socket_.cancel();
    try {
        this->socket_.shutdown(boost::asio::socket_base::shutdown_both);// boost::system::system_error e
    } catch (...)
    {
        // if the connection is open, it must be closed
        // but not possible to know if it is open or closed
        // so this function will run with error in the case where it is already closed
    }
    this->socket_.close(); // this error is also caught, but printed
}