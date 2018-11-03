#include <enulib/currency.hpp>
#include <enulib/enu.hpp>
#include <vector>

#define ENU_SYMBOL S(4, ENU)  
#define BTC_SYMBOL S(8, BTC)  
#define ENUBTC_SYMBOL S(4, ENUBTC)  


using namespace enumivo;

class ex : public contract {
 public:
  ex(account_name self)
      : contract(self) {}

  void receivedenu(const currency::transfer& transfer);
  void receivedbtc(const currency::transfer& transfer);
  void receivedshares(const currency::transfer& transfer);

  void apply(account_name contract, action_name act);

};
