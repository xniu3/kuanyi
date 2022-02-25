#include <iostream>
#include <stdio.h>
using namespace std;
class Myclass{
    public:
        int mynum;
        string myString;
    
};
int main(){
    Myclass myobj;
    myobj.mynum = 1953;
    myobj.myString = "Tongshangkuanyi";
    cout<<myobj.mynum<<endl;
    cout<<myobj.myString<<endl;
    return 0;
}