#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#include<iostream>
#include<vector>
#include<tuple>
size_t WriteCallback(char* , size_t, size_t ,std::string *);
std::vector<int> compute_prefix(const std::string&);
size_t Find(const std::string& , const std::string&);
std::string get_attribute_value(const std::string& , const std::string& );
std::vector<std::tuple<std::string, std::string, std::string>> extract_parameters(const std::string );
std::string sendHttpRequest(std::string );


#endif // FUNCTIONS_H_INCLUDED
