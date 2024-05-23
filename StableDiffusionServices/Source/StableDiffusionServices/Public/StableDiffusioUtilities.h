#pragma once
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Engine/Texture2DDynamic.h"

namespace StableDiffusioUtilities
{

	FORCEINLINE static FString ImageFormatToString(EImageFormat ImageFormat)
	{
		switch (ImageFormat)
		{
		case EImageFormat::Invalid: return "Invalid";
		case EImageFormat::PNG: return "PNG";
		case EImageFormat::JPEG: return "JPEG";
		case EImageFormat::GrayscaleJPEG: return "GrayscaleJPEG";
		case EImageFormat::BMP: return "BMP";
		case EImageFormat::ICO: return "ICO";
		case EImageFormat::EXR: return "EXR";
		case EImageFormat::ICNS: return "ICNS";
		case EImageFormat::TGA: return "TGA";
		case EImageFormat::HDR: return "HDR";
		case EImageFormat::TIFF: return "TIFF";
		case EImageFormat::DDS: return "DDS";
		}

		return "Unknown";
	}
	
	FORCEINLINE static bool WebpFormatCheck(const TArray<uint8>& Bytes)
	{
		// Check if the 'WEBP' signature is present after the 'RIFF' header
		if (Bytes.Num() > 12     //
			&& Bytes[8] == 'W'   //
			&& Bytes[9] == 'E'   //
			&& Bytes[10] == 'B'  //
			&& Bytes[11] == 'P')
		{
			return true;
		}
		
		return false;
	}

	static UTexture2DDynamic* CreateDynamicTextureFromWebpImageData(const TArray<uint8>& RawFileData)
	{
		auto& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		const EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(RawFileData.GetData(), RawFileData.Num());
		
		FString ImageFormatString = ImageFormatToString(ImageFormat);
		UE_LOG(LogTemp, Display, TEXT("Detected image format %s"), *ImageFormatString);

		if (ImageFormat == EImageFormat::Invalid)
		{
			WebpFormatCheck(RawFileData);
			return nullptr;
		}

		auto ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

		if (!ImageWrapper.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Image wrapper is invalid"));
			return nullptr;
		}

		if (!ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.GetAllocatedSize()))
		{
			UE_LOG(LogTemp, Error, TEXT("Setting raw data failed"));
			return nullptr;
		}

		UTexture2DDynamic* Texture = nullptr;
		
		TArray<uint8> UncompressedRGBA;
		bool bIsUncompressedSuccess = ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA);
		if (!bIsUncompressedSuccess)
		{
			return nullptr;
		}

		Texture = UTexture2DDynamic::Create(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());
		if (!Texture)
		{
			
		}

		Texture->SRGB = true;
		Texture->UpdateResource();

		FTexture2DDynamicResource* TextureResource = static_cast<FTexture2DDynamicResource*>(Texture->GetResource());
		if (!TextureResource)
		{
			return nullptr;
		}
		
		ENQUEUE_RENDER_COMMAND(FWriteRawDataToTexture)(
			[TextureResource, RawData = MoveTemp(UncompressedRGBA)](FRHICommandListImmediate& RHICmdList)
			{
				TextureResource->WriteRawToTexture_RenderThread(RawData);
			});

		return Texture;
	}

	static UTexture2D* CreateTextureFromWebpImageData(const TArray<uint8>& RawFileData)
	{
		auto& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		const EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(RawFileData.GetData(), RawFileData.Num());
		
		FString ImageFormatString = ImageFormatToString(ImageFormat);
		UE_LOG(LogTemp, Display, TEXT("Detected image format %s"), *ImageFormatString);

		if (ImageFormat == EImageFormat::Invalid)
		{
			WebpFormatCheck(RawFileData);
			return nullptr;
		}

		auto ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

		if (!ImageWrapper.IsValid())
		{
			UE_LOG(LogTemp, Error, TEXT("Image wrapper is invalid"));
			return nullptr;
		}

		if (!ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
		{
			UE_LOG(LogTemp, Error, TEXT("Setting raw data failed"));
			return nullptr;
		}

		UTexture2D* Texture = nullptr;
		TArray<uint8> UncompressedRGBA;
		if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);
			if (!Texture) return nullptr;

			void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
			FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
			Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
			Texture->UpdateResource();
		}

		return Texture;
	}
}
