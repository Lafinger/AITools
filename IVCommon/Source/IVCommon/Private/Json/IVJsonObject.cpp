// Copyright 2023 ARIT All Rights Reserved.


#include "Json/IVJsonObject.h"

#include "IVCommon.h"
#include "Json/IVJsonValue.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

typedef TJsonWriterFactory< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > FCondensedJsonStringWriterFactory;
typedef TJsonWriter< TCHAR, TCondensedJsonPrintPolicy<TCHAR> > FCondensedJsonStringWriter;

UIVJsonObject::UIVJsonObject(const class FObjectInitializer& PCIP)
    : Super(PCIP)
{
    Reset();
}


UIVJsonObject* UIVJsonObject::ConstructJsonObject(UObject* WorldContextObject)
{
    return NewObject<UIVJsonObject>(WorldContextObject);
}

void UIVJsonObject::Reset()
{
    if (JsonObj.IsValid())
    {
        JsonObj.Reset();
    }

    JsonObj = MakeShareable(new FJsonObject());
}

FString UIVJsonObject::EncodeJson(bool IsCondensedString/*=true*/) const
{
    if (!JsonObj.IsValid())
    {
        return TEXT("");
    }

    FString OutputString;
    if (IsCondensedString)
    {
        TSharedRef< FCondensedJsonStringWriter > Writer = FCondensedJsonStringWriterFactory::Create(&OutputString);
        FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);
    }
    else
    {
        TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
        FJsonSerializer::Serialize(JsonObj.ToSharedRef(), Writer);
    }

    return OutputString;
}

FString UIVJsonObject::EncodeJsonToSingleString() const
{
    FString OutputString = EncodeJson();

    // Remove line terminators
    (void)OutputString.Replace(LINE_TERMINATOR, TEXT(""));

    // Remove tabs
    (void)OutputString.Replace(LINE_TERMINATOR, TEXT("\t"));

    return OutputString;
}

bool UIVJsonObject::DecodeJson(const FString& JsonString)
{
    TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(*JsonString);
    if (FJsonSerializer::Deserialize(Reader, JsonObj) && JsonObj.IsValid())
    {
        return true;
    }

    // If we've failed to deserialize the string, we should clear our internal data
    Reset();

    UE_LOG(LogIVJson, Error, TEXT("Json decoding failed for: %s"), *JsonString);

    return false;
}

TArray<FString> UIVJsonObject::GetFieldNames()
{
    TArray<FString> Result;

    if (!JsonObj.IsValid())
    {
        return Result;
    }

    JsonObj->Values.GetKeys(Result);

    return Result;
}

bool UIVJsonObject::HasField(const FString& FieldName) const
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return false;
    }

    return JsonObj->HasField(FieldName);
}

void UIVJsonObject::RemoveField(const FString& FieldName)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }

    JsonObj->RemoveField(FieldName);
}

UIVJsonValue* UIVJsonObject::GetField(const FString& FieldName) const
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return nullptr;
    }

    TSharedPtr<FJsonValue> NewVal = JsonObj->TryGetField(FieldName);
    if (NewVal.IsValid())
    {
        UIVJsonValue* NewValue = NewObject<UIVJsonValue>();
        NewValue->SetRootValue(NewVal);

        return NewValue;
    }

    return nullptr;
}

void UIVJsonObject::SetField(const FString& FieldName, UIVJsonValue* JsonValue)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }

    JsonObj->SetField(FieldName, JsonValue->GetRootValue());
}

TArray<UIVJsonValue*> UIVJsonObject::GetArrayField(const FString& FieldName)
{
    if (!JsonObj->HasTypedField<EJson::Array>(FieldName))
    {
        UE_LOG(LogIVJson, Warning, TEXT("No field with name %s of type Array"), *FieldName);
    }

    TArray<UIVJsonValue*> OutArray;
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return OutArray;
    }

    TArray< TSharedPtr<FJsonValue> > ValArray = JsonObj->GetArrayField(FieldName);
    for (auto Value : ValArray)
    {
        UIVJsonValue* NewValue = NewObject<UIVJsonValue>();
        NewValue->SetRootValue(Value);

        OutArray.Add(NewValue);
    }

    return OutArray;
}

void UIVJsonObject::SetArrayField(const FString& FieldName, const TArray<UIVJsonValue*>& InArray)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }

    TArray< TSharedPtr<FJsonValue> > ValArray;

    // Process input array and COPY original values
    for (auto InVal : InArray)
    {
        TSharedPtr<FJsonValue> JsonVal = InVal->GetRootValue();

        switch (InVal->GetType())
        {
        case EIVJson::None:
            break;

        case EIVJson::Null:
            ValArray.Add(MakeShareable(new FJsonValueNull()));
            break;

        case EIVJson::String:
            ValArray.Add(MakeShareable(new FJsonValueString(JsonVal->AsString())));
            break;

        case EIVJson::Number:
            ValArray.Add(MakeShareable(new FJsonValueNumber(JsonVal->AsNumber())));
            break;

        case EIVJson::Boolean:
            ValArray.Add(MakeShareable(new FJsonValueBoolean(JsonVal->AsBool())));
            break;

        case EIVJson::Array:
            ValArray.Add(MakeShareable(new FJsonValueArray(JsonVal->AsArray())));
            break;

        case EIVJson::Object:
            ValArray.Add(MakeShareable(new FJsonValueObject(JsonVal->AsObject())));
            break;

        default:
            break;
        }
    }

    JsonObj->SetArrayField(FieldName, ValArray);
}

void UIVJsonObject::MergeJsonObject(UIVJsonObject* InJsonObject, bool Overwrite)
{
    TArray<FString> Keys = InJsonObject->GetFieldNames();

    for (auto Key : Keys)
    {
        if (Overwrite == false && HasField(Key))
        {
            continue;
        }

        SetField(Key, InJsonObject->GetField(Key));
    }
}

float UIVJsonObject::GetNumberField(const FString& FieldName) const
{
    if (!JsonObj.IsValid() || !JsonObj->HasTypedField<EJson::Number>(FieldName))
    {
        UE_LOG(LogIVJson, Warning, TEXT("No field with name %s of type Number"), *FieldName);
        return 0.0f;
    }

    return JsonObj->GetNumberField(FieldName);
}

void UIVJsonObject::SetNumberField(const FString& FieldName, float Number)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }

    JsonObj->SetNumberField(FieldName, Number);
}

FString UIVJsonObject::GetStringField(const FString& FieldName) const
{
    if (!JsonObj.IsValid() || !JsonObj->HasTypedField<EJson::String>(FieldName))
    {
        UE_LOG(LogIVJson, Warning, TEXT("No field with name %s of type String"), *FieldName);
        return TEXT("");
    }

    return JsonObj->GetStringField(FieldName);
}

void UIVJsonObject::SetStringField(const FString& FieldName, const FString& StringValue)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }

    JsonObj->SetStringField(FieldName, StringValue);
}

bool UIVJsonObject::GetBoolField(const FString& FieldName) const
{
    if (!JsonObj.IsValid() || !JsonObj->HasTypedField<EJson::Boolean>(FieldName))
    {
        UE_LOG(LogIVJson, Warning, TEXT("No field with name %s of type Boolean"), *FieldName);
        return false;
    }

    return JsonObj->GetBoolField(FieldName);
}

void UIVJsonObject::SetBoolField(const FString& FieldName, bool InValue)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }

    JsonObj->SetBoolField(FieldName, InValue);
}

UIVJsonObject* UIVJsonObject::GetObjectField(const FString& FieldName) const
{
    if (!JsonObj.IsValid() || !JsonObj->HasTypedField<EJson::Object>(FieldName))
    {
        UE_LOG(LogIVJson, Warning, TEXT("No field with name %s of type Object"), *FieldName);
        return nullptr;
    }

    TSharedPtr<FJsonObject> JsonObjField = JsonObj->GetObjectField(FieldName);

    UIVJsonObject* OutRestJsonObj = NewObject<UIVJsonObject>();
    OutRestJsonObj->SetRootObject(JsonObjField);

    return OutRestJsonObj;
}

void UIVJsonObject::SetObjectField(const FString& FieldName, UIVJsonObject* JsonObject)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }

    JsonObj->SetObjectField(FieldName, JsonObject->GetRootObject());
}

void UIVJsonObject::GetBinaryField(const FString& FieldName, TArray<uint8>& OutBinary) const
{
    if (!JsonObj->HasTypedField<EJson::String>(FieldName))
    {
        UE_LOG(LogIVJson, Warning, TEXT("No field with name %s of type String"), *FieldName);
    }
    TSharedPtr<FJsonValue> JsonValue = JsonObj->TryGetField(FieldName);

    if (FJsonValueBinary::IsBinary(JsonValue))
    {
        OutBinary = FJsonValueBinary::AsBinary(JsonValue);
    }
    else if (JsonValue->Type == EJson::String)
    {
        //If we got a string that isn't detected as a binary via socket.io protocol hack
        //then we need to decode this string as base 64
        TArray<uint8> DecodedArray;
        bool bDidDecodeCorrectly = FBase64::Decode(JsonValue->AsString(), DecodedArray);
        if (!bDidDecodeCorrectly)
        {
            UE_LOG(LogIVJson, Warning, TEXT("USIOJsonObject::GetBinaryField couldn't decode %s as a binary."), *JsonValue->AsString());
        }
        OutBinary = DecodedArray;
    }
    else
    {
        TArray<uint8> EmptyArray;
        OutBinary = EmptyArray;
    }
}

void UIVJsonObject::SetBinaryField(const FString& FieldName, const TArray<uint8>& Bytes)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }
    TSharedPtr<FJsonValueBinary> JsonValue = MakeShareable(new FJsonValueBinary(Bytes));
    JsonObj->SetField(FieldName, JsonValue);
}

TArray<float> UIVJsonObject::GetNumberArrayField(const FString& FieldName)
{
    if (!JsonObj->HasTypedField<EJson::Array>(FieldName))
    {
        UE_LOG(LogIVJson, Warning, TEXT("No field with name %s of type Array"), *FieldName);
    }

    TArray<float> NumberArray;
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return NumberArray;
    }

    TArray<TSharedPtr<FJsonValue> > JsonArrayValues = JsonObj->GetArrayField(FieldName);
    for (TArray<TSharedPtr<FJsonValue> >::TConstIterator It(JsonArrayValues); It; ++It)
    {
        auto Value = (*It).Get();
        if (Value->Type != EJson::Number)
        {
            UE_LOG(LogIVJson, Error, TEXT("Not Number element in array with field name %s"), *FieldName);
        }

        NumberArray.Add((*It)->AsNumber());
    }

    return NumberArray;
}

void UIVJsonObject::SetNumberArrayField(const FString& FieldName, const TArray<float>& NumberArray)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }

    TArray< TSharedPtr<FJsonValue> > EntriesArray;

    for (auto Number : NumberArray)
    {
        EntriesArray.Add(MakeShareable(new FJsonValueNumber(Number)));
    }

    JsonObj->SetArrayField(FieldName, EntriesArray);
}

TArray<FString> UIVJsonObject::GetStringArrayField(const FString& FieldName)
{
    if (!JsonObj->HasTypedField<EJson::Array>(FieldName))
    {
        UE_LOG(LogIVJson, Warning, TEXT("No field with name %s of type Array"), *FieldName);
    }

    TArray<FString> StringArray;
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return StringArray;
    }

    TArray<TSharedPtr<FJsonValue> > JsonArrayValues = JsonObj->GetArrayField(FieldName);
    for (TArray<TSharedPtr<FJsonValue> >::TConstIterator It(JsonArrayValues); It; ++It)
    {
        auto Value = (*It).Get();
        if (Value->Type != EJson::String)
        {
            UE_LOG(LogIVJson, Error, TEXT("Not String element in array with field name %s"), *FieldName);
        }

        StringArray.Add((*It)->AsString());
    }

    return StringArray;
}

void UIVJsonObject::SetStringArrayField(const FString& FieldName, const TArray<FString>& StringArray)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }

    TArray< TSharedPtr<FJsonValue> > EntriesArray;
    for (auto String : StringArray)
    {
        EntriesArray.Add(MakeShareable(new FJsonValueString(String)));
    }

    JsonObj->SetArrayField(FieldName, EntriesArray);
}

TArray<bool> UIVJsonObject::GetBoolArrayField(const FString& FieldName)
{
    if (!JsonObj->HasTypedField<EJson::Array>(FieldName))
    {
        UE_LOG(LogIVJson, Warning, TEXT("No field with name %s of type Array"), *FieldName);
    }

    TArray<bool> BoolArray;
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return BoolArray;
    }

    TArray<TSharedPtr<FJsonValue> > JsonArrayValues = JsonObj->GetArrayField(FieldName);
    for (TArray<TSharedPtr<FJsonValue> >::TConstIterator It(JsonArrayValues); It; ++It)
    {
        auto Value = (*It).Get();
        if (Value->Type != EJson::Boolean)
        {
            UE_LOG(LogIVJson, Error, TEXT("Not Boolean element in array with field name %s"), *FieldName);
        }

        BoolArray.Add((*It)->AsBool());
    }

    return BoolArray;
}

void UIVJsonObject::SetBoolArrayField(const FString& FieldName, const TArray<bool>& BoolArray)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }

    TArray< TSharedPtr<FJsonValue> > EntriesArray;
    for (auto Boolean : BoolArray)
    {
        EntriesArray.Add(MakeShareable(new FJsonValueBoolean(Boolean)));
    }

    JsonObj->SetArrayField(FieldName, EntriesArray);
}

TArray<UIVJsonObject*> UIVJsonObject::GetObjectArrayField(const FString& FieldName)
{
    if (!JsonObj->HasTypedField<EJson::Array>(FieldName))
    {
        UE_LOG(LogIVJson, Warning, TEXT("No field with name %s of type Array"), *FieldName);
    }

    TArray<UIVJsonObject*> OutArray;
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return OutArray;
    }

    TArray< TSharedPtr<FJsonValue> > ValArray = JsonObj->GetArrayField(FieldName);
    for (auto Value : ValArray)
    {
        if (Value->Type != EJson::Object)
        {
            UE_LOG(LogIVJson, Error, TEXT("Not Object element in array with field name %s"), *FieldName);
        }

        TSharedPtr<FJsonObject> NewObj = Value->AsObject();

        UIVJsonObject* NewJson = NewObject<UIVJsonObject>();
        NewJson->SetRootObject(NewObj);

        OutArray.Add(NewJson);
    }

    return OutArray;
}

void UIVJsonObject::SetObjectArrayField(const FString& FieldName, const TArray<UIVJsonObject*>& ObjectArray)
{
    if (!JsonObj.IsValid() || FieldName.IsEmpty())
    {
        return;
    }

    TArray< TSharedPtr<FJsonValue> > EntriesArray;
    for (auto Value : ObjectArray)
    {
        EntriesArray.Add(MakeShareable(new FJsonValueObject(Value->GetRootObject())));
    }

    JsonObj->SetArrayField(FieldName, EntriesArray);
}

TSharedPtr<FJsonObject>& UIVJsonObject::GetRootObject()
{
    return JsonObj;
}

void UIVJsonObject::SetRootObject(const TSharedPtr<FJsonObject>& JsonObject)
{
    JsonObj = JsonObject;
}
