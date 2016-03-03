//
//  main.cpp
//  XDebugClientCLI
//
//  Created by Elijah on 12/24/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#include <iostream>
#include <cstdlib>
#include "server.cpp"

void testXML();

class AAA {
public:
    int a;
};

int main(int argc, const char* argv[])
{
    std::shared_ptr<AAA> a = std::shared_ptr<AAA>(new AAA());
    std::weak_ptr<AAA> weak_ptr = a;
    
    
    AAA* b = a.get();
    std::shared_ptr<AAA> second_strong_ptr = std::shared_ptr<AAA>(b);
    a.reset();
    b = a.get();
    return 1;
    
    testXML();
    try
    {
        
        boost::asio::io_service io_service;
        
        using namespace std; // For atoi.
        server s(io_service, std::atoi("9000"));
        
        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }
    
    return 0;
}



