//
//  BondMarketDataService.hpp
//  MTH9815_TradingSystem
//
//  Created by Aubree Li on 12/11/22.
//

/**
 define BondMarketDataServiceConnector to recieve data from txt file
 define BondMarketDataService to recieve data from connector and add BondAlgoExecutionService as listener
 */

#ifndef BondMarketDataService_h
#define BondMarketDataService_h

#include "soa.hpp"
#include "marketdataservice.hpp"
#include "productservice.hpp"
#include "products.hpp"
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include "utilities.hpp"

//void agg_depth (const vector<Order>& stack, unordered_map<double, long>& depth) {
//    for (auto& elem : stack) {
//        if (depth.find(elem.GetPrice()) != depth.end()) {
//            depth[elem.GetPrice()] += elem.GetQuantity();
//        } else {
//            depth[elem.GetPrice()] = elem.GetQuantity();
//        }
//    }
//}

class BondMarketDataService : public MarketDataService<Bond> {
public:
    BondMarketDataService();

    // override virtual func from MarketDataService
    // Get the best bid/offer order
    const BidOffer& GetBestBidOffer(const string &productId) override;
    // Aggregate the order book
    const OrderBook<Bond>& AggregateDepth(const string &productId) override;

    // override virtual func Service
    // Get data on our service given a key
    OrderBook<Bond>& GetData(string key) override;

    // The callback that a Connector should invoke for any new or updated data
    void OnMessage(OrderBook<Bond> &data) override;

    // Add a listener to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<OrderBook<Bond> > *listener) override {
        listeners.push_back(listener);
    }
    // Get all listeners on the Service.
    const vector<ServiceListener<OrderBook<Bond> > *>& GetListeners() const override;


private:
    map<string, OrderBook<Bond> > orderMap;
    vector<ServiceListener<OrderBook<Bond> > *> listeners;
};




class BondMarketDataServiceConnector : public Connector<OrderBook<Bond> > {
public:
    BondMarketDataServiceConnector (BondMarketDataService* bond_mds, BondProductService* bond_ps);

    // subscribe-only where Publish() does nothing
    void Publish(OrderBook<Bond>& info) override;
    // subscribe data from txt file and invoked via the BondMarketDataService.OnMessage()
    void Subscribe();

private:
    BondMarketDataService* b_marketdata_ser;
    BondProductService* b_prod_ser;
};



// BondMarketDataService
BondMarketDataService::BondMarketDataService() {
    orderMap = map<string, OrderBook<Bond> > ();
}


const BidOffer& BondMarketDataService::GetBestBidOffer(const string &productId) {
    auto product_ob = orderMap.at(productId);   // get the orderbook of a specified product
    vector<Order> offers = product_ob.GetOfferStack();
    vector<Order> bids = product_ob.GetBidStack();

    auto bid_max = bids[0];
    auto offer_min = offers[0];

    for (int i = 1; i < bids.size(); ++i) {
        bid_max = bids[i].GetPrice() > bid_max.GetPrice() ? bids[i] : bid_max;
    }

    for (int i = 1; i < offers.size(); ++i) {
        offer_min = offers[i].GetPrice() < offer_min.GetPrice() ? offers[i] : offer_min;
    }

    return std::move(BidOffer(bid_max, offer_min));
}


const OrderBook<Bond>& BondMarketDataService::AggregateDepth(const string &productId) {
    auto product_ob = orderMap.at(productId);   // get the orderbook of a specified product
    vector<Order> offers = product_ob.GetOfferStack();
    vector<Order> bids = product_ob.GetBidStack();

    // aggregate quantity by the same price
    unordered_map<double, long> bid_depth, ask_depth;
    auto agg_depth = [](const vector<Order>& stack, unordered_map<double, long>& depth) {
        for (auto& elem : stack) {
            if (depth.find(elem.GetPrice()) != depth.end()) {
                depth[elem.GetPrice()] += elem.GetQuantity();
            } else {
                depth[elem.GetPrice()] = elem.GetQuantity();
            }
        }
    };
    // aggregate bid and ask
    agg_depth(bids, bid_depth);
    agg_depth(offers, ask_depth);

    // generate new order stacks after aggregation
    vector<Order> agg_bid, agg_ask;
    for (auto& elem : bid_depth) {
        agg_bid.push_back(Order(elem.first, elem.second, BID));
    }

    for (auto& elem : ask_depth) {
        agg_ask.push_back(Order(elem.first, elem.second, OFFER));
    }

    return std::move(OrderBook<Bond>(product_ob.GetProduct(), agg_bid, agg_ask));
}


OrderBook<Bond>& BondMarketDataService::GetData(string key) {
    return orderMap.at(key);
}


void BondMarketDataService::OnMessage(OrderBook<Bond> &data) {
     // flow data
    string id = data.GetProduct().GetProductId();
    // update the order book
    if (orderMap.find(id) != orderMap.end()) {orderMap.erase(id);}
    orderMap.insert(pair<string, OrderBook<Bond> > (id, data));

    // get best order for listeners : algoexecution
    auto best_order = GetBestBidOffer(id);
    vector<Order> bid, ask;
    bid.push_back(best_order.GetBidOrder());
    ask.push_back(best_order.GetOfferOrder());
    auto best_order_book = OrderBook<Bond>(data.GetProduct(), bid, ask);

    // flow the data to listeners
    for (auto& l : listeners) {
        l -> ProcessAdd(best_order_book);
    }
}


//void BondMarketDataService::AddListener(ServiceListener<OrderBook<Bond> > *listener) {
//    listeners.push_back(listener);
//}


const vector<ServiceListener<OrderBook<Bond> > *>& BondMarketDataService::GetListeners() const {
    return listeners;
}


// BondMarketDataServiceConnector
BondMarketDataServiceConnector::BondMarketDataServiceConnector(BondMarketDataService* bond_mds, BondProductService* bond_ps) : b_marketdata_ser(bond_mds), b_prod_ser(bond_ps) {}

void BondMarketDataServiceConnector::Publish(OrderBook<Bond>& data) {}  // no implementation


void BondMarketDataServiceConnector::Subscribe() {
    // read data from txt file
    fstream newfile;
    newfile.open("marketdata.txt", ios::in);
    if (newfile.is_open()) {
        string data_row;
        while(getline(newfile, data_row)) {
            stringstream line(data_row);
            string temp;
            // the first column is id, store and get the bond
            getline(line, temp, ',');
            auto bond = b_prod_ser->GetData(temp);
            // then, for each row, there are 5 levels of bid and ask respectively
            // read them one by one using getline
            int i = 0;
            vector<Order> bids, asks;

            while (i < 10) {
                getline(line, temp, ',');   // get string price
                double p = PriceStringToInt(temp);
                getline(line, temp, ',');   // get quantity
                long q = stol(temp);
                getline(line, temp, ',');   // get side
                string side = temp;

                if (side == "bid") {
                    bids.push_back(Order(p, q, BID));
                } else {
                    asks.push_back(Order(p, q, OFFER));
                }
                i++;
            }

            OrderBook<Bond> ob(bond, bids, asks);
            // call Service.OnMessage(), flow data
            b_marketdata_ser->OnMessage(ob);
        }
    }
}



#endif /* BondMarketDataService_h */
