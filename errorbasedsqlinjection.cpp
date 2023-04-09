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

// Callback function to store response headers
size_t HeaderCallback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t len = size * nmemb;
    std::string header(ptr, len);
    std::cout << "Header: " << header;
    return len;
}

int main() {
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl\n";
        return 1;
    }

    // URL to send requests to
    std::string url = "http://localhost/tigernetBD-master/tgnet/login.php";

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
    }

    // Close the file
    infile.close();

    std::string errrormessage="You have an error in your SQL syntax; check the manual that corresponds to your";
    bool vulnerable=false;

    // Send requests for each payload in the vector
    for (const auto& payload : payloads) {
        std::string response;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, payload.length());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
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
        if(response.find(errrormessage)!=std::string::npos){
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
