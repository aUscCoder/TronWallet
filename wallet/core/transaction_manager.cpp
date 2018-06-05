#include "transaction_manager.h"
#include "base/json_file_value_serializer.h"
#include "boost/filesystem/operations.hpp"
#include "libdevcore/CommonData.h"

namespace wallet_core
{
    std::wstring GetBtnText(TransactionStatus s)
    {
        std::wstring text;
        switch (s)
        {
        case TransactionStatus::SAVE:
            text = L"Save";
            break;
        case TransactionStatus::SIGN:
            text = L"Sign";
            break;
        case TransactionStatus::BROADCAST:
            text = L"Broadcast";
            break;
        }
        return text;
    }

    bool SaveTransaction(const std::string& dir, std::shared_ptr<protocol::Transaction> transaction)
    {
        boost::filesystem::create_directories(dir);
        std::unique_ptr<base::DictionaryValue> json_details(new base::DictionaryValue);
        auto hex = dev::toHex(transaction->SerializeAsString());
        json_details->SetString("hex", hex);
        auto path = dir  + "\\" + hex.substr(0, 7) + ".json";
        JSONFileValueSerializer serializer(path);
        return serializer.Serialize(*json_details);
    }

    std::shared_ptr<protocol::Transaction> LoadTransaction(const std::string& path)
    {
        std::shared_ptr<protocol::Transaction> transaction = nullptr;
        JSONFileValueDeserializer deserializer(path);
        int error_code = 0;
        std::string error_message;
        std::unique_ptr<base::Value> value = deserializer.Deserialize(&error_code, &error_message);
        if (value && value->IsType(base::Value::TYPE_DICTIONARY))
        {
            const base::DictionaryValue* root = nullptr;
            std::string hex;
            if (value->GetAsDictionary(&root) && root->GetString("hex", &hex))
            {
                auto bytes = dev::fromHex(hex);
                transaction.reset(new protocol::Transaction);
                if (!transaction->ParseFromArray(bytes.data(), bytes.size()))
                    transaction.reset();
            }
        }
        return transaction;
    }

    bool SaveVote(const std::string& dir, const VoteData& vote_data)
    {
        boost::filesystem::create_directories(dir);
        std::unique_ptr<base::DictionaryValue> json_details(new base::DictionaryValue);
        auto hex = dev::toHex(vote_data.transaction->SerializeAsString());
        json_details->SetString("hex", hex);
        std::unique_ptr<base::DictionaryValue> map(new base::DictionaryValue);
        for (const auto& p : vote_data.address_url)
            map->SetString(p.first, p.second);
        json_details->Set("map", std::move(map));
        auto path = dir + "\\" + hex.substr(0, 7) + ".json";
        JSONFileValueSerializer serializer(path);
        return serializer.Serialize(*json_details);
    }

    VoteData LoadVote(const std::string& path)
    {
        VoteData data;
        JSONFileValueDeserializer deserializer(path);
        int error_code = 0;
        std::string error_message;
        std::unique_ptr<base::Value> value = deserializer.Deserialize(&error_code, &error_message);
        if (value && value->IsType(base::Value::TYPE_DICTIONARY))
        {
            const base::DictionaryValue* root = nullptr;
            const base::DictionaryValue* map = nullptr;
            std::string hex;
            if (value->GetAsDictionary(&root) && root->GetString("hex", &hex))
            {
                auto bytes = dev::fromHex(hex);
                data.transaction.reset(new protocol::Transaction);
                if (!data.transaction->ParseFromArray(bytes.data(), bytes.size()))
                    data.transaction.reset();
                else
                {
                    if (root->Get("map", (const base::Value**)&map))
                    {
                        for (base::DictionaryValue::Iterator it(*map); !it.IsAtEnd(); it.Advance())
                        {
                            std::string v;
                            if (it.value().GetAsString(&v))
                                data.address_url.emplace(it.key(), v);
                        }
                    }
                }
            }
        }
        return data;
    }
}
