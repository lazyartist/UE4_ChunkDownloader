#pragma once


DECLARE_LOG_CATEGORY_EXTERN(CM, Log, All);

#define CM_LOG_CALLINFO					(FString(__FUNCTION__)+ TEXT("("+ FString::FromInt(__LINE__) +TEXT(")")))
#define CM_LOG(Verbosity, Format, ...)	UE_LOG(CM, Verbosity, TEXT("%s %s"), *CM_LOG_CALLINFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))
