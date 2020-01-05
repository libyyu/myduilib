#include "HTTPHelper.h"
#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <cstdlib>
#include <string.h>


class MyRequestHandle : public IRequestHandle
{
public:
    ~MyRequestHandle()
    {
        
    }
    void OnDelete(void* userdata)
    {
        printf("Need Destroy MyRequestHandle\n");
    }
    void OnRequestFinished(bool success, const std::string& content, void* userdata)
    {
        if(!success)
        {
            std::cerr << "download error:" << content.c_str() << std::endl;
        }
        else
        {
            std::cout << "Download Finished!" << std::endl;
        }
    }
    int  OnProgress(double total_size, double handle_size, void* userdata)
    {
        long tmp = static_cast<long>(handle_size / total_size * 100);
        printf("\rdownload progress:%ld%%\n", tmp);
        
        return 0;
    }
};

static void On_HTTP_Request_Callback(bool success, const std::string& data, void* userdata)
{
    if (success)
    {
        std::ofstream outfile;
        outfile.open("baidu.html", std::ios_base::binary | std::ios_base::trunc);
        if (outfile.good()) outfile.write(data.c_str(), data.size());
    }
    else
        printf("error:%s\n", data.c_str());
}

void TestHTTPRequest()
{
    auto v = HTTPRequest::Create(true);
    HTTPRequest& http = *v;
    http.SetUrl("http://www.libyyu.com/lidengfeng");
    http.SetSSLVerify(true);
    http.SetTimeout(5000);
    http.SetResultCallback(On_HTTP_Request_Callback);
    http.SetRequestHeader("User-Agent:Mozilla/4.04[en](Win95;I;Nav)");
    http.Run(false, false);
}
void TestHTTPRequestDownload()
{
    auto v = HTTPRequest::Create(true);
    HTTPRequest& http = *v;
    static MyRequestHandle dc;
    http.SetUrl("https://curl.haxx.se/download/curl-7.61.1.tar.gz");
    http.SetSSLVerify(true);
    http.SetTimeout(5000);
    //http.SetDownloadRange(0, 3986003);
    const char* down_file = "./cur3.tar.gz";
    http.SetRequestHandle(&dc);
    http.SetDownloadPath(down_file);
    http.Run(true, true);
}

int main()
{
    TestHTTPRequest();
	TestHTTPRequestDownload();
    HTTPRequest::Tick(0);
    printf("exit \n");
#ifdef _WIN32
	system("pause");
#endif
    return 0;
}
