// Copyright 2023 AIRT,  All Rights Reserved.


#include "IVHTTPNodes.h"

#include "Http.h"
#include "IVHTTPLibrary.h"
#include "IVHTTPRequset.h"
#include "IVHTTPResponse.h"
#include "Json/IVJsonObject.h"
#include "Misc/FileHelper.h"

void UHttpDownloadFileProxy::Activate()
{
	Super::Activate();

	Request->OnRequestComplete      .AddDynamic(this, &UHttpDownloadFileProxy::OnRequestCompleted);
	Request->OnRequestProgress      .AddDynamic(this, &UHttpDownloadFileProxy::OnRequestTick);
	Request->OnRequestHeaderReceived.AddDynamic(this, &UHttpDownloadFileProxy::OnHeadersReceived);

	if (!Request->ProcessRequest())
	{
		OnFileDownloadError.Broadcast(0, 0, 0.f);
		SetReadyToDestroy();
	}
}

UHttpDownloadFileProxy* UHttpDownloadFileProxy::HttpDownloadFile(const FString& FileUrl,
                                                                 const TMap<FString, FString>& UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType,
                                                                 const FString& Content, const TMap<FString, FString>& Headers, const FString& SaveFileLocation)
{
	UHttpDownloadFileProxy* const Proxy = NewObject<UHttpDownloadFileProxy>();

	Proxy->ContentLength = 0;
	Proxy->Downloaded    = 0;

	Proxy->Request = UIVHTTPRequset::CreateRequest();

	Proxy->Request->SetVerb           (Verb);
	Proxy->Request->SetHeaders        (Headers);
	Proxy->Request->SetURL            (UIVHTTPLibrary::AddParametersToUrl(FileUrl, UrlParameters));
	Proxy->Request->SetMimeType       (MimeType);
	Proxy->Request->SetContentAsString(Content);

	Proxy->SaveLocation = SaveFileLocation;

	return Proxy;
}

void UHttpDownloadFileProxy::OnRequestCompleted(UIVHTTPRequset* const Req, UIVHTTPResponse* const Response,
	const bool bConnectedSuccessfully)
{
	if (!bConnectedSuccessfully)
	{
		OnFileDownloadError.Broadcast(ContentLength, Downloaded, GetPercents());
	}
	else if (Response && Response->GetResponseCode() < 400)
	{    
		TArray<uint8> Content;
		Response->GetContent(Content);
		if (!FFileHelper::SaveArrayToFile(Content, *SaveLocation))
		{
			UE_LOG(LogHttp, Error, TEXT("Download file error: Failed to save data to \"%s\"."), *FPaths::ConvertRelativePathToFull(SaveLocation));
			OnFileDownloadError.Broadcast(ContentLength, Downloaded, GetPercents());
		}
		else
		{
			OnFileDownloaded.Broadcast(ContentLength, Downloaded, GetPercents());
		}
	}
	else
	{
		UE_LOG(LogHttp, Error, TEXT("Download file error: Server responded with an invalid code: \"%d\"."), Response ? Response->GetResponseCode() : -1);
		OnFileDownloadError.Broadcast(ContentLength, Downloaded, GetPercents());
	}

	SetReadyToDestroy();
}

void UHttpDownloadFileProxy::OnRequestTick(UIVHTTPRequset* const Req, const int32 BytesSent,
	const int32 BytesReceived)
{
	Downloaded = BytesReceived;

	OnDownloadProgress.Broadcast(ContentLength, Downloaded, GetPercents());
}

void UHttpDownloadFileProxy::OnHeadersReceived(UIVHTTPRequset* const Req, const FString& HeaderName,
	const FString& NewHeaderValue)
{
	if (HeaderName.Equals(TEXT("Content-Length"), ESearchCase::IgnoreCase))
	{
		ContentLength = FCString::Atoi(*NewHeaderValue);
	}
}

UProcessHttpRequestProxy::UProcessHttpRequestProxy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, RequestWrapper(nullptr)
	, BytesSent(0)
	, BytesReceived(0)
{
}

UProcessHttpRequestProxy* UProcessHttpRequestProxy::InlineProcessRequest(UIVHTTPRequset* const Request)
{
	UProcessHttpRequestProxy* const Proxy = NewObject<UProcessHttpRequestProxy>();

	if (Request)
	{
		Proxy->RequestWrapper = Request;

	}
	else
	{
		Proxy->SetReadyToDestroy();
	}

	return Proxy;
}

void UProcessHttpRequestProxy::OnCompleteInternal(UIVHTTPRequset* const Request, UIVHTTPResponse* const Response,
	const bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		OnResponse.Broadcast(Response->GetResponseCode(), Response->GetAllHeaders(),
			Response->GetContentType(), Response->GetContentAsString(),Response->GetContentAsJson(), Request->GetElapsedTime(),
			Request->GetStatus());
	}
	else
	{
		OnError.Broadcast(Response->GetResponseCode(), Response->GetAllHeaders(), 
			Response->GetContentType(), Response->GetContentAsString(), Response->GetContentAsJson(),Request->GetElapsedTime(), 
			Request->GetStatus());
	}
}

void UProcessHttpRequestProxy::OnTickInternal(UIVHTTPRequset* const Request, const int32 InBytesSent,
	const int32 InBytesReceived)
{
	UIVJsonObject* JsonObj = NewObject<UIVJsonObject>();
	OnResponse.Broadcast(100, FHeaders(), TEXT(""), TEXT(""),JsonObj, Request->GetElapsedTime(),
	Request->GetStatus());
}

USendHttpRequestProxyBase::USendHttpRequestProxyBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, RequestWrapper(NewObject<UIVHTTPRequset>())
	, BytesSent(0)
	, BytesReceived(0)
{
	RequestWrapper->OnRequestProgress.AddDynamic(this, &USendHttpRequestProxyBase::_OnTickInternal);
	RequestWrapper->OnRequestComplete.AddDynamic(this, &USendHttpRequestProxyBase::_OnCompleteInternal);
}

void USendHttpRequestProxyBase::SendRequest()
{
	if (!RequestWrapper->ProcessRequest())
	{
		OnErrorInternal(nullptr);
	}
}

void USendHttpRequestProxyBase::_OnCompleteInternal(UIVHTTPRequset* const Request, UIVHTTPResponse* const Response,
                                                    const bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully)
	{
		OnSuccessInternal(Response);
	}
	else
	{
		OnErrorInternal(Response);
	}

	SetReadyToDestroy();
}

void USendHttpRequestProxyBase::_OnTickInternal(UIVHTTPRequset* const Request, const int32 InBytesSent,
	const int32 InBytesReceived)
{
	BytesSent       = InBytesSent;
	BytesReceived   = InBytesReceived;

	OnTickInternal();
}

USendHttpRequestProxy* USendHttpRequestProxy::SendHttpRequest(const FString& ServerUrl,
	const TMap<FString, FString>& UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType,
	const FString& Content, const TMap<FString, FString>& Headers)
{
	USendHttpRequestProxy* const Proxy = NewObject<USendHttpRequestProxy>();

	UIVHTTPRequset* const Request = Proxy->GetRequest();

	Request->SetURL(UIVHTTPLibrary::AddParametersToUrl(ServerUrl, UrlParameters));
	Request->SetMimeType(MimeType);
	Request->SetVerb(Verb);
	Request->SetContentAsString(Content);
	Request->SetHeaders(Headers);

	Proxy->SendRequest();

	return Proxy;
}

void USendHttpRequestProxy::OnTickInternal()
{
	UIVJsonObject* JsonObj = NewObject<UIVJsonObject>();
	OnTick.Broadcast(100, FHeaders(), FString(), FString(), JsonObj,
		GetRequest()->GetElapsedTime(), GetRequest()->GetStatus());
}

void USendHttpRequestProxy::OnErrorInternal(UIVHTTPResponse* const Response)
{
	OnError.Broadcast(Response->GetResponseCode(), 
		FHeaders(Response->GetAllHeaders()), Response->GetContentType(), Response->GetContentAsString(), Response->GetContentAsJson(),
		Response->GetElapsedTime(), GetRequest()->GetStatus());
	SetReadyToDestroy();
}

void USendHttpRequestProxy::OnSuccessInternal(UIVHTTPResponse* const Response)
{
	OnResponse.Broadcast(Response->GetResponseCode(), FHeaders(Response->GetAllHeaders()), 
		Response->GetContentType(), Response->GetContentAsString(), Response->GetContentAsJson(),Response->GetElapsedTime(), 
		GetRequest()->GetStatus());
	SetReadyToDestroy();
}

USendBinaryHttpRequestProxy* USendBinaryHttpRequestProxy::SendBinaryHttpRequest(const FString& ServerUrl,
	const TMap<FString, FString>& UrlParameters, const EHttpVerb Verb, const EHttpMimeType MimeType,
	const TArray<uint8>& Content, const TMap<FString, FString>& Headers)
{
	USendBinaryHttpRequestProxy* const Proxy = NewObject<USendBinaryHttpRequestProxy>();

	UIVHTTPRequset* const Request = Proxy->GetRequest();

	Request->SetURL(UIVHTTPLibrary::AddParametersToUrl(ServerUrl, UrlParameters));
	Request->SetVerb(Verb);
	Request->SetContent(Content);
	Request->SetMimeType(MimeType);
	Request->SetHeaders(Headers);

	Proxy->SendRequest();

	return Proxy;
}

void USendBinaryHttpRequestProxy::OnTickInternal()
{
	OnTick.Broadcast(100, FHeaders(), FString(), TArray<uint8>(),
		GetRequest()->GetElapsedTime(), GetRequest()->GetStatus());
}

void USendBinaryHttpRequestProxy::OnErrorInternal(UIVHTTPResponse* const Response)
{
	OnTick.Broadcast(100, FHeaders(), FString(), TArray<uint8>(),
		GetRequest()->GetElapsedTime(), GetRequest()->GetStatus());
}

void USendBinaryHttpRequestProxy::OnSuccessInternal(UIVHTTPResponse* const Response)
{
	TArray<uint8> Content;
	Response->GetContent(Content);
	OnError.Broadcast(Response->GetResponseCode(),
		FHeaders(Response->GetAllHeaders()), Response->GetContentType(), Content,
		Response->GetElapsedTime(), GetRequest()->GetStatus());
}
