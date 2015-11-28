#include "Include/stdafx.h"
#include "AtlBase.h"
#include "../Include/LogonServerDlg.h"

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLogonServerDlg, CDialog)
	ON_BN_CLICKED(ID_START, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedStopService)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedInitService)
END_MESSAGE_MAP()

//���캯��
CLogonServerDlg::CLogonServerDlg() : CDialog(IDD_DIALOG1)
{
	m_bAskQuit=true;
}

//��������
CLogonServerDlg::~CLogonServerDlg()
{
}

//�ؼ����໯
void CLogonServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_RichEditCtrl);
}

//��ʼ������
BOOL CLogonServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//�������
	if (m_EventService.CreateInstance()==true)
	{
		IUnknownEx * pIUnknownEx=(IUnknownEx *)m_EventService->QueryInterface(IID_IUnknownEx,VER_IUnknownEx);
		m_EventService->SetRichEditHwnd(m_RichEditCtrl.GetSafeHwnd());
		m_LogonService.SetEventService(pIUnknownEx);
	}
	m_RichEditCtrl.SetBackgroundColor(FALSE,RGB(220,220,220));

	//��������
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]!=0)
	{
		OnBnClickedStartService();
	}

	__super::OnInitDialog();

	//��������
	((CEdit *)GetDlgItem(IDC_EDIT8))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_EDIT9))->LimitText(3);
	((CEdit *)GetDlgItem(IDC_EDIT5))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_EDIT2))->LimitText(5);
	((CEdit *)GetDlgItem(IDC_EDIT3))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_EDIT4))->LimitText(31);
	((CEdit *)GetDlgItem(IDC_EDIT10))->LimitText(31);

	//���ز���
	CInitParamter InitParamter;
	InitParamter.LoadInitParamter();

	//���ÿؼ�
	SetDlgItemInt(IDC_EDIT8,InitParamter.m_wListenPort,FALSE);
	SetDlgItemInt(IDC_EDIT9,InitParamter.m_wMaxConnect,FALSE);

	//��¼���ݿ�
	SetDlgItemInt(IDC_EDIT2,InitParamter.m_wUserDataBasePort,FALSE);
	SetDlgItemText(IDC_EDIT3,InitParamter.m_szUserDataBaseUser);
	SetDlgItemText(IDC_EDIT4,InitParamter.m_szUserDataBasePass);
	SetDlgItemText(IDC_EDIT10,InitParamter.m_szUserDataBaseName);

	//��¼���ݿ��ַ
	DWORD dwDataBaseIP=inet_addr(InitParamter.m_szUserDataBaseAddr);
	if (dwDataBaseIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(InitParamter.m_szUserDataBaseAddr);
		if (lpHost!=NULL) dwDataBaseIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	CIPAddressCtrl * pDataBaseIP=(CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS1);
	pDataBaseIP->SetAddress(ntohl(dwDataBaseIP));

	//��վ��ַ
	if (InitParamter.m_szMainPage[0]==0) SetDlgItemText(IDC_EDIT5,szStationPage);
	else SetDlgItemText(IDC_EDIT5,InitParamter.m_szMainPage);

	//���ķ�����
	dwDataBaseIP=inet_addr(InitParamter.m_szCenterServerAddr);
	if (dwDataBaseIP==INADDR_NONE)
	{
		LPHOSTENT lpHost=gethostbyname(InitParamter.m_szCenterServerAddr);
		if (lpHost!=NULL) dwDataBaseIP=((LPIN_ADDR)lpHost->h_addr)->s_addr;
	}
	pDataBaseIP=(CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS2);
	pDataBaseIP->SetAddress(ntohl(dwDataBaseIP));

	return TRUE;

	return TRUE;
}

//ȷ����Ϣ
void CLogonServerDlg::OnOK()
{
	return;
}

//ȡ������
void CLogonServerDlg::OnCancel()
{
	//��ȡ״̬
	bool bRuning=m_LogonService.IsService();

	//ѯ���˳�
	if ((m_bAskQuit==true)&&(bRuning==true))
	{
		CString strMessage=TEXT("��¼���������������У�ȷʵҪ�˳���");
		int iRetCode=AfxMessageBox(strMessage,MB_YESNO|MB_ICONQUESTION|MB_DEFBUTTON2);
		if (iRetCode!=IDYES) return;
	}

	//ֹͣ����
	m_bAskQuit=true;
	if (bRuning==true) m_LogonService.StopService();

	__super::OnCancel();
}

//��Ϣ����
BOOL CLogonServerDlg::PreTranslateMessage(MSG * pMsg)
{
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) return TRUE;
	return __super::PreTranslateMessage(pMsg);
}

//��������
void CLogonServerDlg::OnBnClickedStartService()
{
	if (m_LogonService.StartService() )
	{
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		GetDlgItem(ID_START)->EnableWindow(FALSE);
		if (m_EventService.GetInterface()) m_EventService->ShowEventNotify(TEXT("��¼���������ɹ�"),Level_Normal);
	}

	return;
}

//ֹͣ����
void CLogonServerDlg::OnBnClickedStopService()
{
	if (m_LogonService.StopService()==true)
	{
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
		GetDlgItem(ID_START)->EnableWindow(TRUE);
		if (m_EventService.GetInterface()) m_EventService->ShowEventNotify(TEXT("��¼����ֹͣ�ɹ�"),Level_Normal); 
	}

	return;
}

//ϵͳ����
void CLogonServerDlg::OnBnClickedInitService()
{
	//��ȡ����
	CInitParamter InitParamter;
	InitParamter.m_wListenPort=GetDlgItemInt(IDC_EDIT8);
	InitParamter.m_wMaxConnect=GetDlgItemInt(IDC_EDIT9);

	//��¼���ݿ� 
	InitParamter.m_wUserDataBasePort=GetDlgItemInt(IDC_EDIT2);
	GetDlgItemText(IDC_EDIT3,InitParamter.m_szUserDataBaseUser,sizeof(InitParamter.m_szUserDataBaseUser));
	GetDlgItemText(IDC_EDIT4,InitParamter.m_szUserDataBasePass,sizeof(InitParamter.m_szUserDataBasePass));
	GetDlgItemText(IDC_EDIT10,InitParamter.m_szUserDataBaseName,sizeof(InitParamter.m_szUserDataBaseName));

	//��¼���ݿ��ַ
	DWORD dwDataBaseIP=INADDR_NONE;
	BYTE * pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwDataBaseIP);
	_snprintf(InitParamter.m_szUserDataBaseAddr,sizeof(InitParamter.m_szUserDataBaseAddr),TEXT("%d.%d.%d.%d"),
		pAddrByte[3],pAddrByte[2],pAddrByte[1],pAddrByte[0]);

	//��վ��ַ
	GetDlgItemText(IDC_EDIT5,InitParamter.m_szMainPage,sizeof(InitParamter.m_szMainPage));
	if (lstrcmp(InitParamter.m_szMainPage,szStationPage)==0) InitParamter.m_szMainPage[0]=0;

	//���ķ�����
	dwDataBaseIP=INADDR_NONE;
	pAddrByte=(BYTE *)&dwDataBaseIP;
	((CIPAddressCtrl *)GetDlgItem(IDC_IPADDRESS2))->GetAddress(dwDataBaseIP);
	_snprintf(InitParamter.m_szCenterServerAddr,sizeof(InitParamter.m_szCenterServerAddr),TEXT("%d.%d.%d.%d"),
		pAddrByte[3],pAddrByte[2],pAddrByte[1],pAddrByte[0]);

	//��������
	InitParamter.SaveInitParamter(false);

	return;
}

//////////////////////////////////////////////////////////////////////////

