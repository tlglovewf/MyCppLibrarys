//
//  main.cpp
//  FileTest
//
//  Created by TuLigen on 2018/10/31.
//  Copyright © 2018年 TuLigen. All rights reserved.
//

#if 0

#include <iostream>
#include <Poco/Path.h>
#include <Poco/File.h>

using namespace Poco;
int main(int argc, const char * argv[]) {
    // insert code here...
    
    Path path("/users/TLG/Downloads/somethings/web.html",Path::PATH_NATIVE);
    
    std::cout << path.getNode() << " " << path.getFileName() << " " << path.getExtension() << std::endl;
    
    if(path.isAbsolute())
    {
        std::cout << Path::current() << std::endl;
        File file(path.toString());
        
        if(file.exists() && file.isFile())
        {
            Timestamp t = file.getLastModified();
            double year = t.utcTime() / 1e7 / 3600.0 / 24.0 / 365;
            int y = 1582 + (int)year;
            int days = (year - (int)year) * 365;
    
           std::cout << y << " " << days  << std::endl;
        }
        
    }
    
    return 0;
}
#else

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/SocketAddress.h"

int main(void)
{
    Poco::Net::ServerSocket sev(8080);
    
    for(;;)
    {
        Poco::Net::StreamSocket ss = sev.acceptConnection();
        Poco::Net::SocketStream str(ss);
        str << "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html><head><title>My 1st Web Server</title></head>"
        "<body><h1>Hello, world!</h1></body></html>"
        << std::flush;
    }
    return 0;
}
#endif
