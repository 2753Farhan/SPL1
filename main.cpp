#include <iostream>
#include "BooleanbasedBlind.h"
#include "errorbasedsqlInjection.h"
#include "Timebasedsqlinjection.h"
#include "sqlinjectiondetection.h"
using namespace std;

int main()
{
    cout <<"...............Welcome to the SQL injection and vulnerability detection tool..............\n\n\n";
    while(1){
    cout << "What do you want to do?\n";
    cout << "1.SQL injection detection\n2.SQL vulnerability detection of a website\n3.Quit\n\n";
    int n;
    cin >> n;
    if(n==1) sqlinjectiondetection();
    else if(n==3) break;
    else{
        string url;
        cout <<"Enter the URL of the website: \n";
        cin >> url;
        errorbased(url);
        Booleanbased(url);
        timebased(url);
    }
    }

}
