//
//  BondAlgoExecutionService.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/16/22.
//

/**
 Define BondAlgoExecutionService to listen data from BondMarketDataService
 and flow to BondExecutionService

 Define BondAlgoExecutionServiceListener to listen data from BondMarketDataService

 */

#ifndef BondAlgoExecutionService_h
#define BondAlgoExecutionService_h
#include "AlgoExecution.hpp"
#include "BondMarketDataService.hpp"
#include "soa.hpp"
#include <map>
#include <vector>
using namespace std;

class BondAlgoExecutionService : public Service<string, AlgoExecution<Bond> > {
private:
    map<string, AlgoExecution<Bond> > algoExeMap;
    vector<ServiceListener<AlgoExecution<Bond> >*> listeners;

public:
    BondAlgoExecutionService() { algoExeMap = map<string, AlgoExecution<Bond> >(); }       // cstor

    // override virtual func in Service
    AlgoExecution<Bond>& GetData(string key) override { return algoExeMap.at(key);}

    // usually called by connector, no implementation here
    void OnMessage(AlgoExecution<Bond> &data) override {}

    // Add a listener (BondPositionServiceListener) to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<AlgoExecution<Bond> > *listener) override {
        listeners.push_back(listener);
    }

    // Get all listeners on the Service.
    const vector<ServiceListener<AlgoExecution<Bond> > *>& GetListeners() const override {
        return listeners;
    }

    // execute algo based on orderbook, called by listener when adding process
    void UpdateOrderBook(const OrderBook<Bond>& orderBook);
};



class BondAlgoExecutionServiceListener : public ServiceListener<OrderBook<Bond> > {
public:
    // ctor
    BondAlgoExecutionServiceListener(BondAlgoExecutionService* _bond_algoexe);

    // override virtual functions in ServiceListener class
    // listen from BondExecutionService and pass to BondTradeBookingServce
    void ProcessAdd(OrderBook<Bond>& data) override;

    // no implementation
    void ProcessRemove(OrderBook<Bond> &data) override{}

    // no implementation
    void ProcessUpdate(OrderBook<Bond> &data) override{}

private:
    BondAlgoExecutionService* b_algoexecution_ser;
};



void BondAlgoExecutionService::UpdateOrderBook(const OrderBook<Bond> &order_book) {
    auto key = order_book.GetProduct().GetProductId();
    auto bond_it = algoExeMap.find(key);
    if (bond_it != algoExeMap.end()) {
        // if find in the map, update using AlgoExucution::ExecuteAlgo
        (bond_it->second).UpdateAlgo(order_book);
    } else {
        // if it is new, add to the map
        // then, update it by recursion
        auto new_exe_order=ExecutionOrder<Bond>(order_book.GetProduct(),BID,"0",LIMIT,0,0,0,"0",false);
        algoExeMap.insert(pair<string,AlgoExecution<Bond> >(key, AlgoExecution<Bond>(new_exe_order)));
        UpdateOrderBook(order_book);
        return;
    }

    // notify the listeners
    for(auto& listener:listeners){
        listener->ProcessAdd(bond_it->second);
    }
    return;
}


BondAlgoExecutionServiceListener::BondAlgoExecutionServiceListener (BondAlgoExecutionService* _bond_algoexe) : b_algoexecution_ser(_bond_algoexe) {}


void BondAlgoExecutionServiceListener::ProcessAdd(OrderBook<Bond> &data) {
    b_algoexecution_ser -> UpdateOrderBook(data);
}

#endif /* BondAlgoExecutionService_h */
