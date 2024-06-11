// Fill out your copyright notice in the Description page of Project Settings.

#include "StableDiffusionServicesUtilities.h"

#include "decode.h"
#include "encode.h"
#include "Engine/Texture2DDynamic.h"

bool FWebpUtilities::HasWebp(const uint8* Scr, const SIZE_T& Size)
{
	return WebPGetInfo(Scr, Size, nullptr, nullptr) != INDEX_NONE;
}

bool FWebpUtilities::EncodeWebPData(TArrayView64<FColor> Colors, int32 Width, int32 Height, int32 Stride,
	FEncodeFuncPtr EncodeFuncPtr, const FEncodeProcess& EncodeProcess, float Quality)
{
	if(!ensureAlwaysMsgf(EncodeFuncPtr, TEXT("You must provide an actual encoded execution function, see encode.h.")))
	{
		return false;
	}

	if(!ensureAlwaysMsgf(EncodeProcess, TEXT("You must provide a real code handler.")))
	{
		return false;
	}
	
	if(!ensureAlwaysMsgf(!Colors.IsEmpty(), TEXT("invalid Color data.")))
	{
		return false;
	}

	uint8* Data = nullptr;
	const SIZE_T Size = EncodeFuncPtr((uint8*)Colors.GetData(), Width, Height, Stride, Quality, &Data);
	EncodeProcess(Data, Size);
	WebPFree(Data);
	return true;
}

bool FWebpUtilities::DecodeWebPData(const uint8* Scr, const SIZE_T& Size, FDecodeFuncPtr DecodeFuncPtr, const FDecodeProcess& DecodeProcess)
{
	if(!ensureAlwaysMsgf(HasWebp(Scr, Size), TEXT("invalid webp data.")))
	{
		return false;
	}

	if(!ensureAlwaysMsgf(DecodeFuncPtr != nullptr, TEXT("You must provide an actual encoded execution function, see encode.h.")))
	{
		return false;
	}

	if(!ensureAlwaysMsgf(DecodeProcess != nullptr, TEXT("You must provide a real code handler.")))
	{
		return false;
	}

	int32 Width;
	int32 Height;

	uint8* Dest = DecodeFuncPtr(Scr, Size, &Width, &Height);
	TArray<FColor> OutColors;
	OutColors.AddDefaulted(Width * Height * sizeof(FColor));
	FMemory::Memcpy(OutColors.GetData(), Dest, Width * Height * sizeof(FColor));
	DecodeProcess(OutColors, Width, Height);
	
	WebPFree(Dest);
	return true;
}

UTexture2D* FWebpUtilities::ConvertWebpDataToTexture2D(const uint8* Scr, const SIZE_T& Size)
{
	UTexture2D* OutTexture = nullptr;
	
	if(DecodeWebPData(Scr, Size, &WebPDecodeRGBA, [&OutTexture](TArray<FColor> Colors, int32 Width, int32 Height)
	{
		UTexture2D* TempTexture = UTexture2D::CreateTransient(Width, Height, EPixelFormat::PF_R8G8B8A8);
		void* TextureData = TempTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, Colors.GetData(), Width * Height * 4);
		TempTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
		TempTexture->UpdateResource();
		OutTexture = TempTexture;
	}))
	{
		return OutTexture;
	}

	return nullptr;
}

UTexture2DDynamic* FWebpUtilities:: ConvertWebpDataToTexture2DDynamic(const uint8* Scr, const SIZE_T Size)
{
	UTexture2DDynamic* OutTexture = nullptr;

	if(DecodeWebPData(Scr, Size, &WebPDecodeBGRA, [&OutTexture](TArray<FColor> Colors, int32 Width, int32 Height)
	{
		if(UTexture2DDynamic* Texture = UTexture2DDynamic::Create(Width, Height))
		{
			Texture->SRGB = true;
			Texture->UpdateResource();
			TArray<uint8> ViewData((uint8*)Colors.GetData(), Colors.Num());

			if(FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->GetResource()))
			{
				ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)([TextureResource, RawData = MoveTemp(ViewData)](FRHICommandListImmediate& RHICmdList)
				{
					TextureResource->WriteRawToTexture_RenderThread(RawData);
				});
			}
			
			OutTexture = Texture;
		}
	}))
	{
		return OutTexture;
	}

	return nullptr;
}

FString FComfyUIUtilities::GetWorkflowAPIStringWithPrompt(const FString& InPrompt)
{
	FString WorkflowAPIFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir()) + "AITools/StableDiffusionServices/Resources/ComfyUI/API/workflow_api.json";
	FString Result;
	bool bIsLoadSuccess = FFileHelper::LoadFileToString(Result, *WorkflowAPIFilePath);
	if(!ensureAlwaysMsgf(bIsLoadSuccess || !Result.IsEmpty(), TEXT("Cannot find workflow_api.json.")))
	{
		return Result;
	}

	Result = Result.Replace(TEXT("UserInput_API"), *InPrompt);
	return Result;
}
