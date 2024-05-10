// Copyright 2023 AIRT All Rights Reserved.


#include "IVHTTPRequset.h"

#include "HttpModule.h"
#include "IVHTTPLibrary.h"
#include "IVHTTPResponse.h"

UIVHTTPRequset::UIVHTTPRequset()
	: Super()
{
	Request = FHttpModule::Get().CreateRequest();

	Request->OnProcessRequestComplete().BindUObject(this, &UIVHTTPRequset::OnRequestCompleteInternal );
	Request->OnRequestProgress       ().BindUObject(this, &UIVHTTPRequset::OnRequestProgressInternal );
	Request->OnHeaderReceived        ().BindUObject(this, &UIVHTTPRequset::OnHeaderReceivedInternal  );
}

UIVHTTPRequset* UIVHTTPRequset::CreateRequest()
{
	return NewObject<UIVHTTPRequset>();
}

void UIVHTTPRequset::SetVerb(const EHttpVerb InVerb)
{
	SetCustomVerb(ConvertEnumVerbToString(InVerb));
}

void UIVHTTPRequset::SetHeader(const FString& Key, const FString& Value)
{
	Request->SetHeader(Key, Value);
}

void UIVHTTPRequset::SetHeaders(const TMap<FString, FString>& Headers)
{
	for (const auto& Header : Headers)
	{
		Request->SetHeader(Header.Key, Header.Value);
	}
}

void UIVHTTPRequset::AppendToHeader(const FString& Key, const FString& Value)
{
	Request->AppendToHeader(Key, Value);
}

void UIVHTTPRequset::SetURL(const FString& Url)
{
	Request->SetURL(Url);
}

void UIVHTTPRequset::SetMimeType(const EHttpMimeType MimeType)
{
	Request->AppendToHeader(TEXT("Content-Type"), UIVHTTPLibrary::CreateMimeType(MimeType));
}

void UIVHTTPRequset::SetMimeTypeAsString(const FString& MimeType)
{
	Request->AppendToHeader(TEXT("Content-Type"), MimeType);
}

void UIVHTTPRequset::SetContent(const TArray<uint8>& Content)
{
	Request->SetContent(Content);
}

void UIVHTTPRequset::SetContentAsString(const FString& Content)
{
	Request->SetContentAsString(Content);
}

void UIVHTTPRequset::SetContentAsStreamedFile(const FString& FileName, bool& bFileValid)
{
	bFileValid = Request->SetContentAsStreamedFile(FileName);
}

TMap<FString, FString> UIVHTTPRequset::GetAllHeaders() const
{
	TArray<FString> Headers = Request->GetAllHeaders();

	TMap<FString, FString> OutHeaders;
	
	FString* Key	= nullptr;
	FString* Value	= nullptr;

	const FString Separator = TEXT(": ");

	for (FString & Header : Headers)
	{
		if (Header.Split(Separator, Key, Value, ESearchCase::CaseSensitive))
		{
			OutHeaders.Emplace(*Key, *Value);
		}
	}

	return OutHeaders;
}

void UIVHTTPRequset::GetContent(TArray<uint8>& OutContent) const
{
	OutContent = Request->GetContent();
}

FString UIVHTTPRequset::GetContentAsString() const
{
	const TArray<uint8>& Content = Request->GetContent();

	return BytesToString(Content.GetData(), Content.Num());
}

int32 UIVHTTPRequset::GetContentLength() const
{
	return Request->GetContentLength();
}

FString UIVHTTPRequset::GetContentType() const
{
	return Request->GetContentType();
}

float UIVHTTPRequset::GetElapsedTime() const
{
	return Request->GetContentLength();
}

FString UIVHTTPRequset::GetHeader(const FString& Key) const
{
	return Request->GetContentType();
}

EHttpBPRequestStatus UIVHTTPRequset::GetStatus() const
{
	return static_cast<EHttpBPRequestStatus>(Request->GetStatus());
}

FString UIVHTTPRequset::GetURL() const
{
	return Request->GetURL();
}

FString UIVHTTPRequset::GetURLParameter(const FString& ParameterName) const
{
	return Request->GetURLParameter(ParameterName);
}

FString UIVHTTPRequset::GetVerb() const
{
	return Request->GetVerb();
}

bool UIVHTTPRequset::ProcessRequest()
{
	if (Request->GetContentType() == TEXT(""))
	{
		SetMimeType(EHttpMimeType::txt);
	}

	return Request->ProcessRequest();
}

void UIVHTTPRequset::CancelRequest()
{
	Request->CancelRequest();
}

void UIVHTTPRequset::OnRequestCompleteInternal(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest,
	TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> RawResponse, bool bConnectedSuccessfully)
{
	OnRequestComplete.Broadcast(this, CreateResponse(RawRequest, RawResponse), bConnectedSuccessfully);
}

void UIVHTTPRequset::OnRequestProgressInternal(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest,
	const int32 BytesSent, const int32 BytesReceived)
{
	OnRequestProgress.Broadcast(this, BytesSent, BytesReceived);
}

void UIVHTTPRequset::OnHeaderReceivedInternal(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest,
	const FString& HeaderName, const FString& HeaderValue)
{
	OnRequestHeaderReceived.Broadcast(this, HeaderName, HeaderValue);
}

void UIVHTTPRequset::OnRequestWillRetryInternal(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe> RawRequest,
	TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> RawResponse, float SecondsToRetry)
{
	OnRequestWillRetry.Broadcast(this, CreateResponse(RawRequest, RawResponse), SecondsToRetry);
}

UIVHTTPResponse* UIVHTTPRequset::CreateResponse(TSharedPtr<IHttpRequest, ESPMode::ThreadSafe>& RawRequest,
	TSharedPtr<IHttpResponse, ESPMode::ThreadSafe>& RawResponse)
{
	UIVHTTPResponse* const WrappedResponse = NewObject<UIVHTTPResponse>();

	WrappedResponse->InitInternal(RawResponse, RawRequest->GetElapsedTime());

	return WrappedResponse;
}

FString UIVHTTPRequset::ConvertEnumVerbToString(const EHttpVerb InVerb)
{
	constexpr uint8 VerbsCount = static_cast<uint8>(EHttpVerb::MAX_COUNT);

	const uint8 Uint8Verb = static_cast<uint8>(InVerb);
	
	if (Uint8Verb >= VerbsCount)
	{
		return TEXT("NONE");
	}

	static const FString Converted[VerbsCount] =
	{
		TEXT("GET"),
		TEXT("POST"),
		TEXT("PUT"),
		TEXT("PATCH"),
		TEXT("HEAD"),
	//	TEXT("DELETE")
	};

	return Converted[Uint8Verb];
}

void UIVHTTPRequset::SetCustomVerb(const FString& Verb)
{
	Request->SetVerb(Verb);
}
