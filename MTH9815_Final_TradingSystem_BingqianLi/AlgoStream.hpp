//
//  AlgoStream.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/17/22.
//

#ifndef AlgoStream_h
#define AlgoStream_h

#include "streamingservice.hpp"
#include "pricingservice.hpp"
#include "products.hpp"
#include <iostream>

using namespace std;

template <typename T>
class AlgoStream {
private:
    PriceStream<T> price_stream;

public:
    AlgoStream(const PriceStream<T>& priceStream) : price_stream(priceStream) {}

    PriceStream<T> GetPriceStream() const {
        return price_stream;
    }


    // update price stream with product price
    void UpdateAlgo(Price<T> price) {
        auto product = price.GetProduct();
        // check whether it the same product id to update
        if (product.GetProductId() != price_stream.GetProduct().GetProductId()) return;

        double mid_p = price.GetMid();
        double spread = price.GetBidOfferSpread();
        double bid = mid_p - spread/2.;
        double offer = mid_p + spread/2.;


        //check whether dspread is tight
         if (spread < 1/128.) {
             double hidden_ratio = 2.;
             // Alternate visible sizes between 1000000 and 2000000
             double visible_quantity = 1000000 + (rand() % 1000001);
             double hidden_quantity = visible_quantity * hidden_ratio;
             PriceStreamOrder bid_stream(bid, visible_quantity, hidden_quantity, BID);
             PriceStreamOrder offer_stream(offer, visible_quantity, hidden_quantity, OFFER);
             PriceStream<Bond> priceStream = PriceStream<T>(product, bid_stream, offer_stream);
             price_stream = priceStream;
        }
    }
};




#endif /* AlgoStream_h */
