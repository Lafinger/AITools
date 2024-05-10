// Copyright 2023 AIRT,  All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IVWebSocketWrapper.generated.h"

class IWebSocket;

DECLARE_DYNAMIC_MULTICAST_DELEGATE            (FWebSocketConnectedEvent                                                                                            );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FWebSocketConnectionErrorEvent, const FString&,       ConnectionError                                               );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWebSocketClosedEvent,          int64,                StatusCode,   const FString&,         Reason, bool, bWasClean );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FWebSocketMessageEvent,         const FString&,       MessageString, const UIVJsonObject*, Json                    );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams  (FWebSocketRawMessageEvent,      const TArray<uint8>&, Data,         int32,                    BytesRemaining        );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FWebSocketMessageSentEvent,     const FString&,       MessageString                                                 );
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam   (FWebSocketMessageJsonSentEvent, const UIVJsonObject*, Json                                                          );

/**
 *	socket 状态，用于创建多个socket节点
 **/
UENUM(BlueprintType)
enum class EIVWebSocketIsConnectedExecution : uint8
{
	Connected,
	NotConnected
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class IVWEBSOCKET_API UIVWebSocketWrapper : public UObject
{
	GENERATED_BODY()

	public:
	UIVWebSocketWrapper();

	/**
	* 创建一个 WebSocket 对象以连接到 WebSocket 服务器。
	* @return 一个新的 WebSocket。
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = IVWebSocket)
	static UPARAM(DisplayName = "New WebSocket") UIVWebSocketWrapper* CreateWebSocket();

	/**
	* 使用指定的协议连接到 WebSocket 服务器。
	* @param Url 服务器的 URL。
	* @param Protocol 用于连接的协议。 “ws”或“wss”。
	*/
	UFUNCTION(BlueprintCallable, Category = IVWebSocket)
	void Connect(const FString& Url, const FString& Protocol);

	/**
	* 为连接设置指定的Headers。
	* 如果socket已经有Headers，它将保留旧Headers并用新值覆盖重复的Headers。
	* @param InHeaders 重新输入并合并的Headers。
	*/
	UFUNCTION(BlueprintCallable, Category = IVWebSocket)
	void SetHeaders(const TMap<FString, FString>& InHeaders);
	void SetHeaders(TMap<FString, FString>&& InHeaders);

	/**
	 *  将指定的key/value添加到Header。
	 *  @param Header 要添加的Header的名称。
	 *  @param Value Header的值
	 */
	UFUNCTION(BlueprintCallable, Category = IVWebSocket)
	void AddHeader(const FString & Header, const FString& Value);
	void AddHeader(FString&& Header, FString&& Value);

	/**
	* 从Headers列表中删除指定的Header。
	* @param HeaderToRemove 要删除的Header的名称
	*/
	UFUNCTION(BlueprintCallable, Category = IVWebSocket)
	void RemoveHeader(const FString & HeaderToRemove);

	/**
	* 返回此socket当前是否连接到服务器。
	* @return 是否连接
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = IVWebSocket)
	UPARAM(DisplayName="Is Connected") bool IsConnected() const;

	UFUNCTION(BlueprintCallable, Category = WebSocket, meta = (ExpandEnumAsExecs = Branches))
	void BranchIsConnected(EIVWebSocketIsConnectedExecution& Branches);
	
	/**
	* 将指定的消息发送到 WebSocket 服务器。
	* @param Message 要发送的数据。
	*/
	UFUNCTION(BlueprintCallable, Category = IVWebSocket)
	void SendMessage(const FString& Message);


	/**
	* 将指定的消息发送到 WebSocket 服务器。
	* @param Message 要发送的数据。
	*/
	UFUNCTION(BlueprintCallable, Category = IVWebSocket)
	void SendJsonMessage(const UIVJsonObject* Json);

	/**
	* 将指定的二进制消息发送到 WebSocket 服务器。
	* @param Message 要发送的数据。
	* @param bIsBinary 这是否是二进制消息。
	*/
	UFUNCTION(BlueprintCallable, Category = IVWebSocket)
	void SendRawMessage(const TArray<uint8> & Message, const bool bIsBinary = false);

	/**
	* 关闭与 WebSocket 服务器的连接。
	* @param Code 要发送到服务器的代码。
	* @param Reason 发送到服务器的原因。
	*/
	UFUNCTION(BlueprintCallable, Category = IVWebSocket)
	void Close(const int32 Code = 1000, const FString& Reason = TEXT(""));

	/* 当 WebSocket 成功连接到服务器时调用此事件。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = IVWebSocket)
	FWebSocketConnectedEvent OnConnectedEvent;

	/* 当 WebSocket 无法连接到服务器时调用此事件。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = IVWebSocket)
	FWebSocketConnectionErrorEvent OnConnectionErrorEvent;

	/* 当连接关闭时调用此事件。 连接可以由客户端或服务器关闭。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = IVWebSocket)
	FWebSocketClosedEvent OnCloseEvent;

	/* 当服务器发送字符串消息时调用此事件。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = IVWebSocket)
	FWebSocketMessageEvent OnMessageEvent;

	/* 当服务器发送二进制消息时调用此事件。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = IVWebSocket)
	FWebSocketRawMessageEvent OnRawMessageEvent;

	/* 消息发送到服务器后立即调用此事件。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintAssignable, Category = IVWebSocket)
	FWebSocketMessageSentEvent OnMessageSentEvent;

private:
	UFUNCTION() 
	void OnConnected();

	UFUNCTION()
	void OnConnectionError(const FString & Error);

	UFUNCTION()
	void OnClosed(int32 Status, const FString& Reason, bool bWasClean);

	UFUNCTION()
	void OnMessage(const FString& Message);

	void OnMessageSent(const FString& Message);

	void OnRawMessage(const void* Data, SIZE_T Size, SIZE_T BytesRemaining);

private:
	TMap<FString, FString> Headers;

	TSharedPtr<IWebSocket> NativeSocket;
};
