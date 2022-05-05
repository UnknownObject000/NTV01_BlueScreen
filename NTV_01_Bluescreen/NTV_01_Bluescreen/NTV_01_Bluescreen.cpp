// NTV_01_Bluescreen.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

//NtRaiseHardError演示程序

#include <iostream>
#include <Windows.h>	//Windows API
#include <SubAuth.h>	//PUNICODE_STRING结构体

//准备NtRaiseHardError所需的两个未公开的类型和函数指针，直接从PPT复制。
typedef enum _HARDERROR_RESPONSE_OPTION
{
	OptionAbortRetryIgnore,
	OptionOk,
	OptionOkCancel,
	OptionRetryCancel,
	OptionYesNo,
	OptionYesNoCancel,
	OptionShutdownSystem
} HARDERROR_RESPONSE_OPTION, * PHARDERROR_RESPONSE_OPTION;

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
} HARDERROR_RESPONSE, * PHARDERROR_RESPONSE;

//漏掉了一个PUNICODE_STRING，这个结构体在另一个头文件中
//typedef struct _UNICODE_STRING
//{
//	USHORT Length;
//	USHORT MaximumLength;
//	PWSTR Buffer;
//}UNICODE_STRING, *PUNICODE_STRING;

typedef UINT(CALLBACK* NTRAISEHARDERROR)(NTSTATUS, ULONG, PUNICODE_STRING, PVOID, HARDERROR_RESPONSE_OPTION, PHARDERROR_RESPONSE);

bool GetShutdownPrivilege()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES priv = { 0 };
	LUID ShutdownLUID;
	//打开当前进程的Token，用于调整特权的Token需要使用TOKEN_ADJUST_PRIVILEGES指定
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
		return false;
	//检查获取的Token是否正确
	if ((hToken == NULL) || (hToken == INVALID_HANDLE_VALUE))
		return false;
	//根据特权的名称查找LUID
	//SE_SHUTDOWN_NAME是Windows预定义的宏，值为shutdown特权的名称
	if (!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &ShutdownLUID))
		return false;
	//使用TOKEN_PRIVILIEGES结构体存储获取到的信息
	priv.PrivilegeCount = 1;	//需要更改的特权数 - 一个
	priv.Privileges[0].Luid = ShutdownLUID;	//存入LUID
	priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;	//指定启用该特权
	//调整Token的特权
	if (!AdjustTokenPrivileges(hToken, FALSE, &priv, NULL, NULL, NULL))
		return false;
	//判断是否所有特权修改都生效了
	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
		return false;
	return true;
}

UINT NtRaiseHardError(NTSTATUS ErrorStatus, ULONG NumberOfParameters, PUNICODE_STRING UnicodeStringParameterMask OPTIONAL, PVOID Parameters, HARDERROR_RESPONSE_OPTION ResponseOption, PHARDERROR_RESPONSE Response)
{
	//加载ntdll.dll
	HMODULE hNtdll = LoadLibrary(L"ntdll.dll");
	if (hNtdll == NULL)
		return -1;
	//GetProcAddress获取函数所在的地址并转换为函数指针
	NTRAISEHARDERROR _NtRaiseHardError = (NTRAISEHARDERROR)GetProcAddress(hNtdll, "NtRaiseHardError");
	//释放dll
	FreeLibrary(hNtdll);
	//传递参数，通过函数指针调用函数
	return _NtRaiseHardError(ErrorStatus, NumberOfParameters, UnicodeStringParameterMask, Parameters, ResponseOption, Response);
}

int main()
{
	//先获取Shutdown特权
	if (!GetShutdownPrivilege())
	{
		std::cout << "提权失败" << std::endl;
		return -1;
	}
	HARDERROR_RESPONSE Resp;
	//要制造蓝屏，我们只需要关注第一个参数和第五个参数
	//第一个参数是蓝屏代码，会在蓝屏界面中显示出来
	//第五个参数必须指定为OptionShutdownSystem
	//第六个参数返回的Resp并不需要理会，因为这个函数执行后电脑已经蓝屏了
	//需要注意的是，第一个参数的蓝屏代码一旦超出范围就有可能导致无法正常蓝屏
	//0xc00000021a(x)
	//0xc000021a(√)
	return NtRaiseHardError(0xc000021a, 0, NULL, NULL, OptionShutdownSystem, &Resp);
}