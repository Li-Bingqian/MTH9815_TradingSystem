//
//  BondPricingService.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/13/22.
//

/**
 Define BondPricingService to recieve data from BondPricingServiceConnector
 and flow data to BondAlgoStreamingServiceListener

 Define BondPricingServiceConnector to read data from price.txt
 */

#ifndef BondPricingService_h
#define BondPricingService_h

#include "pricingservice.hpp"
#include "products.hpp"
#include "soa.hpp"
#include "utilities.hpp"

using namespace std;

class BondPricingService: public PricingService<Bond>  {
private:
    map<string, Price<Bond> > priceMap;
    vector<ServiceListener<Price<Bond> >* > listeners;

public:
    BondPricingService();       // cstor

    // override virtual func in Service
    Price<Bond>& GetData(string key) override;

    // The callback that a Connector should invoke for any new or updated data
    // add new bond price to pricemap
    void OnMessage(Price<Bond> &data) override;

    // Add a listener (BondAlgoStreamingService) to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Price<Bond> > *listener) override;

    // Get all listeners on the Service.
    const vector<ServiceListener<Price<Bond> > *>& GetListeners() const override;
};



class BondPricingServiceConnector: public Connector<Price<Bond> > {
public:
    BondPricingServiceConnector (BondPricingService* bond_pricing, BondProductService* bond_product);

    // subscribe-only where Publish() does nothing
    void Publish(Price<Bond>& info) override;
    // subscribe data from price.txt file and invoked via the BondMarketDataService.OnMessage()
    void Subscribe();

private:
    BondPricingService* b_pricing_ser;
    BondProductService* b_prod_ser;
};


BondPricingService::BondPricingService() {
    priceMap = map<string, Price<Bond> >();
}


Price<Bond>& BondPricingService::GetData(string key) {
    return priceMap.at(key);
}


void BondPricingService::OnMessage(Price<Bond> &data) {
    // flow data
    string id = data.GetProduct().GetProductId();
    // update the price map
    if (priceMap.find(id) != priceMap.end()) {priceMap.erase(id);}
    priceMap.insert(pair<string, Price<Bond> > (id, data));

    // flow the data to listeners
    for (auto& l : listeners) {
        l -> ProcessAdd(data);
    }
}


void BondPricingService::AddListener(ServiceListener<Price<Bond> > *listener) {
    listeners.push_back(listener);
}


const vector<ServiceListener<Price<Bond> > *>& BondPricingService::GetListeners() const {
    return listeners;
}


BondPricingServiceConnector::BondPricingServiceConnector (BondPricingService* bond_pricing, BondProductService* bond_product) : b_pricing_ser(bond_pricing), b_prod_ser(bond_product) {

}


void BondPricingServiceConnector::Publish(Price<Bond> &info) {
    return;
}


void BondPricingServiceConnector::Subscribe() {
    // read data from txt file
    fstream newfile;
    newfile.open("price.txt", ios::in);
    if (newfile.is_open()) {
        string data_row;
        while(getline(newfile, data_row)) {
            stringstream line(data_row);
            string temp;
            // the first column is id, store and get the bond
            getline(line, temp, ',');
            auto bond = b_prod_ser->GetData(temp);
            // mid price
            getline(line, temp, ',');
            auto p = PriceStringToInt(temp);
            // spread
            getline(line, temp, ',');
            auto spread = PriceStringToInt(temp);

            Price<Bond> bondPrice(bond, p, spread);
            // call Service.OnMessage(), flow data
            b_pricing_ser->OnMessage(bondPrice);
        }
    }
}

#endif /* BondPricingService_h */
