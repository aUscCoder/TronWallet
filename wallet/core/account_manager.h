#pragma once
#include <string>
#include "key_manager.h"
#include <windows.h>

namespace wallet_core
{
    enum WalletType
    {
        HotWallet = 0,
        ColdWallet,
    };

    class AccountManager
    {
    public:
        static AccountManager& _();
        AccountManager(const AccountManager&) = delete;

        void SetWalletType(WalletType type) { wallet_type_ = type; }

        std::string Address();
        std::string EncodedAddress();
        bool CheckLogin();
        void Logout();

        void SetCallbackWnd(HWND wnd);
        void CreateAccount(const std::string& name, const std::string& password);
        std::string Login(const std::string& paaword);
        bool HasAccount();
        dev::Secret GetPrivate(const dev::Address& address);
        std::string GenerateAddress();
        void LoadAddresses();
        std::vector<std::string> GetAddresses();

        std::string GetAccountName();
        dev::Secret LookupPrivate(const std::string& address);

    private:
        AccountManager() = default;
        dev::KeyPair MakeKey() const;
        bool OpenWallet();
        boost::filesystem::path AccountPath();
        boost::filesystem::path ColdWalletPath();
        boost::filesystem::path KeysFilePath();

    private:
        WalletType wallet_type_ = HotWallet;
        HWND callback_wnd_ = NULL;
        std::unique_ptr<KeyManager> m_keyManager;
        std::pair<std::string, std::string> address_pair_;

        std::unordered_map<dev::h128, std::string> address_lookup_;
        std::unordered_map<dev::h128, dev::bytesSec> cached_;
    };

}
