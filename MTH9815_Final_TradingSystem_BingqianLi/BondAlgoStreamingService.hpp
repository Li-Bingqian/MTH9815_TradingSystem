//
//  BondAlgoStreamingService.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/17/22.
//

#ifndef BondAlgoStreamingService_h
#define BondAlgoStreamingService_h
#include "AlgoStream.hpp"
#include "soa.hpp"
#include "products.hpp"
#include <map>
#include <vector>
using namespace std;

class BondAlgoStreamingService : public Service<string, AlgoStream<Bond> > {
private:
    map<string, AlgoStream<Bond> > algoStreamMap;
    vector<ServiceListener<AlgoStream<Bond> >*> listeners;

public:
    BondAlgoStreamingService() { algoStreamMap = map<string, AlgoStream<Bond> >(); }       // cstor

    // override virtual func in Service
    AlgoStream<Bond>& GetData(string key) override { return algoStreamMap.at(key);}

    // usually called by connector, no implementation here
    void OnMessage(AlgoStream<Bond> &data) override {}

    // Add a listener (BondPositionServiceListener) to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<AlgoStream<Bond> > *listener) override {
        listeners.push_back(listener);
    }

    // Get all listeners on the Service.
    const vector<ServiceListener<AlgoStream<Bond> > *>& GetListeners() const override {
        return listeners;
    }

    // add price based on orderbook, called by listener when adding process
    void UpdatePrice(const Price<Bond>& price);
};



class BondAlgoStreamingServiceListener : public ServiceListener<Price<Bond> > {
public:
    // ctor
    BondAlgoStreamingServiceListener(BondAlgoStreamingService* _bond_algostream);

    // override virtual functions in ServiceListener class
    // listen from BondExecutionService and pass to BondTradeBookingServce
    virtual void ProcessAdd(Price<Bond>& data) override;

    // no implementation
    virtual void ProcessRemove(Price<Bond> &data) override{}

    // no implementation
    virtual void ProcessUpdate(Price<Bond> &data) override{}

private:
    BondAlgoStreamingService* b_algostream_ser;
};


void BondAlgoStreamingService::UpdatePrice(const Price<Bond> &price) {
    auto key = price.GetProduct().GetProductId();
    auto bond_it = algoStreamMap.find(key);
    if (bond_it != algoStreamMap.end()) {
        // if find in the map, update using AlgoStream::ExecuteAlgo
        (bond_it->second).UpdateAlgo(price);
    } else {
        // if it is new, add to the map
        // then, update it by recursion
        PriceStreamOrder new_bid(0,0,0,BID);
        PriceStreamOrder new_offer(0,0,0,OFFER);
        PriceStream<Bond> new_price_stream(price.GetProduct(), new_bid, new_offer);
        algoStreamMap.insert(pair<string,AlgoStream<Bond> >(key, AlgoStream<Bond>(new_price_stream)));
        UpdatePrice(price);
        return;
    }

    // notify the listeners
    for(auto& listener:listeners){
        listener->ProcessAdd(bond_it->second);
    }
    return;
}


BondAlgoStreamingServiceListener::BondAlgoStreamingServiceListener (BondAlgoStreamingService* _bond_algostream) : b_algostream_ser(_bond_algostream) {}


void BondAlgoStreamingServiceListener::ProcessAdd(Price<Bond> &data) {
    b_algostream_ser -> UpdatePrice(data);
}

#endif /* BondAlgoStreamingService_h */

