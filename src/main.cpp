#include "service/OMSService.hpp"
#include "adapters/deribit/DeribitClient.hpp"
#include <iostream>

int main()
{
    OMSService oms;

    DeribitClient deribit(
        "0TNl3dx4",
        "bQE6syR1WNRF4d6rw1x3DDS_VrYP8EVTR0qfWbRHTSg");

    Order o{
        "BTC-PERPETUAL",
        Side::Buy,
        OrderType::Limit,
        25000,
        10
    };

    std::cout << oms.placeOrder(deribit, o) << std::endl;
}