//
//  productservice.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/11/22.
//

#ifndef productservice_hpp
#define productservice_hpp

#include "products.hpp"
#include "soa.hpp"
#include <iostream>
#include <map>


/**
 * Bond Product Service to own reference data over a set of bond securities.
 * Key is the productId string, value is a Bond.
 */
class BondProductService : public Service<string,Bond>
{

public:
    // BondProductService ctor
    BondProductService();

    // Return the bond data for a particular bond product identifier
    Bond& GetData(string productId) override;

    // Add a bond to the service (convenience method)
    void Add(Bond &bond);

    // override pure virtue func in Service
    // but not implementation in this service
    void OnMessage(Bond &data) override {}
    void AddListener(ServiceListener<Bond > *listener) override{}
    const vector< ServiceListener<Bond >* >& GetListeners() const override;

private:
    map<string,Bond> bondMap; // cache of bond products

};


BondProductService::BondProductService() {
    bondMap = map<string, Bond>();
}

Bond& BondProductService::GetData(string productId)
{
  return bondMap[productId];
}

void BondProductService::Add(Bond &bond)
{
  bondMap.insert(pair<string,Bond>(bond.GetProductId(), bond));
}

const vector<ServiceListener<Bond >* >& BondProductService::GetListeners() const {
    return std::move(vector< ServiceListener<Bond >* >());
}
#endif /* productservice_hpp */
