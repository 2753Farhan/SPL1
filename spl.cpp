#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
using namespace std;
string removeExtraWhitespace(string str) {
    string result = "";

    // Flag to keep track of whether the last character was a whitespace character
    bool lastCharWasWhitespace = false;

    // Loop over each character in the input string
    for (char c : str) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            // If the current character is a whitespace character...
            if (!lastCharWasWhitespace) {
                // ...and the last character was not a whitespace character, add a single space to the output string
                result += ' ';
                lastCharWasWhitespace = true;
            }
        } else {
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

            if (qtype == "select")
            {
                if (line.find("* from") != string::npos || line.find("from") != string::npos || line.find("where") != string::npos || line.find("group by") != string::npos || line.find("order by") != string::npos)
                {
                }
                else pres = false;
            }
            else if (qtype == "insert")
            {
                if (line.find("into") != string::npos || line.find("values") != string::npos)
                {
                }
                //else pres = false;
            }
            else if (qtype == "update")
            {
                if (line.find("set") != string::npos || line.find("where") != string::npos)
                {
                }
                else pres = false;
            }
            else if (qtype == "delete")
            {
                if (line.find("from") != string::npos || line.find("where") != string::npos)
                {
                }
                else pres = false;
            }
            else if (qtype == "alter")
            {
                if (line.find("table") != string::npos || line.find("column") != string::npos)
                {
                }
                else pres = false;
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
                    if(end_pos!=string :: npos){
                        query += line.substr(0,end_pos);
                    }
                    else query += original_line + '\n';
                }
                if (line.find(";") != string::npos)
                {   query=removeExtraWhitespace(query);
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
    while (getline(file, line))
    {
        int queryIndex = line.find("Query");
        if (queryIndex != string::npos)
        {
            queries.push_back(line.substr(queryIndex + 6));
        }
    }
    for(int i=0; i<queries.size(); i++)
    {
        queries[i]+=";";
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
string corresponding_phpquery(vector<string> phpquery,string logstring)
{
    int Size=0;
    string temp,st;
    for(auto val:phpquery)
    {
        temp=longest_common_subsequence(val,logstring);
        //cout << temp << "\n";
        if(temp.length()>Size)
        {
            Size=temp.length();
            st=val;
        }

    }
    return st;


}

int main()
{
    vector<string> phpqueries,mysqllogqueries;
    phpqueries=extract_queries("register.php");
    mysqllogqueries=extractlogQueries("mysql.log");

    for(string val : phpqueries)
    {
        cout << val<<"\n" ;
    }
    /*
    for(string val : mysqllogqueries)
    {
        cout <<val <<"\n" ;
    }
    for(auto val : mysqllogqueries)
    {
        //cout <<removeattributeValues(val)<< " "<<removeattributeValues(corresponding_phpquery(phpqueries,val));
        if(removeattributeValues(val)==removeattributeValues(corresponding_phpquery(phpqueries,val)))
        {
            cout << val << " is a normal sql query\n";
        }
        else cout << "Abnormal sql query\n";
    }
    */
    return 0;
}
//SELECT * FROM user WHERE ID='1' or '1=1'--'AND password='1234'

