#include <iostream>
#include <string>
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

    std::string url = "http://localhost/test/login.php";
    std::string username;
    std::string password = "123456";
    std::string response;

    std::cout << "Enter username: ";
    std::cin >> username;

    std::string payload = "username=" + username + "&password=" + password;

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

    curl_easy_cleanup(curl);

    if (response.empty()) {
        std::cerr << "Empty response data\n";
        return 1;
    }

    std::cout << "Response: " << response << "\n";

    const std::string success_message = "login successful";
    const std::string failure_message = "login failed";
    const std::string error_message = "SQL error";

    if (response.find(success_message) != std::string::npos) {
        std::cout << "Login successful\n";
    } else if (response.find(failure_message) != std::string::npos) {
        std::cout << "Login failed\n";
    } else if (response.find(error_message) != std::string::npos) {
        std::cout << "Website is vulnerable to SQL injection\n";
    } else {
        std::cout << "Unknown response\n";
    }

    return 0;
}
