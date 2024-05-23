// // Fill out your copyright notice in the Description page of Project Settings.
//
//
// #include "WebUITxt2ImgAsyncAction.h"
// #include "HttpModule.h"
// #include "IImageWrapper.h"
// #include "IImageWrapperModule.h"
// #include "JsonObjectConverter.h"
// #include "Engine/Texture2DDynamic.h"
// #include "Interfaces/IHttpResponse.h"
// #include "Json/IVJsonLibrary.h"
//
// UWebUITxt2ImgAsyncAction::UWebUITxt2ImgAsyncAction(const FObjectInitializer& ObjectInitializer)
// 	: Super(ObjectInitializer)
// {
// }
//
// UWebUITxt2ImgAsyncAction* UWebUITxt2ImgAsyncAction::Txt2ImgWithParamsFile(const UObject* WorldContextObject, const FString& InPrompt, const FString& InNegativePrompt, const FString& InParamsFilePath, const FString& InHostOrIP, int32 InPort)
// {
// 	if(InHostOrIP.IsEmpty() || InPort < 0 || InParamsFilePath.IsEmpty() || InPrompt.IsEmpty())
// 	{
// 		UE_LOG(StableDiffusionHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: The input is null or invalid!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// 		return nullptr;
// 	}
//
// 	if (!FPaths::FileExists(InParamsFilePath))
// 	{
// 		UE_LOG(StableDiffusionHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: Cannot find %s!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *InParamsFilePath);
// 		return nullptr;
// 	}
//
// 	// To FString
// 	FString FileContents;
// 	if(!FFileHelper::LoadFileToString(FileContents,*InParamsFilePath))
// 	{
// 		UE_LOG(StableDiffusionHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: Load %s to string error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *InParamsFilePath);
// 		return nullptr;
// 	}
//
// 	// To FJsonObject
// 	TSharedPtr<FJsonObject> RootJsonObject;
// 	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(FileContents);
// 	if(!FJsonSerializer::Deserialize(JsonReader, RootJsonObject))
// 	{
// 		UE_LOG(StableDiffusionHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: Failed to deserialize JSON string!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// 		return nullptr;
// 	}
// 	RootJsonObject->SetStringField(TEXT("prompt"), InPrompt);
// 	RootJsonObject->SetStringField(TEXT("negative_prompt"), InNegativePrompt);
//
// 	// To FString
// 	FString ConvertJsonString;
// 	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ConvertJsonString);
// 	if(!FJsonSerializer::Serialize(RootJsonObject.ToSharedRef(), Writer))
// 	{
// 		UE_LOG(StableDiffusionHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: Failed to Serialize JSON string!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// 		return nullptr;
// 	}
// 	
// 	FString ConvertJsonStringClear = ConvertJsonString.Replace(TEXT("\n"), TEXT(""));
// 	ConvertJsonStringClear = ConvertJsonStringClear.Replace(TEXT("\r"), TEXT(""));
// 	ConvertJsonStringClear = ConvertJsonStringClear.Replace(TEXT("\t"), TEXT(""));
//
// 	return Txt2ImgWithJsonString(WorldContextObject, ConvertJsonStringClear, InHostOrIP, InPort);
// }
//
// // UTxt2ImgAsyncAction* UTxt2ImgAsyncAction::Txt2ImgWithStruct(const UObject* WorldContextObject, const FTxt2ImgPayload& InJsonStruct, const FString& InHostOrIP, int32 InPort)
// // {
// // 	if(InHostOrIP.IsEmpty() || InPort < 0)
// // 	{
// // 		UE_LOG(StableDiffusionHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: The input is null or invalid!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// // 		return nullptr;
// // 	}
// // 	
// // 	FString ConvertJsonString;
// // 	bool bConverterSuccess = FJsonObjectConverter::UStructToJsonObjectString(InJsonStruct,ConvertJsonString);
// //
// // 	if(!bConverterSuccess)
// // 	{
// // 		UE_LOG(StableDiffusionHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: UStruct convert to json string error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// // 		return nullptr;
// // 	}
// // 	
// // 	FString ConvertJsonStringClear = ConvertJsonString.Replace(TEXT("\n"), TEXT(""));
// // 	ConvertJsonStringClear = ConvertJsonStringClear.Replace(TEXT("\r"), TEXT(""));
// // 	ConvertJsonStringClear = ConvertJsonStringClear.Replace(TEXT("\t"), TEXT(""));
// // 	return Txt2ImgWithJsonString(WorldContextObject, ConvertJsonStringClear, InHostOrIP, InPort);
// // }
//
// UWebUITxt2ImgAsyncAction* UWebUITxt2ImgAsyncAction::Txt2ImgWithJsonString(const UObject* WorldContextObject, const FString& InJsonString, const FString& InHostOrIP, int32 InPort)
// {
// 	if(InHostOrIP.IsEmpty() || InPort < 0 || InJsonString.IsEmpty())
// 	{
// 		UE_LOG(StableDiffusionHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: The input is null or invalid!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// 		return nullptr;
// 	}
// 	
// 	UWebUITxt2ImgAsyncAction* Txt2ImgAsyncAction = NewObject<UWebUITxt2ImgAsyncAction>();
// 	//http://127.0.0.1:7860/sdapi/v1/txt2img
// 	Txt2ImgAsyncAction->Url = FString::Format(TEXT("http://{0}:{1}/sdapi/v1/txt2img"), TArray<FStringFormatArg>({InHostOrIP, InPort}));
// 	Txt2ImgAsyncAction->Txt2ImgInternal(WorldContextObject, Txt2ImgAsyncAction->Url, InJsonString);
// 	
// 	return Txt2ImgAsyncAction;
// }
//
// void UWebUITxt2ImgAsyncAction::Txt2ImgInternal(const UObject* WorldContextObject, const FString& InUrl, const FString& InPayload)
// {
// 	if(InUrl.IsEmpty() || InPayload.IsEmpty())
// 	{
// 		UE_LOG(StableDiffusionHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: The input is null or invalid!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// 		return ;
// 	}
// 	
// 	this->RegisterWithGameInstance(WorldContextObject);
// 	
// 	HttpRequest = FHttpModule::Get().CreateRequest();
// 	HttpRequest->SetURL(InUrl);
// 	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));
// 	HttpRequest->SetVerb(TEXT("POST"));
// 	HttpRequest->SetContentAsString(InPayload);
// 	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UWebUITxt2ImgAsyncAction::OnTxt2ImgCompleted);
// 	HttpRequest->ProcessRequest();
// }
//
// void UWebUITxt2ImgAsyncAction::OnTxt2ImgCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
// {
// 	// if (bConnectedSuccessfully && Response.IsValid() && Response->GetResponseCode() == 200 && Response->GetContentLength() > 0)
// 	if(!Request || !Request.IsValid() || !Response || !Response.IsValid() || !bConnectedSuccessfully)
// 	{
// 		UE_LOG(StableDiffusionHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: %s connect error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__), *Url);
// 		ErrorDelegate.Broadcast(FString("Http connect error!"), nullptr);
// 		return;
// 	}
//
// 	FString JsonStr = Response->GetContentAsString();
// 	TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create(JsonStr);
// 	TSharedPtr<FJsonValue> CurrentJsonValue;
// 	bool bParseSuccess = FJsonSerializer::Deserialize(JsonReader, CurrentJsonValue);
// 	if(!bParseSuccess)
// 	{
// 		UE_LOG(StableDiffusionHTTPAsyncAction, Error, TEXT("ThreadID:%d, %s: Parsing response string error!"), FPlatformTLS::GetCurrentThreadId(), *FString(__FUNCTION__));
// 		ErrorDelegate.Broadcast(FString("Parsing response string error!"), nullptr);
// 		return;
// 	}
// 	
// 	TArray<TSharedPtr<FJsonValue>> ImageJsonObjectArray = CurrentJsonValue->AsObject()->GetArrayField(
// 		TEXT("images"));
//
// 	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(
// 		FName("ImageWrapper"));
// 	TSharedPtr<IImageWrapper> ImageWrappers[3] =
// 	{
// 		ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG),
// 		ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG),
// 		ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP),
// 	};
//
// 	for (int j = 0; j < ImageJsonObjectArray.Num(); j++)
// 	{
// 		auto CurrentImage = ImageJsonObjectArray[j]->AsString();
// 		TArray<uint8> CurrentImageBytes;
// 		FBase64::Decode(CurrentImage, CurrentImageBytes);
// 		for (auto ImageWrapper : ImageWrappers)
// 		{
// 			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(
// 				CurrentImageBytes.GetData(), CurrentImageBytes.GetAllocatedSize()))
// 			{
// 				TArray64<uint8> RawData;
// 				const ERGBFormat InFormat = ERGBFormat::BGRA;
// 				if (ImageWrapper->GetRaw(InFormat, 8, RawData))
// 				{
// 					if (UTexture2DDynamic* Texture = UTexture2DDynamic::Create(
// 						ImageWrapper->GetWidth(), ImageWrapper->GetHeight()))
// 					{
// 						Texture->SRGB = true;
// 						Texture->UpdateResource();
//
// 						FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture
// 							->GetResource());
// 						if (TextureResource)
// 						{
// 							ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
// 								[TextureResource, RawData = MoveTemp(RawData)](FRHICommandListImmediate& RHICmdList)
// 								{
// 									TextureResource->WriteRawToTexture_RenderThread(RawData);
// 								});
// 						}
// 						CompletedDelegate.Broadcast(FString("Success"), Texture);
// 						return;
// 					}
// 				}
// 			}
// 		}
// 	}
// 	
// 	ErrorDelegate.Broadcast(FString("Error"), nullptr);
// }
