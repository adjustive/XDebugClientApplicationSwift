//
//  XDebugSession.cpp
//  XDebugClientCLI
//
//  Created by Elijah on 12/25/15.
//  Copyright © 2015 elijah. All rights reserved.
//

//#include "XDebugSession.hpp"
#include "XDebugSessionBase.cpp"
#include <sstream>
#include <string>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>

using boost::property_tree::ptree;

class XDebugSession: public XDebugSessionBase
{
public:
    XDebugSession(boost::asio::io_service& io_service) : XDebugSessionBase(io_service)
    {
        
    }
    
    void processStringRead(const char* s, int length)
    {
        ptree pt;
        std::string string = std::string(s);
        std::istringstream input = std::istringstream(string);
        
        try {
            boost::property_tree::xml_parser::read_xml(input,pt);
            this->processXMLPropertyTree(pt);
        } catch (...)
        {
            return;
        }
        int a = 1;
    }
    
    void processXMLPropertyTree(ptree &pt)
    {
        //ptree::const_iterator end = pt.end();
        //for (ptree::const_iterator it = pt.begin(); it != end; ++it) {
            //std::cout << it->first << ": " << it->second.get_value<std::string>() << std::endl;
            //print(it->second);
        //}
        
    }
};