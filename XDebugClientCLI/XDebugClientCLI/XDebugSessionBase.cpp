//
//  XDebugSessionBase.cpp
//  XDebugClientCLI
//
//  Created by Elijah on 12/25/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#include <cstdlib>
#include <stdio.h>
#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <fstream>

using boost::asio::ip::tcp;

class XDebugSessionBase
{
public:
    XDebugSessionBase(boost::asio::io_service& io_service)
    : socket_(io_service)
    {
    }
    
    tcp::socket& socket()
    {
        return socket_;
    }
    
    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_buffer_size),
                                boost::bind(&XDebugSessionBase::handle_read, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
    }
    
    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred)
    {
        if (!error)
        {
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
                return; // fail silently
            }
            char* buffered_payload = &a[null_pos+1];
            int xdebug_bytes_sent = std::atoi(a) + 1; // sends an additional null byte
            int xdebug_max_bytes_in_buffer = max_buffer_size - null_pos - 1;
            if (xdebug_bytes_sent <= xdebug_max_bytes_in_buffer)
            {
                buffered_payload[xdebug_bytes_sent] = 0; // append null byte
                this->processStringRead(buffered_payload, xdebug_bytes_sent);
            } else {
                if (xdebug_bytes_sent > 1024*50)
                {
                    return; // set a max packet size of 50 MB
                }
                /* BUILD A NEW BUFFER */
                char* new_buffer_data = new char[xdebug_bytes_sent+1];
                int curr_pos = 0;
                memccpy(new_buffer_data, buffered_payload, curr_pos, max_buffer_size);
                /* READ THE REST OF THE STRING INTO THE BUFFER UPTO (x) bytes */
                boost::system::error_code error_i;
                int max_new_buffer_size = (int)(xdebug_bytes_sent - xdebug_max_bytes_in_buffer);
                boost::asio::mutable_buffers_1 new_buffer = boost::asio::buffer(&new_buffer_data[xdebug_max_bytes_in_buffer], max_new_buffer_size);
                std::size_t bytes_read_i = socket_.read_some(new_buffer, error_i);
                int bytes_read_total = (int)(xdebug_max_bytes_in_buffer + bytes_read_i);
                if (!error_i && bytes_read_total == xdebug_bytes_sent)
                {
                    new_buffer_data[xdebug_bytes_sent] = 0; // append null byte just in case
                    this->processStringRead(new_buffer_data, xdebug_bytes_sent);
                    delete new_buffer_data;
                } else {
                    delete new_buffer_data;
                    this->finish();
                    return;
                }
            }
            
            
            //std::printf("__%lu___",b->length());
            //std::printf("__%lu___\n",sizeof(char));
            std::string cc = "status -i transaction_id";
            if ((true))
            {
                if (!this->write(cc.c_str(), (int)cc.length()))
                {
                    return;
                }
                //std::ofstream outfile;
                //outfile.open("/tmp/test.txt", std::ios_base::app);
                
                //boost::asio::async_write(socket_,
                //                       boost::asio::buffer(cc, cc.size()),
                //                     boost::bind(&session::handle_write, this,
                //                               boost::asio::placeholders::error));
            }
        }
        else
        {
            this->finish();
            return;
        }
        this->start(); // continue reading
    }
    
    bool write(const char* data,int length)
    {
        boost::system::error_code error;
        boost::asio::const_buffers_1 new_buffer = boost::asio::buffer(data, length);
        socket_.write_some(new_buffer, error);
        const char a = 0;
        if (!error)
        {
            new_buffer = boost::asio::buffer(&a, 1);
            socket_.write_some(new_buffer,error);
        }
        if (error)
        {
            this->finish();
            return false;
        }
        return true;
    }
    
    void handle_write(const boost::system::error_code& error)
    {
        // do not use 2 handle_write operations in one function, as the object may be deleted
        if (error)
        {
            this->finish();
        }
    }
    
    virtual void processStringRead(const char* s, int length)
    {
        std::cout << s << "\n";
    }
    
    void finish()
    {
        delete this;
    }
    
private:
    tcp::socket socket_;
    enum { max_buffer_size = 10 };
    char data_[max_buffer_size+1]; // add one to allow appending of null byte
};