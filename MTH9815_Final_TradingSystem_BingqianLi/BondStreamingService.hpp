//
//  BondStreamingService.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/19/22.
//

#ifndef BondStreamingService_h
#define BondStreamingService_h

#include "streamingservice.hpp"
#include "BondAlgoStreamingService.hpp"

class BondStreamingServiceConnector;

class BondStreamingService : public StreamingService<Bond> {
private:
    map<string, PriceStream<Bond> > priceStreamMap;
    vector<ServiceListener<PriceStream<Bond> >*> listeners;
    BondStreamingServiceConnector* b_streaming_con;

public:
    BondStreamingService(BondStreamingServiceConnector* bond_stream) : b_streaming_con(bond_stream) {}       // cstor

    // override virtual func in Service
    PriceStream<Bond>& GetData(string key) override { return priceStreamMap.at(key);}

    // usually called by connector, no implementation here
    void OnMessage(PriceStream<Bond> &data) override {}

    // Add a listener (BondPositionServiceListener) to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<PriceStream<Bond> > *listener) override {
        listeners.push_back(listener);
    }

    // Get all listeners on the Service.
    const vector<ServiceListener<PriceStream<Bond> > *>& GetListeners() const override {
        return listeners;
    }

    // print the price
    void PublishPrice(PriceStream<Bond>& price_stream) override;

    // called by BondStreamingServiceListener
    void UpdateAlgo(const AlgoStream<Bond>& algo);
};


class BondStreamingServiceConnector : public Connector<PriceStream<Bond> > {
public:
    // ctor
    BondStreamingServiceConnector() {}

    // override pure virtual functions in base class
    // publish ExecutionOrder, called by BondExecutionService
    void Publish(PriceStream<Bond>& data) override;

    // publish only, no subsribe
    void Subscribe() {}
};



class BondStreamingServiceListener : public ServiceListener<AlgoStream<Bond> > {
public:
    // ctor
    BondStreamingServiceListener(BondStreamingService* bond_stream) : b_streaming_ser(bond_stream) {}

    // override virtual functions in ServiceListener class
    // listen from BondAlgoExecutionService
    // and call BondExecutionService::AddAlgoExecution and BondExecutionService::ExecuteOrder
    void ProcessAdd(AlgoStream<Bond>& data) override;

    // no implementation
    void ProcessRemove(AlgoStream<Bond> &data) override{}

    // no implementation
    void ProcessUpdate(AlgoStream<Bond> &data) override{}

private:
    BondStreamingService* b_streaming_ser;
};

// print the price
void BondStreamingService::PublishPrice(PriceStream<Bond> &price_stream) {
    b_streaming_con->Publish(price_stream);
}


void BondStreamingService::UpdateAlgo(const AlgoStream<Bond> &algo) {
    auto stream = algo.GetPriceStream();
    auto id = stream.GetProduct().GetProductId();
    if (priceStreamMap.find(id) != priceStreamMap.end()) {priceStreamMap.erase(id);}

    priceStreamMap.insert(pair<string,PriceStream<Bond> >(id,stream));
    for(auto& listener :listeners) {
            listener->ProcessAdd(stream);
        }
}


void BondStreamingServiceConnector::Publish(PriceStream<Bond> &data) {
    auto bond = data.GetProduct();
    auto bid = data.GetBidOrder();
    auto offer = data.GetOfferOrder();

    cout<<bond.GetProductId()<< "\n"
        <<"\tBid\t"<<"Price: "<<bid.GetPrice()<<"\tVisibleQuantity: "<<bid.GetVisibleQuantity()
        <<"\tHiddenQuantity: "<<bid.GetHiddenQuantity()<<"\n"
        <<"\tAsk\t"<<"Price: "<<offer.GetPrice()<<"\tVisibleQuantity: "<<offer.GetVisibleQuantity()
        <<"\tHiddenQuantity: "<<offer.GetHiddenQuantity()<<"\n";
}


void BondStreamingServiceListener::ProcessAdd(AlgoStream<Bond> &data) {
    b_streaming_ser->UpdateAlgo(data);
    auto price_stream = data.GetPriceStream();
    b_streaming_ser->PublishPrice(price_stream);
}
#endif /* BondStreamingService_h */
