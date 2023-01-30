#include <iostream>
#include <string>
using namespace std;

string removeattributeValues(string inputString) {
    string outputString;
    string currentQuotationState = "QuotEnd";
    char currentChar;

    if (inputString.empty()) {
        return outputString;
    }

    for (int i = 0; i < inputString.length(); i++) {
        currentChar = inputString[i];

        if (currentChar == '\"' || currentChar == '\'') {
            if (i > 0 && inputString[i-1] != '\\') {
                currentQuotationState = (currentQuotationState == "QuotStart") ? "QuotEnd" : "QuotStart";
            }
            outputString += currentChar;
        } else {
            if (currentQuotationState == "QuotEnd") {
                outputString += currentChar;
            } else {
                if (i > 0 && inputString[i-1] == '\\') {
                    outputString += currentChar;
                }
            }
        }
    }

    return outputString;
}

int main() {
    string inputString;
    cout << "Enter a SQL query:" << endl;
    getline(cin, inputString);

    string outputString = removeattributeValues(inputString);
    cout << "Output String: " << outputString << endl;

    return 0;
}
//SELECT * FROM user WHERE ID='1' or '1=1'--'AND password='1234'

