#pragma once
#include <map>
#include <memory>
#include <string>
#include "core/Tron.pb.h"

namespace wallet_core
{
    bool SaveTransaction(const std::string& dir, std::shared_ptr<protocol::Transaction> transaction);
    std::shared_ptr<protocol::Transaction> LoadTransaction(const std::string& path);

    enum class TransactionStatus
    {
        SAVE = 0,
        SIGN,
        BROADCAST,
    };

    std::wstring GetBtnText(TransactionStatus s);

    struct VoteData
    {
        std::shared_ptr<protocol::Transaction> transaction;
        std::map<std::string, std::string> address_url;
    };

    bool SaveVote(const std::string& dir, const VoteData& vote_data);

    VoteData LoadVote(const std::string& path);
}
