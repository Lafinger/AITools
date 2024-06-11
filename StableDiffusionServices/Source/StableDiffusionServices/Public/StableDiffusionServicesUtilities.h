// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
	
typedef SIZE_T (*FEncodeFuncPtr)(const uint8* /*Scr*/, int32 /*Width*/, int32 /*Height*/, int32 /*Stride*/, float /*Quality*/, uint8** /*OutPut*/);
	
typedef uint8* (*FDecodeFuncPtr)(const uint8* /*Sc*/, SIZE_T /*Size*/, int32* /*Width*/, int32* /*Height*/);

typedef TFunction<void(uint8*, SIZE_T)> FEncodeProcess;

typedef TFunction<void(TArray<FColor>, int32, int32)> FDecodeProcess;

/**
 * 
 */
class STABLEDIFFUSIONSERVICES_API FWebpUtilities
{
public:
	
	/** 检查数据是否包含 Webp 标头 */
	static bool HasWebp(const uint8* Scr, const SIZE_T& Size);
	
	static bool EncodeWebPData(TArrayView64<FColor> Colors, int32 Width, int32 Height, int32 Stride, FEncodeFuncPtr EncodeFuncPtr, const FEncodeProcess& EncodeProcess, float Quality = 100);
	
	static bool DecodeWebPData(const uint8* Scr, const SIZE_T& Size, FDecodeFuncPtr DecodeFuncPtr, const FDecodeProcess& DecodeProcess);
	
	/**
	 * 将 Webp 源数据转换为 UTexture2D 对象.
	 * @param Scr 数据源
	 * @param Size 数据大小.
	 * @return 2D 纹理.
	 */
	static UTexture2D* ConvertWebpDataToTexture2D(const uint8* Scr, const SIZE_T& Size);
	
	/**
	 * 将 Webp 源数据转换为 UTexture2DDynamic 对象.
	 * @param Scr 数据源
	 * @param Size 数据大小.
	 * @return 2D 纹理.
	 */
	static UTexture2DDynamic* ConvertWebpDataToTexture2DDynamic(const uint8* Scr, const SIZE_T Size);
};


class STABLEDIFFUSIONSERVICES_API FComfyUIUtilities
{
public:
	
	/** 检查数据是否包含 Webp 标头 */
	static FString GetWorkflowAPIStringWithPrompt(const FString& InPrompt);
};


