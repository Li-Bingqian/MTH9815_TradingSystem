//
//  GUIService.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/21/22.
//

#ifndef GUIService_hpp
#define GUIService_hpp

#include "soa.hpp"
#include "products.hpp"
#include <string>
#include <iostream>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "soa.hpp"
#include "products.hpp"
#include <string>
#include <iostream>
using namespace boost::posix_time;
using namespace std;

class GUIServiceConnector;

class PriceWithTime: public Price<Bond>{
private:
    ptime timeStamp_;
public:
    PriceWithTime(ptime time_stamp,Price<Bond> _price) : timeStamp_(time_stamp), Price<Bond>(_price){
    }

    ptime GetTimeStamp(){
        return timeStamp_;
    }
};


class GUIService : public PricingService<Bond> {
public:
    GUIService(GUIServiceConnector* connector) : gui_ser_con(connector) {
        throtteTime=millisec(3);
        lastTime=microsec_clock::local_time();
        priceMap = map<string, Price<Bond> >();
    }

    // called by listener
    void SendToThrottle(Price<Bond>& data);

    // override virtual func in Service, no implementation
    virtual Price<Bond>& GetData(string key) override {return priceMap.at(key);}

    // usually called by connector, no implementation here
    void OnMessage(Price<Bond> &data) override {}

    // Add a listener (BondPositionServiceListener) to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Price<Bond> > *listener) override {listeners.push_back(listener);}

    // Get all listeners on the Service.
    virtual const vector<ServiceListener<Price<Bond> > *>& GetListeners() const override {return listeners;}

private:
    GUIServiceConnector* gui_ser_con;
    boost::posix_time::ptime lastTime;
    boost::posix_time::time_duration throtteTime;
    vector<ServiceListener<Price<Bond> > *> listeners;
    map<string, Price<Bond> > priceMap;
};


class GUIServiceConnector : public Connector<PriceWithTime> {
public:
    GUIServiceConnector(){}

    // do nothing, publish only
    void Subscribe() {}

    // print price with time
    void Publish(PriceWithTime& data) override {
        auto time=data.GetTimeStamp();
        auto mid=data.GetMid();
        auto spread=data.GetBidOfferSpread();

        ofstream out;
        out.open("gui.txt",ios::app);
        out<<time<<","<<data.GetProduct().GetProductId()<<","<<mid<<","<<spread<<endl;
    }
};



class GUIServiceListener : public ServiceListener<Price<Bond> > {
private:
    GUIService* gui_ser;

public:
    // ctor
    GUIServiceListener(GUIService* gui_serivice) : gui_ser(gui_serivice) {}

    // override all pure virtual functions in base class
    // call bondPositionService AddTrade
    void ProcessAdd(Price<Bond> &data) override {
        gui_ser->SendToThrottle(data);
    }

    // no implementation
    void ProcessRemove(Price<Bond> &data) override {}

    // no implementation
    void ProcessUpdate(Price<Bond> &data) override{}

};


void GUIService::SendToThrottle(Price<Bond> &data) {
        auto newTime = boost::posix_time::microsec_clock::local_time();
        auto duration = newTime - lastTime;
        if (duration >= throtteTime){
            lastTime=newTime;
            auto temp = PriceWithTime(newTime, data);
            gui_ser_con->Publish(temp);
        }
}


#endif /* GUIService_hpp */
