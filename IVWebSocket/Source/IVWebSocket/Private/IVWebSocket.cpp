// Copyright Epic Games, Inc. All Rights Reserved.

#include "IVWebSocket.h"

#include "WebSocketsModule.h"

#define LOCTEXT_NAMESPACE "FIVWebSocketModule"

DEFINE_LOG_CATEGORY(IVWebSocket);
void FIVWebSocketModule::StartupModule()
{
	const FName WebSocketsModuleName = TEXT("WebSockets");

	if (!FModuleManager::Get().IsModuleLoaded(WebSocketsModuleName))
	{
		FWebSocketsModule& Module = FModuleManager::LoadModuleChecked<FWebSocketsModule>(WebSocketsModuleName);
	}
}

void FIVWebSocketModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FIVWebSocketModule, IVWebSocket)