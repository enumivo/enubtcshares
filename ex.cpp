#include "ex.hpp"

#include <cmath>
#include <enulib/action.hpp>
#include <enulib/asset.hpp>
#include "enu.token.hpp"

using namespace enumivo;
using namespace std;

void ex::receivedenu(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.btc.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get BTC balance
  double btc_balance = enumivo::token(N(iou.coin)).
	   get_balance(N(enu.btc.mm), enumivo::symbol_type(BTC_SYMBOL).name()).amount;
  btc_balance = btc_balance/100000000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUBTC_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  double a = btc_balance;
  double b = 2 * btc_balance * enu_balance;
  double c = - btc_balance * enu_balance * received;

  double eu = (sqrt((b*b)-(4*a*c)) - b)/(2*a);
  double ee = received - eu;

  double new_shares = shares * (ee/(eu+enu_balance));

  auto quantity = asset(10000*new_shares, ENUBTC_SYMBOL);

  action(permission_level{_self, N(active)}, N(shares.coin), N(issue),
         std::make_tuple(transfer.from, quantity,
                         std::string("Issue new ENUBTC shares")))
      .send();

  action(permission_level{_self, N(active)}, N(enu.token), N(transfer),
         std::make_tuple(_self, N(enu.btc.mm), transfer.quantity,
                         std::string("Invest ENUBTC shares with ENU")))
      .send();
}

void ex::receivedbtc(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.btc.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get BTC balance
  double btc_balance = enumivo::token(N(iou.coin)).
	   get_balance(N(enu.btc.mm), enumivo::symbol_type(BTC_SYMBOL).name()).amount;
  btc_balance = btc_balance/100000000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUBTC_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/100000000;

  double a = enu_balance;
  double b = 2 * enu_balance * btc_balance;
  double c = - enu_balance * btc_balance * received;

  double ue = (sqrt((b*b)-(4*a*c)) - b)/(2*a);
  double uu = received - ue;

  double new_shares = shares * (uu/(ue+btc_balance));

  auto quantity = asset(10000*new_shares, ENUBTC_SYMBOL);

  action(permission_level{_self, N(active)}, N(shares.coin), N(issue),
         std::make_tuple(transfer.from, quantity,
                         std::string("Issue new ENUBTC shares")))
      .send();

  action(permission_level{_self, N(active)}, N(iou.coin), N(transfer),
         std::make_tuple(_self, N(enu.btc.mm), transfer.quantity,
                         std::string("Invest ENUBTC shares with ENU")))
      .send();
}

void ex::receivedshares(const currency::transfer &transfer) {
  if (transfer.to != _self) {
    return;
  }

  // get ENU balance
  double enu_balance = enumivo::token(N(enu.token)).
	   get_balance(N(enu.btc.mm), enumivo::symbol_type(ENU_SYMBOL).name()).amount;
  enu_balance = enu_balance/10000;

  // get BTC balance
  double btc_balance = enumivo::token(N(iou.coin)).
	   get_balance(N(enu.btc.mm), enumivo::symbol_type(BTC_SYMBOL).name()).amount;
  btc_balance = btc_balance/100000000;

  // get total shares
  double shares = enumivo::token(N(shares.coin)).
	   get_supply(enumivo::symbol_type(ENUBTC_SYMBOL).name()).amount;
  shares = shares/10000;

  double received = transfer.quantity.amount;
  received = received/10000;

  double btcportion = btc_balance*(received/shares);
  auto btc = asset(100000000*btcportion, BTC_SYMBOL);

  action(permission_level{N(enu.btc.mm), N(active)}, N(iou.coin), N(transfer),
         std::make_tuple(N(enu.btc.mm), transfer.from, btc,
                         std::string("Divest ENUBTC shares for BTC")))
      .send();

  double enuportion = enu_balance*(received/shares);
  auto enu = asset(10000*enuportion, ENU_SYMBOL);

  action(permission_level{N(enu.btc.mm), N(active)}, N(enu.token), N(transfer),
         std::make_tuple(N(enu.btc.mm), transfer.from, enu,
                         std::string("Divest ENUBTC shares for ENU")))
      .send();

  action(permission_level{_self, N(active)}, N(shares.coin), N(retire),
         std::make_tuple(transfer.quantity, std::string("Retire ENUBTC shares")))
      .send();
}

void ex::apply(account_name contract, action_name act) {

  if (contract == N(enu.token) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == ENU_SYMBOL,
                 "Must send ENU");
    receivedenu(transfer);
    return;
  }

  if (contract == N(iou.coin) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == BTC_SYMBOL,
                 "Must send BTC");
    receivedbtc(transfer);
    return;
  }

  if (contract == N(shares.coin) && act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();

    enumivo_assert(transfer.quantity.symbol == ENUBTC_SYMBOL,
                 "Must send ENUBTC");
    receivedshares(transfer);
    return;
  }

  if (act == N(transfer)) {
    auto transfer = unpack_action_data<currency::transfer>();
    enumivo_assert(false, "Must send BTC or ENU or ENUBTC");
    return;
  }

  if (contract != _self) return;

}

extern "C" {
[[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {
  ex enubtcshares(receiver);
  enubtcshares.apply(code, action);
  enumivo_exit(0);
}
}
