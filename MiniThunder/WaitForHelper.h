#pragma once

class CWaitForHelper
{
public:
	/**********************************
	描述: 处理Windows消息
	参数: 
	bDispatchQuitMsg	: 是否转发 WM_QUIT 消息
	TRUE : 转发（默认）
	FALSE: 不转发，并返回 FALSE

	返回值:		TRUE  : 收完消息
	FALSE : bDispatchQuitMsg 参数为 FALSE 并收到 WM_QUIT 消息		
	**********************************/
	virtual BOOL PeekMessageLoop(BOOL bDispatchQuitMsg = TRUE);

	/**********************************
	描述: 等待指定时间, 同时处理Windows消息
	参数: (参考: MsgWaitForMultipleObjectsEx() )
	dwHandles		: 数组元素个数
	szHandles		: 对象句柄数组
	dwMilliseconds	: 等待时间 (毫秒)
	dwWakeMask		: 消息过滤标识
	dwFlags			: 等待类型

	返回值: (0 ~ dwHandles - 1): 等待成功
	WAIT_TIMEOUT		: 超时
	WAIT_FAILED			: 执行失败
	**********************************/
	virtual DWORD WaitForMultipleObjectsWithMessageLoop(DWORD dwHandles, HANDLE szHandles[], DWORD dwMilliseconds = INFINITE, DWORD dwWakeMask = QS_ALLINPUT, DWORD dwFlags = MWMO_INPUTAVAILABLE);

	/**********************************
	描述: 等待指定时间, 同时处理Windows消息
	参数: (参考: MsgWaitForMultipleObjectsEx() )
	hHandle			: 对象句柄
	dwMilliseconds	: 等待时间 (毫秒)
	dwWakeMask		: 消息过滤标识
	dwFlags			: 等待类型

	返回值: TRUE: 等待成功，FALSE: 超时		
	**********************************/
	virtual BOOL MsgWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds = INFINITE, DWORD dwWakeMask = QS_ALLINPUT, DWORD dwFlags = MWMO_INPUTAVAILABLE);
};

