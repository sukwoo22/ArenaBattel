// Fill out your copyright notice in the Description page of Project Settings.


#include "ABMessage.h"

FABMsgLogHelp& FABMsgLogHelp::Get()
{
	static FABMsgLogHelp MsgLogHelp;
	return MsgLogHelp;
}

FString FABMsgLogHelp::GetEnumToString(EMessageID EnumValue)
{
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EMessageID"), true);
	if (!enumPtr)
	{
		return FString("Invalid");
	}
	return enumPtr->GetNameStringByIndex((int32)EnumValue);
}

int32 FABMsgLogHelp::FindFuncSignaureIndex(const FString& str)
{
	int cnt = 3;
	int i = 0;
	for (; i < str.Len(); ++i)
	{
		if (str[i] == ':')
		{
			--cnt;
			if (cnt == 0)
			{
				i += 2;
				break;
			}
		}
	}
	return i;
}

FString FABMsgLogHelp::GetFuncName(const FString& str)
{
	int index = FindFuncSignaureIndex(str);
	FString FuncName = str.Left(index);
	return FuncName;
}
