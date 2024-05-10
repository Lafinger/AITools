// // Fill out your copyright notice in the Description page of Project Settings.
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "ChatModelsAsyncActionBase.h"
// #include "IVWebSocketNodes.h"
// #include "SparkModelAsyncAction.generated.h"
//
// DECLARE_LOG_CATEGORY_EXTERN(LogSparkModelAsyncAction, Log, All);
//
// UCLASS(BlueprintType, meta = (NotPlaceable, ExposedAsyncProxy = Task))
// class USparkModelAsyncAction  : public UChatModelsAsyncActionBase
// {
// 	GENERATED_BODY()
// public:
// 	UPROPERTY(BlueprintAssignable, meta=(DisplayName = "Connected"))
// 	FChatModelsAsyncActionBaseDelegate ConnectedDelegate;
// 	
// 	USparkModelAsyncAction(const FObjectInitializer& ObjectInitializer);
//
// 	virtual void Activate() override;
// 	
// 	/**
// 	* 建立星火大模型连接，并发送信息。
// 	* @param Message 发送的信息。
// */
// 	UFUNCTION(BlueprintCallable, Category = "Chat Models", meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "Headers", DisplayName = "Spark Model Connect"))
// 	static USparkModelAsyncAction* Connect(FString Message, bool bIsStreamingOutput);
//
// 	UFUNCTION(BlueprintCallable, Category = "Chat Models")
// 	void Close(const int32 Code = 1000, const FString& Reason = TEXT(""));
// 	
// 	int32 CaculateTokenCount(FString Message);
// 	
// protected:
// 	FORCEINLINE UIVWebSocketWrapper* GetSocket() { return Socket; }
//
// 	void InitSocket(UIVWebSocketWrapper* const InSocket, const FString & Url, const FString Protocol);
// 	
// private:
// 	UFUNCTION()
// 	void OnConnectedInternal();
// 	UFUNCTION()
// 	void OnConnectionErrorInternal(const FString& Error);
// 	UFUNCTION()
// 	void OnCompletedInternal(const int64 Status, const FString& Reason, const bool bWasClean);
// 	UFUNCTION()
// 	void OnMessageInternal(const FString& Message,const UIVJsonObject* Json);
// 	
// private:
// 	 FString GenerateSocketJsonString(FString NewMessage);
// 	
// 	 FString CreateURL();
//
// protected:
// 	UPROPERTY()
// 	UIVWebSocketWrapper* Socket;
// 	
// private:
// 	FString Host;
// 	FString Path;
// 	FString Protocol;
// 	FString Url;
//
// 	FString RequestMessage;
// 	FString ResponseMessage;
//
// 	bool bImageUnderstanding;
// 	bool bIsStreamingOutput;
// };
//
