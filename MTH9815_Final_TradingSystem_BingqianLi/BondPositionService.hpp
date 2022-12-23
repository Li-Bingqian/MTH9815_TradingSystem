//
//  BondPositionService.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/19/22.
//

#ifndef BondPositionService_h
#define BondPositionService_h

#include "BondTradeBookingService.hpp"
#include "positionservice.hpp"
#include "soa.hpp"

class BondPositionService : public PositionService<Bond> {
public:
    BondPositionService() {
        positionMap=map<string,Position<Bond> >();
    }

    // override virtual func in Service
    Position<Bond>& GetData(string key) override { return positionMap.at(key);}

    // usually called by connector, no implementation here
    void OnMessage(Position<Bond> &data) override {}

    // Add a listener (BondPositionServiceListener) to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Position<Bond> > *listener) override {
        listeners.push_back(listener);
    }

    // Get all listeners on the Service.
    const vector<ServiceListener<Position<Bond> > *>& GetListeners() const override {
        return listeners;
    }

    // called by BondPositionServiceListener
    void AddTrade(const Trade<Bond>& trade) override;

private:
    map<string, Position<Bond> > positionMap;
    vector<ServiceListener<Position<Bond> >*> listeners;
};


class BondPositionServiceListener : public ServiceListener<Trade<Bond> > {
private:
    BondPositionService* b_position_ser;

public:
    // ctor
    BondPositionServiceListener(BondPositionService* bond_position);

    // override all pure virtual functions in base class
    // call bondPositionService AddTrade
    void ProcessAdd(Trade<Bond> &data) override;

    // no implementation
    void ProcessRemove(Trade<Bond> &data) override {}

    // no implementation
    void ProcessUpdate(Trade<Bond> &data) override{}

};


void BondPositionService::AddTrade(const Trade<Bond> &trade) {
    auto bond = trade.GetProduct();
    string id = bond.GetProductId();
    long q = (trade.GetSide()==SELL) ? -trade.GetQuantity() : trade.GetQuantity();

    if (positionMap.find(id) != positionMap.end()) {
        (positionMap.find(id)->second).AddPosition(trade.GetBook(), q);
    } else {
        positionMap.insert(pair<string,Position<Bond> >(id,Position<Bond>(bond)));
        AddTrade(trade);
        return;
    }

    for(auto& listener : listeners) {
        listener->ProcessAdd(positionMap.find(id)->second);
    }
}

BondPositionServiceListener::BondPositionServiceListener(BondPositionService* bond_position) : b_position_ser(bond_position) {}

void BondPositionServiceListener::ProcessAdd(Trade<Bond> &data) {
    b_position_ser->AddTrade(data);
}

#endif /* BondPositionService_h */
