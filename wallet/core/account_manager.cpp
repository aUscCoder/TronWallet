#include "account_manager.h"
#include "base/logging.h"
#include "libdevcore/Common.h"
#include "libdevcore/CommonData.h"
#include "libdevcore/SHA3.h"
#include "libdevcrypto/Hash.h"
#include "libdevcrypto/Common.h"
#include "core_util.h"
#include "gui/wallet_evnets.h"
#include "base/json_string_value_serializer.h"
#include "base/file_util.h"

using namespace wallet_core;
using namespace dev;

AccountManager& AccountManager::_() {
    static AccountManager o;
    return o;
}

std::string AccountManager::Address()
{
    return address_pair_.first;
}

std::string AccountManager::EncodedAddress()
{
    return address_pair_.second;
}

void AccountManager::Logout()
{
    m_keyManager->clear(ToAddress(address_pair_.first));
    address_pair_.first.clear();
    address_pair_.second.clear();
}

bool AccountManager::CheckLogin()
{
    bool login = !address_pair_.first.empty();
    if (!login && callback_wnd_)
    {
        auto msg = new std::wstring(L"Please Login first");
        ::PostMessage(callback_wnd_, Event_Hide_Status, (WPARAM)msg, 0);
    }
    return login;
}

void AccountManager::SetCallbackWnd(HWND wnd)
{
    callback_wnd_ = wnd;
}

std::string AccountManager::GetAccountName()
{
    std::string name;
    if (!Address().empty())
        name = m_keyManager->accountName(ToAddress(Address()));
    return name;
}

void AccountManager::CreateAccount(const std::string& name, const std::string& password)
{
    OpenWallet();
    std::string lock;
    std::string lockHint;
    auto k = MakeKey();
    h128 u = m_keyManager->import(k.secret(), name, password, lockHint);
    auto address = ToTronAddress(k.address());
    address_pair_ = std::make_pair(address, AddressEncodeBase58Check(address));
    if (!m_keyManager->exists())
        m_keyManager->create(password);
}

bool AccountManager::HasAccount()
{
    OpenWallet();
    return !m_keyManager->accounts().empty();
}

dev::Secret AccountManager::GetPrivate(const dev::Address& address)
{
    return m_keyManager->secret(address);
}

std::string AccountManager::GenerateAddress()
{
    std::string address;
    OpenWallet();
    auto k = MakeKey();
    address = ToTronAddress(k.address());
    address = AddressEncodeBase58Check(address);
    auto uuid = h128::random();
    address_lookup_.emplace(uuid, address);
    cached_.emplace(uuid, std::move(k.secret().asBytesSec()));

    std::string json_string;
    JSONStringValueSerializer serializer(&json_string);
    std::unique_ptr<base::DictionaryValue> root(new base::DictionaryValue);

    for (const auto& p : cached_)
    {
        auto it = address_lookup_.find(p.first);
        if (it != address_lookup_.end())
        {
            root->SetString(it->second, dev::toHex(p.second.ref()));
        }
    }
    serializer.Serialize(*root);

    dev::bytesRef bytes((byte*)json_string.data(), json_string.size());
    auto chipper = m_keyManager->encryptUseEncryptedKey(bytes, ToAddress(Address()));
    auto dir = ColdWalletPath();
    boost::filesystem::create_directories(dir);
    auto path = dir.string() + "\\" + toHex(Address());
    base::file::WriteFile(path, std::string((char*)chipper.data(), bytes.size()));
    return address;
}

void AccountManager::LoadAddresses()
{
    if (!cached_.empty() && !address_lookup_.empty())
        return;

    auto dir = ColdWalletPath();
    auto path = dir.string() + "\\" + toHex(Address());
    bool success = false;
    std::string data;
    std::tie(success, data) = base::file::ReadFileToString(path);
    if (success)
    {
        dev::bytesRef bytes((byte*)data.data(), data.size());
        auto decrypted = m_keyManager->decryptUseEncryptedKey(bytes, ToAddress(Address()));
        JSONStringValueDeserializer deserializer(decrypted);
        int error_code = 0;
        std::string error_message;
        std::unique_ptr<base::Value> value = deserializer.Deserialize(&error_code, &error_message);
        if (value && value->IsType(base::Value::TYPE_DICTIONARY))
        {
            const base::DictionaryValue* root = nullptr;
            if (value->GetAsDictionary(&root))
            {
                for (base::DictionaryValue::Iterator it(*root); !it.IsAtEnd(); it.Advance())
                {
                    std::string v;
                    const auto& address = it.key();
                    if (!address.empty() && it.value().GetAsString(&v))
                    {
                        auto key = dev::fromHex(v);
                        auto uuid = h128::random();
                        cached_.emplace(uuid, dev::bytesSec(key));
                        address_lookup_.emplace(uuid, address);
                    }
                }
            }
        }
    }
}

std::vector<std::string> AccountManager::GetAddresses()
{
    if (cached_.empty() || address_lookup_.empty())
        LoadAddresses();

    std::vector<std::string> addresses;
    for (const auto& p : address_lookup_)
        addresses.push_back(p.second);
    return addresses;
}

dev::Secret AccountManager::LookupPrivate(const std::string& address)
{
    dev::Secret key;
    auto it = std::find_if(address_lookup_.begin(), address_lookup_.end(), [&address](const std::pair<dev::h128, std::string>& p) {
        return p.second == address;
    });

    if (it != address_lookup_.end())
    {
        auto itkey = cached_.find(it->first);
        if (itkey != cached_.end())
            key = dev::Secret(itkey->second);
    }
    return key;
}

std::string AccountManager::Login(const std::string& password)
{
    OpenWallet();
    std::string address;
    std::string err;

    if (m_keyManager->store().keys().empty())
    {
        err = "no account exist.";
    }
    else
    {
        for (const auto& u : m_keyManager->store().keys())
        {
            bytesSec s = m_keyManager->store().secret(u, [&password] {return password; });
            if (!s.empty())
            {
                auto key_hex = toHex(s.ref());
                auto haha = fromHex(key_hex);
                std::string lockHint;
                std::string name;
                m_keyManager->importExisting(u, name, password, lockHint);
                auto a = m_keyManager->address(u);
                address = ToTronAddress(a);
                address_pair_ = std::make_pair(address, AddressEncodeBase58Check(address));
                break;
            }
        }

        if (address.empty())
            err = "password not right";
        else
        {
            m_keyManager->load(password);
        }

    }
    return err;
}

KeyPair AccountManager::MakeKey() const
{
    bool icap = true;
    KeyPair k(Secret::random());
    while (icap && k.address()[0])
        k = KeyPair(Secret(sha3(k.secret().ref())));
    return k;
}

bool AccountManager::OpenWallet()
{
    if (!m_keyManager)
        m_keyManager.reset(new KeyManager(KeysFilePath(), AccountPath()));
    return true;
}

boost::filesystem::path AccountManager::AccountPath()
{
    if (wallet_type_ == HotWallet)
        return getDataDir("tron") / boost::filesystem::path("account");
    else if (wallet_type_ == ColdWallet)
        return getDataDir("tron") / boost::filesystem::path("cold_account");
}

boost::filesystem::path AccountManager::ColdWalletPath()
{
    return getDataDir("tron") / boost::filesystem::path("cold_account") / boost::filesystem::path("cold_wallet");
}

boost::filesystem::path AccountManager::KeysFilePath()
{
    if (wallet_type_ == HotWallet)
        return getDataDir("tron") / boost::filesystem::path("account") / boost::filesystem::path("keys");
    else if (wallet_type_ == ColdWallet)
        return getDataDir("tron") / boost::filesystem::path("cold_account") / boost::filesystem::path("keys");
}



