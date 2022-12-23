//
//  utilities.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/13/22.
//

#ifndef utilities_h
#define utilities_h

#include <iostream>   // std::cout
#include <string>
using namespace std;

double PriceStringToInt(string s) {
    int len = s.size();
    s[len-1] = (s[len-1] == '+') ? '4' : s[len-1];
    // fractional notation transformation
    int a =stoi(s.substr(0, len-4));
    double b =stoi(s.substr(len-3,2))/32.;
    double c = stoi(s.substr(len-1,1))/256.;
    return a+b+c;
}


string PriceIntToString(int a, int b, int c) {
    string A = to_string(a);
    string B = to_string(b);
    string C = to_string(c);
    if (c==4) C = "+";
    if (b<10) B = "0" + B;
    string p = A+"-"+B+C;
    return p;
}


string fill_str (string v, int width, char c) {
    string result;
    std::stringstream inter;
    inter << std::setw(width) << std::setfill(c) << v;
    inter >> result;
    return result;
}


#endif /* utilities_h */
