//
//  BondRiskService.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/20/22.
//

#ifndef BondRiskService_h
#define BondRiskService_h

#include "BondPositionService.hpp"
#include "riskservice.hpp"
#include "DataGenerator.hpp"    // to get bond information
#include <iostream>

class BondRiskService : public RiskService<Bond> {
public:
    BondRiskService() {
        riskMap=map<string,PV01<Bond> >();
    }

    // override virtual func in Service
    PV01<Bond>& GetData(string key) override { return riskMap.at(key);}

    // usually called by connector, no implementation here
    void OnMessage(PV01<Bond> &data) override {}

    // Add a listener (BondPositionServiceListener) to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<PV01<Bond> > *listener) override {
        listeners.push_back(listener);
    }

    // Get all listeners on the Service.
    const vector<ServiceListener<PV01<Bond> > *>& GetListeners() const override {
        return listeners;
    }

    // called by BondPositionServiceListener
    void AddPosition(Position<Bond> &position) override;

    // Get the bucketed risk for the bucket sector
    PV01< BucketedSector<Bond> >& GetBucketedRisk(const BucketedSector<Bond> &sector) const override;


private:
    map<string, PV01<Bond> > riskMap;
    vector<ServiceListener<PV01<Bond> >*> listeners;
};



class BondRiskServiceListener : public ServiceListener<Position<Bond> > {
private:
    BondRiskService* b_risk_ser;

public:
    // ctor
    BondRiskServiceListener(BondRiskService* bond_risk) : b_risk_ser(bond_risk) {}

    // override all pure virtual functions in base class
    // call bondPositionService AddTrade
    virtual void ProcessAdd(Position<Bond> &data) override;

    // no implementation
    void ProcessRemove(Position<Bond> &data) override {}

    // no implementation
    void ProcessUpdate(Position<Bond> &data) override{}

};



void BondRiskService::AddPosition(Position<Bond> &position) {
    long aggregate = position.GetAggregatePosition();
    auto bond = position.GetProduct();
    string id = bond.GetProductId();

    if (riskMap.find(id) != riskMap.end()) {
        (riskMap.find(id)->second).AddQuantity(aggregate);
    } else {
        riskMap.insert(pair<string, PV01<Bond> >(id,PV01<Bond>(bond, bondRisk[id], aggregate)));

        AddPosition(position);
        return;
    }

    for(auto& listener : listeners) {
        listener->ProcessAdd(riskMap.find(id)->second);
    }
}


PV01< BucketedSector<Bond> >& BondRiskService::GetBucketedRisk(const BucketedSector<Bond> &sector) const {
    double pv01 = 0;
    long q = 0;
        for(auto& product : sector.GetProducts()) {
            pv01+=riskMap.at(product.GetProductId()).GetPV01();
            q += riskMap.at(product.GetProductId()).GetQuantity();
        }

    PV01<BucketedSector<Bond> > pv01_bucket(sector, pv01, q);
    return pv01_bucket;
}


void BondRiskServiceListener::ProcessAdd(Position<Bond> &data) {
    b_risk_ser->AddPosition(data);
}

#endif /* BondRiskService_h */
