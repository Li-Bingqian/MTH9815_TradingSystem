//
//  BondInquiryService.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/13/22.
//

/**
 Define

 BondInquiryServic recieves data from BondInquiryServicConnector
 and flow datra into BondHistoricalDataServiceListener

 BondInquiryServicConnector read data from inquiries.txt
 */

#ifndef BondInquiryService_h
#define BondInquiryService_h

#include "soa.hpp"
#include "inquiryservice.hpp"
#include "utilities.hpp"
#include "productservice.hpp"

class BondInquiryService : public InquiryService<Bond>{
private:
    map<string, Inquiry<Bond> > inquiryMap;
    vector<ServiceListener<Inquiry<Bond> >* > listeners;

public:
    BondInquiryService();       // cstor

    // override virtual func in Service
    Inquiry<Bond>& GetData(string key) override;

    // The callback that a Connector should invoke for any new or updated data
    // add new bond price to pricemap
    void OnMessage(Inquiry<Bond> &data) override;

    // Add a listener (BondAlgoStreamingService) to the Service for callbacks on add, remove, and update events
    // for data to the Service.
    void AddListener(ServiceListener<Inquiry<Bond> > *listener) override;

    // Get all listeners on the Service.
    const vector<ServiceListener<Inquiry<Bond> > *>& GetListeners() const override;

public:

  // Send a quote back to the client
    void SendQuote(const string &inquiryId, double price) override;

  // Reject an inquiry from the client
    void RejectInquiry(const string &inquiryId) override;
};


class BondInquiryServiceConnector : public Connector<Inquiry<Bond> > {
public:
    // cstor
    BondInquiryServiceConnector (BondInquiryService* bond_inquiry, BondProductService* bond_product);

    // subscribe-only where Publish() does nothing
    void Publish(Inquiry<Bond>& info) override;
    // subscribe data from inquiry.txt file
    void Subscribe();

private:
    BondInquiryService* b_inquiry_ser;
    BondProductService* b_prod_ser;

};


BondInquiryService::BondInquiryService() {
    inquiryMap = map<string, Inquiry<Bond> >();
}


Inquiry<Bond>& BondInquiryService::GetData(string key) {
    return inquiryMap.at(key);
}


void BondInquiryService::OnMessage(Inquiry<Bond> &data) {
    // send an update of the Inquiry object with a DONE state
    data.SetState(data.GetPrice(), DONE);
    // add to map
    inquiryMap.insert(pair<string,Inquiry<Bond> >(data.GetInquiryId(),data));
    // flow the data to listeners
    for (auto& l : listeners) {
        l -> ProcessAdd(data);
    }
}


void BondInquiryService::AddListener(ServiceListener<Inquiry<Bond> > *listener) {
    listeners.push_back(listener);
}


const vector<ServiceListener<Inquiry<Bond> > *>& BondInquiryService::GetListeners() const {
    return listeners;
}


void BondInquiryService::SendQuote(const string &inquiryId, double price) {
    // SEND quote to the client. But no client in this case, does nothing
}


void BondInquiryService::RejectInquiry(const string &inquiryId) {
    // REJECT quote to the client. But no client in this case, does nothing
}



BondInquiryServiceConnector::BondInquiryServiceConnector (BondInquiryService* bond_inquiry, BondProductService* bond_product) : b_inquiry_ser(bond_inquiry), b_prod_ser(bond_product) {}


void BondInquiryServiceConnector::Subscribe() {
    // read data from txt file
    fstream newfile;
    newfile.open("inquiry.txt", ios::in);
    if (newfile.is_open()) {
        string data_row;
        while(getline(newfile, data_row)) {
            stringstream line(data_row);
            string temp;
            getline(line, temp, ',');
            auto bond = b_prod_ser->GetData(temp);
            // inquiry id
            getline(line, temp, ',');
            auto inquiry_id = temp;
            // side
            getline(line, temp, ',');
            Side side = (temp=="buy" ? BUY : SELL);
            // quantity
            getline(line, temp, ',');
            auto quantity = stol(temp);
            // quote
            getline(line, temp, ',');
            auto quote = PriceStringToInt(temp);
            // book
            getline(line, temp, ',');
            InquiryState inquiry_state = RECEIVED;

            Inquiry<Bond> inquiry(inquiry_id, bond, side, quantity, quote, inquiry_state);
            // call Service.OnMessage(), flow data
            b_inquiry_ser -> OnMessage(inquiry);
        }
    }
}


void BondInquiryServiceConnector::Publish(Inquiry<Bond> &info) {}

#endif /* BondInquiryService_h */
