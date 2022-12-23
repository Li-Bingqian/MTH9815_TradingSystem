//
//  BondTradeBookingService.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/13/22.
//

/**
 Define BondTradeBookingService to recieve data from BondTradeBookingServiceConnector
 and flow data to BondPositionServiceListener

 Define BondTradeBookingServiceConnector to read data from trade.txt

 Define BondTradeBookingServiceListener to listen from BondExecutionService
 */

#ifndef BondTradeBookingService_h
#define BondTradeBookingService_h

#include "tradebookingservice.hpp"
#include "productservice.hpp"
#include "BondExecutionService.hpp"
#include "utilities.hpp"

using namespace std;

class BondTradeBookingService: public TradeBookingService<Bond>  {
private:
    map<string, Trade<Bond> > tradeMap;
    vector<ServiceListener<Trade<Bond> >*> listeners;

public:
    BondTradeBookingService();       // cstor

    // override virtual func in Service
    Trade<Bond>& GetData(string key) override;

    // The callback that a Connector should invoke for any new or updated data
    // add new bond price to pricemap
    void OnMessage(Trade<Bond> &data) override;

    // Add a listener (BondPositionServiceListener) to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Trade<Bond> > *listener) override;

    // Get all listeners on the Service.
    const vector<ServiceListener<Trade<Bond> > *>& GetListeners() const override;
    // Book the trade
    void BookTrade(Trade<Bond> &trade) override;
};


class BondTradeBookingServiceConnector: public Connector<Trade<Bond> > {
public:
    // cstor
    BondTradeBookingServiceConnector (BondTradeBookingService* bond_trade, BondProductService* bond_product);

    // subscribe-only where Publish() does nothing
    void Publish(Trade<Bond>& info) override;
    // subscribe data from price.txt file and invoked via the BondMarketDataService.OnMessage()
    void Subscribe();

private:
    BondTradeBookingService* b_tradebooking_ser;
    BondProductService* b_prod_ser;
};


class BondTradeBookingServiceListener : public ServiceListener<ExecutionOrder<Bond> > {
public:
    // ctor
    BondTradeBookingServiceListener(BondTradeBookingService* _bond_tradebook);

    // override virtual functions in ServiceListener class
    // listen from BondExecutionService and pass to BondTradeBookingServce
    void ProcessAdd(ExecutionOrder<Bond>& data) override;

    // no implementation
    void ProcessRemove(ExecutionOrder<Bond> &data) override{}

    // no implementation
    void ProcessUpdate(ExecutionOrder<Bond> &data) override{}

private:
    BondTradeBookingService* b_tradebooking_ser;
};


// ----------------- BondTradeBookingService --------------------------
// ctor
BondTradeBookingService::BondTradeBookingService() {
    tradeMap = map<string, Trade<Bond> >();
}


Trade<Bond>& BondTradeBookingService::GetData(string key) {
    return tradeMap.at(key);
}


void BondTradeBookingService::OnMessage(Trade<Bond> &data) {
    BookTrade(data);    // Book the trade
}


void BondTradeBookingService::AddListener(ServiceListener<Trade<Bond> > *listener) {
    listeners.push_back(listener);
}


const vector<ServiceListener<Trade<Bond> > *>& BondTradeBookingService::GetListeners() const {
    return listeners;
}


void BondTradeBookingService::BookTrade(Trade<Bond> &trade) {
    string id = trade.GetProduct().GetProductId();
    // update the trade map
    if (tradeMap.find(id) != tradeMap.end()) {tradeMap.erase(id);}
    tradeMap.insert(pair<string, Trade<Bond> > (id, trade));

//    auto trade = trade_temp;.
    // flow the data to listeners
    for (auto& l : listeners) {
        l -> ProcessAdd(trade);
    }
}


void BondTradeBookingServiceConnector::Subscribe() {
    // read data from txt file
    fstream newfile;
    newfile.open("trades.txt", ios::in);
    if (newfile.is_open()) {
        string data_row;
        while(getline(newfile, data_row)) {
            stringstream line(data_row);
            string temp;
            // the first column is id, store and get the bond
            getline(line, temp, ',');
            auto bond = b_prod_ser->GetData(temp);
            // trade id
            getline(line, temp, ',');
            auto trade_id = temp;
            // price
            getline(line, temp, ',');
            auto price = PriceStringToInt(temp);
            // book
            getline(line, temp, ',');
            auto book = temp;
            // quantity
            getline(line, temp, ',');
            auto quantity = stol(temp);
            getline(line, temp, ',');
            Side side = (temp=="buy" ? BUY : SELL);

            Trade<Bond> new_trade(bond,trade_id,price,book,quantity,side);
            // call Service.OnMessage(), flow data
            b_tradebooking_ser->OnMessage(new_trade);

        }
    }
}


void BondTradeBookingServiceConnector::Publish(Trade<Bond> &info) {}


BondTradeBookingServiceConnector::BondTradeBookingServiceConnector(BondTradeBookingService* bond_trade, BondProductService* bond_product) : b_tradebooking_ser(bond_trade), b_prod_ser(bond_product) {}


BondTradeBookingServiceListener::BondTradeBookingServiceListener(BondTradeBookingService* bond_trade) : b_tradebooking_ser(bond_trade) {}


void BondTradeBookingServiceListener::ProcessAdd(ExecutionOrder<Bond> &data) {
    auto bond = data.GetProduct();
    double price = data.GetPrice();
    long quantity = data.GetVisibleQuantity();
    string tradeID = "Execution";
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
    Side side = (data.GetSide() == BID) ? SELL : BUY;
    Trade<Bond> trade(bond, tradeID, price, book, quantity, side);

    b_tradebooking_ser->OnMessage(trade);   // add to book
}


#endif /* BondTradeBookingService_h */
