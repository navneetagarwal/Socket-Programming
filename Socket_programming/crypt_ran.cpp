#include<iostream>
#include<fstream>
#include<crypt.h>
using namespace std;

int main() {

int i =0;

 ofstream myWriteFile;
 ifstream myReadFile;
 myReadFile.open("Output.txt");
 myWriteFile.open("Crypt.txt");
 char output[100];
 if (myReadFile.is_open()) {
 while (!myReadFile.eof() && i<10) {


    myReadFile >> output;
    string n(crypt(output,"aa"));
    string y = n + "\n";
    myWriteFile<< y.c_str();
    i++;
 }
}
myReadFile.close();
return 0;
}