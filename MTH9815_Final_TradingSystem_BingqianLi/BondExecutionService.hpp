//
//  BondExecutionService.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/17/22.
//

/**
 Define BondExecutionService listen data from BondAlgoExecutionService
 and flow to BondAlgoExecutionServiceConnector to publish

 Define BondAlgoExecutionServiceConnector to print data out

 Define BondExecutionServiceListener to listen data from BondAlgoExecutionService
 */

#ifndef BondExecutionService_h
#define BondExecutionService_h

#include "BondAlgoExecutionService.hpp"
#include "soa.hpp"
#include "executionservice.hpp"
using namespace std;

class BondExecutionServiceConnector;

class BondExecutionService : public ExecutionService<Bond> {
private:
    map<string, ExecutionOrder<Bond> > executionMap;
    vector<ServiceListener<ExecutionOrder<Bond> >*> listeners;
    BondExecutionServiceConnector* b_execution_con;

public:
    BondExecutionService(BondExecutionServiceConnector* bond_exe_con);      // cstor

    // override virtual func in Service
    ExecutionOrder<Bond>& GetData(string key) override { return executionMap.at(key);}

    // usually called by connector, no implementation here
    void OnMessage(ExecutionOrder<Bond> &data) override {}

    // Add a listener (BondPositionServiceListener) to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<ExecutionOrder<Bond> > *listener) override {
        listeners.push_back(listener);
    }

    // Get all listeners on the Service.
    const vector<ServiceListener<ExecutionOrder<Bond> > *>& GetListeners() const override {
        return listeners;
    }

    // add price based on orderbook, called by listener when adding process
    void ExecuteOrder(ExecutionOrder<Bond>& exe_order, Market market) override;

    // update algo to AlgoMap and add listeners
    // called by BondExecutionServiceListener
    void UpdateAlgoExecution(const AlgoExecution<Bond>& algo);
};



class BondExecutionServiceConnector : public Connector<ExecutionOrder<Bond> > {
public:
    // ctor
    BondExecutionServiceConnector() {}

    // override pure virtual functions in base class
    // publish ExecutionOrder, called by BondExecutionService
    void Publish(ExecutionOrder<Bond>& data) override;

    // publish only, no subsribe
    void Subscribe() {}
};


class BondExecutionServiceListener : public ServiceListener<AlgoExecution<Bond> > {
public:
    // ctor
    BondExecutionServiceListener(BondExecutionService* bond_execution);

    // override virtual functions in ServiceListener class
    // listen from BondAlgoExecutionService
    // and call BondExecutionService::AddAlgoExecution and BondExecutionService::ExecuteOrder
    void ProcessAdd(AlgoExecution<Bond>& data) override;

    // no implementation
    void ProcessRemove(AlgoExecution<Bond> &data) override{}

    // no implementation
    void ProcessUpdate(AlgoExecution<Bond> &data) override{}

private:
    BondExecutionService* b_execution_ser;
};



BondExecutionService::BondExecutionService(BondExecutionServiceConnector* bond_exe_con) : b_execution_con(bond_exe_con) {
    executionMap = map<string, ExecutionOrder<Bond> >();
}       // cstor


void BondExecutionService::ExecuteOrder(ExecutionOrder<Bond> &exe_order, Market market){
    b_execution_con->Publish(exe_order);
}


void BondExecutionService::UpdateAlgoExecution(const AlgoExecution<Bond> &algo) {
    auto exe_order = algo.GetOrder();
    string order_id = exe_order.GetProduct().GetProductId();
    // update executionMap
    if (executionMap.find(order_id) != executionMap.end()) {executionMap.erase(order_id);}

    executionMap.insert(pair<string,ExecutionOrder<Bond> >(order_id,exe_order));
    for(auto& listener :listeners) {
            listener->ProcessAdd(exe_order);
        }
}


void BondExecutionServiceConnector::Publish(ExecutionOrder<Bond>& data) {
    auto bond = data.GetProduct();
    string oder_type;
    switch(data.GetOrderType()) {
        case FOK: oder_type = "FOK"; break;
        case MARKET: oder_type = "MARKET"; break;
        case LIMIT: oder_type = "LIMIT"; break;
        case STOP: oder_type = "STOP"; break;
        case IOC: oder_type = "IOC"; break;
    }
    cout<<bond.GetProductId()<<" OrderId: "<<data.GetOrderId()<< "\n"
        <<"\tPricingSide: "<<(data.GetSide()==BID? "Bid":"Ask")
        <<" OrderType: "<<oder_type<<"\tIsChildOrder: "<<(data.IsChildOrder()?"True":"False")
        <<"\n"
        <<"\tPrice: "<<data.GetPrice()<<"\tVisibleQuantity: "<<data.GetVisibleQuantity()
        <<"\tHiddenQuantity: "<<data.GetHiddenQuantity()<<endl<<endl;
}



BondExecutionServiceListener::BondExecutionServiceListener(BondExecutionService* bond_execution) : b_execution_ser(bond_execution) {}


void BondExecutionServiceListener::ProcessAdd(AlgoExecution<Bond> &data) {
    auto exe_order = data.GetOrder();
    b_execution_ser->UpdateAlgoExecution(data);
    b_execution_ser->ExecuteOrder(exe_order, BROKERTEC);
}

#endif /* BondExecutionService_h */
