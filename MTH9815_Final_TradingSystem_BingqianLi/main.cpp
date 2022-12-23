//
//  main.cpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/11/22.
//

#include <iostream>
#include "executionservice.hpp"
#include "historicaldataservice.hpp"
#include "BondMarketDataService.hpp"
#include "DataGenerator.hpp"
#include "BondPricingService.hpp"
#include "BondTradeBookingService.hpp"
#include "BondInquiryService.hpp"
#include "BondAlgoExecutionService.hpp"
#include "BondAlgoStreamingService.hpp"
#include "BondExecutionService.hpp"
#include "BondStreamingService.hpp"
#include "BondPositionService.hpp"
#include "BondRiskService.hpp"
#include "GUIService.hpp"
#include "BondHistoricalDataService.hpp"

int main() {
    // insert code here...
    MarketDataGenerator(1000000 );
    PriceGenerator(1000000);
//    MarketDataGenerator(100 );
//    PriceGenerator(100);
    TradesGenerator(10);
    InquriesGenerator(10);
    std::cout << "Hello, World!\n";


    vector<Bond> bonds;
       for(int i=0; i<bondCusip.size(); i++){
           bonds.push_back(Bond(bondCusip[i], CUSIP, "Treasury", bondCoupon[i], bondMaturity[i]));
       }

       BondProductService* bondproductservice = new BondProductService();
       for(int i=0; i<bondCusip.size(); i++){
           bondproductservice->Add(bonds[i]);
       }

       BondPricingService* bondpricingservice= new BondPricingService();
       BondPricingServiceConnector* bondpricingserviceconnector = new BondPricingServiceConnector(bondpricingservice, bondproductservice);

       BondAlgoStreamingService* bondalgostreamingservice = new BondAlgoStreamingService();
       BondAlgoStreamingServiceListener* bondalgostreamingservicelistener = new BondAlgoStreamingServiceListener(bondalgostreamingservice);

       bondpricingservice->AddListener(bondalgostreamingservicelistener);

       BondStreamingServiceConnector* bondstreamingserviceconnector = new BondStreamingServiceConnector();
       BondStreamingService* bondstreamingservice = new BondStreamingService(bondstreamingserviceconnector);
       BondStreamingServiceListener* bondstreamingservicelistener = new BondStreamingServiceListener(bondstreamingservice);

       bondalgostreamingservice->AddListener(bondstreamingservicelistener);




       BondMarketDataService* bondmarketdataservice = new BondMarketDataService();
       BondMarketDataServiceConnector* bondmarketdataserviceconnector = new BondMarketDataServiceConnector(bondmarketdataservice, bondproductservice);

       BondAlgoExecutionService * bondalgoexecutionservice = new BondAlgoExecutionService();
       BondAlgoExecutionServiceListener* bondalgoexecutionservicelistener = new BondAlgoExecutionServiceListener(bondalgoexecutionservice);

       bondmarketdataservice->AddListener(bondalgoexecutionservicelistener);

       BondExecutionServiceConnector* bondexecutionserviceconnector = new BondExecutionServiceConnector();
       BondExecutionService* bondexecutionservice = new BondExecutionService(bondexecutionserviceconnector);
       BondExecutionServiceListener* bondexecutionservicelistener = new BondExecutionServiceListener(bondexecutionservice);

       bondalgoexecutionservice->AddListener(bondexecutionservicelistener);




       BondTradeBookingService* bondtradebookingservice = new BondTradeBookingService();
       BondTradeBookingServiceConnector* bondtradebookingserviceconnector = new BondTradeBookingServiceConnector(bondtradebookingservice, bondproductservice);
       BondTradeBookingServiceListener* bondtradebookingservicelistener = new BondTradeBookingServiceListener(bondtradebookingservice);

       bondexecutionservice->AddListener(bondtradebookingservicelistener);

       BondPositionService* bondpositionservice = new BondPositionService();
       BondPositionServiceListener* bondpositionservicelistener = new BondPositionServiceListener(bondpositionservice);

       bondtradebookingservice->AddListener(bondpositionservicelistener);

       BondRiskService* bondriskservice = new BondRiskService();
       BondRiskServiceListener* bondriskservicelistener = new BondRiskServiceListener(bondriskservice);

       bondpositionservice->AddListener(bondriskservicelistener);




       BondInquiryService* bondinquiryservice = new BondInquiryService();
       BondInquiryServiceConnector* bondinquiryserviceconnector = new BondInquiryServiceConnector(bondinquiryservice, bondproductservice);

       GUIServiceConnector* guiserviceconnector = new GUIServiceConnector();
       GUIService* guiservice = new GUIService(guiserviceconnector);
       GUIServiceListener* guiservicelistener = new GUIServiceListener(guiservice);

       bondpricingservice->AddListener(guiservicelistener);




       BondHistoricalPositionServiceConnector* bondhistoricalpositionserviceconnector = new BondHistoricalPositionServiceConnector();
       BondHistoricalPositionService* bondhistoricalpositionservice = new BondHistoricalPositionService(bondhistoricalpositionserviceconnector);
       BondHistoricalPositionServiceListener* bondhistoricalpositionservicelistener = new BondHistoricalPositionServiceListener(bondhistoricalpositionservice);
       bondpositionservice->AddListener(bondhistoricalpositionservicelistener);

       BondHistoricalRiskServiceConnector* bondhistoricalriskserviceconnector = new BondHistoricalRiskServiceConnector();
       BondHistoricalRiskService* bondhistoricalriskservice = new BondHistoricalRiskService(bondhistoricalriskserviceconnector);
       BondHistoricalRiskServiceListener* bondhistoricalriskservicelistener = new BondHistoricalRiskServiceListener(bondhistoricalriskservice);
       bondriskservice->AddListener(bondhistoricalriskservicelistener);

       BondHistoricalExecutionServiceConnector* bondhistoricalexecutionserviceconnector = new BondHistoricalExecutionServiceConnector();
       BondHistoricalExecutionService* bondhistoricalexecutionservice = new BondHistoricalExecutionService(bondhistoricalexecutionserviceconnector);
       BondHistoricalExecutionServiceListener* bondhistoricalexecutionservicelistener = new BondHistoricalExecutionServiceListener(bondhistoricalexecutionservice);
       bondexecutionservice->AddListener(bondhistoricalexecutionservicelistener);

       BondHistoricalStreamingServiceConnector* bondhistoricalstreamingserviceconnector = new BondHistoricalStreamingServiceConnector();
       BondHistoricalStreamingService* bondhistoricalstreamingservice = new BondHistoricalStreamingService(bondhistoricalstreamingserviceconnector);
       BondHistoricalStreamingServiceListener* bondhistoricalstreamingservicelistener = new BondHistoricalStreamingServiceListener(bondhistoricalstreamingservice);
       bondstreamingservice->AddListener(bondhistoricalstreamingservicelistener);

       BondHistoricalInquiryServiceConnector* bondhistoricalinquiryserviceconnector = new BondHistoricalInquiryServiceConnector();
       BondHistoricalInquiryService* bondhistoricalinquiryservice = new BondHistoricalInquiryService(bondhistoricalinquiryserviceconnector);
       BondHistoricalInquiryServiceListener* bondhistoricalinquiryservicelistener = new BondHistoricalInquiryServiceListener(bondhistoricalinquiryservice);
       bondinquiryservice->AddListener(bondhistoricalinquiryservicelistener);


       bondpricingserviceconnector->Subscribe();
       bondmarketdataserviceconnector->Subscribe();
       bondtradebookingserviceconnector->Subscribe();
       bondinquiryserviceconnector->Subscribe();

    return 0;
}

