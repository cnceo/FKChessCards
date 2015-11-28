#include "Include/stdafx.h"
#include "../Include/LogonServer.h"
#include "../Include/LogonServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//Ӧ�ó������
CLogonServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLogonServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CLogonServerApp::CLogonServerApp()
{
}

//��ʼ������
BOOL CLogonServerApp::InitInstance()
{
	__super::InitInstance();

	//��ʼ�����
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//����ע���
	SetRegistryKey(szSystemRegKey);

	//��ʾ������
	CLogonServerDlg ServerDlg;
	m_pMainWnd=&ServerDlg;
	ServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
