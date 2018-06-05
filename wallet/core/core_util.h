#pragma once
#include <memory>
#include <string>
#include <tuple>
#include "libdevcrypto/Common.h"
namespace protocol 
{
    class Transaction;
    class Account;
}

#define TRX_DROP_RATE 1000000

namespace wallet_core
{
    uint64_t GetTime(const std::string& format_time, bool hour24);

    std::wstring GetTime(uint64_t t, bool hour24 = false);
    uint64_t currentTimeMillis();
    std::string AddressEncodeBase58Check(const std::string& address);
    std::tuple<bool, std::string> AddressDecodeBase58Check(const std::string& encode_address);
    std::string ToTronAddress(const dev::Address& address);
    dev::Address ToAddress(const std::string& tron_address);
    bool SignTransaction(const dev::Secret& key, std::shared_ptr<protocol::Transaction> transaction);
    bool SignTransaction(std::shared_ptr<protocol::Transaction> transaction);

    bool CheckAddress(std::string& address);
    int64_t CalcTronPower(std::shared_ptr<::protocol::Account> account);

    double ToTrx(int64_t amount);
    int64_t ToDrop(double amount);

    std::string ConfigDir();
}
