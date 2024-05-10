// // Fill out your copyright notice in the Description page of Project Settings.
//
//
// #include "SparkModelAsyncAction.h"
//
// #include "ChatModelsSettings.h"
// #include "ChatModelsSubsystem.h"
// #include "Json/IVJsonObject.h"
// #include "IVWebSocketWrapper.h"
// #include "GenericPlatform/GenericPlatformHttp.h"
// #include "hmac_sha256/hmac_sha256.h"
// #include "Json/IVJsonValue.h"
//
// DEFINE_LOG_CATEGORY(LogSparkModelAsyncAction);
//
// #define SHA256_HASH_SIZE 32
// const int START_STATUS = 0;
// const int RUNNING_STATUS = 1;
// const int END_STATUS = 2;
// const int MAX_TOKEN_COUNT = 8192;
//
// USparkModelAsyncAction::USparkModelAsyncAction(
// 	const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// 	,Socket(nullptr)
// 	,Host(TEXT("spark-api.xf-yun.com"))
// 	,Path(TEXT("/v3.1/chat"))
// 	,bImageUnderstanding(false)
// {
// 	if(UChatModelsSettings* Settings = UChatModelsSettings::Get())
// 	{
// 		Path= "/" + Settings->SparkModelSetting.GetVersion() + "/chat";
// 	}
// }
//
// void USparkModelAsyncAction::Activate()
// {
// 	Super::Activate();
// 	
// 	if (!Socket)
// 	{
// 		UE_LOG(LogSparkModelAsyncAction, Warning, TEXT("ThreadID:%d, %s: Passed an invalid socket to \"Connect WebSocket\"."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// 		ErrorDelegate.Broadcast(TEXT("Invalid socket"));
// 		SetReadyToDestroy();
// 		return;
// 	}
//
// 	if (Socket->IsConnected())
// 	{
// 		UE_LOG(LogSparkModelAsyncAction, Warning, TEXT("ThreadID:%d, %s: Connect() failed: The socket is already connected."), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// 		ErrorDelegate.Broadcast(TEXT("Socket already connected"));
// 		SetReadyToDestroy();
// 		return;
// 	}
//
// 	Socket->OnConnectedEvent		.AddDynamic(this, &USparkModelAsyncAction::OnConnectedInternal);
// 	Socket->OnConnectionErrorEvent	.AddDynamic(this, &USparkModelAsyncAction::OnConnectionErrorInternal);
// 	Socket->OnCloseEvent			.AddDynamic(this, &USparkModelAsyncAction::OnCompletedInternal);
// 	Socket->OnMessageEvent			.AddDynamic(this, &USparkModelAsyncAction::OnMessageInternal);
//
// 	Socket->Connect(Url, Protocol);
// }
//
// FString USparkModelAsyncAction::CreateURL() 
// {
// 	
// 	const UChatModelsSettings* Settings = GetDefault<UChatModelsSettings>();
// 	if (!Settings)
// 	{
// 		UE_LOG(LogSparkModelAsyncAction, Error, TEXT("ThreadID:%d, %s: setting is not valid"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// 		return FString();
// 	}
//
// 	FString OutUrl;
// 	FString Date =  FDateTime::UtcNow().ToHttpDate();
// 	FString APISecret = Settings->SparkModelSetting.APISecret;
// 	FString APIKey = Settings->SparkModelSetting.APIKey;
// 	
// 	FString SignatureOrigin = FString("host: ") + Host + "\n";
// 	SignatureOrigin += FString("date: ") + Date + "\n";
// 	SignatureOrigin += FString("GET ") + Path + " HTTP/1.1";
// 	
// 	uint8 Source[SHA256_HASH_SIZE];
// 	uint32 SecretSize = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*APISecret));
// 	uint32 TmpSize = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*SignatureOrigin));
// 	uint32 Size = hmac_sha256(TCHAR_TO_UTF8(*APISecret), SecretSize, TCHAR_TO_UTF8(*SignatureOrigin), TmpSize, Source, SHA256_HASH_SIZE);
//
// 	if (Size == 0) {
// 		UE_LOG(LogSparkModelAsyncAction, Error, TEXT("ThreadID:%d, %s: hmac_sha256 failed"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// 		return FString();
// 	}
//
// 	FString signature = FBase64::Encode(Source, SHA256_HASH_SIZE);
//
// 	FString Algorithm = "hmac-sha256";
// 	FString Authorization_Origin = FString::Format(TEXT("api_key=\"{0}\", algorithm=\"{1}\", headers=\"host date request-line\", signature=\"{2}\""), TArray<FStringFormatArg>({ APIKey, Algorithm, signature }));
// 	uint32 Length = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*Authorization_Origin));
// 	FString UnencodedString = FBase64::Encode((uint8*)TCHAR_TO_UTF8(*Authorization_Origin), Length);
//
// 	FString EncodeAuthorization = FGenericPlatformHttp::UrlEncode(UnencodedString);
// 	FString EncodeDate = FGenericPlatformHttp::UrlEncode(Date);
// 	OutUrl = FString::Format(TEXT("{0}?authorization={1}&date={2}&host={3}"), TArray<FStringFormatArg>({ "ws://"+Host+Path, EncodeAuthorization, EncodeDate, Host }));
//
// 	UE_LOG(LogSparkModelAsyncAction, Warning, TEXT("ThreadID:%d, %s: OutUrl : %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *OutUrl);
// 	return OutUrl;
// }
//
//
//
// USparkModelAsyncAction* USparkModelAsyncAction::Connect(FString Message,bool bIsStreamingOutput)
// {
// 	ThisClass* const Proxy = NewObject<ThisClass>();
//
// 	UIVWebSocketWrapper* const Socket = UIVWebSocketWrapper::CreateWebSocket();
// 	
// 	Proxy->InitSocket(Socket,Proxy->CreateURL(), TEXT(""));
//
// 	UChatModelsSubsystem* Subsystem = GEngine->GetEngineSubsystem<UChatModelsSubsystem>();
//
// 	FContextHistory HistoryMessage;
// 	HistoryMessage.Role = "user";
// 	HistoryMessage.Context = Message;
// 	
// 	Subsystem->ClearHistories(); // 永远只有一条历史
// 	
// 	Subsystem->AddHistory(HistoryMessage);
// 	Proxy->RequestMessage = Proxy->GenerateSocketJsonString(Message);
// 	Proxy->bIsStreamingOutput = bIsStreamingOutput;
// 	
// 	return Proxy;
// }
//
// void USparkModelAsyncAction::Close(const int32 Code, const FString& Reason)
// {
// 	Socket->Close(Code);
// }
//
// int32 USparkModelAsyncAction::CaculateTokenCount(FString Message)
// {
// 	int32 Result = FMath::CeilToInt32(Message.Len() / 1.5);
// 	return Result;
// }
//
// void USparkModelAsyncAction::InitSocket(UIVWebSocketWrapper* const InSocket, const FString& InUrl,
// 	const FString InProtocol)
// {
// 	Socket   = InSocket;
// 	Url		 = InUrl;
// 	Protocol = InProtocol;
// }
//
// void USparkModelAsyncAction::OnConnectedInternal()
// {
// 	ConnectedDelegate.Broadcast(TEXT("RequestMessage"));
//
// 	Socket->SendMessage(RequestMessage);
// }
//
// void USparkModelAsyncAction::OnConnectionErrorInternal(const FString& Error)
// {
// 	ErrorDelegate.Broadcast(TEXT("Error"));
// }
//
// void USparkModelAsyncAction::OnCompletedInternal(const int64 Status, const FString& Reason, const bool bWasClean)
// {
// 	FContextHistory HistoryMessage;
// 	HistoryMessage.Role="assistant";
// 	HistoryMessage.Context=ResponseMessage;
//
// 	UChatModelsSubsystem* Subsystem = GEngine->GetEngineSubsystem<UChatModelsSubsystem>();
// 	Subsystem->AddHistory(HistoryMessage);
// 	AsyncTask(ENamedThreads::GameThread, [this]()
// 	{
// 		this->CompletedDelegate.Broadcast(this->ResponseMessage);
// 	});
// 	SetReadyToDestroy();
// }
//
// void USparkModelAsyncAction::OnMessageInternal(const FString& Message, const UIVJsonObject* Json)
// {
// 	UE_LOG(LogSparkModelAsyncAction, Log, TEXT("ThreadID:%d, %s: OnMessageInternal : %s"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *Message);
// 	
// 	if(UIVJsonObject* Payload = Json->GetObjectField("payload"))
// 	{
// 		if(UIVJsonObject* Choices = Payload->GetObjectField("choices"))
// 		{
// 			TArray<UIVJsonValue*> Text = Choices->GetArrayField("text");
// 			if(Text.IsValidIndex(0))
// 			{
// 				UIVJsonObject* OutMessage = Text[0]->AsObject();
// 				FString TempMessage = OutMessage->GetStringField("content");
// 				if(!bIsStreamingOutput)
// 				{
// 					ResponseMessage += TempMessage;
// 					ResponseDelegate.Broadcast(ResponseMessage);
// 				}
// 				else
// 				{
// 					ResponseDelegate.Broadcast(TempMessage);
// 				}
// 				
// 			}
// 		}
// 	}	
// 	
// }
//
// FString USparkModelAsyncAction::GenerateSocketJsonString(FString NewMessage)
// {
// 	UChatModelsSettings* Settings = UChatModelsSettings::Get();
// 	if(!Settings)
// 	{
// 		return FString();
// 	}
//
// 	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
// 	
// 	TSharedPtr<FJsonObject> Header_JsonObject = MakeShareable(new FJsonObject);
// 	Header_JsonObject->SetStringField("app_id", Settings->SparkModelSetting.APPID);
// 	Header_JsonObject->SetStringField("uid", "IVSparkDesk");
// 	JsonObject->SetObjectField("header", Header_JsonObject);
//
// 	TSharedPtr<FJsonObject> Parameter_JsonObject = MakeShareable(new FJsonObject);
// 	TSharedPtr<FJsonObject> Parameter_Chat_JsonObject = MakeShareable(new FJsonObject);
// 	Settings->SparkModelSetting.AddQueryField(Parameter_Chat_JsonObject, bImageUnderstanding);
//
// 	Parameter_JsonObject->SetObjectField("chat", Parameter_Chat_JsonObject);
// 	JsonObject->SetObjectField("parameter", Parameter_JsonObject);
//
// 	TSharedPtr<FJsonObject> Payload_JsonObject = MakeShareable(new FJsonObject);
// 	TSharedPtr<FJsonObject> Payload_Message_JsonObject = MakeShareable(new FJsonObject);
//
// 	TArray<TSharedPtr<FJsonValue>> ChatArray;
// 	//int32 Total_TokenCount = CaculateTokenCount(NewMessage);
//
// 	UChatModelsSubsystem* Subsystem = GEngine->GetEngineSubsystem<UChatModelsSubsystem>();
// 	
// 	TArray<FContextHistory> History = Subsystem->ContextHistories;
//
// 	if(!History.IsEmpty())
// 	{
// 		auto& HistoryMessage = History[0];
// 		int32 TokenCount = CaculateTokenCount(HistoryMessage.Context); // 永远只有一条历史
// 		if(TokenCount < 8100)
// 		{
// 			TSharedRef<FJsonObject> JsonMessageObject = MakeShareable(new FJsonObject());
// 			JsonMessageObject->SetStringField("role", HistoryMessage.Role);
// 			JsonMessageObject->SetStringField("content", HistoryMessage.Context);
// 			if(bImageUnderstanding)
// 			{
// 				JsonMessageObject->SetStringField("content_type", "image");
// 			}
// 			TSharedPtr<FJsonValue> JsonMessageValue = MakeShareable(new FJsonValueObject(JsonMessageObject));
// 			ChatArray.Insert(JsonMessageValue, 0);
// 		}
// 		Subsystem->ClearHistories();
// 	}
// 	// for (int32 i = History.Num() - 1 ; i >= 0; i--)
// 	// {
// 	// 	auto& HistoryMessage =History[i];
// 	// 	int32 TokenCount = CaculateTokenCount(HistoryMessage.Context); // 永远只有一条历史
// 	// 	
// 	//
// 	// 	TSharedRef<FJsonObject> JsonMessageObject = MakeShareable(new FJsonObject());
// 	// 	JsonMessageObject->SetStringField("role", HistoryMessage.Role);
// 	// 	JsonMessageObject->SetStringField("content", HistoryMessage.Context);
// 	// 	if(bImageUnderstanding &&i==0)
// 	// 	{
// 	// 		JsonMessageObject->SetStringField("content_type", "image");
// 	// 	}
// 	// 	TSharedPtr<FJsonValue> JsonMessageValue = MakeShareable(new FJsonValueObject(JsonMessageObject));
// 	// 	ChatArray.Insert(JsonMessageValue, 0);
// 	// }
// 	
//
// 	Payload_Message_JsonObject->SetArrayField("text", ChatArray);
// 	Payload_JsonObject->SetObjectField("message", Payload_Message_JsonObject);
// 	JsonObject->SetObjectField("payload", Payload_JsonObject);
//
// 	FString SocketMessage;
// 	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&SocketMessage);
// 	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
//
//
// 	return SocketMessage;
// }
