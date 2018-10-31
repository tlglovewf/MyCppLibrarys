
#include "Poco/Net/HTTPServer.h"   //继承自TCPServer 实现了一个完整的HTTP多线程服务器
#include "Poco/Net/HTTPRequestHandler.h"   //抽象基类类 被HttpServer所创建 用来处理Http的请求
#include "Poco/Net/HTTPRequestHandlerFactory.h" //HTTPRequestHandler的工厂 给予工厂设计模式
#include "Poco/Net/HTTPServerParams.h"    //被用来指定httpserver以及HTTPRequestHandler的参数
#include "Poco/Net/HTTPServerRequest.h"  //ServerRequest的抽象子类用来指定 服务器端的 http请求
#include "Poco/Net/HTTPServerResponse.h" //ServerResponse的抽象子类用来指定服务器端的http响应
#include "Poco/Net/ServerSocket.h"  //提供了一个TCP服务器套接字接口
#include "Poco/Net/WebSocket.h"  //这个类实现了RFC 6455 web套接字接口  专门针对web服务器用
#include "Poco/Net/NetException.h"  //网络异常
#include "Poco/Util/ServerApplication.h" //Application的子类 所有服务器程序 包括 Reactor FTP HTTP等都用到 算是服务器的启动类
#include "Poco/Util/Option.h"  //存储了命令行选项
#include "Poco/Util/OptionSet.h"  //一个Opention对象的集合
#include "Poco/Util/HelpFormatter.h" //从OptionSet格式化帮助信息
#include "Poco/Format.h"  //格式化函数的实现类似于 C的 sprintf函数 具体看文档
#include <iostream> using Poco::Net::ServerSocket;
using Poco::Net::WebSocket;
using Poco::Net::WebSocketException;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPRequestHandlerFactory;
using Poco::Net::HTTPServer;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPServerResponse;
using Poco::Net::HTTPServerParams;
using Poco::Timestamp;
using Poco::ThreadPool;
using Poco::Util::ServerApplication;
using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;


/*
 *  需要浏览器支持html5 中的websocket
 *
 */




//////页面处理器 链接到来的时候 直接打印html内容
class PageRequestHandler: public HTTPRequestHandler
/// Return a HTML document with some JavaScript creating
/// a WebSocket connection.
{
public:
void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    
    response.setChunkedTransferEncoding(true);
    response.setContentType("text/html");
    std::ostream& ostr = response.send();
    ostr << "<html>";
    ostr << "<head>";
    ostr << "<title>WebSocketServer</title>";
    ostr << "<script type=\"text/javascript\">";
    ostr << "function WebSocketTest()";
    ostr << "{";
    ostr << "  if (\"WebSocket\" in window)";
    ostr << "  {";
    ostr << "    var ws = new WebSocket(\"ws://" << request.serverAddress().toString() << "/ws\");";
    ostr << "    ws.onopen = function()";
    ostr << "      {";
    ostr << "        alert(\"Web socket open.\");";
    ostr << "        ws.send(\"Hello, world!\");";
    ostr << "      };";
    ostr << "    ws.onmessage = function(evt)";
    ostr << "      { ";
    ostr << "        var msg = evt.data;";
    ostr << "        alert(\"Message received: \" + msg);";
    ostr << "        ws.close();";
    ostr << "      };";
    ostr << "    ws.onclose = function()";
    ostr << "      { ";
    ostr << "        alert(\"Web socket close.\");";
    ostr << "      };";
    ostr << "  }";
    ostr << "  else";
    ostr << "  {";
    ostr << "     alert(\"This browser does not support WebSockets.\");";
    ostr << "  }";
    ostr << "}";
    ostr << "</script>";
    ostr << "</head>";
    ostr << "<body>";
    ostr << "  <h1>WebSocket Server</h1>";
    ostr << "  <p><a href=\"javascript:WebSocketTest()\">Run WebSocket Script</a></p>";
    ostr << "</body>";
    ostr << "</html>";
}
};

/////////http请求处理器 1
class WebSocketRequestHandler: public HTTPRequestHandler
/// Handle a WebSocket connection.
{
public:
void handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{
    Application& app = Application::instance();
    try
    {
        WebSocket ws(request, response);
        app.logger().information("WebSocket connection established.");
        char buffer[1024];
        int flags;
        int n;
        do
        {
            n = ws.receiveFrame(buffer, sizeof(buffer), flags);
            app.logger().information(Poco::format("Frame received (length=%d, flags=0x%x).", n, unsigned(flags)));
            char str[]="server get information.";
//            ws.sendFrame(buffer, n, flags);
            ws.sendFrame(str, strlen(str));
        }
        while (n > 0 || (flags & WebSocket::FRAME_OP_BITMASK) != WebSocket::FRAME_OP_CLOSE);
        app.logger().information("WebSocket connection closed.");
    }
    catch (WebSocketException& exc)
    {
        app.logger().log(exc);
        switch (exc.code())
        {
            case WebSocket::WS_ERR_HANDSHAKE_UNSUPPORTED_VERSION:
                response.set("Sec-WebSocket-Version", WebSocket::WEBSOCKET_VERSION);
                // fallthrough case WebSocket::WS_ERR_NO_HANDSHAKE:
            case WebSocket::WS_ERR_HANDSHAKE_NO_VERSION:
            case WebSocket::WS_ERR_HANDSHAKE_NO_KEY:
                response.setStatusAndReason(HTTPResponse::HTTP_BAD_REQUEST);
                response.setContentLength(0);
                response.send();
                break;
        }
    }
}
};

//HTTP请求处理器工厂
class RequestHandlerFactory: public HTTPRequestHandlerFactory
{
public:
    HTTPRequestHandler* createRequestHandler(const HTTPServerRequest& request) //覆盖方法
    {
        Application& app = Application::instance();
        app.logger().information("Request from " + request.clientAddress().toString()
                                 + ": " + request.getMethod()
                                 + " " + request.getURI()
                                 + " " + request.getVersion());
        
        //打印所有链接请求
        for (HTTPServerRequest::ConstIterator it = request.begin(); it != request.end(); ++it)
        {
            app.logger().information(it->first + ": " + it->second);
        }
        //可以选择输出html页面 或者不输出  通过http请求参数

        bool bfind = request.find("Upgrade") != request.end();
        if( bfind && request["Upgrade"] == "websocket")
            return new WebSocketRequestHandler;
        else return new PageRequestHandler;
    }
};


//////////网络应用程序的启动器 都是ServerApplication
class WebSocketServer: public Poco::Util::ServerApplication {
public:
    WebSocketServer(): _helpRequested(false)
    {
    }
    
    ~WebSocketServer()
    {
    }
    
protected:
    ///再启动的时候先调用
    void initialize(Application& self)
    {
        loadConfiguration(); // load default configuration files, if present         ServerApplication::initialize(self);
    }
    //在释放的时候调用
    void uninitialize()
    {
        ServerApplication::uninitialize();
    }
    
    ///覆盖基类的函数 定义一些命令选项
    void defineOptions(OptionSet& options)
    {
        ServerApplication::defineOptions(options);
        
        options.addOption(
                          Option("help", "h", "display help information on command line arguments")
                          .required(false)
                          .repeatable(false));
    }
    ///处理命令行选项
    void handleOption(const std::string& name, const std::string& value)
    {
        ServerApplication::handleOption(name, value);
        
        if (name == "help")
            _helpRequested = true;
    }
    
    //打印帮助
    void displayHelp()
    {
        HelpFormatter helpFormatter(options());
        helpFormatter.setCommand(commandName());
        helpFormatter.setUsage("OPTIONS");
        helpFormatter.setHeader("A sample HTTP server supporting the WebSocket protocol.");
        helpFormatter.format(std::cout);
    }
    //在初始化完毕之后调用
    int main(const std::vector<std::string>& args)
    {
        if (_helpRequested)
        {
            displayHelp();
        }
        else
        {
            // get parameters from configuration file
            //从配置文件中获取端口
            unsigned short port = (unsigned short) config().getInt("WebSocketServer.port", 9980);
            
            // 安装一个ServerSocket
            Poco::Net::ServerSocket svs(port);
            // s安装一个HttpServer实例  并且传递 请求处理器工厂  和一个HttpServerParams对象
            HTTPServer srv(new RequestHandlerFactory, svs, new HTTPServerParams);
            // 启动服务器
            srv.start();
            // 等待kill 或者控制台CTRL+C
            waitForTerminationRequest();
            // 停止HTTP服务器
            srv.stop();
        }
        return Application::EXIT_OK;  //返回正常退出状态
    }
    
private:
    bool _helpRequested;
};


//启动web 服务器
POCO_SERVER_MAIN(WebSocketServer)

