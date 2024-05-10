// Fill out your copyright notice in the Description page of Project Settings.


#include "IVXunFeiNodes.h"

#include "IVXunFeiSpeech.h"
#include "IVXunFeiSpeechFunctionLibrary.h"
#include "IVXunFeiSpeechSettings.h"
#include "IWebSocket.h"
#include "JsonObjectConverter.h"
#include "WebSocketsModule.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Json/IVJsonObject.h"
#include "Json/IVJsonValue.h"
#include "hmac_sha256/hmac_sha256.h"
#include "iVisualFileHeplerBPLibrary.h"

#if WITH_EDITOR
#include <Editor.h>
#endif


#define SHA256_HASH_SIZE 32
const int START_STATUS = 0;
const int RUNNING_STATUS = 1;
const int END_STATUS = 2;
const int MAX_TOKEN_COUNT = 8192;

struct FXunFeiWaveHeard
{

	int8 ChunkID[4];

	uint32 ChunkSize;

	int8 Format[4];
};


struct FXunFeiWaveFmt
{

	int8 SubChunkID[4];

	uint32 SubChunkSize;

	uint16 AudioFormat;

	uint16 NumChannel;
	
	uint32 SampleRate;

	uint32 ByteRate;

	uint16 BlockAlign;

	uint16 BitsForSample;
	
};


struct FXunFeiWaveData
{

	int8 DataChunkID[4];

	uint32 DataChunkSize;

};


void UIVFunFeiSpeechAsyncProxyBase::Cancel()
{
	if(Socket)
	{
		Socket->Close();
	}

	SetReadyToDestroy();
}

void UIVFunFeiSpeechAsyncProxyBase::Activate()
{
	UE_LOG(LogIVXunFei, Display, TEXT("Task: %s (%d); Function: %s; Message: Activating task"), *TaskName.ToString(), GetUniqueID(), *FString(__func__));

	bIsTaskActive = true;

	Super::Activate();

	if (!StartTaskWork())
	{
		UE_LOG(LogIVXunFei, Error, TEXT("Task: %s (%d); Function: %s; Message: Failed to activate task"), *TaskName.ToString(), GetUniqueID(), *FString(__func__));
		SetReadyToDestroy();
		return;
	}

#if WITH_EDITOR
	if (bIsEditorTask)
	{
		SetFlags(RF_Standalone);
	}
	else
	{
		FEditorDelegates::PrePIEEnded.AddUObject(this, &UIVFunFeiSpeechAsyncProxyBase::PrePIEEnded);
	}
#endif
	
}

void UIVFunFeiSpeechAsyncProxyBase::SetReadyToDestroy()
{
	UE_LOG(LogIVXunFei, Display, TEXT("Task: %s (%d); Function: %s; Message: Setting task as Ready to Destroy"), *TaskName.ToString(), GetUniqueID(), *FString(__func__));
	bIsReadyToDestroy = true;

#if WITH_EDITOR
	
	if (bIsEditorTask)
	{
		ClearFlags(RF_Standalone);
		MarkAsGarbage();
	}

	if (FEditorDelegates::PrePIEEnded.IsBoundToObject(this))
	{
		FEditorDelegates::PrePIEEnded.RemoveAll(this);
	}
#endif

	Super::SetReadyToDestroy();
}

bool UIVFunFeiSpeechAsyncProxyBase::StartTaskWork()
{
	UE_LOG(LogIVXunFei, Display, TEXT("Task: %s (%d); Function: %s; Message: Starting IVFunFeiSpeech task"), *TaskName.ToString(), GetUniqueID(), *FString(__func__));

	return UFunFeiSpeechTaskStatus::IsTaskStillValid(this);
}

void UIVFunFeiSpeechAsyncProxyBase::Connect()
{

}

void UIVFunFeiSpeechAsyncProxyBase::CreateURL(FString APISecret, FString APIKey)
{
	FString Date =  FDateTime::UtcNow().ToHttpDate();
	
	FString SignatureOrigin = FString("host: ") + Host + "\n";
	SignatureOrigin += FString("date: ") + Date + "\n";
	SignatureOrigin += FString("GET ") + Path + " HTTP/1.1";
	
	uint8 Source[SHA256_HASH_SIZE];
	uint32 SecretSize = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*APISecret));
	uint32 TmpSize = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*SignatureOrigin));
	uint32 Size = hmac_sha256(TCHAR_TO_UTF8(*APISecret), SecretSize, TCHAR_TO_UTF8(*SignatureOrigin), TmpSize, Source, SHA256_HASH_SIZE);

	if (Size == 0) {
		UE_LOG(LogIVXunFei, Error, TEXT("hmac_sha256 failed"));
		return ;
	}

	FString signature = FBase64::Encode(Source, SHA256_HASH_SIZE);

	FString Algorithm = "hmac-sha256";
	FString Authorization_Origin = FString::Format(TEXT("api_key=\"{0}\", algorithm=\"{1}\", headers=\"host date request-line\", signature=\"{2}\""), TArray<FStringFormatArg>({ APIKey, Algorithm, signature }));
	uint32 Length = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*Authorization_Origin));
	FString UnencodedString = FBase64::Encode((uint8*)TCHAR_TO_UTF8(*Authorization_Origin), Length);

	FString EncodeAuthorization = FGenericPlatformHttp::UrlEncode(UnencodedString);
	FString EncodeDate = FGenericPlatformHttp::UrlEncode(Date);
	Url = FString::Format(TEXT("{0}?authorization={1}&date={2}&host={3}"), TArray<FStringFormatArg>({ "ws://"+Host+Path, EncodeAuthorization, EncodeDate, Host }));
}


void UIVFunFeiSpeechAsyncProxyBase::SendMessage(const FString& Message)
{
	if (IsConnected())
	{
		Socket->Send(Message);
	}
	else
	{
		UE_LOG(LogIVXunFei, Warning, TEXT("Task: %s (%d);SendMessage() called but the WebSocket is not connected."),*TaskName.ToString(), GetUniqueID());
	}
}

bool UIVFunFeiSpeechAsyncProxyBase::IsConnected() const
{
	return Socket && Socket->IsConnected();
}

void UIVFunFeiSpeechAsyncProxyBase::OnConnectedInternal()
{
	UE_LOG(LogIVXunFei, Log, TEXT("Task: %s (%d);OnConnectedInternal() WebSocket Connected"),*TaskName.ToString(), GetUniqueID());
}

void UIVFunFeiSpeechAsyncProxyBase::OnConnectionErrorInternal(const FString& Error)
{
	UE_LOG(LogIVXunFei, Warning, TEXT("Task: %s (%d);OnConnectionErrorInternal() WebSocket Connected Error"),*TaskName.ToString(), GetUniqueID());
}

void UIVFunFeiSpeechAsyncProxyBase::OnCloseInternal(const int32 Status, const FString& Reason, const bool bWasClean)
{
	UE_LOG(LogIVXunFei, Log, TEXT("Task: %s (%d);OnCloseInternal() WebSocket Closed,Reason:%s"),*TaskName.ToString(), GetUniqueID(),*Reason);
}

void UIVFunFeiSpeechAsyncProxyBase::OnMessageInternal(const FString& Message)
{
	UE_LOG(LogIVXunFei, Log, TEXT("Task: %s (%d);OnMessageInternal() WebSocket on  Message"),*TaskName.ToString(), GetUniqueID());
}

#if WITH_EDITOR
void UIVFunFeiSpeechAsyncProxyBase::PrePIEEnded(bool bIsSimulating)
{
	if (!UFunFeiSpeechTaskStatus::IsTaskStillValid(this))
	{
		return;
	}

	UE_LOG(LogIVXunFei, Display, TEXT("Task: %s (%d); Function: %s; Message: Trying to finish task due to PIE end"), *TaskName.ToString(), GetUniqueID(), *FString(__func__));

	bEndingPIE = true;
	Cancel();
}
#endif

bool UFunFeiSpeechTaskStatus::IsTaskReadyToDestroy(const UIVFunFeiSpeechAsyncProxyBase* const Test)
{
	return IsValid(Test) && Test->bIsReadyToDestroy;
}

bool UFunFeiSpeechTaskStatus::IsTaskStillValid(const UIVFunFeiSpeechAsyncProxyBase* const Test)
{
	bool bOutput = IsValid(Test) && !IsTaskReadyToDestroy(Test);

#if WITH_EDITOR
	bOutput = bOutput && !Test->bEndingPIE;
#endif

	return bOutput;
}

UIVSparkDeskWebSocketAsyncProxy::UIVSparkDeskWebSocketAsyncProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	,bImageUnderstanding(false)
	,bSplice(false)
{
	Host=TEXT("spark-api.xf-yun.com");
	Path=TEXT("/v3.1/chat");
}


void UIVSparkDeskWebSocketAsyncProxy::Connect()
{
	Socket = FWebSocketsModule::Get().CreateWebSocket(Url);
	Socket->OnConnected().AddUObject(this, &UIVSparkDeskWebSocketAsyncProxy::OnConnectedInternal);
	Socket->OnConnectionError().AddUObject(this, &UIVSparkDeskWebSocketAsyncProxy::OnConnectionErrorInternal);
	Socket->OnClosed().AddUObject(this, &UIVSparkDeskWebSocketAsyncProxy::OnCloseInternal);
	Socket->OnMessage().AddUObject(this, &UIVSparkDeskWebSocketAsyncProxy::OnMessageInternal);
	Socket->Connect();
}

UIVSparkDeskWebSocketAsyncProxy* UIVSparkDeskWebSocketAsyncProxy::SparkDeskChat(FString Message,FSparkDeskOptions InSparkDeskOptions, bool bInSplice)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	FSparkDeskMessage HistoryMessage;
	HistoryMessage.Role="user";
	HistoryMessage.Content=Message;
	UIVXunFeiSpeechFunctionLibrary::AddHistory(HistoryMessage);
	Proxy->TaskName = *FString(__func__);
	Proxy->RequestMessage=Proxy->GenerateSocketJsonString(Message);
	Proxy->bSplice=bInSplice;
	Proxy->SparkDeskOptions=InSparkDeskOptions;
	Proxy->Path= "/"+InSparkDeskOptions.GetVersion()+"/chat";
	
	const UIVXunFeiSpeechSettings* Settings = GetDefault<UIVXunFeiSpeechSettings>();
	Proxy->CreateURL(Settings->SparkDeskParams.APISecret,Settings->SparkDeskParams.APIKey);
	Proxy->Connect();
	return Proxy;
}

UIVSparkDeskWebSocketAsyncProxy* UIVSparkDeskWebSocketAsyncProxy::SparkDeskChaTextureUnderstanding(FString Message,
	UTexture2D* Texture,FSparkDeskOptions SparkDeskOptions)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	FSparkDeskMessage HistoryMessage;
	HistoryMessage.Role="user";
	HistoryMessage.Content= UiVisualFileHeplerBPLibrary::MakeTexture2Base64EncodeData(Texture);
	UIVXunFeiSpeechFunctionLibrary::AddHistory(HistoryMessage);

	HistoryMessage.Role="user";
	HistoryMessage.Content=Message;
	UIVXunFeiSpeechFunctionLibrary::AddHistory(HistoryMessage);

	Proxy->TaskName = *FString(__func__);
	Proxy->Path=TEXT("/v2.1/image");
	Proxy->Host=TEXT("spark-api.cn-huabei-1.xf-yun.com");
	Proxy->bImageUnderstanding=true;
	Proxy->RequestMessage=Proxy->GenerateSocketJsonString(Message);

	const UIVXunFeiSpeechSettings* Settings = GetDefault<UIVXunFeiSpeechSettings>();
	Proxy->CreateURL(Settings->SparkDeskParams.APISecret,Settings->SparkDeskParams.APIKey);
	Proxy->Connect();
	return Proxy;
	
}

UIVSparkDeskWebSocketAsyncProxy* UIVSparkDeskWebSocketAsyncProxy::SparkDeskChaExplorerImageUnderstanding(
	FString Message, const FString& DefaultPath, FSparkDeskOptions SparkDeskOptions)
{
	UTexture2D* Texture= UiVisualFileHeplerBPLibrary::LoadImageFromExplorer(FPaths::ConvertRelativePathToFull(DefaultPath));

	if(Texture)
	{
		return SparkDeskChaTextureUnderstanding(Message,Texture,SparkDeskOptions);
	}
	else
	{
		return nullptr;
	}
}

void UIVSparkDeskWebSocketAsyncProxy::OnConnectedInternal()
{
	Super::OnConnectedInternal();
	OnConnected.Broadcast(TEXT("RequestMessage"));

	SendMessage(RequestMessage);
}

void UIVSparkDeskWebSocketAsyncProxy::OnConnectionErrorInternal(const FString& Error)
{
	Super::OnConnectionErrorInternal(Error);
	OnConnected.Broadcast(Error);

	SetReadyToDestroy();
}

void UIVSparkDeskWebSocketAsyncProxy::OnCloseInternal(const int32 Status, const FString& Reason, const bool bWasClean)
{
	Super::OnCloseInternal(Status,Reason,bWasClean);
	FSparkDeskMessage HistoryMessage;
	HistoryMessage.Role="assistant";
	HistoryMessage.Content=ResponseMessage;
	
	UIVXunFeiSpeechFunctionLibrary::AddHistory(HistoryMessage);
	
	OnClose.Broadcast( ResponseMessage);
	
	SetReadyToDestroy();
}

void UIVSparkDeskWebSocketAsyncProxy::OnMessageInternal(const FString& Message)
{
	UIVJsonObject* Json=NewObject<UIVJsonObject>();
	if(!Json->DecodeJson(Message))
	{
		UIVJsonValue* JsonValue=UIVJsonValue::ValueFromJsonString(Message);
		if(!IsValid(JsonValue))
		{
			return;
		}
		Json=JsonValue->AsObject();
	}
	
	if(UIVJsonObject* Payload=Json->GetObjectField("payload"))
	{
		if(UIVJsonObject* Choices=Payload->GetObjectField("choices"))
		{
			TArray<UIVJsonValue*> Text=Choices->GetArrayField("text");
			if(Text.IsValidIndex(0))
			{
				UIVJsonObject* OutMessage=Text[0]->AsObject();
				FString TempMessage=OutMessage->GetStringField("content");
				if(bSplice)
				{
					ResponseMessage +=TempMessage;
					OnMessage.Broadcast( ResponseMessage);
				}
				else
				{
					ResponseMessage +=TempMessage;
					OnMessage.Broadcast( TempMessage);
				}
				
			}
		}
	}
	else
	{
		OnConnected.Broadcast(Message);
		UE_LOG(LogIVXunFei, Warning, TEXT("Task: %s (%d);OnConnectionErrorInternal() WebSocket Connected Error,Message:%s"),*TaskName.ToString(), GetUniqueID(),*Message);
	}	
}

int32 UIVSparkDeskWebSocketAsyncProxy::CaculateTokenCount(FString Message)
{
	int32 Result = FMath::CeilToInt32(Message.Len() / 1.5);
	return Result;
}


FString UIVSparkDeskWebSocketAsyncProxy::GenerateSocketJsonString(FString NewMessage)
{
	UIVXunFeiSpeechSettings* Settings=UIVXunFeiSpeechSettings::Get();
	if(!Settings)
	{
		return FString();
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	
	TSharedPtr<FJsonObject> Header_JsonObject = MakeShareable(new FJsonObject);
	Header_JsonObject->SetStringField("app_id", Settings->SparkDeskParams.APPID);
	Header_JsonObject->SetStringField("uid", "IVSparkDesk");
	JsonObject->SetObjectField("header", Header_JsonObject);

	TSharedPtr<FJsonObject> Parameter_JsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> Parameter_Chat_JsonObject = MakeShareable(new FJsonObject);
	SparkDeskOptions.AddQueryField(Parameter_Chat_JsonObject,bImageUnderstanding);

	Parameter_JsonObject->SetObjectField("chat", Parameter_Chat_JsonObject);
	JsonObject->SetObjectField("parameter", Parameter_JsonObject);

	TSharedPtr<FJsonObject> Payload_JsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> Payload_Message_JsonObject = MakeShareable(new FJsonObject);

	TArray<TSharedPtr<FJsonValue>> ChatArray;
	//int32 Total_TokenCount = CaculateTokenCount(NewMessage);
	
	
	TArray<FSparkDeskMessage> History=UIVXunFeiSpeechFunctionLibrary::History;
	
	for (int32 i = History.Num() - 1 ; i >= 0; i--)
	{
		auto& HistoryMessage =History[i];
		int32 TokenCount = CaculateTokenCount(HistoryMessage.Content);

		TSharedRef<FJsonObject> JsonMessageObject = MakeShareable(new FJsonObject());
		JsonMessageObject->SetStringField("role", HistoryMessage.Role);
		JsonMessageObject->SetStringField("content", HistoryMessage.Content);
		if(bImageUnderstanding &&i==0)
		{
			JsonMessageObject->SetStringField("content_type", "image");
		}
		TSharedPtr<FJsonValue> JsonMessageValue = MakeShareable(new FJsonValueObject(JsonMessageObject));
		ChatArray.Insert(JsonMessageValue, 0);
	}
	

	Payload_Message_JsonObject->SetArrayField("text", ChatArray);
	Payload_JsonObject->SetObjectField("message", Payload_Message_JsonObject);
	JsonObject->SetObjectField("payload", Payload_JsonObject);

	FString SocketMessage;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&SocketMessage);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);


	return SocketMessage;
}

UIVXunFeiTTSWebSocketAsyncProxyBase::UIVXunFeiTTSWebSocketAsyncProxyBase(const FObjectInitializer& ObjectInitializer)
:Super(ObjectInitializer)
{
	Host=TEXT("tts-api.xfyun.cn");
	Path=TEXT("/v2/tts");
}

FString UIVXunFeiTTSWebSocketAsyncProxyBase::GenerateSocketJsonString(FString NewMessage)
{
	UIVXunFeiSpeechSettings* Settings = UIVXunFeiSpeechSettings::Get();
	if(!Settings)
	{
		return FString();
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	
	TSharedPtr<FJsonObject> Common_JsonObject = MakeShareable(new FJsonObject);
	Common_JsonObject->SetStringField("app_id", Settings->TTSParams.APPID);
	JsonObject->SetObjectField("common", Common_JsonObject);

	TSharedPtr<FJsonObject> Parameter_JsonObject = MakeShareable(new FJsonObject);
	TTSOptions.AddQueryField(Parameter_JsonObject);
	JsonObject->SetObjectField("business", Parameter_JsonObject);

	TSharedPtr<FJsonObject> Data_JsonObject = MakeShareable(new FJsonObject);
	Data_JsonObject->SetNumberField("status",2);
	auto UTF8String = StringCast<UTF8CHAR>(*NewMessage);
	FString Base64Text = FBase64::Encode((uint8*)UTF8String.Get(), UTF8String.Length());
	Data_JsonObject->SetStringField("text", Base64Text);
	JsonObject->SetObjectField("data", Data_JsonObject);

	FString SocketMessage;
	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&SocketMessage);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
 	
	return SocketMessage;
}

void UIVXunFeiTTSWebSocketAsyncProxyBase::ConvertPCMToWave(const TArray<uint8>& InPCMData, TArray<uint8>& OutWaveData)
{
	OutWaveData.Empty();
	FXunFeiWaveHeard WaveHeard;
	FXunFeiWaveFmt WaveFmt;
	FXunFeiWaveData WaveData;

	FMemory::Memcpy(WaveHeard.ChunkID, "RIFF", strlen("RIFF"));
	FMemory::Memcpy(WaveHeard.Format, "WAVE", strlen("WAVE"));
	WaveHeard.ChunkSize = 36 + InPCMData.Num() * sizeof(uint8);

	FMemory::Memcpy(WaveFmt.SubChunkID, "fmt ", strlen("fmt "));
	WaveFmt.SubChunkSize = 16;
	WaveFmt.AudioFormat = 1;
	WaveFmt.NumChannel = 1;
	WaveFmt.SampleRate = 16000;
	WaveFmt.BitsForSample = 16;
	WaveFmt.ByteRate = 16000 * 1 * 16 / 8;
	WaveFmt.BlockAlign = 1 * 16 / 8;

	FMemory::Memcpy(WaveData.DataChunkID, "data", strlen("data"));
	WaveData.DataChunkSize = InPCMData.Num() * sizeof(uint8);


	OutWaveData.AddUninitialized(sizeof(FXunFeiWaveHeard));
	FMemory::Memcpy(OutWaveData.GetData(), &WaveHeard, sizeof(FXunFeiWaveHeard));


	int32 Index = OutWaveData.AddUninitialized(sizeof(FXunFeiWaveFmt));
	FMemory::Memcpy(&OutWaveData[Index], &WaveFmt, sizeof(FXunFeiWaveFmt));


	Index = OutWaveData.AddUninitialized(sizeof(FXunFeiWaveData));
	FMemory::Memcpy(&OutWaveData[Index], &WaveData, sizeof(FXunFeiWaveData));


	OutWaveData.Append(InPCMData);
}


UIVXunFeiTTSWebSocketAsyncProxy::UIVXunFeiTTSWebSocketAsyncProxy(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Host=TEXT("tts-api.xfyun.cn");
	Path=TEXT("/v2/tts");
}

UIVXunFeiTTSWebSocketAsyncProxy* UIVXunFeiTTSWebSocketAsyncProxy::XunFeiTextToSoundWave(const UObject* WorldContextObject, FString Message,
	FXunFeiTTSOptions InTTSOptions)
{
	ThisClass* const Proxy = NewObject<ThisClass>();
	
	Proxy->RegisterWithGameInstance(WorldContextObject);
	Proxy->AddToRoot();
	
	Proxy->TaskName = *FString(__func__);
	const UIVXunFeiSpeechSettings* Settings = GetDefault<UIVXunFeiSpeechSettings>();
	Proxy->CreateURL(Settings->TTSParams.APISecret, Settings->TTSParams.APIKey);
	Proxy->TTSOptions = InTTSOptions;
	Proxy->RequestMessage = Proxy->GenerateSocketJsonString(Message);
	Proxy->Connect();
	return Proxy;
}

void UIVXunFeiTTSWebSocketAsyncProxy::Connect()
{
	Socket = FWebSocketsModule::Get().CreateWebSocket(Url);
	Socket->OnConnected().AddUObject(this, &UIVXunFeiTTSWebSocketAsyncProxy::OnConnectedInternal);
	Socket->OnConnectionError().AddUObject(this, &UIVXunFeiTTSWebSocketAsyncProxy::OnConnectionErrorInternal);
	Socket->OnClosed().AddUObject(this, &UIVXunFeiTTSWebSocketAsyncProxy::OnCloseInternal);
	Socket->OnMessage().AddUObject(this, &UIVXunFeiTTSWebSocketAsyncProxy::OnMessageInternal);
	Socket->Connect();
}

void UIVXunFeiTTSWebSocketAsyncProxy::SetReadyToDestroy()
{
	this->RemoveFromRoot();
	Super::SetReadyToDestroy();
}

void UIVXunFeiTTSWebSocketAsyncProxy::OnConnectedInternal()
{
	UE_LOG(LogIVXunFei, Display, TEXT("%s: Xun fei TTS web socket has connected."), *FString(__FUNCTION__));
	Super::OnConnectedInternal();
	OnConnected.Broadcast(nullptr,0.0f);
	OnConnectedC.ExecuteIfBound(nullptr,0.0f);
	SendMessage(RequestMessage);
}

void UIVXunFeiTTSWebSocketAsyncProxy::OnConnectionErrorInternal(const FString& Error)
{
	Super::OnConnectionErrorInternal(Error);
	OnConnectionError.Broadcast(nullptr,0.0f);

	SetReadyToDestroy();
}

void UIVXunFeiTTSWebSocketAsyncProxy::OnCloseInternal(const int32 Status, const FString& Reason, const bool bWasClean)
{
	Super::OnCloseInternal(Status, Reason, bWasClean);
	
	SetReadyToDestroy();
}

void UIVXunFeiTTSWebSocketAsyncProxy::OnMessageInternal(const FString& Message)
{
	FXGXunFeiTTSRespInfo XunFeiTTSRespInfo;
	FJsonObjectConverter::JsonObjectStringToUStruct(Message, &XunFeiTTSRespInfo);

	if (XunFeiTTSRespInfo.code == 0)
	{
		FString Audio = XunFeiTTSRespInfo.data.audio;
		int32	Status = XunFeiTTSRespInfo.data.status;
		FString Ced = XunFeiTTSRespInfo.data.ced;

		bool bRightStatus = Status == 1 || Status == 2;

		if (!Audio.IsEmpty() && bRightStatus)
		{
			TArray<uint8> AudioData;
			FBase64::Decode(Audio, AudioData);
			XunFeiAudioData.Append(AudioData);
		}

		if (Status == 2)
		{
			TArray<uint8> WaveData;
			ConvertPCMToWave(XunFeiAudioData,WaveData);
			USoundWave* SoundWave = UiVisualFileHeplerBPLibrary::ConvertAudioDataToSoundWave(WaveData);
			
			OnClose.Broadcast(SoundWave,SoundWave->Duration);
			OnCloseC.ExecuteIfBound(SoundWave,SoundWave->Duration);
			UE_LOG(LogIVXunFei, Display, TEXT("%s: Xun fei TTS sound wave has completed."), *FString(__FUNCTION__));
			SetReadyToDestroy();
		}


	}
	else
	{
		FString ErrorMessage = FString::Printf(TEXT("MessageError-Code:[%d]-Message:[%s]-sid-[%s]"),
			XunFeiTTSRespInfo.code,
			*(XunFeiTTSRespInfo.message),
			*(XunFeiTTSRespInfo.sid));

		UE_LOG(LogIVXunFei, Error, TEXT("%s:%s"), *FString(__FUNCTION__), *ErrorMessage);
		
		OnConnectionError.Broadcast(nullptr,0.0f);
		OnConnectionErrorC.ExecuteIfBound(nullptr,0.0f);
		SetReadyToDestroy();
	}

}




UIVXunFeiTTSWavFileWebSocketAsyncProxy::UIVXunFeiTTSWavFileWebSocketAsyncProxy(
	const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
 {

}

UIVXunFeiTTSWavFileWebSocketAsyncProxy* UIVXunFeiTTSWavFileWebSocketAsyncProxy::XunFeiTextToWavFile(FString Message,
	FXunFeiTTSOptions InTTSOptions, const FString& InFilePath)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->TaskName = *FString(__func__);
	Proxy->FilePath=InFilePath;
	const UIVXunFeiSpeechSettings* Settings = GetDefault<UIVXunFeiSpeechSettings>();
	Proxy->CreateURL(Settings->TTSParams.APISecret,Settings->TTSParams.APIKey);
	Proxy->	TTSOptions=InTTSOptions;
	Proxy->RequestMessage=Proxy->GenerateSocketJsonString(Message);
	Proxy->Connect();
	return Proxy;
}

void UIVXunFeiTTSWavFileWebSocketAsyncProxy::Connect()
{
 	Socket = FWebSocketsModule::Get().CreateWebSocket(Url);
 	Socket->OnConnected().AddUObject(this, &UIVXunFeiTTSWavFileWebSocketAsyncProxy::OnConnectedInternal);
 	Socket->OnConnectionError().AddUObject(this, &UIVXunFeiTTSWavFileWebSocketAsyncProxy::OnConnectionErrorInternal);
 	Socket->OnClosed().AddUObject(this, &UIVXunFeiTTSWavFileWebSocketAsyncProxy::OnCloseInternal);
 	Socket->OnMessage().AddUObject(this, &UIVXunFeiTTSWavFileWebSocketAsyncProxy::OnMessageInternal);
 	Socket->Connect();
}

void UIVXunFeiTTSWavFileWebSocketAsyncProxy::OnConnectedInternal()
{
 	Super::OnConnectedInternal();
 	OnConnected.Broadcast(0.0f);
 	SendMessage(RequestMessage);
}

void UIVXunFeiTTSWavFileWebSocketAsyncProxy::OnConnectionErrorInternal(const FString& Error)
{
 	Super::OnConnectionErrorInternal(Error);
 	OnConnectionError.Broadcast(0.0f);

 	SetReadyToDestroy();
}

void UIVXunFeiTTSWavFileWebSocketAsyncProxy::OnCloseInternal(const int32 Status, const FString& Reason,
	const bool bWasClean)
{
 	Super::OnCloseInternal(Status, Reason, bWasClean);
	
 	SetReadyToDestroy();
}

void UIVXunFeiTTSWavFileWebSocketAsyncProxy::OnMessageInternal(const FString& Message)
{
 	FXGXunFeiTTSRespInfo XunFeiTTSRespInfo;
 	FJsonObjectConverter::JsonObjectStringToUStruct(Message, &XunFeiTTSRespInfo);

 	if (XunFeiTTSRespInfo.code == 0)
 	{
 		FString Audio = XunFeiTTSRespInfo.data.audio;
 		int32	Status = XunFeiTTSRespInfo.data.status;
 		FString Ced = XunFeiTTSRespInfo.data.ced;

 		bool bRightStatus = Status == 1 || Status == 2;

 		if (!Audio.IsEmpty() && bRightStatus)
 		{
 			TArray<uint8> AudioData;
 			FBase64::Decode(Audio, AudioData);
 			XunFeiAudioData.Append(AudioData);
 		}

 		if (Status == 2)
 		{
 			TArray<uint8> WaveData;
 			ConvertPCMToWave(XunFeiAudioData,WaveData);
 			FFileHelper::SaveArrayToFile(WaveData, *FilePath);
 			USoundWave* SoundWave = UiVisualFileHeplerBPLibrary::ConvertAudioDataToSoundWave(WaveData);
 			
 			OnClose.Broadcast(SoundWave->Duration);
 			SetReadyToDestroy();
 		}


 	}
 	else
 	{
 		FString ErrorMessage = FString::Printf(TEXT("MessageError-Code:[%d]-Message:[%s]-sid-[%s]"),
			 XunFeiTTSRespInfo.code,
			 *(XunFeiTTSRespInfo.message),
			 *(XunFeiTTSRespInfo.sid));

 		UE_LOG(LogIVXunFei, Error, TEXT("%s:%s"), *FString(__FUNCTION__), *ErrorMessage);
		
 		OnConnectionError.Broadcast(0.0f);
 		SetReadyToDestroy();

 	}
}


UIVXunFeiMultiFrameTTSAsyncProxy::UIVXunFeiMultiFrameTTSAsyncProxy(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
	,NumQueue(0)
	,TTSNum(INDEX_NONE)
{
}

UIVXunFeiMultiFrameTTSAsyncProxy* UIVXunFeiMultiFrameTTSAsyncProxy::XunFeiNultiFrameTextToSoundWave(const UObject* WorldContextObject, const TArray<FString>& MessageArray, FXunFeiTTSOptions TTSOptions)
{
	ThisClass* const Proxy = NewObject<ThisClass>();
	
	Proxy->TTSNum=MessageArray.Num();
	for(int32 Index=0;Index<MessageArray.Num();Index++)
	{
		UIVXunFeiTTSWebSocketAsyncProxy* TTSWebSocketAsync=UIVXunFeiTTSWebSocketAsyncProxy::XunFeiTextToSoundWave(WorldContextObject, MessageArray[Index],TTSOptions);
		Proxy->TTSWebSocketAsyncProxyMap.Add(TTSWebSocketAsync->GetUniqueID(),TTSWebSocketAsync);
		Proxy->QueueMap.Add(TTSWebSocketAsync->GetUniqueID(),Index);

			TTSWebSocketAsync->OnCloseC.BindLambda([Proxy,TTSWebSocketAsync](USoundWave* SoundWave, const float& SoundDuration)
			{
				uint32 ID=TTSWebSocketAsync->GetUniqueID();
				Proxy->SoundMap.Add(Proxy->QueueMap.FindChecked(ID),SoundWave);
				Proxy->NumQueue++;
				if(Proxy->NumQueue==Proxy->TTSNum)
				{
					TArray<int32> Keys;
					TArray<USoundWave*> Values;
					Proxy->SoundMap.KeySort([](int32 A, int32 B)
					{
						return A < B;
					});
					for (auto& Elem :Proxy->SoundMap)
					{
						Keys.Add(Elem.Key);
						Values.Add(Elem.Value.Get());
					}

					Proxy->Completed.Broadcast(Keys,Values);
 
				}
			});
	}
	return Proxy;
}

void UIVXunFeiMultiFrameTTSAsyncProxy::Cancel()
{
	for (auto& Elem :TTSWebSocketAsyncProxyMap)
	{
		UIVXunFeiTTSWebSocketAsyncProxy* AsyncProxy=Elem.Value.Get();
		if(AsyncProxy)
		{
			if(UFunFeiSpeechTaskStatus::IsTaskActive(AsyncProxy))
			{
				AsyncProxy->Cancel();
			}
		}
	}
	TArray<int32> Index;
	TArray<USoundWave*> Sounds;
	OnCanceled.Broadcast(Index,Sounds);

	SetReadyToDestroy();
}


bool UFunFeiSpeechTaskStatus::IsTaskActive(const UIVFunFeiSpeechAsyncProxyBase* const Test)
{
	return IsValid(Test) && Test->bIsTaskActive;
}
