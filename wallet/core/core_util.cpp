#include "core_util.h"
#include "base/utf_string_conversions.h"
#include "libdevcore/base58.h"
#include "libdevcore/SHA3.h"
#include "libdevcrypto/Hash.h"
#include "core/Tron.pb.h"
#include <windows.h>
#include "account_manager.h"

#define TRANSACTION_DELAY_TIME 8 * 60 * 60 * 1000

namespace wallet_core
{
    uint64_t currentTimeMillis()
    {
        time_t clock;
        struct tm tm;
        SYSTEMTIME wtm;
        GetLocalTime(&wtm);
        tm.tm_year = wtm.wYear - 1900;
        tm.tm_mon = wtm.wMonth - 1;
        tm.tm_mday = wtm.wDay;
        tm.tm_hour = wtm.wHour;
        tm.tm_min = wtm.wMinute;
        tm.tm_sec = wtm.wSecond;
        tm.tm_isdst = -1;
        clock = mktime(&tm);
        uint64_t t = clock * 1000;
        t += wtm.wMilliseconds;
        return t;
    }

    uint64_t GetTime(const std::string& format_time, bool hour24)
    {
        uint64_t milliseconds = 0;
        std::string time_format = hour24 ? "%m/%d/%Y %H:%M" : "%m/%d/%Y %I:%M %p";
        std::tm t = {};
        std::istringstream ss(format_time);
        ss >> std::get_time(&t, time_format.c_str());
        if (!ss.fail()) 
        {
            auto clock = mktime(&t);
            milliseconds = clock * 1000;
        }
        return milliseconds;
    }

    std::wstring GetTime(uint64_t t, bool hour24 /* false */)
    {
        std::wstring time;
        std::time_t seconds = t / 1000;
        char mbstr[100] = { 0 };
        auto tm = std::localtime(&seconds);
        std::string time_format = hour24 ? "%m/%d/%Y %H:%M" : "%m/%d/%Y %I:%M %p";
        if (tm && std::strftime(mbstr, sizeof(mbstr), time_format.c_str(), tm))
        {
            if (!base::UTF8ToWide(mbstr, strlen(mbstr), &time))
            {
                time.clear();
            }
        }
        return time;
    }

    std::string AddressEncodeBase58Check(const std::string& address)
    {
        std::vector<unsigned char> vec(address.begin(), address.end());
        return EncodeBase58Check(vec);
    }

    std::tuple<bool, std::string> AddressDecodeBase58Check(const std::string& encode_address)
    {
        std::vector<unsigned char> ret;
        auto s = DecodeBase58Check(encode_address, ret);
        std::string address((char*)ret.data(), ret.size());
        return std::make_tuple(s, address);
    }

    std::string ToTronAddress(const dev::Address& address)
    {
        auto bytes = address.asBytes();
        bytes.insert(bytes.begin(), 0x41);
        return std::string((char*)bytes.data(), bytes.size());
    }

    dev::Address ToAddress(const std::string& tron_address)
    {
        if (!tron_address.empty())
        {
            dev::bytes address(tron_address.size() - 1);
            memcpy(&address[0], &tron_address[1], address.size());
            return dev::Address(address);
        }
        return dev::ZeroAddress;
    }

    bool SignTransaction(const dev::Secret& key, std::shared_ptr<protocol::Transaction> transaction)
    {
        bool success = false;
        if (!key.asBytesSec().empty())
        {
            auto raw = transaction->mutable_raw_data();
            raw->set_expiration(currentTimeMillis() + TRANSACTION_DELAY_TIME);
            raw->set_timestamp(currentTimeMillis());
            auto raw_bytes = raw->SerializeAsString();
            auto hash = dev::sha256(raw_bytes);
            auto sig_bytes = dev::sign(key, hash).asBytes();
            auto sig_str = std::string((char*)sig_bytes.data(), sig_bytes.size());
            if (raw->contract_size() > 0)
            {
                for (int i = 0; i < raw->contract_size(); i++)
                {
                    transaction->add_signature(sig_str);
                }
                success = true;
            }
        }
        return success;
    }

    bool SignTransaction(std::shared_ptr<protocol::Transaction> transaction)
    {
        bool success = false;
        auto tron_address = AccountManager::_().Address();
        if (!tron_address.empty())
        {
            auto address = ToAddress(tron_address);
            auto key = AccountManager::_().GetPrivate(address);
            if (!key.asBytesSec().empty())
            {
                auto raw = transaction->mutable_raw_data();
                raw->set_timestamp(currentTimeMillis());
                auto raw_bytes = raw->SerializeAsString();
                auto hash = dev::sha256(raw_bytes);
                auto sig_bytes = dev::sign(key, hash).asBytes();
                auto sig_str = std::string((char*)sig_bytes.data(), sig_bytes.size());
                if (raw->contract_size() > 0)
                {
                    for (int i = 0; i < raw->contract_size(); i++)
                    {
                        transaction->add_signature(sig_str);
                    }
                    success = true;
                }
            }
        }
        return success;
    }

    bool CheckAddress(std::string& address)
    {
        bool valid = false;
        if (!address.empty())
            std::tie(valid, address) = AddressDecodeBase58Check(address);
        return valid;
    }

    int64_t CalcTronPower(std::shared_ptr<::protocol::Account> account)
    {
        int64_t freeze = 0;
        for (int i = 0; i < account->frozen_size(); i++)
        {
            auto& f = account->frozen(i);
            freeze += f.frozen_balance();
        }
        return freeze;
    }

    double ToTrx(int64_t amount)
    {
        return amount / TRX_DROP_RATE;
    }

    int64_t ToDrop(double amount)
    {
        return amount * TRX_DROP_RATE;
    }

    std::string ConfigDir()
    {
        const static auto dir = getDataDir("tron") / boost::filesystem::path("config");
        return dir.string();
    }
}
