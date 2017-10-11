#include "StdAfx.h"
#include "WaitForHelper.h"

BOOL CWaitForHelper::PeekMessageLoop(BOOL bDispatchQuitMsg)
{
	BOOL value = TRUE;

	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT && bDispatchQuitMsg)
		{
			value = FALSE;
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return value;
}

DWORD CWaitForHelper::WaitForMultipleObjectsWithMessageLoop(DWORD dwHandles, HANDLE szHandles[], DWORD dwMilliseconds, DWORD dwWakeMask, DWORD dwFlags)
{
	DWORD dwResult = WAIT_FAILED;
	DWORD dwBeginTime = (dwMilliseconds == INFINITE) ? INFINITE : GetTickCount();

	while (TRUE)
	{
		int iWaitTime;
		if (dwBeginTime != INFINITE)
		{
			iWaitTime = dwMilliseconds - (GetTickCount() - (dwBeginTime));

			if (iWaitTime <= 0)
			{
				dwResult = WAIT_TIMEOUT;
				break;
			}
		}
		else
		{
			iWaitTime = INFINITE;
		}

		dwResult = MsgWaitForMultipleObjectsEx(dwHandles, szHandles, iWaitTime, dwWakeMask, dwFlags);
		ASSERT(dwResult != WAIT_FAILED);

		if (dwResult == (WAIT_OBJECT_0 + dwHandles))
		{
			if (!PeekMessageLoop(TRUE))
			{
				dwResult = WAIT_OBJECT_0;
				break;
			}
		}
		else
		{
			break;
		}	
	}

	return dwResult;
}

BOOL CWaitForHelper::MsgWaitForSingleObject(HANDLE hHandle, DWORD dwMilliseconds, DWORD dwWakeMask, DWORD dwFlags)
{
	DWORD dwResult = WaitForMultipleObjectsWithMessageLoop(1, &hHandle, dwMilliseconds, dwWakeMask, dwFlags);

	switch(dwResult)
	{
	case WAIT_OBJECT_0:
		return TRUE;
	case WAIT_FAILED:
		ASSERT(FALSE);
		return FALSE;
	case WAIT_TIMEOUT:
		return FALSE;
	default:
		ASSERT(FALSE);
	}

	return FALSE;
}