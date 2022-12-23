# MTH9815_TradingSystem
Final project for MTH9815
## How to run
1. Run the main.exe file for compiled version directly  
OR  
2. Change the correpsonding path (including boost path) and run  
```
g++ -std=c++11 -I/usr/local/boost_1_80_0/ -L/usr/local/boost_1_80_0/libs/ -Wall -m64 main.cpp -o main  
```
```
./main
```

## How to run
1. Due to the large dataset, it might take 25-35min for full size data to run.  
2. Github cannot upload extreme large files. Hence, the uploaded .txt files are generated under 1000 market data and price data.   
3. Follow the above instructions, the full size of 1000000 market and price data test will run.
