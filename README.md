# NTV01_BlueScreen
NTAPI制造蓝屏视频源代码，用于演示未公开的NtAPI - NtRaiseHardError

## 关于NtRaiseHardError

### 函数原型
```c++
UINT NtRaiseHardError(NTSTATUS ErrorStatus, ULONG NumberOfParameters, PUNICODE_STRING UnicodeStringParameterMask OPTIONAL, PVOID Parameters, HARDERROR_RESPONSE_OPTION ResponseOption, PHARDERROR_RESPONSE Response);
```

### 函数指针类型
```c++
typedef UINT(CALLBACK* typeNtRaiseHardError)(NTSTATUS, ULONG, PUNICODE_STRING, PVOID, HARDERROR_RESPONSE_OPTION, PHARDERROR_RESPONSE);
```

### 未导出类型
 - PHARDERROR_RESPONSE_OPTION
```c++
typedef enum _HARDERROR_RESPONSE_OPTION
{
	OptionAbortRetryIgnore,
	OptionOk,
	OptionOkCancel,
	OptionRetryCancel,
	OptionYesNo,
	OptionYesNoCancel,
	OptionShutdownSystem
} HARDERROR_RESPONSE_OPTION, *PHARDERROR_RESPONSE_OPTION;
```

 - PHARDERROR_RESPONSE
```c++
typedef enum _HARDERROR_RESPONSE 
{
	ResponseReturnToCaller,
	ResponseNotHandled,
	ResponseAbort,
	ResponseCancel,
	ResponseIgnore,
	ResponseNo,
	ResponseOk,
	ResponseRetry,
	ResponseYes
} HARDERROR_RESPONSE, *PHARDERROR_RESPONSE;
```

 - PUNICODE_STRING
```c++
#include <SubAuth.h>
```
或
```c++
typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
}UNICODE_STRING, *PUNICODE_STRING;
```

### 所在DLL
 - ntdll.dll
