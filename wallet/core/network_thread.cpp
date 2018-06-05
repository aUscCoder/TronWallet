#include "network_thread.h"
#include "api/api.grpc.pb.h"
#include "gui/wallet_evnets.h"
#include <chrono>
#include <string>
#include "account_manager.h"
#include <vector>
#include "core_util.h"
#include "base/json_file_value_serializer.h"

using namespace wallet_core;

CNetworkThread::CNetworkThread() : io_service_work_(io_service_) {
    Load();
    CreateFullStub();
    CreateSolidutyStub();
}

CNetworkThread::~CNetworkThread() {
    io_service_.stop();
}

void CNetworkThread::Run()
{
    io_service_.run();
}

void CNetworkThread::Load()
{
    auto config_path = ConfigDir() + "/config.json";
    JSONFileValueDeserializer deserializer(config_path);
    int error_code = 0;
    std::string error_message;
    std::unique_ptr<base::Value> value = deserializer.Deserialize(&error_code, &error_message);
    if (value && value->IsType(base::Value::TYPE_DICTIONARY))
    {
        const base::DictionaryValue* root = nullptr;
        if (value->GetAsDictionary(&root) && root)
        {
            auto GetFunc = [root](const std::string& node_name)
            {   
                std::string value;
                std::string address, port;
                if (root->GetString(node_name + ".address", &address) && !address.empty() && root->GetString(node_name + ".port", &port) && !port.empty())
                {
                    value =  address + ":" + port;
                }
                return value;
            };

            auto full = GetFunc("full");
            if (!full.empty())
                full_address_port_ = full;

            auto solidity = GetFunc("solidity");
            if (!solidity.empty())
                solidity_address_port_ = solidity;
        }
    }
}

void CNetworkThread::CreateFullStub()
{
    auto channel = grpc::CreateChannel(full_address_port_, grpc::InsecureChannelCredentials());
    stub_ = protocol::Wallet::NewStub(channel);
}

void CNetworkThread::CreateSolidutyStub()
{
    auto channel = grpc::CreateChannel(solidity_address_port_, grpc::InsecureChannelCredentials());
    solidity_stub_ = protocol::WalletSolidity::NewStub(channel);
    extension_stub_ = protocol::WalletExtension::NewStub(channel);
}

void CNetworkThread::ReSetSub(const std::string& full_address_port, const std::string& solidity_address_port)
{
    PostTask([=] {
        auto Clear = [this]()
        {
            if (!requestContextPool_.empty())
            {
                for (const auto& p : requestContextPool_)
                    p.second->TryCancel();
                requestContextPool_.clear();
            }
        };

        if (!full_address_port.empty() && full_address_port != full_address_port_)
        {
            Clear();
            full_address_port_ = full_address_port;
            CreateFullStub();
        }

        if (!solidity_address_port.empty() && solidity_address_port_ != solidity_address_port)
        {
            Clear();
            solidity_address_port_ = solidity_address_port;
            CreateSolidutyStub();
        }
    });
}

void CNetworkThread::PostTask(std::function<void()> func)
{
    io_service_.post(func);
}

CNetworkThread& CNetworkThread::_() {
    static CNetworkThread o;
    return o;
}

void CNetworkThread::InvokeCallback(void* callback)
{
    ::PostMessage(callbackWnd_, WalletEvent::Event_Network_Response, 0, (LPARAM)callback);
}

std::shared_ptr<grpc::ClientContext> CNetworkThread::GetRequestContext(NetworkRequestType type)
{
    std::lock_guard<std::mutex> guard(contextPoolLock_);
    std::shared_ptr<grpc::ClientContext> context = nullptr;
    auto it = requestContextPool_.find(type);
    if (it != requestContextPool_.end())
        context = it->second;
    return context;
}

std::shared_ptr<grpc::ClientContext> CNetworkThread::RegisterRequestContext(NetworkRequestType type)
{
    std::lock_guard<std::mutex> guard(contextPoolLock_);
    auto context = std::make_shared<grpc::ClientContext>();
    requestContextPool_.emplace(type, context);
    return context;
}

void CNetworkThread::UnregiesterRequestContext(NetworkRequestType type)
{
    std::lock_guard<std::mutex> guard(contextPoolLock_);
    auto it = requestContextPool_.find(type);
    if (it != requestContextPool_.end())
        requestContextPool_.erase(it);
}

//////////////////////////////////////////////////////////////////////////

#define RPC_PROC(RequestType, RequestArg, RetType, RpcFunc, Stub) \
{ \
if (lastCallType_ != RequestType) {\
  auto context = GetRequestContext(lastCallType_); \
  if (context != nullptr) {\
    context->TryCancel();\
    std::string msg = "request type " + std::to_string(lastCallType_) + " canceled\n";\
    OutputDebugStringA(msg.c_str());\
  }\
} \
else {\
  std::string msg = "request type " + std::to_string(RequestType) + " duplicated, abort this one\n";\
  OutputDebugStringA(msg.c_str());\
  return 0;\
}\
auto context = GetRequestContext(RequestType);\
if (context == nullptr) \
{\
  context = RegisterRequestContext(RequestType);\
  lastCallType_ = RequestType;\
  PostTask([=]() {\
    auto retValue = std::make_shared<RetType>();\
    auto status = Stub->RpcFunc(context.get(), *RequestArg, retValue.get());\
    std::string msg = "request type " + std::to_string(RequestType) + " invoked\n";\
    OutputDebugStringA(msg.c_str());\
    msg = "request type " + std::to_string(RequestType) + " finished with status = " + status.error_message() + "\n";\
    OutputDebugStringA(msg.c_str());\
    UnregiesterRequestContext(RequestType);\
    if (status.error_code() == grpc::StatusCode::UNAVAILABLE) \
    {\
        msg = "request type " + std::to_string(RequestType) + " request again\n";\
        OutputDebugStringA(msg.c_str());\
        auto new_context = RegisterRequestContext(RequestType);\
        status = Stub->RpcFunc(new_context.get(), *RequestArg, retValue.get());\
        UnregiesterRequestContext(RequestType);\
    }\
    InvokeCallback(new std::function<void()>(std::bind(callback, retValue)));\
    lastCallType_ = RequestCount;\
  });\
}\
return 0; \
}

int CNetworkThread::GetAccount(std::shared_ptr<protocol::Account> account, std::function<void(std::shared_ptr<protocol::Account>)> callback)
{
    RPC_PROC(Request_GetAccount, account, protocol::Account, GetAccount, stub_);
}

int CNetworkThread::GetAccountNet(std::shared_ptr<protocol::Account> account, std::function<void(std::shared_ptr<protocol::AccountNetMessage>)> callback)
{
    RPC_PROC(Request_AccountNetMessage, account, protocol::AccountNetMessage, GetAccountNet, stub_);
}

int CNetworkThread::CreateTransaction(std::shared_ptr<protocol::TransferContract> transfercontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_CreateTransaction, transfercontract, protocol::Transaction, CreateTransaction, stub_);
}

int CNetworkThread::BroadcastTransaction(std::shared_ptr<protocol::Transaction> transaction, std::function<void(std::shared_ptr<protocol::Return>)> callback)
{
    RPC_PROC(Request_BroadcastTransaction, transaction, protocol::Return, BroadcastTransaction, stub_);
}

int CNetworkThread::UpdateAccount(std::shared_ptr<protocol::AccountUpdateContract> accountupdatecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_UpdateAccount, accountupdatecontract, protocol::Transaction, UpdateAccount, stub_);
}

int CNetworkThread::VoteWitnessAccount(std::shared_ptr<protocol::VoteWitnessContract> votewitnesscontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_VoteWitnessAccount, votewitnesscontract, protocol::Transaction, VoteWitnessAccount, stub_);
}

int CNetworkThread::CreateAssetIssue(std::shared_ptr<protocol::AssetIssueContract> assetissuecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_CreateAssetIssue, assetissuecontract, protocol::Transaction, CreateAssetIssue, stub_);
}

int CNetworkThread::ListWitnesses(std::shared_ptr<protocol::EmptyMessage> emptymessage, std::function<void(std::shared_ptr<protocol::WitnessList>)> callback)
{
    RPC_PROC(Request_ListWitnesses, emptymessage, protocol::WitnessList, ListWitnesses, solidity_stub_);
}

int CNetworkThread::UpdateWitness(std::shared_ptr<protocol::WitnessUpdateContract> witnessupdatecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_UpdateWitness, witnessupdatecontract, protocol::Transaction, UpdateWitness, stub_);
}

int CNetworkThread::CreateWitness(std::shared_ptr<protocol::WitnessCreateContract> witnesscreatecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_CreateWitness, witnesscreatecontract, protocol::Transaction, CreateWitness, stub_);
}

int CNetworkThread::TransferAsset(std::shared_ptr<protocol::TransferAssetContract> transferassetcontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_TransferAsset, transferassetcontract, protocol::Transaction, TransferAsset, stub_);
}

int CNetworkThread::ParticipateAssetIssue(std::shared_ptr<protocol::ParticipateAssetIssueContract> participateassetissuecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_ParticipateAssetIssue, participateassetissuecontract, protocol::Transaction, ParticipateAssetIssue, stub_);
}

int CNetworkThread::FreezeBalance(std::shared_ptr<protocol::FreezeBalanceContract> freezebalancecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_FreezeBalance, freezebalancecontract, protocol::Transaction, FreezeBalance, stub_);
}

int CNetworkThread::UnfreezeBalance(std::shared_ptr<protocol::UnfreezeBalanceContract> unfreezebalancecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_UnfreezeBalance, unfreezebalancecontract, protocol::Transaction, UnfreezeBalance, stub_);
}

int CNetworkThread::WithdrawBalance(std::shared_ptr<protocol::WithdrawBalanceContract> withdrawbalancecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_WithdrawBalance, withdrawbalancecontract, protocol::Transaction, WithdrawBalance, stub_);
}

int CNetworkThread::ListNodes(std::shared_ptr<protocol::EmptyMessage> emptymessage, std::function<void(std::shared_ptr<protocol::NodeList>)> callback)
{
    RPC_PROC(Request_ListNodes, emptymessage, protocol::NodeList, ListNodes, stub_);
}

int CNetworkThread::GetAssetIssueList(std::shared_ptr<protocol::EmptyMessage> emptymessage, std::function<void(std::shared_ptr<protocol::AssetIssueList>)> callback)
{
    RPC_PROC(Request_GetAssetIssueList, emptymessage, protocol::AssetIssueList, GetAssetIssueList, solidity_stub_);
}

int CNetworkThread::GetAssetIssueByAccount(std::shared_ptr<protocol::Account> account, std::function<void(std::shared_ptr<protocol::AssetIssueList>)> callback)
{
    RPC_PROC(Request_GetAssetIssueByAccount, account, protocol::AssetIssueList, GetAssetIssueByAccount, stub_);
}

int CNetworkThread::GetAssetIssueByName(std::shared_ptr<protocol::BytesMessage> bytesmessage, std::function<void(std::shared_ptr<protocol::AssetIssueContract>)> callback)
{
    RPC_PROC(Request_GetAssetIssueByName, bytesmessage, protocol::AssetIssueContract, GetAssetIssueByName, stub_);
}

int CNetworkThread::GetNowBlock(std::shared_ptr<protocol::EmptyMessage> emptymessage, std::function<void(std::shared_ptr<protocol::Block>)> callback)
{
    RPC_PROC(Request_GetNowBlock, emptymessage, protocol::Block, GetNowBlock, stub_);
}

int CNetworkThread::GetBlockByNum(std::shared_ptr<protocol::NumberMessage> numbermessage, std::function<void(std::shared_ptr<protocol::Block>)> callback)
{
    RPC_PROC(Request_GetBlockByNum, numbermessage, protocol::Block, GetBlockByNum, stub_);
}

int CNetworkThread::TotalTransaction(std::shared_ptr<protocol::EmptyMessage> emptymessage, std::function<void(std::shared_ptr<protocol::NumberMessage>)> callback)
{
    RPC_PROC(Request_TotalTransaction, emptymessage, protocol::NumberMessage, TotalTransaction, solidity_stub_);
}

int CNetworkThread::GetBlockById(std::shared_ptr<protocol::BytesMessage> bytesmessage, std::function<void(std::shared_ptr<protocol::Block>)> callback)
{
    RPC_PROC(Request_GetBlockById, bytesmessage, protocol::Block, GetBlockById, stub_);
}

int CNetworkThread::GetBlockByLimitNext(std::shared_ptr<protocol::BlockLimit> blocklimit, std::function<void(std::shared_ptr<protocol::BlockList>)> callback)
{
    RPC_PROC(Request_GetBlockByLimitNext, blocklimit, protocol::BlockList, GetBlockByLimitNext, stub_);
}

int CNetworkThread::GetBlockByLatestNum(std::shared_ptr<protocol::NumberMessage> numbermessage, std::function<void(std::shared_ptr<protocol::BlockList>)> callback)
{
    RPC_PROC(Request_GetBlockByLatestNum, numbermessage, protocol::BlockList, GetBlockByLatestNum, stub_);
}

int CNetworkThread::GetTransactionById(std::shared_ptr<protocol::BytesMessage> bytesmessage, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_GetTransactionById, bytesmessage, protocol::Transaction, GetTransactionById, stub_);
}

int CNetworkThread::UpdateAsset(std::shared_ptr<protocol::UpdateAssetContract> updateassertcontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback)
{
    RPC_PROC(Request_UpdateAsset, updateassertcontract, protocol::Transaction, UpdateAsset, stub_);
}

// 
int CNetworkThread::GetTransactionsFromThis(std::shared_ptr<protocol::AccountPaginated> account, std::function<void(std::shared_ptr<protocol::TransactionList>)> callback)
{
    RPC_PROC(Request_getTransactionsFromThis, account, protocol::TransactionList, GetTransactionsFromThis, extension_stub_);
}

int CNetworkThread::GetTransactionsToThis(std::shared_ptr<protocol::AccountPaginated> account, std::function<void(std::shared_ptr<protocol::TransactionList>)> callback)
{
    RPC_PROC(Request_getTransactionsToThis, account, protocol::TransactionList, GetTransactionsToThis, extension_stub_);
}

void CNetworkThread::Login(std::string password, std::function<void(std::string)> callback)
{
    PostTask([=]() {
        auto err = wallet_core::AccountManager::_().Login(password);
        InvokeCallback(new std::function<void()>(std::bind(callback, err)));
    });
}

void CNetworkThread::CreateAccountWrapper(const std::string& name, const std::string& password, std::function<void(void)> callback)
{
    PostTask([=]() {
        wallet_core::AccountManager::_().CreateAccount(name, password);
        InvokeCallback(new std::function<void()>(std::bind(callback)));
    });
}

void CNetworkThread::GetAccountWrapper(const std::string& address, std::function<void(std::shared_ptr<protocol::Account>)> callback)
{
    auto account = std::make_shared<protocol::Account>();
    account->set_address(address);
    GetAccount(account, callback);
}

