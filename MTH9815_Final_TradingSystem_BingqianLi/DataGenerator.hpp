//
//  DataGenerator.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/13/22.
//

/**
Define data generator functions, including
 markerdata.txt,
 price.txt
 trades.txt
 */

#ifndef DataGenerator_h
#define DataGenerator_h

#include "utilities.hpp"
#include <fstream>
#include <random>
#include <vector>
//#include "tradebookingservice.hpp"
using namespace std;
using namespace boost::gregorian;

template <typename T, size_t N>
T* begin_array(T(&arr)[N]) { return &arr[0]; }
template <typename T, size_t N>
T* end_array(T(&arr)[N]) { return &arr[0]+N; }

std::vector<int> gen_zero_vec(int n) {
    std::vector<int> v;
    for (int i = 0; i<n; ++i) {v.push_back(0);}
    return v;
}


void MarketDataGenerator();
void PriceGenerator();
void TradesGenerator();
void InquriesGenerator();

// Bond information
//vector<string> bondCusip({"91282CFX4", // 2Years
//"91282CGA3", // 3Years
//"91282CFZ9", // 5Years
//"91282CFY2", // 7Years
//"91282CFV8", // 10Years
//"912810TM0", // 20Years
//"912810TL2"}); // 30Years

string vv1[] = {"91282CFX4", // 2Years
    "91282CGA3", // 3Years
    "91282CFZ9", // 5Years
    "91282CFY2", // 7Years
    "91282CFV8", // 10Years
    "912810TM0", // 20Years
    "912810TL2"}; // 30Years
std::vector<string> bondCusip(begin_array(vv1), end_array(vv1));

string vv2[] = {"US2Y", // 2Years
"US3Y", // 3Years
"US5Y", // 5Years
"US7Y", // 7Years
"US10Y", // 10Years
"US20Y", // 20Years
"US30Y"}; // 30Years
std::vector<string> bondTicker(begin_array(vv2), end_array(vv2));

//vector<string> bondTicker({"US2Y", // 2Years
//"US3Y", // 3Years
//"US5Y", // 5Years
//"US7Y", // 7Years
//"US10Y", // 10Years
//"US20Y", // 20Years
//"US30Y"}); // 30Years

float vv3[] = {.045, // 2Years
0.04, // 3Years
0.03875, // 5Years
0.03875, // 7Years
0.04125, // 10Years
0.04, // 20Years
0.04}; // 30Years
std::vector<float> bondCoupon(begin_array(vv3), end_array(vv3));

//vector<float> bondCoupon({.045, // 2Years
//0.04, // 3Years
//0.03875, // 5Years
//0.03875, // 7Years
//0.04125, // 10Years
//0.04, // 20Years
//0.04}); // 30Years

date vv4[] = {date(2024,Nov,30), // 2Years
    date(2025,Dec,15), // 3Years
    date(2027,Nov,30), // 5Years
    date(2029,Nov,30), // 7Years
    date(2032,Nov,15), // 10Years
    date(2042,Nov,15), // 20Years
    date(2052,Nov,15)}; // 30Years
std::vector<date> bondMaturity(begin_array(vv4), end_array(vv4));

//vector<date> bondMaturity({date(2024,Nov,30), // 2Years
//date(2025,Dec,15), // 3Years
//date(2027,Nov,30), // 5Years
//date(2029,Nov,30), // 7Years
//date(2032,Nov,15), // 10Years
//date(2042,Nov,15), // 20Years
//date(2052,Nov,15)}); // 30Years


//map<string,float> bondRisk({{"91282CFX4",0.015}, // 2Years
//{"91282CGA3",0.03}, // 3Years
//{"91282CFZ9",0.05}, // 5Years
//{"91282CFY2",0.07}, // 7Years
//{"91282CFV8",0.09}, // 10Years
//{"912810TM0", 0.11},// 20Years
//{"912810TL2",0.2}}); // 30Years

std::pair<string,float> map_data[] = {
    std::make_pair("91282CFX4",0.015), // 2Years
    std::make_pair("91282CGA3",0.03), // 3Years
    std::make_pair("91282CFZ9",0.05), // 5Years
    std::make_pair("91282CFY2",0.07), // 7Years
    std::make_pair("91282CFV8",0.09), // 10Years
    std::make_pair("912810TM0", 0.11),// 20Years
    std::make_pair("912810TL2",0.2)}; // 30Years


std::map<string,float> bondRisk(map_data, map_data + sizeof map_data / sizeof map_data[0]);


void MarketDataGenerator(int n) {
    // n: number of data for each bond
    ofstream market_data_file;
//    market_data_file.open("/Users/aubreeli/Documents/Graduate_lbq/Baruch/Fall/SoftwareEng/final project/MTH9815_Final_TradingSystem_BingqianLi/MTH9815_Final_TradingSystem_BingqianLi/marketdata.txt");
    market_data_file.open("marketdata.txt");

    // stop generating bond i if generated_num[i] = n
    vector<int> generatred_num = gen_zero_vec(7);

    for (int j=0; j<7*n; ++j) {
        int bond_id = rand() % 7;
        while (generatred_num[bond_id] == n)
            bond_id = rand() % 7;   // choose a new bond
        ++generatred_num[bond_id];

        // record bond ticker
        market_data_file << bondCusip[bond_id] << ",";

        // generate price, 5 bids and 5 offers; each pair has a fixed spread
        for(int i=0; i<5; ++i) {
            int a=rand()%2+99;
            int b=rand()%32;
            int c=rand()%8;
            string bid_p = PriceIntToString(a,b,c);
            string bid_q = to_string((i + 1) * 100000);
            market_data_file<<bid_p<<","<<bid_q<<",bid,";

            // add a fixed spread to generate ask
            c+=(i+1);
            if(c>7){
                c-=8;
                b+=1;
            }
            if(b>31){
                b-=32;
                a+=1;
            }
            string ask_p = PriceIntToString(a,b,c);
            string ask_q = to_string((i + 1) * 100000);
            market_data_file<<ask_p<<","<<ask_q<<",ask,";
        }
        market_data_file << endl;
    }
    market_data_file.close();
    std::cout << "marketdata.txt Generated!\n";
}


/**
 create prices which oscillate between 99 and 101
 The bid/offer spread should oscillate between 1/128 and 1/64.
 */
void PriceGenerator (int n) {
    // n: number of data for each bond
    ofstream price_file;
    price_file.open("price.txt");

    // stop generating bond i if generated_num[i] = n
    vector<int> generatred_num  = gen_zero_vec(7);

    for (int j=0; j<7*n; ++j) {
        int bond_id = rand() % 7;
        while (generatred_num[bond_id] == n)
            bond_id = rand() % 7;   // choose a new bond
        ++generatred_num[bond_id];
        string cusip = bondCusip[bond_id];

        int a=rand() % 2 + 99;
        int b=rand() % 32;
        int c=rand() % 8;
        string price = PriceIntToString(a, b, c);

        //  The bid/offer spread should oscillate between 1/128 and 1/64.
        int d = rand() % 4;

        string spread;
        switch (d) {
            case 1:
                spread = "0-001";
                break;
            case 2:
                spread = "0-002";
                break;
            case 3:
                spread = "0-003";
                break;
            case 0:
                spread = "0-00+";
                break;
            default:
                break;
        }
        price_file << cusip << "," << price << "," << spread <<endl;
    }
    price_file.close();
    std::cout << "price.txt Generated!\n";
}



void TradesGenerator(int n) {
    // n: number of data for each bond
    ofstream trades_file;
    trades_file.open("trades.txt");

    // stop generating bond i if generated_num[i] = n
    vector<int> generatred_num  = gen_zero_vec(7);

    for (int j=0; j<7*n; ++j) {
        int bond_id = rand() % 7;
        while (generatred_num[bond_id] == n)
            bond_id = rand() % 7;   // choose a new bond
        ++generatred_num[bond_id];
        string cusip = bondCusip[bond_id];
        
        int a=rand()%2+99;
        int b=rand()%32;
        int c=rand()%8;
        string price = PriceIntToString(a, b, c);
        
        int i = rand()%3;
        string book;
        switch(i){
            case 0:
                book = "TRSY1"; break;
            case 1:
                book = "TRSY2"; break;
            case 2:
                book = "TRSY3"; break;
        }
        
        string tradeID;
        i = rand()%5;
        switch(i){
            case 0:
                tradeID = "abc"; break;
            case 1:
                tradeID = "def";  break;
            case 2:
                tradeID = "ghi";  break;
            case 3:
                tradeID = "jkl";  break;
            case 4:
                tradeID = "mno";  break;
        }
        
        trades_file << cusip << "," << tradeID << "," << price << "," << book << ",";
        trades_file << to_string((rand()%5+1)*1000000)<<",";
        trades_file << (rand()%2==0 ? "buy":"sell") << endl;
    }
    trades_file.close();
    std::cout << "trades.txt Generated!\n";

}



void InquriesGenerator(int n) {
    // n: number of data for each bond
    ofstream inquries_file;
    inquries_file.open("inquiries.txt");

    // stop generating bond i if generated_num[i] = n
    vector<int> generatred_num  = gen_zero_vec(7);

    for (int j=0; j<7*n; ++j) {
        int bond_id = rand() % 7;
        while (generatred_num[bond_id] == n)
            bond_id = rand() % 7;   // choose a new bond
        ++generatred_num[bond_id];
        string cusip = bondCusip[bond_id];
        string id = fill_str(to_string(j), 8, '0');
        inquries_file << cusip << ","  << id << "," << (rand()%2==0?"buy":"sell") << ",";
        inquries_file << to_string((rand()%5+1)*1000000)<<",";

        int a=rand()%2+99;
        int b=rand()%32;
        int c=rand()%8;
        string price = PriceIntToString(a, b, c);

        inquries_file<<price<<"," << "Recieved" << endl;
    }

    inquries_file.close();
    std::cout << "inquries.txt Generated!\n";
}

#endif /* DataGenerator_h */


