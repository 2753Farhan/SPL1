#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <curl/curl.h>
#include <tuple>
#include <windows.h>
#include "functions.h"

using namespace std;




// Find the value of an attribute in an HTML tag
void errorbased(string url)
{
    cout <<"Performing error based SQL injection...\n\n";
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize curl\n";
    }

    // URL to send requests to
    //std::string url = "http://localhost/tigernetBD-master/tgnet/login.php";

    // Open the file containing payloads
    std::ifstream infile("payloads.txt");
    if (!infile.is_open())
    {
        std::cerr << "Failed to open payloads file\n";
        curl_easy_cleanup(curl);
        //return 1;
    }

    // Store the payloads in a vector
    std::vector<std::string> payloads;
    std::string payload;
    while (std::getline(infile, payload))
    {
        payloads.push_back(payload);
        //std::cout << payload<<"\n";
    }

    // Close the file
    infile.close();

    std::string errrormessage="You have an error in your SQL syntax; check the manual that corresponds to your";
    bool vulnerable=false;

    std::string response=sendHttpRequest(url);
    //std::string url = "http://localhost/DVWA-master/vulnerabilities/sqli/?id=" +payload+"&Submit=Submit";


    // Send requests for each payload in the vector
    for (const auto  payload : payloads)
    {

        std::cout <<"Used payload : "<<payload<<"\n......................\n";

        std::vector<std::tuple<std::string, std::string, std::string>> params = extract_parameters(response);

        // Print the parameter names
        string url1=url;
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
        std::cout << url1 << "\n";

        std::string response1=sendHttpRequest(url1);


        //std::cout << "Response: " << response1 << "\n";
        if(Find(response1,errrormessage)!=std::string::npos)
        {
            vulnerable=true;
        }

    }
    HANDLE col;
    col= GetStdHandle(STD_OUTPUT_HANDLE);
    if(vulnerable)
    {
        SetConsoleTextAttribute(col,4);
        cout <<  "The website is vulnerable to Error based sql injection\n\n";
        SetConsoleTextAttribute(col,15);
    }
    else
    {

        SetConsoleTextAttribute(col,2);
        cout <<  "The website is not vulnerable to Error based sql injection\n\n";
        SetConsoleTextAttribute(col,15);
    }
    //return 0;
}
