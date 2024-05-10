// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Kismet/BlueprintAsyncActionBase.h"
#include "IVWebSocketNodes.generated.h"

class UIVWebSocketWrapper;
class UIVJsonObject;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FWebSocketEvent, UIVWebSocketWrapper* const, Socket, int32 const, StatusCode, const FString&, Message,const UIVJsonObject*, Json);

/**
  * 蓝图异步辅助节点的基类
  **/
UCLASS(Abstract)
class IVWEBSOCKET_API UIVWebSocketConnectAsyncProxyBase : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
public:
    UIVWebSocketConnectAsyncProxyBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};
    
    /* WebSocket 连接到服务器后调用。 */
    UPROPERTY(BlueprintAssignable)
    FWebSocketEvent OnConnected;
    
    /* 当WebSocket无法连接到服务器时调用。 */
    UPROPERTY(BlueprintAssignable)
    FWebSocketEvent OnConnectionError;
    
    /* 当与服务器的连接关闭时调用。 */
    UPROPERTY(BlueprintAssignable)
    FWebSocketEvent OnClose;

    /* 服务器发送消息时调用。 */
    UPROPERTY(BlueprintAssignable)
    FWebSocketEvent OnMessage;

    virtual void Activate();

protected:
    FORCEINLINE UIVWebSocketWrapper* GetSocket() { return Socket; }

    void InitSocket(UIVWebSocketWrapper* const InSocket, const FString & Url, const FString Protocol);

private:
    UFUNCTION()
    void OnConnectedInternal();
    UFUNCTION()
    void OnConnectionErrorInternal(const FString& Error);
    UFUNCTION()
    void OnCloseInternal(const int64 Status, const FString& Reason, const bool bWasClean);
    UFUNCTION()
    void OnMessageInternal(const FString& Message,const UIVJsonObject* Json);

protected:
    UPROPERTY()
    UIVWebSocketWrapper* Socket;

private:
    FString Url;
    FString Protocol;
};


UCLASS()
class UIVWebSocketConnectAsyncProxy final : public UIVWebSocketConnectAsyncProxyBase
{
    GENERATED_BODY()
public:
    UIVWebSocketConnectAsyncProxy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};

    /**
        * 轻松连接到服务器并处理连接事件。
        * @param WebSocket 用于连接的 WebSocket。
        * @param Url 我们要连接的服务器的 URL。
        * @param Protocol 要使用的协议。 应该是“ws”或“wss”。
     */
    UFUNCTION(BlueprintCallable, Category=WebSocket, meta = (BlueprintInternalUseOnly = "true", DisplayName = "Connect WebSocket to WebSocket server"))
    static UIVWebSocketConnectAsyncProxy* Connect(UIVWebSocketWrapper* const WebSocket, const FString & Url, const FString & Protocol);

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnReconnectionEvent, UIVWebSocketWrapper*, WebSocket, int32, RemainingAttempts);

UCLASS()
class UWebSocketCreateConnectAsyncProxy final : public UIVWebSocketConnectAsyncProxyBase
{
    GENERATED_BODY()
public:
    UWebSocketCreateConnectAsyncProxy(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {};

    /**
        * 帮助节点轻松连接到服务器并处理连接事件。
        * @param Url 我们要连接的服务器的 URL。
        * @param Protocol 要使用的协议。 应该是“ws”或“wss”。
        * @param Headers 升级期间使用的标头。
    */
    UFUNCTION(BlueprintCallable, Category=WebSocket, meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "Headers", DisplayName = "Connect to WebSocket server"))
    static UWebSocketCreateConnectAsyncProxy* Connect(const FString & Url, const FString & Protocol, const TMap<FString, FString> & Headers);

};

UCLASS()
class UWebSocketReconnectProxy final : public UBlueprintAsyncActionBase
{
    GENERATED_BODY()
public:
    /**
    * socket接到服务器。
    */
    UPROPERTY(BlueprintAssignable)
    FOnReconnectionEvent Connected;
    
    /**
     * 尝试重新连接失败。
    */
    UPROPERTY(BlueprintAssignable)
    FOnReconnectionEvent AttemptFailed;

    /**
     * 所有尝试都失败
    */
    UPROPERTY(BlueprintAssignable)
    FOnReconnectionEvent Failed;

public:
    /**
    * 尝试重新连接到指定的服务器。
    * @param WebSocket 我们要连接的 WebSocket。
    * @param Protocol 要使用的协议。
    * @param URL 服务器的 URL。
    * @param MaxAttemptCount 重新连接失败之前的最大尝试次数。 通过 < 1 表示无限制。
    */
    UFUNCTION(BlueprintCallable, Category = WebSocket, meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "", DisplayName = "Reconnect to WebSocket server"))
    static UWebSocketReconnectProxy* Reconnect(UIVWebSocketWrapper* const WebSocket, FString Protocol, FString URL, int32 MaxAttemptCount = 1);

    virtual void Activate() override;

private:
    UFUNCTION()
    void OnError(const FString& ConnectionError);
    UFUNCTION()
    void OnConnected();

    void OnFailed();

    void AttemptConnection();

private:
    UPROPERTY()
    UIVWebSocketWrapper* WebSocket;

    FString Protocol;
    FString URL;
    int32 RemainingAttempts;
};

