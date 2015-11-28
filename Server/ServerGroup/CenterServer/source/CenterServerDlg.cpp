#include "Include/stdafx.h"
#include "AtlBase.h"
#include "../include/CenterServerDlg.h"
#include "../resource.h"


//BEGIN_MESSAGE_MAP(CSystemOptionDlg, CDialog)
//END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CCenterServerDlg, CDialog)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_INIT_SERVICE, OnBnClickedInitService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
END_MESSAGE_MAP()

////
////CSystemOptionDlg
////
//CSystemOptionDlg::CSystemOptionDlg() : CDialog(IDD_SYSTEM_OPTION)
//{
//}
//
//CSystemOptionDlg::~CSystemOptionDlg()
//{
//}
//
////�ؼ����໯
//void CSystemOptionDlg::DoDataExchange(CDataExchange * pDX)
//{
//	__super::DoDataExchange(pDX);
//}
//
////��ʼ������
//BOOL CSystemOptionDlg::OnInitDialog()
//{
//	__super::OnInitDialog();
//
//	//��������
//	((CEdit *)GetDlgItem(IDC_LISTEN_PORT))->LimitText(5);
//	((CEdit *)GetDlgItem(IDC_MAX_CONNECT))->LimitText(3);
//	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_USER))->LimitText(31);
//	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_PASS))->LimitText(31);
//	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_NAME))->LimitText(31);
//
//	//���ز���
//	CInitParamter InitParamter;
//	InitParamter.LoadInitParamter();
//
//	//���ÿؼ�
//	SetDlgItemInt(IDC_LISTEN_PORT,InitParamter.m_wListenPort,FALSE);
//	SetDlgItemInt(IDC_MAX_CONNECT,InitParamter.m_wMaxConnect,FALSE);
//
//	//��Ϣ���ݿ�
//	SetDlgItemText(IDC_SERVER_DATABASE_USER,InitParamter.m_szServerDataBaseUser);
//	SetDlgItemText(IDC_SERVER_DATABASE_PASS,InitParamter.m_szServerDataBasePass);
//	SetDlgItemText(IDC_SERVER_DATABASE_NAME,InitParamter.m_szServerDataBaseName);
//
//	//��Ϣ���ݿ��ַ
//	DWORD dwDataBaseIP=inet_addr(InitParamter.m_szServerDataBaseAddr);
//	if (dwDataBaseIP==INADDR_NONE)
//	{
//		LPHOSTENT lpHost=gethostbyname(InitParamter.m_szServerDataBaseAddr);
//		if (lpHost!=NULL) dwDataBaseIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
//	}
//	CIPAddressCtrl * pDataBaseIP=(CIPAddressCtrl *)GetDlgItem(IDC_SERVER_DATABASE_IP);
//	pDataBaseIP->SetAddress(ntohl(dwDataBaseIP));
//
//	return TRUE;
//}
//
////ȷ������
//void CSystemOptionDlg::OnOK()
//{
//	//��ȡ����
//	CInitParamter InitParamter;
//	InitParamter.m_wListenPort=GetDlgItemInt(IDC_LISTEN_PORT);
//	InitParamter.m_wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);
//
//	//��Ϣ���ݿ�
//	GetDlgItemText(IDC_SERVER_DATABASE_USER,InitParamter.m_szServerDataBaseUser,sizeof(InitParamter.m_szServerDataBaseUser));
//	GetDlgItemText(IDC_SERVER_DATABASE_PASS,InitParamter.m_szServerDataBasePass,sizeof(InitParamter.m_szServerDataBasePass));
//	GetDlgItemText(IDC_SERVER_DATABASE_NAME,InitParamter.m_szServerDataBaseName,sizeof(InitParamter.m_szServerDataBaseName));
//
//	//��Ϣ���ݿ��ַ
//	DWORD dwDataBaseIP=INADDR_NONE;
//	BYTE * pAddrByte=(BYTE *)&dwDataBaseIP;
//	((CIPAddressCtrl *)GetDlgItem(IDC_SERVER_DATABASE_IP))->GetAddress(dwDataBaseIP);
//	_snprintf(InitParamter.m_szServerDataBaseAddr,sizeof(InitParamter.m_szServerDataBaseAddr),TEXT("%d.%d.%d.%d"),
//		pAddrByte[3],pAddrByte[2],pAddrByte[1],pAddrByte[0]);
//
//	//��������
//	InitParamter.SaveInitParamter(false);
//
//	__super::OnOK();
//}


//
//CCenterServerDlg
//
CCenterServerDlg::CCenterServerDlg() : CDialog(IDD_DIALOG1)
{
	m_bAskQuit=true;
}

CCenterServerDlg::~CCenterServerDlg()
{
}

//�ؼ����໯
void CCenterServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVICE_EVENT, m_RichEditCtrl);
}

//��ʼ������
BOOL CCenterServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//�������
	if (m_EventService.CreateInstance() )
	{
		IUnknownEx * pIUnknownEx=(IUnknownEx *)m_EventService->QueryInterface(IID_IUnknownEx,VER_IUnknownEx);
		m_EventService->SetRichEditHwnd(m_RichEditCtrl.GetSafeHwnd());
		m_CenterService.SetEventService(pIUnknownEx);
	}

	m_RichEditCtrl.SetBackgroundColor(FALSE,RGB(220,220,220));

	//��������
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]!=0)
	{
		OnBnClickedStartService();
	}


	//��������
	((CEdit *)GetDlgItem(IDC_LISTEN_PORT))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_MAX_CONNECT))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_USER))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_PASS))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_SERVER_DATABASE_NAME))->LimitText(31);

	//���ز���
	CInitParamter InitParamter;
	InitParamter.LoadInitParamter();

	//���ÿؼ�
	SetDlgItemInt(IDC_LISTEN_PORT,InitParamter.m_wListenPort,FALSE);
	SetDlgItemInt(IDC_MAX_CONNECT,InitParamter.m_wMaxConnect,FALSE);

	//��Ϣ���ݿ�
	SetDlgItemText(IDC_SERVER_DATABASE_USER,InitParamter.m_szServerDataBaseUser);
	SetDlgItemText(IDC_SERVER_DATABASE_PASS,InitParamter.m_szServerDataBasePass);
	SetDlgItemText(IDC_SERVER_DATABASE_NAME,InitParamter.m_szServerDataBaseName);

	//��Ϣ���ݿ��ַ
	DWORD dwDataBaseIP=inet_addr(InitParamter.m_szServerDataBaseAddr);
	if (dwDataBaseIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(InitParamter.m_szServerDataBaseAddr);
		if (lpHost!=NULL) dwDataBaseIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	CIPAddressCtrl * pDataBaseIP=(CIPAddressCtrl *)GetDlgItem(IDC_SERVER_DATABASE_IP);
	pDataBaseIP->SetAddress(ntohl(dwDataBaseIP));

	return TRUE;
}

//ȷ����Ϣ
void CCenterServerDlg::OnOK()
{
	return;
}

//ȡ������
void CCenterServerDlg::OnCancel()
{
	//��ȡ״̬
	bool bRuning=m_CenterService.IsService();

	//ѯ���˳�
	if ((m_bAskQuit==true)&&(bRuning==true))
	{
		CString strMessage=TEXT("���ķ��������������У�ȷʵҪ�˳���");
		int iRetCode=AfxMessageBox(strMessage,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if (iRetCode!=IDYES) 
		{
			return;
		}
	}

	//ֹͣ����
	m_bAskQuit=true;
	if (bRuning==true) 
	{
		m_CenterService.StopService();
	}

	__super::OnCancel();
}

//��Ϣ����
BOOL CCenterServerDlg::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) 
	{
		return TRUE;
	}
	
	return __super::PreTranslateMessage(pMsg);
}

//��������
void CCenterServerDlg::OnBnClickedStartService()
{
	if (m_CenterService.StartService()==true)
	{
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
		if (m_EventService.GetInterface()) 
		{
			m_EventService->ShowEventNotify(TEXT("���ķ��������ɹ�"),Level_Normal);
		}
	}

	return;
}

//ֹͣ����
void CCenterServerDlg::OnBnClickedStopService()
{
	if (m_CenterService.StopService()==true)
	{
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
		if (m_EventService.GetInterface()) 
		{
			m_EventService->ShowEventNotify(TEXT("���ķ���ֹͣ�ɹ�"),Level_Normal); 
		}
	}

	return;
}

//ϵͳ����
void CCenterServerDlg::OnBnClickedInitService()
{

	//��ȡ����
	CInitParamter InitParamter;
	InitParamter.m_wListenPort=GetDlgItemInt(IDC_LISTEN_PORT);
	InitParamter.m_wMaxConnect=GetDlgItemInt(IDC_MAX_CONNECT);

	//��Ϣ���ݿ�
	GetDlgItemText(IDC_SERVER_DATABASE_USER,InitParamter.m_szServerDataBaseUser,sizeof(InitParamter.m_szServerDataBaseUser));
	GetDlgItemText(IDC_SERVER_DATABASE_PASS,InitParamter.m_szServerDataBasePass,sizeof(InitParamter.m_szServerDataBasePass));
	GetDlgItemText(IDC_SERVER_DATABASE_NAME,InitParamter.m_szServerDataBaseName,sizeof(InitParamter.m_szServerDataBaseName));

	//��Ϣ���ݿ��ַ
	DWORD dwDataBaseIP=INADDR_NONE;
	BYTE * pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_SERVER_DATABASE_IP))->GetAddress(dwDataBaseIP);
	_snprintf(InitParamter.m_szServerDataBaseAddr,sizeof(InitParamter.m_szServerDataBaseAddr),TEXT("%d.%d.%d.%d"),
		pAddrByte[3],pAddrByte[2],pAddrByte[1],pAddrByte[0]);

	//��������
	InitParamter.SaveInitParamter(false);

	return;
}


//-----------------------------------------------
//					the end
//-----------------------------------------------