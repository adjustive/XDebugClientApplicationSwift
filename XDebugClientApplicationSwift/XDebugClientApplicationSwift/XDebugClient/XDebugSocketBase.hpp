//
//  XDebugSocketBase.hpp
//  XDebugClientCLI
//
//  Created by Elijah on 12/25/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#ifndef XDebugSocketBase_h
#define XDebugSocketBase_h


#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class XDebugSocketBase
{
public:
    XDebugSocketBase(boost::asio::io_service& io_service); //: socket_(io_service)
    
    tcp::socket& socket();
    
    void start();
    
    ~XDebugSocketBase();
    bool write(std::string* data);
protected:
    virtual bool processStringRead(const char* s, int length);
    virtual void finish();
    void closeSocket();
private:
    virtual std::shared_ptr<XDebugSocketBase> getBaseSharedPointer(); // must be overwritten
    bool pointer_is_init = 0;
    std::shared_ptr<XDebugSocketBase> base_ptr;
    void handle_read(const boost::system::error_code& error,
                     size_t bytes_transferred);
    
    
    void handle_write(const boost::system::error_code& error);
    
    tcp::socket socket_;
    enum { max_buffer_size = 10 };
    char data_[max_buffer_size+1]; // add one to allow appending of null byte
};

#endif /* XDebugSocketBase_h */

