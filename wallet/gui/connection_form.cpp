#include "connection_form.h"
#include <regex>
#include "base/json_file_value_serializer.h"
#include "base/utf_string_conversions.h"
#include "common_ui/UI.h"
#include "core/core_util.h"
#include "boost/filesystem/operations.hpp"
#include "core/network_thread.h"
#include "UILite/Controls/UIEdit.h"

using namespace wallet_core;
using namespace wallet_gui;
using namespace CommonUI;

enum class ControlID
{
    EDIT_FULL_NODE_IP = 8000,
    EDIT_FULL_NODE_PORT,
    EDIT_SOLIDITY_NODE_IP,
    EDIT_SOLIDITY_NODE_PORT,
    BTN_RESET,
    BTN_SAVE,
};

CConnectionForm::CConnectionForm() : CBaseForm(L"Connection")
{
}

void CConnectionForm::OnUICreate()
{
    UISetSize(450, 240);
    item_infos_ =
    {
        { { { ControlType::LABEL, L""} },{ 0, 12 }, { 0, 5 } },
        { { { ControlType::LABEL, L"Full node ip", 0, L"", { 100, 15} }, { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_FULL_NODE_IP, L"", {160, 28} }, { ControlType::LABEL, L"Port", 0, L"", { 30, 15 } }, { ControlType::EDIT, L"", (DWORD)ControlID::EDIT_FULL_NODE_PORT, L"", { 60, 28 } } },{ 30, 5},{ 0, normal_btn_height }, false },
        { { { ControlType::LABEL, L"Solidity node ip", 0, L"",{ 100, 15 } },{ ControlType::EDIT, L"", (DWORD)ControlID::EDIT_SOLIDITY_NODE_IP, L"",{ 160, 28 } },{ ControlType::LABEL, L"Port", 0, L"",{ 30, 15 } },{ ControlType::EDIT, L"", (DWORD)ControlID::EDIT_SOLIDITY_NODE_PORT, L"",{ 60, 28 } } },{ 30, 20 },{ 0, normal_btn_height }, false },

        { { { ControlType::NORMAL_BUTTON, L"Save and connect", (DWORD)ControlID::BTN_SAVE}, { ControlType::NORMAL_BUTTON, L"Rest", (DWORD)ControlID::BTN_RESET}}, { 30, 10 }, { 0, normal_btn_height } },
    };
    CBaseForm::OnUICreate();
    GetSub((DWORD)ControlID::EDIT_FULL_NODE_PORT)->UIAddOrRemoveStyle(true, UIEditStyle_Number);
    GetSub((DWORD)ControlID::EDIT_SOLIDITY_NODE_PORT)->UIAddOrRemoveStyle(true, UIEditStyle_Number);
}


void CConnectionForm::OnButtonClick(DWORD dwControlID)
{
    auto id = (ControlID)dwControlID;
    if (id == ControlID::BTN_RESET)
    {
        Reset();
    }
    else if (id == ControlID::BTN_SAVE)
    {
        const static std::regex ip_pattern("(\\d{1,3}(\\.\\d{1,3}){3})");
        std::string full_address, solidity_address;
        std::string full_port, solidity_port;

        auto CheckIPAddress = [this](const std::string& address, const std::string& port)
        {
            if (address.empty() != port.empty())
            {
                MessageBox(L"IP address or port can't empty!");
                return false;
            }

            if (!address.empty())
            {
                std::smatch match;
                if (!std::regex_search(address, match, ip_pattern))
                {
                    MessageBox(L"IP address format error!");
                    return false;
                }
            }
            return true;
        };

        bool valide = false;

        do
        {
            full_address = base::WideToUTF8(GetSub((DWORD)ControlID::EDIT_FULL_NODE_IP)->UIGetText());
            full_port = base::WideToUTF8(GetSub((DWORD)ControlID::EDIT_FULL_NODE_PORT)->UIGetText());
            if (!CheckIPAddress(full_address, full_port))
                break;

            solidity_address = base::WideToUTF8(GetSub((DWORD)ControlID::EDIT_SOLIDITY_NODE_IP)->UIGetText());
            solidity_port = base::WideToUTF8(GetSub((DWORD)ControlID::EDIT_SOLIDITY_NODE_PORT)->UIGetText());
            if (!CheckIPAddress(solidity_address, solidity_port))
                break;

            valide = true;

        } while (false);


        if (valide)
        {
            Save(full_address, full_port, solidity_address, solidity_port);
        }
    }
}

void CConnectionForm::Save(const std::string& full_address, const std::string& full_port, const std::string& solidity_address, const std::string& solidity_port)
{
    auto dir = ConfigDir();
    boost::filesystem::create_directories(dir);

    auto path = dir + "/config.json";
    std::unique_ptr<base::DictionaryValue> json_details(new base::DictionaryValue);
    auto CreateNode = [](const std::string& ip, const std::string& port)
    {
        std::unique_ptr<base::DictionaryValue> node(new base::DictionaryValue);
        node->SetString("address", ip);
        node->SetString("port", port);
        return node;
    };

    json_details->Set("full", CreateNode(full_address, full_port));
    json_details->Set("solidity", CreateNode(solidity_address, solidity_port));
    JSONFileValueSerializer serializer(path);

    if (!serializer.Serialize(*json_details))
        MessageBox(L"Save failed!");
    else
        CNetworkThread::_().ReSetSub(full_address + ":" + full_port, solidity_address + ":" + solidity_port);
}

void CConnectionForm::Reset()
{
    static const std::vector<ControlID> edits = { ControlID::EDIT_FULL_NODE_IP, ControlID::EDIT_FULL_NODE_PORT, ControlID::EDIT_SOLIDITY_NODE_IP, ControlID::EDIT_SOLIDITY_NODE_PORT };
    for (auto id : edits)
    {
        ((CUIEdit*)GetSub((DWORD)id))->UISetText(L"");
    }
}

