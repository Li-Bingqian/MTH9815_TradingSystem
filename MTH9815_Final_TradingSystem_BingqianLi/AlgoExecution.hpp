//
//  AlgoExecution.hpp
//  MTH9815_Final_TradingSystem_BingqianLi
//
//  Created by Aubree Li on 12/16/22.
//

#ifndef AlgoExecution_h
#define AlgoExecution_h

#include "executionservice.hpp"
#include "utilities.hpp"
#include "BondMarketDataService.hpp"
#include <random>

template <typename T>
class AlgoExecution {
private:
    ExecutionOrder<T> execution_order_;
    static int order_id_;
    Market market_;

public:
    AlgoExecution(ExecutionOrder<T> exe_order): execution_order_(exe_order), market_(CME) {}

    ExecutionOrder<T> GetOrder() const { return execution_order_; }

    Market GetMarket() { return market_; }

    void UpdateAlgo(OrderBook<T> order_book) {
        PricingSide pricing_side=PricingSide(order_id_ % 2);
        auto product = order_book.GetProduct();
        // check whether it is the product to execute
        if(product.GetProductId() != execution_order_.GetProduct().GetProductId()) return;

        OrderType order_type;
        // randomly choose order type
        int order_type_rd = rand() % 5;
        switch (order_type_rd) {
            case 0:order_type=FOK;break;
            case 1:order_type=IOC;break;
            case 2:order_type=MARKET;break;
            case 3:order_type=LIMIT;break;
            case 4:order_type=STOP;break;
            default:break;
        }

        double hidden_ratio = 2.;

        auto offer_order = order_book.GetOfferStack()[0];
        auto bid_order = order_book.GetBidStack()[0];
        // get price and quantity data
        double price;
        long visible_quantity, hidden_quantity;
        if(pricing_side==BID){
            price=bid_order.GetPrice();
            // check spread
            if (offer_order.GetPrice() - price < 1./128.) {
                visible_quantity = bid_order.GetQuantity();
                hidden_quantity = visible_quantity * hidden_ratio;
            }
        } else {
            double price=offer_order.GetPrice();
            if (price - bid_order.GetPrice() < 1./128.) {
                visible_quantity = offer_order.GetQuantity();
                hidden_quantity = visible_quantity * hidden_ratio;
            }
        }

        execution_order_ = ExecutionOrder<T>(product, pricing_side,
                                             fill_str(to_string(order_id_), 8, '0'),
                                             order_type, price, visible_quantity,
                                             hidden_quantity ,
                                             fill_str(to_string(order_id_), 8, '0')
                                             , false);
        order_id_++;
    }
};

template <class T>
int AlgoExecution<T>::order_id_=0;



#endif /* AlgoExecution_h */
