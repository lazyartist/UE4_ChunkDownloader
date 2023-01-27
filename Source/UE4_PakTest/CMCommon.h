#pragma once


DECLARE_LOG_CATEGORY_EXTERN(CM, Log, All);

#define CM_LOG_CALLINFO					(FString(__FUNCTION__)+ TEXT("("+ FString::FromInt(__LINE__) +TEXT(")")))
#define CM_LOG(Verbosity, Format, ...)	UE_LOG(CM, Verbosity, TEXT("%s %s"), *CM_LOG_CALLINFO, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

namespace CMUtils
{
	template<class T>
	static FString EnumToFString(const TCHAR* InEnumTypeName, const T EnumValue)
	{
		const UEnum* Enums = FindObject<UEnum>(ANY_PACKAGE, InEnumTypeName, true);
		if (Enums == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("Can't convert : %s, %d"), InEnumTypeName, static_cast<int32>(EnumValue));
			// CM_LOG(Error, "Can't convert : %s, %d", InEnumTypeName, static_cast<int32>(EnumValue));
			static FString FailString(TEXT("[Fail]"));
			return FailString;
		}
		return Enums->GetNameStringByValue(static_cast<int64>(EnumValue));
	}
}