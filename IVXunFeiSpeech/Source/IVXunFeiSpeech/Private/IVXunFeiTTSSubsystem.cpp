// Fill out your copyright notice in the Description page of Project Settings.


#include "IVXunFeiTTSSubsystem.h"

#include "AudioCapture.h"
#include "HttpModule.h"
#include "IVXunFeiSpeech.h"
#include "IVXunFeiSpeechTypes.h"
#include "JsonObjectConverter.h"
#include "IWebSocket.h"
#include "WebSocketsModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Voice/IVAudioCaptureSubsystem.h"
#include "Voice/FIVVoiceRunnable.h"

void UIVXunFeiTTSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogIVXunFei, Display, TEXT("%s: IVXunFeiTTS Engine Subsystem initialized."), *FString(__func__));
}

void UIVXunFeiTTSSubsystem::Deinitialize()
{
	UE_LOG(LogIVXunFei, Display, TEXT("%s: IVXunFeiTTS Engine Subsystem deinitialized."), *FString(__func__));
	
	Super::Deinitialize();
}

void UIVXunFeiTTSSubsystem::BeginRealTimeSpeechToText(FXunFeiRealTimeSTTOptions& RealTimeSTTOptions,
	FInitRealTimeSTTDelegate InInitRealTimeSTTDelegate,
	FRealTimeSTTNoTranslateDelegate InRealTimeSTTNoTranslateDelegate,
	FRealTimeSTTTranslateDelegate InRealTimeSTTTranslateDelegate)
{

	UIVAudioCaptureSubsystem* Subsystem = GEngine->GetEngineSubsystem<UIVAudioCaptureSubsystem>();
			
	FAudioCaptureDeviceInfo AudioCaptureDeviceInfo;

	Subsystem->StartCapturingAudio();

	bool bGetAudioDevice = Subsystem->GetAudioCaptureDeviceInfo(AudioCaptureDeviceInfo);

	bool bRightAudioRateAndChannles = AudioCaptureDeviceInfo.SampleRate == 48000 && AudioCaptureDeviceInfo.NumInputChannels == 2;

	bRightAudioRateAndChannles = true;

	if (!bGetAudioDevice || !bRightAudioRateAndChannles)
	{
		FString AudioErrorMessage = TEXT("AudioCaptureDevice is not right!");

		UE_LOG(LogIVXunFei, Error, TEXT("[%s]:[%s]"), *FString(__FUNCTION__), *AudioErrorMessage);

		Subsystem->StopCapturingAudio();

		CallInitRealTimeSTTDelegate(false, AudioErrorMessage);

		StopRealTimeSpeechToText();

		return;
	}
	
	if (ReakTimeSTTStatus != EXunFeiRealTimeSTTStatus::Ready)
	{
		InInitRealTimeSTTDelegate.ExecuteIfBound(false, TEXT("XGXunFeiRealTimeSTT is Running !"));

		UE_LOG(LogIVXunFei, Warning, TEXT("[%s] try to connect to iFLyTek,But It is Running"), *FString(__FUNCTION__));

		return;
	}

	UE_LOG(LogIVXunFei, Display, TEXT("[%s] is trying to connect to iFLyTek"), *FString(__FUNCTION__));

	ReakTimeSTTStatus = EXunFeiRealTimeSTTStatus::Init;

	InitRealTimeSTTDelegate = InInitRealTimeSTTDelegate;
	RealTimeSTTNoTranslateDelegate = InRealTimeSTTNoTranslateDelegate;
	RealTimeSTTTranslateDelegate = InRealTimeSTTTranslateDelegate;

	FModuleManager::Get().LoadModuleChecked("WebSockets");
	FString ServerProtocol = TEXT("ws");
	FString ServerURL = TEXT("ws://rtasr.xfyun.cn/v1/ws?");
	ServerURL += RealTimeSTTOptions.GenerateRequireParams();


	AsyncTask(ENamedThreads::GameThread, [ServerURL, ServerProtocol, this]() {

		Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
		Socket->OnConnected().AddUObject(this, &UIVXunFeiTTSSubsystem::OnRealTimeSTTConnected);
		Socket->OnConnectionError().AddUObject(this, &UIVXunFeiTTSSubsystem::OnRealTimeSTTConnectionError);
		Socket->OnClosed().AddUObject(this, &UIVXunFeiTTSSubsystem::OnRealTimeSTTClosed);
		Socket->OnMessage().AddUObject(this, &UIVXunFeiTTSSubsystem::OnRealTimeSTTMessage);
		Socket->OnMessageSent().AddUObject(this, &UIVXunFeiTTSSubsystem::OnRealTimeSTTMessageSent);

		Socket->Connect();
		});
}

void UIVXunFeiTTSSubsystem::StopRealTimeSpeechToText()
{
	if (ReakTimeSTTStatus == EXunFeiRealTimeSTTStatus::Ready)
	{
		return;
	}
	
	UIVAudioCaptureSubsystem* Subsystem = GEngine->GetEngineSubsystem<UIVAudioCaptureSubsystem>();
	if (Subsystem)
	{
		Subsystem->StopCapturingAudio();
	}

	RealeaseVoiceConsumeRunnable();

	EndRealTimeSTTSendVoiceData();
}

void UIVXunFeiTTSSubsystem::BeginStreamingSpeechToText(FXunFeiStreamingSTTOptions& InStreamingSTTOptions)
{
	StreamingSTTResult.Empty();
	LastStreamingSTTResult.Empty();
	StreamingSTTOptions = InStreamingSTTOptions;
	
	UIVAudioCaptureSubsystem* Subsystem = GEngine->GetEngineSubsystem<UIVAudioCaptureSubsystem>();
			
	FAudioCaptureDeviceInfo AudioCaptureDeviceInfo;
	
	Subsystem->StartCapturingAudio();
	
	bool bGetAudioDevice = Subsystem->GetAudioCaptureDeviceInfo(AudioCaptureDeviceInfo);
	
	bool bRightAudioRateAndChannles = AudioCaptureDeviceInfo.SampleRate == 48000 && AudioCaptureDeviceInfo.NumInputChannels == 2;
	
	bRightAudioRateAndChannles = true;
	
	if (!bGetAudioDevice || !bRightAudioRateAndChannles)
	{
		FString AudioErrorMessage = TEXT("AudioCaptureDevice is not right!");
	
		UE_LOG(LogIVXunFei, Error, TEXT("[%s]:[%s]"), *FString(__FUNCTION__), *AudioErrorMessage);
	
		Subsystem->StopCapturingAudio();
		
		StopStreamingSpeechToText();

		StreamingSTTErrorDelegate.Broadcast(TEXT("AudioCaptureDevice is not right!"));
		return;
	}
	
	if (ReakTimeSTTStatus != EXunFeiRealTimeSTTStatus::Ready)
	{
		UE_LOG(LogIVXunFei, Error, TEXT("[%s] Xun fei streaming STT is Running, it cannot concurrency!"), *FString(__FUNCTION__));
		
		StreamingSTTErrorDelegate.Broadcast(TEXT("XunFei Streaming TTS is Running, it cannot concurrency!"));
		return;
	}
	
	UE_LOG(LogIVXunFei, Display, TEXT("[%s] Try to connect to xun fei server"), *FString(__FUNCTION__));
	
	ReakTimeSTTStatus = EXunFeiRealTimeSTTStatus::Init;

	
	
	FModuleManager::Get().LoadModuleChecked("WebSockets");
	FString ServerProtocol = TEXT("ws");
	FString StreamingSTTHost;
	if (TEXT("zh_cn") == InStreamingSTTOptions.Language || TEXT("en_us") == InStreamingSTTOptions.Language)
	{
		StreamingSTTHost = TEXT("iat-api.xfyun.cn");
	}
	// else if (TEXT("en_us") == InStreamingSTTOptions.Language)
	// {
	// 	StreamingSTTHost = TEXT("ws-api.xfyun.cn");
	// }
	else
	{
		StreamingSTTHost = TEXT("iat-niche-api.xfyun.cn");
	}
	FString StreamingSTTPath = TEXT("/v2/iat");
	FString ServerURL = StreamingSTTOptions.GenerateURL(StreamingSTTHost, StreamingSTTPath);
	
	AsyncTask(ENamedThreads::GameThread, [ServerURL, ServerProtocol, this]() {

		Socket = FWebSocketsModule::Get().CreateWebSocket(ServerURL, ServerProtocol);
		Socket->OnConnected().AddUObject(this, &UIVXunFeiTTSSubsystem::OnStreamingSTTConnected);
		Socket->OnConnectionError().AddUObject(this, &UIVXunFeiTTSSubsystem::OnStreamingSTTConnectionError);
		Socket->OnClosed().AddUObject(this, &UIVXunFeiTTSSubsystem::OnStreamingSTTClosed);
		Socket->OnMessage().AddUObject(this, &UIVXunFeiTTSSubsystem::OnStreamingSTTMessage);
		Socket->OnMessageSent().AddUObject(this, &UIVXunFeiTTSSubsystem::OnStreamingSTTMessageSent);

		Socket->Connect();
		});
}

void UIVXunFeiTTSSubsystem::StopStreamingSpeechToText()
{
	if (ReakTimeSTTStatus == EXunFeiRealTimeSTTStatus::Ready)
	{
		return;
	}
	
	UIVAudioCaptureSubsystem* Subsystem = GEngine->GetEngineSubsystem<UIVAudioCaptureSubsystem>();
	if (Subsystem)
	{
		Subsystem->StopCapturingAudio();
	}

	RealeaseVoiceConsumeRunnable();

	EndStreamingSTTSendVoiceData();
}

void UIVXunFeiTTSSubsystem::BeginTranslate(const FString& SourceText, FXunFeiTranslateOptions& XunFeiTranslateOptions, FTranslateCompletedDelegate InTranslateCompletedDelegate)
{
	TranslateCompletedDelegate = InTranslateCompletedDelegate;
	
	if(SourceText.IsEmpty())
	{
		UE_LOG(LogIVXunFei, Error, TEXT("[%s]: Xun fei translate source text is empty!"), *FString(__FUNCTION__));
		TranslateCompletedDelegate.ExecuteIfBound(false, FString(TEXT("Xun fei translate source text is empty!")));
		return;
	}
	
	// Base64编码
	auto SourceTextUTF8 = StringCast<UTF8CHAR>(*SourceText);
	FString SourceTextBase64 = FBase64::Encode((uint8*)SourceTextUTF8.Get(), SourceTextUTF8.Length());
	XunFeiTranslateOptions.PayloadText = SourceTextBase64;
	
	UE_LOG(LogIVXunFei, Display, TEXT("[%s]: Xun Fei translate start connect."), *FString(__FUNCTION__));

	// 创建一个请求对象
	FHttpModule* HttpModule = static_cast<FHttpModule*>(FModuleManager::Get().GetModule("HTTP"));
	HttpRequest = HttpModule->CreateRequest();
	
	// Http请求的默认超时为300秒, 目前150秒超时
	HttpRequest->SetTimeout(150);

	FString Host = TEXT("itrans.xf-yun.com");
	FString Path = TEXT("/v1/its");
	FString URL = XunFeiTranslateOptions.GenerateURL(Host, Path);
	// 设置请求URL
	HttpRequest->SetURL(URL);
	// 设置请求方法
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader("Content-Type", "application/json");
	
	// 设置请求内容
	FString RequestBody = XunFeiTranslateOptions.GenerateRequireParams();
	HttpRequest->SetContentAsString(RequestBody);
	UE_LOG(LogIVXunFei, Display, TEXT("[%s]: request body : %s"), *FString(__FUNCTION__), *RequestBody);

	// 绑定流式委托
	// HttpRequest->OnRequestWillRetry().BindUObject(this, &UIVXunFeiTTSSubsystem::OnRequestWillRetry);
	// HttpRequest->OnRequestProgress().BindUObject(this, &UIVXunFeiTTSSubsystem::OnRequestProgress);
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UIVXunFeiTTSSubsystem::OnTranslateCompleted);
			
	//发送请求
	HttpRequest->ProcessRequest();
}

void UIVXunFeiTTSSubsystem::StopTranslate()
{
	if(!HttpRequest.IsValid())
	{
		return;
	}
	HttpRequest->CancelRequest();
}

void UIVXunFeiTTSSubsystem::OnRealTimeSTTConnected()
{
	UE_LOG(LogIVXunFei, Log, TEXT("%s:Connect Succeess!"), *FString(__FUNCTION__));
}

void UIVXunFeiTTSSubsystem::OnRealTimeSTTConnectionError(const FString& Error)
{
	UE_LOG(LogIVXunFei, Error, TEXT("%s:ConnectError,Message:[%s]!"), *FString(__FUNCTION__), *Error);
	
	FString Message = FString(__FUNCTION__) + TEXT("-ConnectError,Message:") + Error;

	StopRealTimeSpeechToText();

	ReakTimeSTTStatus = EXunFeiRealTimeSTTStatus::Ready;
	
	CallInitRealTimeSTTDelegate(false, Message);
}

void UIVXunFeiTTSSubsystem::OnRealTimeSTTClosed(const int32 Status, const FString& Reason, const bool bWasClean)
{
	UE_LOG(LogIVXunFei, Log, TEXT("[%s]:ConnectOnClosed,StatusCode:[%d],Reason:[%s],bWasClean:[%s]!"),
		*FString(__FUNCTION__), Status, *Reason, bWasClean ? TEXT("true") : TEXT("false"));

	StopRealTimeSpeechToText();

	ReakTimeSTTStatus = EXunFeiRealTimeSTTStatus::Ready;
}

void UIVXunFeiTTSSubsystem::OnRealTimeSTTMessage(const FString& Message)
{
	FXunFeiRealTImeSTTRespInfo RealTImeSTTRespInfo;
	if (!Message.IsEmpty() && FJsonObjectConverter::JsonObjectStringToUStruct(Message, &RealTImeSTTRespInfo))
	{
		UE_LOG(LogTemp,Warning,TEXT("%s"),*Message);
		
		if (RealTImeSTTRespInfo.action.Equals(TEXT("started")))
		{
			if (ReakTimeSTTStatus != EXunFeiRealTimeSTTStatus::Init)
			{
				FString StartedErroeMessage = TEXT("[%s]:Unknow Procedure Error");
	
				UE_LOG(LogIVXunFei, Error, TEXT("[%s]:[%s]"), *FString(__FUNCTION__), *StartedErroeMessage);
	
				CallInitRealTimeSTTDelegate(false, StartedErroeMessage);
	
				StopRealTimeSpeechToText();
	
				return;
	
			}
	
			ReakTimeSTTStatus = EXunFeiRealTimeSTTStatus::Processing;
			
			VoiceRunnable = MakeShared<FIVVoiceRunnable>(TEXT("XunFeiVoiceRunnable"));
			FRunnableThread* RunnableThread = FRunnableThread::Create(VoiceRunnable.Get(), *(VoiceRunnable->ThreadName));
			VoiceRunnable->OnAudioBufferResult.BindUObject(this,&UIVXunFeiTTSSubsystem::SendRealTimeSTTVoiceData);
			FString InitMessage = TEXT("XunFeiRealTimeSTT Init Success !");
	
			CallInitRealTimeSTTDelegate(true, InitMessage);
	
			UE_LOG(LogIVXunFei, Display, TEXT("[%s]:[%s]"), *FString(__FUNCTION__), *InitMessage);
	
		}
		else if (RealTImeSTTRespInfo.action.Equals(TEXT("error")))
		{
			UE_LOG(LogIVXunFei, Error, TEXT("[%s]--Error--[%s]"), *FString(__FUNCTION__), *(RealTImeSTTRespInfo.desc));
	
			CallInitRealTimeSTTDelegate(false, *(RealTImeSTTRespInfo.desc));
	
			StopRealTimeSpeechToText();
	
			return;
	
		}
		{
			TSharedPtr<FJsonObject> ResultDataObj;
			TSharedRef<TJsonReader<>> DataReader = TJsonReaderFactory<>::Create(RealTImeSTTRespInfo.data);
			FJsonSerializer::Deserialize(DataReader, ResultDataObj);
	
			bool bFindDiz = ResultDataObj->HasField(TEXT("biz"));
	
			if (bFindDiz)
			{
				FXunFeiRealTimeSTTTranslateData RealTimeSTTTranslateData;
				if (FJsonObjectConverter::JsonObjectStringToUStruct(RealTImeSTTRespInfo.data, &RealTimeSTTTranslateData))
				{
					if (RealTimeSTTTranslateData.ed > 0)
					{
						FString dst = RealTimeSTTTranslateData.dst;
						FString src = RealTimeSTTTranslateData.src;
	
						UE_LOG(LogIVXunFei, Log, TEXT("[%s]: [dst]-[%s]||[src]-[%s]"), *FString(__FUNCTION__), *dst, *src);
	
						CallRealTimeSTTTranslateDelegate(src, dst);
	
	
					}
				}
	
			}
			else
			{
	
				FXunFeiRealTimeSTTNoTranslateData RealTimeSTTNoTranslateData;
				if (FJsonObjectConverter::JsonObjectStringToUStruct(RealTImeSTTRespInfo.data, &RealTimeSTTNoTranslateData))
				{
	
					bool bFinal = !RealTimeSTTNoTranslateData.cn.st.ed.Equals(TEXT("0"));
	
					if (bFinal)
					{
						TArray<FXunFeiRealTimeSTTNoTranslateWSData> WSDatas = RealTimeSTTNoTranslateData.cn.st.rt[0].ws;
	
						FString OutText;
	
						for (auto& TmpWs : WSDatas)
						{
							for (auto& TmpCW : TmpWs.cw)
							{
								OutText += TmpCW.w;
							}
						}
	
						UE_LOG(LogIVXunFei, Log, TEXT("[%s]:[src]-[%s] "), *FString(__FUNCTION__), *OutText);
						CallRealTimeSTTNoTranslateDelegate(OutText);
					}
	
	
				}
	
			}
	
		}
	
	}
}

void UIVXunFeiTTSSubsystem::OnRealTimeSTTMessageSent(const FString& Message)
{
	UE_LOG(LogIVXunFei, Warning, TEXT("%s:Send Message : [%s]!"), *FString(__FUNCTION__), *Message);
}

void UIVXunFeiTTSSubsystem::OnStreamingSTTConnected()
{
	UE_LOG(LogIVXunFei, Warning, TEXT("%s: Xun fei streaming STT connect succeess."), *FString(__FUNCTION__));
	if (ReakTimeSTTStatus == EXunFeiRealTimeSTTStatus::Init)
	{
		ReakTimeSTTStatus = EXunFeiRealTimeSTTStatus::Processing;
		
		StreamingSTTOptions.Status = 0;
		StreamingSTTOptions.Audio = FString();
		FString RequireParams = StreamingSTTOptions.GenerateRequireParams();
		{
			FScopeLock Lock(&SocketCriticalSection);
			if (Socket.IsValid() && Socket->IsConnected())
			{
				Socket->Send(RequireParams);
			}
		}
		
		VoiceRunnable = MakeShared<FIVVoiceRunnable>(TEXT("XunFeiVoiceRunnable"));
		FRunnableThread* RunnableThread = FRunnableThread::Create(VoiceRunnable.Get(), *(VoiceRunnable->ThreadName));
		VoiceRunnable->OnAudioBufferResult.BindUObject(this,&UIVXunFeiTTSSubsystem::SendStreamingSTTVoiceData);
		
		StreamingSTTInitdDelegate.Broadcast();
	}
}

void UIVXunFeiTTSSubsystem::OnStreamingSTTConnectionError(const FString& Error)
{
	UE_LOG(LogIVXunFei, Error, TEXT("%s:Websocket connect error, message : [%s]!"), *FString(__FUNCTION__), *Error);
	StreamingSTTErrorDelegate.Broadcast(Error);
	
	StopStreamingSpeechToText();
}

void UIVXunFeiTTSSubsystem::OnStreamingSTTClosed(const int32 Status, const FString& Reason, const bool bWasClean)
{
	UE_LOG(LogIVXunFei, Log, TEXT("[%s]: Websocket closed, Status : [%d], Reason : [%s], bWasClean:[%s]!"),
		*FString(__FUNCTION__), Status, *Reason, bWasClean ? TEXT("true") : TEXT("false"));

	StopStreamingSpeechToText();
}

void UIVXunFeiTTSSubsystem::OnStreamingSTTMessage(const FString& Message)
{
	if(ReakTimeSTTStatus != EXunFeiRealTimeSTTStatus::Processing)
	{
		UE_LOG(LogIVXunFei, Error, TEXT("%s: Xun fei streaming STT connect exception!"), *FString(__FUNCTION__));
		StreamingSTTErrorDelegate.Broadcast(TEXT("Xun fei streaming STT connect exception!"));
		return;
	}

	FString OnceResult;
	int32 ResponseCode = 0;
	FString ResponseMessage;
	int32 ResponseStatus = 0;
	FString ResponsePgs;
	bool ResponseLs = false;
	
	TSharedPtr<FJsonObject> RootJsonObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*Message);
	if (FJsonSerializer::Deserialize(Reader, RootJsonObj) && RootJsonObj.IsValid())
	{
		ResponseCode = RootJsonObj->GetIntegerField(TEXT("code"));
		ResponseMessage = RootJsonObj->GetStringField(TEXT("message"));
		TSharedPtr<FJsonObject> DataJsonObject = RootJsonObj->GetObjectField(TEXT("data"));
		ResponseStatus = DataJsonObject->GetIntegerField(TEXT("status"));
		TSharedPtr<FJsonObject> ResultJsonObject =  DataJsonObject->GetObjectField(TEXT("result"));
		ResponsePgs = ResultJsonObject->GetStringField(TEXT("pgs"));
		ResponseLs = ResultJsonObject->GetBoolField(TEXT("ls"));
		
		if(0 != ResponseCode)
		{
			UE_LOG(LogIVXunFei, Error, TEXT("%s: Response error, error code : [%d], reason : [%s]!"), *FString(__FUNCTION__), ResponseCode, *ResponseMessage);
			StreamingSTTErrorDelegate.Broadcast(ResponseMessage);
			StopStreamingSpeechToText();
		}
		
		TArray<TSharedPtr<FJsonValue>> WsJsonValues = ResultJsonObject->GetArrayField(TEXT("ws"));
		for (TSharedPtr<FJsonValue> WsJsonValue : WsJsonValues)
		{
			TSharedPtr<FJsonObject> WsJsonObject = WsJsonValue->AsObject();
			TArray<TSharedPtr<FJsonValue>> CwJsonValues = WsJsonObject->GetArrayField(TEXT("cw"));
			// for (TSharedPtr<FJsonValue> CwJsonValue : CwJsonValues)
			// {
			// 	TSharedPtr<FJsonObject> CwJsonObject = CwJsonValue->AsObject();
			// 	CwJsonObject->GetStringField(TEXT("w"));
			// }
			if(CwJsonValues.IsEmpty())
			{
				break;
			}
			TSharedPtr<FJsonObject> CwJsonObject = CwJsonValues[0]->AsObject();
			FString CwString = CwJsonObject->GetStringField(TEXT("w"));

			
			// 拼接
			if("rpl" == ResponsePgs)
			{
				OnceResult += CwString;
				LastStreamingSTTResult = OnceResult;
			}
			else if("apd" == ResponsePgs && 0 == ResponseStatus)
			{
				StreamingSTTResult.Empty();
				LastStreamingSTTResult.Empty();
			}
			else if("apd" == ResponsePgs && 1 == ResponseStatus)
			{
				StreamingSTTResult += LastStreamingSTTResult;
				LastStreamingSTTResult.Empty();
			}
			else if("apd" == ResponsePgs && 2 == ResponseStatus)
			{
				LastStreamingSTTResult += CwString;
				StreamingSTTResult += LastStreamingSTTResult;
				LastStreamingSTTResult.Empty();
			}
		}
	}
	
	// 为了展示动态修正的功能
	FString ShowResult = StreamingSTTResult + LastStreamingSTTResult;
	UE_LOG(LogIVXunFei, Display, TEXT("%s: code : [%d], message : [%s], status : [%d], pgs : [%s], ls : [%s], once result : [%s],\n result : [%s]."), *FString(__FUNCTION__), ResponseCode, *ResponseMessage, ResponseStatus, *ResponsePgs, ResponseLs?TEXT("true"):TEXT("false"), *OnceResult, *ShowResult);
	StreamingSTTResponseDelegate.Broadcast(ShowResult);

	// 结束标识
	if(2 == ResponseStatus)
	{
		StopStreamingSpeechToText();
	}
}

void UIVXunFeiTTSSubsystem::OnStreamingSTTMessageSent(const FString& Message)
{
}

void UIVXunFeiTTSSubsystem::OnTranslateCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if(!bConnectedSuccessfully || !Request || !Request.IsValid() || !Response || !Response.IsValid())
	{
		UE_LOG(LogIVXunFei, Error, TEXT("[%s]: HTTP connect error!"), *FString(__FUNCTION__));
		TranslateCompletedDelegate.ExecuteIfBound(false, FString(TEXT("HTTP connect error!")));
		return;
	}
	UE_LOG(LogIVXunFei, Display, TEXT("[%s]: Response completed elapsed time : %f seconds"), *FString(__FUNCTION__), Request->GetElapsedTime());


	// 解析
	FString Result = Response->GetContentAsString();
	
	int32 ResultCode = 0;
	FString ResultMessage;
	FString ResultSid;
	
	FString ResultSeq;
	FString ResultStatus;
	FString ResultText;
	
	TSharedPtr<FJsonObject> RootJsonObj;
	TSharedRef<TJsonReader<>> RootJsonObjReader = TJsonReaderFactory<>::Create(*Result);
	if (!FJsonSerializer::Deserialize(RootJsonObjReader, RootJsonObj) || !RootJsonObj.IsValid())
	{
		UE_LOG(LogIVXunFei, Error, TEXT("[%s]: Xun fei translate result parsing to json failed, result: %s!"), *FString(__FUNCTION__), *Result);
		TranslateCompletedDelegate.ExecuteIfBound(false, FString(TEXT("Xun fei translate result parsing to json failed!")));
		return;
	}
	
	TSharedPtr<FJsonObject> HeaderJsonObject = RootJsonObj->GetObjectField(TEXT("header"));
	ResultCode = HeaderJsonObject->GetIntegerField(TEXT("code"));
	ResultMessage = HeaderJsonObject->GetStringField(TEXT("message"));
	ResultSid = HeaderJsonObject->GetStringField(TEXT("sid"));
	if(ResultCode != 0)
	{
		UE_LOG(LogIVXunFei, Error, TEXT("[%s]: Xun fei translate request error, code: %d, message: %s!"), *FString(__FUNCTION__), ResultCode, *ResultMessage);
		TranslateCompletedDelegate.ExecuteIfBound(false, FString(TEXT("Xun fei translate request error!")));
		return;
	}

	TSharedPtr<FJsonObject> PayloadJsonObject = RootJsonObj->GetObjectField(TEXT("payload"));
	TSharedPtr<FJsonObject> ResultJsonObject = PayloadJsonObject->GetObjectField(TEXT("result"));
	ResultSeq = ResultJsonObject->GetStringField(TEXT("seq"));
	ResultStatus = ResultJsonObject->GetStringField(TEXT("status"));
	ResultText = ResultJsonObject->GetStringField(TEXT("text"));
	
	if(ResultText.IsEmpty())
	{
		UE_LOG(LogIVXunFei, Error, TEXT("[%s]: Xun fei translate result is empty!"), *FString(__FUNCTION__));
		TranslateCompletedDelegate.ExecuteIfBound(false, FString(TEXT("Xun fei translate result is empty!")));
		return;
	}

	// Base64解码
	TArray<uint8> DecodeResultArray;
	FBase64::Decode(ResultText, DecodeResultArray);
	FString DecodeResultText = UTF8_TO_TCHAR(DecodeResultArray.GetData());
	UE_LOG(LogIVXunFei, Display, TEXT("[%s]: DecodeResultText : %s"), *FString(__FUNCTION__), *DecodeResultText);
	// 保证是Json格式的字符串且能被正常解析
	int32 SubStrIndex = DecodeResultText.Find(TEXT("\"}}"), ESearchCase::IgnoreCase, ESearchDir::FromEnd, -1);
	FString DecodeResultJsonText = DecodeResultText.Left(SubStrIndex + 3);
	UE_LOG(LogIVXunFei, Display, TEXT("[%s]: DecodeResultJsonText : %s"), *FString(__FUNCTION__), *DecodeResultJsonText);

	// 解析
	TSharedPtr<FJsonObject> DecodeResultTextJsonObj;
	TSharedRef<TJsonReader<>> DecodeResultTextJsonObjReader = TJsonReaderFactory<>::Create(*DecodeResultJsonText);
	if (!FJsonSerializer::Deserialize(DecodeResultTextJsonObjReader, DecodeResultTextJsonObj) || !DecodeResultTextJsonObj.IsValid())
	{
		UE_LOG(LogIVXunFei, Error, TEXT("[%s]: Xun fei translate decoded result text parsing to json failed, result: %s!"), *FString(__FUNCTION__), *DecodeResultJsonText);
		TranslateCompletedDelegate.ExecuteIfBound(false, FString(TEXT("Xun fei translate decoded result text parsing to json failed!")));
		return;
	}
	FString DecodeResultTextFrom;
	FString DecodeResultTextTo;
	FString DecodeResultTextSrc;
	FString DecodeResultTextDst;
	DecodeResultTextFrom = DecodeResultTextJsonObj->GetStringField(TEXT("from"));
	DecodeResultTextTo = DecodeResultTextJsonObj->GetStringField(TEXT("to"));
	TSharedPtr<FJsonObject> TransResultJsonObject = DecodeResultTextJsonObj->GetObjectField(TEXT("trans_result"));
	DecodeResultTextSrc = TransResultJsonObject->GetStringField(TEXT("src"));
	DecodeResultTextDst = TransResultJsonObject->GetStringField(TEXT("dst"));

	if(DecodeResultTextFrom.IsEmpty() || DecodeResultTextTo.IsEmpty() || DecodeResultTextSrc.IsEmpty() || DecodeResultTextDst.IsEmpty())
	{
		UE_LOG(LogIVXunFei, Error, TEXT("[%s]: Xun fei translate decoded result text json has empty field!"), *FString(__FUNCTION__));
		TranslateCompletedDelegate.ExecuteIfBound(false, FString(TEXT("Xun fei translate decoded result text json has empty field!")));
		return;
	}

	TranslateCompletedDelegate.ExecuteIfBound(true, DecodeResultTextDst);
	UE_LOG(LogIVXunFei, Display, TEXT("[%s]: Xun fei translate %s -> %s!"), *FString(__FUNCTION__), *DecodeResultTextSrc, *DecodeResultTextDst);
}

void UIVXunFeiTTSSubsystem::CallInitRealTimeSTTDelegate(bool bInitResult, FString InitMessage)
{
	FInitRealTimeSTTDelegate TempRealTimeSTTDelegate = InitRealTimeSTTDelegate;

	AsyncTask(ENamedThreads::GameThread, [=]() {

		TempRealTimeSTTDelegate.ExecuteIfBound(bInitResult, InitMessage);

		});
}

void UIVXunFeiTTSSubsystem::CallRealTimeSTTNoTranslateDelegate(FString InSrcText)
{
	FRealTimeSTTNoTranslateDelegate TempRealTimeSTTNoTranslateDelegate = RealTimeSTTNoTranslateDelegate;

	AsyncTask(ENamedThreads::GameThread, [=]() {

		TempRealTimeSTTNoTranslateDelegate.ExecuteIfBound(InSrcText);

		});
}

void UIVXunFeiTTSSubsystem::CallRealTimeSTTTranslateDelegate(FString InSrcText, FString InDstText)
{
	FRealTimeSTTTranslateDelegate TempRealTimeSTTTranslateDelegate = RealTimeSTTTranslateDelegate;

	AsyncTask(ENamedThreads::GameThread, [=]() {

		TempRealTimeSTTTranslateDelegate.ExecuteIfBound(InSrcText, InDstText);

		});
}

void UIVXunFeiTTSSubsystem::SendRealTimeSTTVoiceData(TArray<float>& InVoiceData)
{
	TArray<int16> ToChangeAuidoData;
	TArray<uint8> BinaryDataToSend;

	for (int32 VoiceIndex = 0; VoiceIndex < 1024; VoiceIndex++)
	{
		ToChangeAuidoData.Add((int16)FMath::Clamp<int32>(FMath::FloorToInt(32767.0f * InVoiceData[VoiceIndex]), -32768, 32767));
		uint8 Bytes[2];
		Bytes[0] = (uint8)(ToChangeAuidoData[VoiceIndex] & 0xFF);
		Bytes[1] = (uint8)((ToChangeAuidoData[VoiceIndex] >> 8) & 0xFF);
		BinaryDataToSend.Add(Bytes[0]);
		BinaryDataToSend.Add(Bytes[1]);
	}

	FScopeLock Lock(&SocketCriticalSection);

	if (Socket.IsValid() && Socket->IsConnected())
	{
		Socket->Send(BinaryDataToSend.GetData(), BinaryDataToSend.Num(), true);
	}
}

void UIVXunFeiTTSSubsystem::SendStreamingSTTVoiceData(TArray<float>& InVoiceData)
{
	TArray<int16> ToChangeAuidoData;
	TArray<uint8> BinaryDataToSend;

	for (int32 VoiceIndex = 0; VoiceIndex < 1024; VoiceIndex++)
	{
		ToChangeAuidoData.Add((int16)FMath::Clamp<int32>(FMath::FloorToInt(32767.0f * InVoiceData[VoiceIndex]), -32768, 32767));
		uint8 Bytes[2];
		Bytes[0] = (uint8)(ToChangeAuidoData[VoiceIndex] & 0xFF);
		Bytes[1] = (uint8)((ToChangeAuidoData[VoiceIndex] >> 8) & 0xFF);
		BinaryDataToSend.Add(Bytes[0]);
		BinaryDataToSend.Add(Bytes[1]);
	}

	// FString AudioContent = UTF8_TO_TCHAR(reinterpret_cast<const char*>(BinaryDataToSend.GetData()));
	// uint32 Length = FCStringAnsi::Strlen(TCHAR_TO_UTF8(*AudioContent));
	FString AudioContentBase64 = FBase64::Encode(BinaryDataToSend);
	{
		FScopeLock Lock(&SocketCriticalSection);
		if (Socket.IsValid() && Socket->IsConnected())
		{
			StreamingSTTOptions.Status = 1;
			StreamingSTTOptions.Audio = AudioContentBase64;
			FString RequireParams = StreamingSTTOptions.GenerateRequireParams();
			Socket->Send(RequireParams);
		}
	}
}

void UIVXunFeiTTSSubsystem::RealeaseVoiceConsumeRunnable()
{
	if (VoiceRunnable.IsValid())
	{
		VoiceRunnable->Stop();
	}
}

void UIVXunFeiTTSSubsystem::EndRealTimeSTTSendVoiceData()
{
	FScopeLock Lock(&SocketCriticalSection);

	if (Socket.IsValid() && Socket->IsConnected())
	{
		FString EndStr = TEXT("{\"end\": true}");

		const char* CharValue = TCHAR_TO_UTF8(*EndStr);

		int32 Length = strlen(CharValue);

		Socket->Send(CharValue, Length, true);
		Socket->Close();
		Socket.Reset();
	}
}

void UIVXunFeiTTSSubsystem::EndStreamingSTTSendVoiceData()
{
	{
		FScopeLock Lock(&SocketCriticalSection);
		if (Socket.IsValid() && Socket->IsConnected())
		{
			FString CloseString = TEXT("{\"data\":{\"status\":2}");
			Socket->Send(CloseString);
			Socket->Close();
			Socket.Reset();
		}
	}

	ReakTimeSTTStatus = EXunFeiRealTimeSTTStatus::Ready;
	FString FinalResult = StreamingSTTResult + LastStreamingSTTResult;
	StreamingSTTCompletedDelegate.Broadcast(FinalResult);
}
