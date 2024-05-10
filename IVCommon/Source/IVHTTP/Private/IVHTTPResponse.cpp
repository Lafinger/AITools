// Copyright 2023 AIRT All Rights Reserved.


#include "IVHTTPResponse.h"

#include "Json/IVJsonObject.h"
#include "Interfaces/IHttpResponse.h"

UIVHTTPResponse::UIVHTTPResponse()
	: Super()
	, RequestDuration(0.f)
{
	
}

void UIVHTTPResponse::GetContent(TArray<uint8>& OutContent) const
{
	
	if (Response)
	{
		OutContent = Response->GetContent();
	}
}

FString UIVHTTPResponse::GetContentAsString() const
{
	if (Response)
	{
		return Response->GetContentAsString();
	}
	return TEXT("");
}

UIVJsonObject* UIVHTTPResponse::GetContentAsJson() const
{
	UIVJsonObject* JsonObj = NewObject<UIVJsonObject>();
	if (Response)
	{
		if(JsonObj->DecodeJson(Response->GetContentAsString()))
		{
			return  JsonObj;
		}

		return  JsonObj;
	}
	return  JsonObj;
}

int32 UIVHTTPResponse::GetContentLength() const
{
	return Response ? Response->GetContentLength() : 0;
}

FString UIVHTTPResponse::GetContentType() const
{
	return Response ? Response->GetContentType() : TEXT("");
}

FString UIVHTTPResponse::GetHeader(const FString& Key) const
{
	return Response ? Response->GetHeader(Key) : TEXT("");
}

int32 UIVHTTPResponse::GetResponseCode() const
{
	return Response ? Response->GetResponseCode() : -1;
}

FString UIVHTTPResponse::GetURL() const
{
	return Response ? Response->GetURL() : TEXT("");
}

FString UIVHTTPResponse::GetURLParameter(const FString& ParameterName) const
{
	return Response ? Response->GetURLParameter(ParameterName) : TEXT("");
}

float UIVHTTPResponse::GetElapsedTime() const
{
	return RequestDuration;
}

void UIVHTTPResponse::InitInternal(TSharedPtr<IHttpResponse, ESPMode::ThreadSafe> InResponse,
	const float& InRequestDuration)
{
	Response = InResponse;
	RequestDuration = InRequestDuration;
}

TMap<FString, FString> UIVHTTPResponse::GetAllHeaders() const
{
	if (!Response)
	{
		return TMap<FString, FString>();
	}

	TArray<FString> Headers = Response->GetAllHeaders();

	TMap<FString, FString> OutHeaders;
	
	FString* Key;
	FString* Value;

	for (FString & Header : Headers)
	{
		Key		= nullptr;
		Value	= nullptr;

		Header.Split(TEXT(": "), Key, Value, ESearchCase::CaseSensitive);

		if (Key && Value)
		{
			OutHeaders.Emplace(*Key, *Value);
		}
	}

	return OutHeaders;
}
