#include "stdafx.h"
#include "MiniThunderDlg.h"
#include "afxdialogex.h"
#include <windows.h>

class CryHelp : public CDialogEx
{
public:
	CryHelp();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CryHelp::CryHelp() : CDialogEx(CryHelp::IDD)
{
}

void CryHelp::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CryHelp, CDialogEx)
END_MESSAGE_MAP()


// CMiniThunderDlg 对话框



CMiniThunderDlg::CMiniThunderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMiniThunderDlg::IDD, pParent), m_DownEngine(L"./xldl.dll"), m_DownCancel(false), m_Downloading(false), m_Event(FALSE, TRUE)
{
	m_uTimerIdForQuery = 1;
	m_uElapseForQuery = 60;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMiniThunderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_RECV_SPEED, m_Recv_Speed);
	DDX_Control(pDX, IDC_STATIC_RECV_PRECENT, m_Recv_Precent);
	DDX_Control(pDX, IDC_STATIC_FILE_TOTALSIZE, m_File_TotalSize);
	DDX_Control(pDX, IDC_STATIC_RECV_TOTALDOWNLOAD, m_Recv_TotalDownload);
	DDX_Control(pDX, IDC_STATIC_SHARE_SPEED, m_Share_Speed);
	DDX_Control(pDX, IDC_STATIC_SHARE_COUNT, m_Share_Count);
	DDX_Control(pDX, IDC_DOWNLOAD_PROGRESS, m_Download_Progress);
}

BEGIN_MESSAGE_MAP(CMiniThunderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_STAR_DOWN, &CMiniThunderDlg::OnBnClickedButtonStarDown)
	ON_BN_CLICKED(IDC_BUTTON_STOP_DOWN, &CMiniThunderDlg::OnBnClickedButtonStopDown)
END_MESSAGE_MAP()


// CMiniThunderDlg 消息处理程序

BOOL CMiniThunderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	if (!m_DownEngine.InitEngine())
	{
		AfxMessageBox("更新引擎初始化失败\n请前往官方网站下载最新版本");
	}
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}
void CMiniThunderDlg::OnClose()
{
	PostMessage(WM_QUIT);
	CDialog::OnClose();
}
void CMiniThunderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CryHelp dlg;
		dlg.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CMiniThunderDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CMiniThunderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CMiniThunderDlg::PrintTaskInfo(DOWN_TASK_STATUS Status, DownTaskInfo info)
{
	switch (Status)
	{
	case NOITEM:
	{
		SetWindowText("未知错误");
		break;
	}
	case TSC_ERROR:
	{
		PrintTaskError(info.fail_code, info);
		m_DownEngine.TaskDelete(m_hTask);
		DownTaskParam deleteParam;
		StrCpyW(deleteParam.szFilename, info.szFilename);
		StrCpyW(deleteParam.szSavePath, TaskParam.szSavePath);
		m_DownEngine.DelTempFile(deleteParam);
		break;
	}
	case TSC_PAUSE:
	{
		SetWindowText("暂停下载");
		break;
	}
	case TSC_DOWNLOAD:
	{
		std::wstring szBuffer;
		szBuffer += L"正在下载";
		szBuffer += L":";
		szBuffer += info.szFilename;
		SetWindowTextW(m_hWnd, szBuffer.c_str());
		break;
	}
	case TSC_COMPLETE:
	{
		SetWindowText("下载完成");
		break;
	}
	case TSC_STARTPENDING:
	{
		SetWindowText("发送请求");
		break;
	}
	case TSC_STOPPENDING:
	{
		SetWindowText("结束请求");
		break;
	}
	}
}
void CMiniThunderDlg::PrintTaskError(TASK_ERROR_TYPE status, DownTaskInfo info)
{
	switch (status)
	{
	case TASK_ERROR_UNKNOWN:
	{
		SetWindowText("未知错误");
		break;
	}
	case TASK_ERROR_DISK_CREATE:
	{
		SetWindowText("创建文件失败");
		break;
	}
	case TASK_ERROR_DISK_WRITE:
	{
		SetWindowText("写文件失败");
		break;
	}
	case TASK_ERROR_DISK_READ:
	{
		SetWindowText("读文件失败");
		break;
	}
	case TASK_ERROR_DISK_RENAME:
	{
		SetWindowText("重命名失败");
		break;
	}
	case TASK_ERROR_DISK_PIECEHASH:
	{
		SetWindowText("文件片校验失败");
		break;
	}
	case TASK_ERROR_DISK_FILEHASH:
	{
		SetWindowText("文件全文校验失败");
		break;
	}
	case TASK_ERROR_DISK_DELETE:
	{
		SetWindowText("删除文件失败失败");
		break;
	}
	case TASK_ERROR_DOWN_INVALID:
	{
		SetWindowText("无效的更新地址");
		break;
	}
	case TASK_ERROR_PROXY_AUTH_TYPE_UNKOWN:
	{
		SetWindowText("代理类型未知");
		break;
	}
	case TASK_ERROR_PROXY_AUTH_TYPE_FAILED:
	{
		SetWindowText("代理认证失败");
		break;
	}
	case TASK_ERROR_HTTPMGR_NOT_IP:
	{
		SetWindowText("更新中无可用资源");
		break;
	}
	case TASK_ERROR_TIMEOUT:
	{
		SetWindowText("更新任务超时");
		break;
	}
	case TASK_ERROR_CANCEL:
	{
		SetWindowText("更新任务取消");
		break;
	}
	case TASK_ERROR_TP_CRASHED:
	{
		SetWindowText("MINITP崩溃");
		break;
	}
	case TASK_ERROR_ID_INVALID:
	{
		SetWindowText("TaskId 非法");
		break;
	}
	}
}
void CMiniThunderDlg::OnTimer(UINT nIDEvent)
{
	DownTaskInfo info;
	TCHAR szBuffer[32];
	if (m_Downloading)
	{
		if (m_DownEngine.TaskQueryInfoEx(m_hTask, info))
		{
			PrintTaskInfo(info.stat, info);
			_stprintf(szBuffer, "当前速度：%0.2fMbps", ((info.nSpeed / 1024.0f) / 1024.0f));
			m_Recv_Speed.SetWindowText(szBuffer);
			_stprintf(szBuffer, "已经更新：%0.2f%%", info.fPercent * 100.0f);
			m_Recv_Precent.SetWindowText(szBuffer);
			_stprintf(szBuffer, "文件大小：%0.2fMB", ((info.nTotalSize / 1024.0f) / 1024.0f));
			m_File_TotalSize.SetWindowText(szBuffer);
			_stprintf(szBuffer, "更新进度：%0.2fMB", ((info.nTotalDownload / 1024.0f) / 1024.0f));
			m_Recv_TotalDownload.SetWindowText(szBuffer);
			_stprintf(szBuffer, "资源加速：%0.2fMB", (((info.nTotalDownload - info.nDonationOrgin) / 1024.0f) / 1024.0f));
			m_Share_Speed.SetWindowText(szBuffer);
			_stprintf(szBuffer, "资源数量：%d/%d", info.nSrcUsing, info.nSrcTotal);
			m_Share_Count.SetWindowText(szBuffer);
			m_Download_Progress.SetPos((UINT)(info.fPercent * 100.0f));
		}
		if (info.stat == TSC_COMPLETE || info.stat == TSC_ERROR || info.fail_code != TASK_ERROR_UNKNOWN)
		{
			m_Downloading = false;
			m_Event.SetEvent();
			KillTimer(m_uTimerIdForQuery);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}
bool CMiniThunderDlg::InitDownEngine(const std::wstring url, const std::wstring exe)
{
	m_Download_Progress.SetPos(0);
	m_Downloading = true;
	m_Event.ResetEvent();

	TaskParam = m_DownEngine.CreateDownTaskParam(url.c_str(), exe.c_str(), L"./");
	m_hTask = m_DownEngine.TaskCreate(TaskParam);
	if (m_DownEngine.TaskStart(m_hTask))
	{
		SetTimer(m_uTimerIdForQuery, m_uElapseForQuery, NULL);
		return true;
	}
	return false;
}

bool CMiniThunderDlg::InitDownEngine(const std::string url)
{
	std::wstring UrlBuffer(url.length(), L' ');
	std::copy(url.begin(), url.end(), UrlBuffer.begin());
	return InitDownEngine(UrlBuffer, UrlBuffer.substr(UrlBuffer.find_last_of('/') + 1));
}

void CMiniThunderDlg::OnBnClickedButtonStarDown()
{
	CString szBuffer;
	if (GetDlgItemText(IDC_BUTTON_STAR_DOWN, szBuffer))
	{
		if (szBuffer == _T("开始下载"))
		{
			GetDlgItem(IDC_BUTTON_STAR_DOWN)->EnableWindow(FALSE);
			m_DownCancel = false;

			CString lpszBuffer;
			if (GetDlgItemText(IDC_EDIT_URL, lpszBuffer))
			{
				if (InitDownEngine(lpszBuffer.GetBuffer()))
				{
					GetDlgItem(IDC_BUTTON_STOP_DOWN)->EnableWindow(TRUE);
					if (!WaitForHelper.MsgWaitForSingleObject(m_Event.m_hObject))
					{
						AfxMessageBox("Event Init Fail");
					}
				}
			}
			else {
				AfxMessageBox("请输入URL");
			}
			GetDlgItem(IDC_BUTTON_STAR_DOWN)->EnableWindow(TRUE);
		}
		if (szBuffer == _T("停止下载"))
		{
			if (!m_Downloading && m_hTask)
			{
				if (m_DownEngine.TaskStop(m_hTask))
				{
					m_DownCancel = true;
					m_Downloading = false;
					KillTimer(m_uTimerIdForQuery);
					m_Event.SetEvent();
					m_DownEngine.TaskDelete(m_hTask);
					SetDlgItemText(IDC_BUTTON_STOP_DOWN, "暂停下载");
					SetDlgItemText(IDC_BUTTON_STAR_DOWN, "开始下载");
					GetDlgItem(IDC_BUTTON_STOP_DOWN)->EnableWindow(FALSE);
					m_Download_Progress.SetPos(0);
					m_Recv_Speed.SetWindowText("当前速度：");
					m_Recv_Precent.SetWindowText("已经下载：");
					m_File_TotalSize.SetWindowText("文件大小：");
					m_Recv_TotalDownload.SetWindowText("更新进度：");
					m_Share_Speed.SetWindowText("资源加速：");
					m_Share_Count.SetWindowText("资源数量：");
					SetWindowText("MiniThunder");
				}
			}
		}
	}

}


void CMiniThunderDlg::OnBnClickedButtonStopDown()
{
	CString szBuffer;
	if (GetDlgItemText(IDC_BUTTON_STOP_DOWN, szBuffer))
	{
		if (szBuffer == _T("暂停下载"))
		{
			if (m_Downloading && m_hTask)
			{
				if (m_DownEngine.TaskForceStop(m_hTask))
				{
					SetDlgItemText(IDC_BUTTON_STOP_DOWN, "继续下载");
					SetDlgItemText(IDC_BUTTON_STAR_DOWN, "停止下载");
					GetDlgItem(IDC_BUTTON_STAR_DOWN)->EnableWindow(TRUE);
					m_DownCancel = true;
					m_Downloading = false;
				}
			}
		}
		if (szBuffer == _T("继续下载"))
		{
			if (m_DownCancel && m_hTask)
			{
				if (m_DownEngine.TaskStart(m_hTask))
				{
					//SetTimer(m_uTimerIdForQuery, m_uElapseForQuery, NULL);
					m_DownCancel = false;
					m_Downloading = true;
					SetDlgItemText(IDC_BUTTON_STOP_DOWN, "暂停下载");
					GetDlgItem(IDC_BUTTON_STAR_DOWN)->EnableWindow(FALSE);
				}
			}
		}
	}
}
