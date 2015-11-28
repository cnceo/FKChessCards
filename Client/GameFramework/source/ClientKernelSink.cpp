#include "Include/Stdafx.h"
#include "../include/GameFrameDlg.h"
#include "../include/GameFrameControl.h"
#include "../include/ClientKernelSink.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CClientKernelSink::CClientKernelSink(CGameFrameDlg & GameFrameDlg) : m_GameFrameDlg(GameFrameDlg)
{
}

//��������
CClientKernelSink::~CClientKernelSink()
{
}

//�ӿڲ�ѯ
void * __cdecl CClientKernelSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientKernelSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IClientKernelSink,Guid,dwQueryVer);
	return NULL;
}

//��ȡ����
HWND __cdecl CClientKernelSink::GetFrameWnd()
{
	return m_GameFrameDlg.m_hWnd;
}

//����״̬
void __cdecl CClientKernelSink::ResetGameFrame()
{
	//���ý���
	m_GameFrameDlg.m_pGameFrameView->ResetData();
	m_GameFrameDlg.m_pGameFrameView->ResetGameView();
	m_GameFrameDlg.ResetGameFrame();
	m_GameFrameDlg.m_pGameFrameControl->ResetGameFrameControl();

	return;
}

//�رմ���
void __cdecl CClientKernelSink::CloseGameFrame(bool bForceClose)
{
	if (m_GameFrameDlg.m_hWnd!=NULL) m_GameFrameDlg.PostMessage(WM_CLOSE,0,0);
	return;
}

//ʱ����Ϣ
bool __cdecl CClientKernelSink::OnEventTimer(WORD wChairID, UINT nElapse, UINT nTimerID)
{
	//��������
	WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(wChairID);
	m_GameFrameDlg.m_pGameFrameView->SetUserTimer(wViewChairID,nElapse);

	//�¼�֪ͨ
	return m_GameFrameDlg.OnTimerMessage(wChairID,nElapse,nTimerID);
}

//������Ϣ
bool __cdecl CClientKernelSink::OnEventSocket(const CMD_Command & Command, const void * pBuffer, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GF_FRAME:	//�����Ϣ
		{
			return m_GameFrameDlg.OnFrameMessage(Command.wSubCmdID,pBuffer,wDataSize);
		}
	case MDM_GF_GAME:	//��Ϸ��Ϣ
		{
			return m_GameFrameDlg.OnGameMessage(Command.wSubCmdID,pBuffer,wDataSize);
		}
	}

	return false;
}

//������Ϣ
bool __cdecl CClientKernelSink::OnEventProcess(const IPC_Head & Head, const void * pBuffer, WORD wDataSize, HWND hWndSend)
{
	return false;
}

//��Ϸ״̬
bool __cdecl CClientKernelSink::OnEventGameScene(BYTE cbGameStation, bool bLookonOther, void * pBuffer, WORD wDataSize)
{
	return m_GameFrameDlg.OnGameSceneMessage(cbGameStation,bLookonOther,pBuffer,wDataSize);
}

//������Ϣ
bool __cdecl CClientKernelSink::OnUserChatMessage(tagUserData * pSendUserData, tagUserData * pRecvUserData, LPCTSTR pszChatMessage, COLORREF crColor)
{
	//��ʾ��Ϣ
	if (pRecvUserData==NULL) m_GameFrameDlg.m_MessageProxyHelper->InsertUserChat(pSendUserData->szName,pszChatMessage,crColor,MS_NORMAL);
	else m_GameFrameDlg.m_MessageProxyHelper->InsertUserChat(pSendUserData->szName,pRecvUserData->szName,pszChatMessage,crColor,MS_NORMAL);

	return true;
}

//�û�����
void __cdecl CClientKernelSink::OnEventUserEnter(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	m_GameFrameDlg.m_pGameFrameControl->m_UserListView.m_wKindID=m_GameFrameDlg.m_ClientKernelHelper->GetServerAttribute()->wKindID;
	m_GameFrameDlg.m_pGameFrameControl->m_UserListView.m_wGameGenre=m_GameFrameDlg.m_ClientKernelHelper->GetServerAttribute()->wGameGenre;


	//���ý���
	m_GameFrameDlg.m_pGameFrameControl->InsertUserItem(pUserData);
	if (bLookonUser==false)
	{
		WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(wChairID);
		m_GameFrameDlg.m_pGameFrameView->SetUserInfo(wViewChairID,pUserData);
	}



	//�����û�
	if (pUserData!=m_GameFrameDlg.m_ClientKernelHelper->GetMeUserInfo())
	{
		int nItem=m_GameFrameDlg.m_pGameFrameControl->m_ChatObject.AddString(pUserData->szName);
		m_GameFrameDlg.m_pGameFrameControl->m_ChatObject.SetItemData(nItem,pUserData->dwUserID);


	}

	return;
}

//�û��뿪
void __cdecl CClientKernelSink::OnEventUserLeft(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	//���ý���
	m_GameFrameDlg.m_pGameFrameControl->DeleteUserItem(pUserData);
	if (bLookonUser==false)
	{
		WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(wChairID);
		m_GameFrameDlg.m_pGameFrameView->SetUserInfo(wViewChairID,NULL);
	}

	m_GameFrameDlg.m_MessageProxyHelper->InsertUserName(pUserData->szName);
	m_GameFrameDlg.m_MessageProxyHelper->InsertGeneralString(TEXT("�뿪��"),0,0,true);

	//ɾ���û�
	CComboBox * pChatObject=&m_GameFrameDlg.m_pGameFrameControl->m_ChatObject;
	int nCount=pChatObject->GetCount();
	for (int i=1;i<nCount;i++)
	{
		if (pChatObject->GetItemData(i)==pUserData->dwUserID)
		{
			if (pChatObject->GetCurSel()==i) pChatObject->SetCurSel(0);
			pChatObject->DeleteString(i);
			break;
		}
	}

	return;
}

//�û�����
void __cdecl CClientKernelSink::OnEventUserScore(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	//���ý���
	m_GameFrameDlg.m_pGameFrameControl->UpdateUserItem(pUserData);
	if (bLookonUser==false) m_GameFrameDlg.m_pGameFrameView->UpdateGameView(NULL);

	return;
}

//�û�״̬
void __cdecl CClientKernelSink::OnEventUserStatus(tagUserData * pUserData, WORD wChairID, bool bLookonUser)
{
	//���ý���
	m_GameFrameDlg.m_pGameFrameControl->UpdateUserItem(pUserData);
	if (bLookonUser==false) m_GameFrameDlg.m_pGameFrameView->UpdateGameView(NULL);

	return;
}

//ʱ�����
void __cdecl CClientKernelSink::OnEventTimerKilled(WORD wChairID, UINT nTimerID)
{
	//���ý���
	WORD wViewChairID=m_GameFrameDlg.SwitchViewChairID(wChairID);
	m_GameFrameDlg.m_pGameFrameView->SetUserTimer(wViewChairID,0);

	return;
}

//�Թ�״̬
void __cdecl CClientKernelSink::OnEventLookonChanged(bool bLookonUser, void * pBuffer, WORD wDataSize)
{
	return;
}
//��ʼ����Ƶ
bool __cdecl CClientKernelSink::OnInitVideo()
{
	return m_GameFrameDlg.OnInitVideo( );
}

//////////////////////////////////////////////////////////////////////////
