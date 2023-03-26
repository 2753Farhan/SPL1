#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;

void removeUnpairedQuoteMarks(string& s)
{
    size_t quote_idx = s.find('"');
    while (quote_idx != string::npos)
    {
        size_t last_quote_idx = s.rfind('"');
        if (quote_idx == last_quote_idx) // quotes are paired
        {
            quote_idx = s.find('"', last_quote_idx + 1);
            continue;
        }
        if ((last_quote_idx != string::npos) && ((last_quote_idx - quote_idx) % 2 == 1))
        {
            // quotes are unpaired
            s.erase(quote_idx, 1);
            s.erase(last_quote_idx - 1, 1);
        }
        else if (last_quote_idx == string::npos)
        {
            // unpaired quote mark at the end of the string
            s.erase(quote_idx, 1);
        }
        else
        {
            // unpaired quote mark at the beginning of the string
            s.erase(last_quote_idx, 1);
        }
        quote_idx = s.find('"', last_quote_idx - 1);
    }
}
bool IsValidQuery(string s)
{
    string orgnl=s;
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    if(s.find("select")!=string::npos)
    {
        if(s.find("from")!=string::npos)
        {
            return true;
        }
        else return false;
    }
    else if(s.find("insert")!=string::npos)
    {
        if(s.find("into")!=string::npos)
        {
            return true;
        }
        else return false;
    }
    else if(s.find("update")!=string::npos)
    {
        if(s.find("set")!=string::npos)
        {
            return true;
        }
        else return false;
    }
    else if(s.find("delete")!=string::npos)
    {
        if(s.find("from")!=string::npos)
        {
            return true;
        }
        else return false;
    }
    else if(s.find("alter")!=string::npos)
    {
        if(s.find("table")!=string::npos)
        {
            return true;
        }
        else return false;
    }
    else if(s.find("drop")!=string::npos)
    {
        if(s.find("table")!=string::npos)
        {
            return true;
        }
        if(s.find("database")!=string::npos)
        {
            return true;
        }
        else return false;
    }


}

string removeExtraWhitespace(string str)
{
    string result = "";

    // Flag to keep track of whether the last character was a whitespace character
    bool lastCharWasWhitespace = false;

    // Loop over each character in the input string
    for (char c : str)
    {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
        {
            // If the current character is a whitespace character...
            if (!lastCharWasWhitespace)
            {
                // ...and the last character was not a whitespace character, add a single space to the output string
                result += ' ';
                lastCharWasWhitespace = true;
            }
        }
        else
        {
            // If the current character is not a whitespace character, add it to the output string
            result += c;
            lastCharWasWhitespace = false;
        }
    }

    return result;
}

vector<string> extract_queries(string file_path)
{
    ifstream file(file_path);
    ifstream file1("querytype.txt");
    string line;
    string query = "";
    vector<string> queries;
    vector<string> query_types;

    if (file1.is_open()) // read query types from file
    {
        while (getline(file1, line))
        {
            query_types.push_back(line);
        }
        file1.close();
    }
    else
    {
        cout << "Unable to open querytype file" << endl;
    }

    if (file.is_open())
    {
        bool found=false;
        while (getline(file, line))
        {
            // Create a copy of the line for comparison without modification
            string original_line = line;
            // Convert the line to lowercase for easier comparison
            transform(line.begin(), line.end(), line.begin(), ::tolower);
            int comment_start = line.find("//");
            if (comment_start != string::npos)
            {
                line = line.substr(0, comment_start);
            }
            comment_start = line.find("echo");
            if (comment_start != string::npos)
            {
                line = line.substr(0, comment_start);
            }
            int command_pos,end_pos;
            bool flag=true;
            string qtype;
            bool pres=false;

            for (string type : query_types) // check if line contains a query type
            {
                if (line.find(type) != string::npos)
                {
                    pres = true;
                    qtype = type;
                    break;
                }
            }



            // Check if the line contains a SQL command
            if (pres)
            {
                found=true;
                command_pos=line.find(qtype);
                end_pos=line.find(";");
                // Add the portion of the line after the command to the query
                query += original_line.substr(command_pos,end_pos-command_pos+1) + '\n';
                flag=false;
            }

            if (found)
            {
                if(flag)
                {
                    end_pos=line.find(";");
                    if(end_pos!=string :: npos)
                    {
                        query += line.substr(0,end_pos+1);
                        //cout << query << "\n";
                    }
                    else query += original_line + '\n';
                }
                if (line.find(";") != string::npos)
                {
                    query=removeExtraWhitespace(query);
                    //cout << query <<"\n";
                    if(query.find("\"")!=string ::npos)
                        removeUnpairedQuoteMarks(query);
                    //cout << query <<"\n";
                    if(IsValidQuery(query))
                        queries.push_back(query);
                    query = "";
                    found = false;
                }
            }
        }
        file.close();
    }
    else
    {
        cout << "Unable to open file" << endl;
    }
    return queries;
}
vector<string> extractlogQueries(const string& logFile)
{
    vector<string> queries;
    ifstream file(logFile);
    if (!file.is_open())
    {
        cout << "Error opening log file" << endl;
    }

    string line;
    string query = "";
    while (getline(file, line))
    {
        int queryIndex = line.find("Query");
        if (queryIndex != string::npos)
        {
            if (!query.empty())
            {
                queries.push_back(query);
            }
            query = line.substr(queryIndex + 6);
        }
        else if (line.find("Quit") == string::npos && line.find("Init DB") == string::npos &&
                 line.find("Connect") == string::npos && line.find("Quit") == string::npos &&
                 (line.find("SELECT") == 0 || line.find("INSERT") == 0 ||
                  line.find("UPDATE") == 0 || line.find("DELETE") == 0 ||
                  line.find("REPLACE") == 0 || line.find("CREATE") == 0 ||
                  line.find("ALTER") == 0 || line.find("DROP") == 0 ||
                  line.find("TRUNCATE") == 0 || line.find("LOAD DATA") == 0))
        {
            query += line;
        }

    }

    if (!query.empty())
    {
        removeExtraWhitespace(query);
        queries.push_back(query);
    }

    file.close();
    return queries;
}
string removeattributeValues(string inputString)
{
    string outputString;
    string currentQuotationState = "QuotEnd";
    char currentChar;

    if (inputString.empty())
    {
        return outputString;
    }

    for (int i = 0; i < inputString.length(); i++)
    {
        currentChar = inputString[i];

        if (currentChar == '\"' || currentChar == '\'')
        {
            if (i > 0 && inputString[i-1] != '\\')
            {
                currentQuotationState = (currentQuotationState == "QuotStart") ? "QuotEnd" : "QuotStart";
            }
            outputString += currentChar;
        }
        else
        {
            if (currentQuotationState == "QuotEnd")
            {
                outputString += currentChar;
            }
            else
            {
                if (i > 0 && inputString[i-1] == '\\')
                {
                    outputString += currentChar;
                }
            }
        }
    }

    return outputString;
}
string longest_common_subsequence(string s1, string s2)
{
    int m = s1.length(), n = s2.length();
    vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
    for (int i = 1; i <= m; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            if (s1[i - 1] == s2[j - 1])
            {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            }
            else
            {
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
            }
        }
    }
    string result = "";
    int i = m, j = n;
    while (i > 0 && j > 0)
    {
        if (s1[i - 1] == s2[j - 1])
        {
            result = s1[i - 1] + result;
            i--;
            j--;
        }
        else if (dp[i - 1][j] > dp[i][j - 1])
        {
            i--;
        }
        else
        {
            j--;
        }
    }
    return result;
}
string detect_query_type(string query_string) {
    string query_type = "";
    string upper_query_string = "";
    int pos = query_string.find(" ");
    if (pos != string::npos) {
        upper_query_string = query_string.substr(0, pos);
    } else {
        upper_query_string = query_string;
    }

    for (int i = 0; i < upper_query_string.length(); i++) {
        query_type += toupper(upper_query_string[i]);
    }

    if (query_type == "SELECT") {
        return "SELECT";
    } else if (query_type == "INSERT") {
        return "INSERT";
    } else if (query_type == "UPDATE") {
        return "UPDATE";
    } else if (query_type == "DELETE") {
        return "DELETE";
    } else if (query_type == "CREATE") {
        return "CREATE";
    } else if (query_type == "DROP") {
        return "DROP";
    } else if (query_type == "ALTER") {
        return "ALTER";
    } else if (query_type == "TRUNCATE") {
        return "TRUNCATE";
    } else {
        return "UNKNOWN";
    }
}


string corresponding_phpquery(vector<string> phpquery,string logstring)
{
    int Size=0;
    string temp,st="",qtype1,qtype2;
    qtype1=detect_query_type(logstring);
    for(string val : phpquery)
    {
        qtype2=detect_query_type(val);
        if(qtype1==qtype2){
            temp=longest_common_subsequence(val,logstring);
            //cout << temp << "\n";
            if(temp.length()>Size)
            {
                Size=temp.length();
                st=val;
            }
        }


    }
    return st;


}
string xorStrings(string str1, string str2)
{
    string result;
    for (int i = 0; i < str1.length() && i < str2.length(); i++)
    {
        result += (str1[i] ^ str2[i]);
    }
    return result;
}
bool isXorZero(string str1, string str2)
{
    string result = xorStrings(str1, str2);
    for (int i = 0; i < result.length(); i++)
    {
        if (result[i] != 0)
        {
            return false;
        }
    }
    return true;
}
vector<string> removeUnnecessaryLogquery(vector<string> php,vector<string> log){
    vector<string> queryType;
    for(auto  val: php){
        queryType.push_back(detect_query_type(val));
    }
    for(int i=0;i<log.size();i++){
        string qtype=detect_query_type(log[i]);
        bool notPresent=true;
        for(auto val: queryType ){
            if(val==qtype){
                notPresent=false;
            }
        }
        if(notPresent){
            log.erase(log.begin()+i);
            i--;
        }
    }
    return log;
}
int main()
{
    vector<string> phpqueries,mysqllogqueries;

    phpqueries=extract_queries("register.php");
    mysqllogqueries=extractlogQueries("MSI.log");
    mysqllogqueries=removeUnnecessaryLogquery(phpqueries,mysqllogqueries);
    cout << "Extracted php queries: \n\n";
    for(string val : phpqueries)
    {
        cout << val << "\nAfter removing attributes:\n";
        cout <<removeattributeValues(val)<<"\n\n" ;
    }
    cout << "Extracted log queries:\n\n";
    for(string val : mysqllogqueries)
    {
        cout << val << "\nAfter removing attributes:\n";
        cout <<removeattributeValues(val) <<"\n\n" ;
    }

    for(auto val : mysqllogqueries)
    {
        string logquery=removeattributeValues(val);
        string phpquery=removeattributeValues(corresponding_phpquery(phpqueries,val));
        if(isXorZero(logquery,phpquery))
        {
            cout << val <<" is a valid query\n";
        }
        else cout << val <<" is a defective query\n";

    }
    return 0;
}
//SELECT * FROM user WHERE ID='1' or '1=1'--'AND password='1234'

