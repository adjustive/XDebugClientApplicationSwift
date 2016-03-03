//
//  XMLTest.cpp
//  XDebugClientCLI
//
//  Created by Elijah on 12/25/15.
//  Copyright © 2015 elijah. All rights reserved.
//

#include <stdio.h>
#include <string>
#include <iostream>
#include "pugixml.hpp"


void parseXML(pugi::xml_document &doc);
void testXML()
{
    std::string a = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>    <init xmlns=\"urn:debugger_protocol_v1\" xmlns:xdebug=\"http://xdebug.org/dbgp/xdebug\" fileuri=\"file:///Users/elijah/Documents/eclipse_workspace/OtherProjects/OtherProjects/auto_prepend.php\" language=\"PHP\" protocol_version=\"1.0\" appid=\"1956\" idekey=\"ECLIPSE_DBGP\">    <engine version=\"2.3.2\"><![CDATA[Xdebug]]></engine>    <author><![CDATA[Derick Rethans]]></author>    <url><![CDATA[http://xdebug.org]]></url>                  <copyright><![CDATA[Copyright (c) 2002-2015 by Derick Rethans]]></copyright>                  </init>";
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_buffer(a.c_str(), a.size());
    if (result)
    {
        parseXML(doc);
    } else {
        std::cout << "Error 123";
    }
}

void parseXML(pugi::xml_document &doc)
{
    pugi::xml_node root_node = doc.root();
    if (root_node)
    {
        pugi::xml_node first_child = root_node.first_child();
        if (strcmp(first_child.name(),"init") == 0)
        {
            //fileuri attribute, appid, idekey
        }
    }
}