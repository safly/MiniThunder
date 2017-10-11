
// MiniThunder.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "CryMain.h"
#include "MiniThunderDlg.h"

CryMain theApp;

BEGIN_MESSAGE_MAP(CryMain, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CryMain::CryMain()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

BOOL CryMain::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();
	AfxEnableControlContainer();
	CShellManager *pShellManager = new CShellManager;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	CMiniThunderDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{

	}
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}
	return FALSE;
}

