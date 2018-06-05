#pragma once
#include "asio/asio.hpp"
#include <functional>
#include <memory>
#include <map>
#include <mutex>
#include "grpc++/grpc++.h"
#include "api/api.pb.h"
#include "api/api.grpc.pb.h"

class CNetworkThread
{
public:
    enum NetworkRequestType
    {
        Request_GetAccount,
        Request_AccountNetMessage,
        Request_CreateTransaction,
        Request_BroadcastTransaction,
        Request_UpdateAccount,
        Request_VoteWitnessAccount,
        Request_CreateAssetIssue,
        Request_ListWitnesses,
        Request_UpdateWitness,
        Request_CreateWitness,
        Request_TransferAsset,
        Request_ParticipateAssetIssue,
        Request_FreezeBalance,
        Request_UnfreezeBalance,
        Request_WithdrawBalance,
        Request_ListNodes,
        Request_GetAssetIssueList,
        Request_GetAssetIssueByAccount,
        Request_GetAssetIssueByName,
        Request_GetNowBlock,
        Request_GetBlockByNum,
        Request_TotalTransaction,
        Request_GetBlockById,
        Request_GetBlockByLimitNext,
        Request_GetBlockByLatestNum,
        Request_GetTransactionById,
        Request_UpdateAsset,

        // soliditynode
        Request_getTransactionsFromThis,
        Request_getTransactionsToThis,
        RequestCount
    };
    CNetworkThread();
    ~CNetworkThread();

    static CNetworkThread& _();

    void Run();
    void Load();
    void CreateFullStub();
    void CreateSolidutyStub();
    void ReSetSub(const std::string& full_address_port, const std::string& solidity_address_port);


    int GetAccount(std::shared_ptr<protocol::Account> account, std::function<void(std::shared_ptr<protocol::Account>)> callback);
    int GetAccountNet(std::shared_ptr<protocol::Account> account, std::function<void(std::shared_ptr<protocol::AccountNetMessage>)> callback);
    int CreateTransaction(std::shared_ptr<protocol::TransferContract> transfercontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int BroadcastTransaction(std::shared_ptr<protocol::Transaction> transaction, std::function<void(std::shared_ptr<protocol::Return>)> callback);
    int UpdateAccount(std::shared_ptr<protocol::AccountUpdateContract> accountupdatecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int VoteWitnessAccount(std::shared_ptr<protocol::VoteWitnessContract> votewitnesscontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int CreateAssetIssue(std::shared_ptr<protocol::AssetIssueContract> assetissuecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int ListWitnesses(std::shared_ptr<protocol::EmptyMessage> emptymessage, std::function<void(std::shared_ptr<protocol::WitnessList>)> callback);
    int UpdateWitness(std::shared_ptr<protocol::WitnessUpdateContract> witnessupdatecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int CreateWitness(std::shared_ptr<protocol::WitnessCreateContract> witnesscreatecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int TransferAsset(std::shared_ptr<protocol::TransferAssetContract> transferassetcontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int ParticipateAssetIssue(std::shared_ptr<protocol::ParticipateAssetIssueContract> participateassetissuecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int FreezeBalance(std::shared_ptr<protocol::FreezeBalanceContract> freezebalancecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int UnfreezeBalance(std::shared_ptr<protocol::UnfreezeBalanceContract> unfreezebalancecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int WithdrawBalance(std::shared_ptr<protocol::WithdrawBalanceContract> withdrawbalancecontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int ListNodes(std::shared_ptr<protocol::EmptyMessage> emptymessage, std::function<void(std::shared_ptr<protocol::NodeList>)> callback);
    int GetAssetIssueList(std::shared_ptr<protocol::EmptyMessage> emptymessage, std::function<void(std::shared_ptr<protocol::AssetIssueList>)> callback);
    int GetAssetIssueByAccount(std::shared_ptr<protocol::Account> account, std::function<void(std::shared_ptr<protocol::AssetIssueList>)> callback);
    int GetAssetIssueByName(std::shared_ptr<protocol::BytesMessage> bytesmessage, std::function<void(std::shared_ptr<protocol::AssetIssueContract>)> callback);
    int GetNowBlock(std::shared_ptr<protocol::EmptyMessage> emptymessage, std::function<void(std::shared_ptr<protocol::Block>)> callback);
    int GetBlockByNum(std::shared_ptr<protocol::NumberMessage> numbermessage, std::function<void(std::shared_ptr<protocol::Block>)> callback);
    int TotalTransaction(std::shared_ptr<protocol::EmptyMessage> emptymessage, std::function<void(std::shared_ptr<protocol::NumberMessage>)> callback);
    int GetBlockById(std::shared_ptr<protocol::BytesMessage> bytesmessage, std::function<void(std::shared_ptr<protocol::Block>)> callback);
    int GetBlockByLimitNext(std::shared_ptr<protocol::BlockLimit> blocklimit, std::function<void(std::shared_ptr<protocol::BlockList>)> callback);
    int GetBlockByLatestNum(std::shared_ptr<protocol::NumberMessage> numbermessage, std::function<void(std::shared_ptr<protocol::BlockList>)> callback);
    int GetTransactionById(std::shared_ptr<protocol::BytesMessage> bytesmessage, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);
    int UpdateAsset(std::shared_ptr<protocol::UpdateAssetContract> updateassertcontract, std::function<void(std::shared_ptr<protocol::Transaction>)> callback);

    // 
    int GetTransactionsFromThis(std::shared_ptr<protocol::AccountPaginated> account, std::function<void(std::shared_ptr<protocol::TransactionList>)> callback);
    int GetTransactionsToThis(std::shared_ptr<protocol::AccountPaginated> account, std::function<void(std::shared_ptr<protocol::TransactionList>)> callback);

    void SetCallbackWnd(HWND hWnd) { callbackWnd_ = hWnd; }
    void Login(std::string password, std::function<void(std::string)> callback);
    void CreateAccountWrapper(const std::string& name, const std::string& password, std::function<void(void)> callback);
    void GetAccountWrapper(const std::string& address, std::function<void(std::shared_ptr<protocol::Account>)> callback);

private:
    void PostTask(std::function<void()> func);
    void InvokeCallback(void* callback);

    std::shared_ptr<grpc::ClientContext> GetRequestContext(NetworkRequestType type);
    std::shared_ptr<grpc::ClientContext> RegisterRequestContext(NetworkRequestType type);
    void UnregiesterRequestContext(NetworkRequestType type);


private:

    std::string full_address_port_ = "47.254.16.55:50051";
    std::string solidity_address_port_ = "47.254.39.153:50051";

    asio::io_service io_service_;
    asio::io_service::work io_service_work_;
    std::unique_ptr<protocol::Wallet::Stub> stub_;
    std::unique_ptr<protocol::WalletSolidity::Stub> solidity_stub_;
    std::unique_ptr<protocol::WalletExtension::Stub> extension_stub_;

    HWND callbackWnd_;
    std::mutex contextPoolLock_;
    std::map<NetworkRequestType, std::shared_ptr<grpc::ClientContext>> requestContextPool_;
    NetworkRequestType lastCallType_ = RequestCount;
};