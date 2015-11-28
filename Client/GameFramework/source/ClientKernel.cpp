#include "Include/Stdafx.h"
#include "../include/ClientKernel.h"
#include "../include/GameFrameControl.h"
#include "../include/GlobalOption.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CClientKernel, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()



//////////////////////////////////////////////////////////////////////////

//���캯��
CClientKernel::CClientKernel()
{
	//������Ϣ
	m_dwUserID=0L;
	m_wTableID=INVALID_TABLE;
	m_wChairID=INVALID_CHAIR;
	memset(&m_OptionBuffer,0,sizeof(m_OptionBuffer));
	memset(&m_ServerAttribute,0,sizeof(m_ServerAttribute));

	//��Ϸ��Ϣ
	m_bInquire=true;
	m_bLookonMode=false;
	m_pMeUserItem=NULL;
	m_bGameStatus=GS_FREE;
	memset(m_pUserItem,0,sizeof(m_pUserItem));

	//������Ϣ
	m_bAllowLookon=false;

	//��ʱ����Ϣ
	m_nTimerID=0;
	m_nElapseCount=0;
	m_hWndGameFrame=NULL;
	m_wTimerChairID=INVALID_CHAIR;

	//ͨѶ����
	m_hShareMemory=NULL;
	m_pShareMemory=NULL;

	//�ӿ�ָ��
	m_pIMessageProxy=NULL;
	m_pIClientKernelSink=NULL;

	return;
}

//��������
CClientKernel::~CClientKernel(void)
{
	//�����ڴ�
	if (m_pShareMemory!=NULL)
	{
		UnmapViewOfFile(m_pShareMemory);
		m_pShareMemory=NULL;
	}
	if (m_hShareMemory!=NULL)
	{
		CloseHandle(m_hShareMemory);
		m_hShareMemory=NULL;
	}

	return;
}

//�ӿڲ�ѯ
void * __cdecl CClientKernel::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IClientKernel,Guid,dwQueryVer);
	QUERYINTERFACE(IChannelMessageSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IClientKernel,Guid,dwQueryVer);
	return NULL;
}

//���ýӿ�
bool __cdecl CClientKernel::SetMessageProxy(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIMessageProxy=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IMessageProxy);
	ASSERT(m_pIMessageProxy!=NULL);
	return (m_pIMessageProxy!=NULL);
}

//�ں�����
bool __cdecl CClientKernel::InitClientKernel(LPCTSTR lpszComLine, IUnknownEx * pIUnknownEx)
{
	//Ч�����
	ASSERT(lpszComLine!=NULL);
	if (lpszComLine==NULL) return false;

	//��������
	if (m_hWnd==NULL) 
	{
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,GetDesktopWindow(),100);
	}

	//��ȡ���
	m_pIClientKernelSink=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IClientKernelSink);
	if (m_pIClientKernelSink==NULL) return false;
	m_hWndGameFrame=m_pIClientKernelSink->GetFrameWnd();

	//�����д���
	if (lpszComLine[0]!=0)
	{
		//��� TOKEN
		int nStringLength=0;
		CString strRoomToken;
		LPCTSTR pszRoomToken=TEXT("/RoomToken:");
		LPCTSTR lpszBeginString=lpszComLine;
		while (true)
		{
			LPCTSTR lpszEndString=_tcschr(lpszBeginString,TEXT(' '));
			nStringLength=(lpszEndString==NULL)?lstrlen(lpszBeginString):(int)(lpszEndString-lpszBeginString);

			//�жϱ�ʶ
			const int nTokenLength=lstrlen(pszRoomToken);
			if ((nStringLength>=nTokenLength)&&(memcmp(lpszBeginString,pszRoomToken,nTokenLength*sizeof(TCHAR))==0))
			{
				CopyMemory(strRoomToken.GetBufferSetLength(nStringLength-nTokenLength),lpszBeginString+nTokenLength,
					(nStringLength-nTokenLength)*sizeof(TCHAR));
				strRoomToken.ReleaseBuffer();
				break;
			}

			//���ñ���
			if (lpszEndString==NULL) break;
			lpszBeginString=(lpszEndString+1);
		}

		//�����ڴ�
		if (strRoomToken.GetLength()>0)
		{
			m_hShareMemory=OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,strRoomToken);
			if (m_hShareMemory==NULL) return false;
			m_pShareMemory=(tagShareMemory *)MapViewOfFile(m_hShareMemory,FILE_MAP_ALL_ACCESS,0,0,0);
			if (m_pShareMemory==NULL) return false;
			if (m_pShareMemory->wDataSize<sizeof(tagShareMemory)) return false;
			m_pShareMemory->hWndGameFrame=m_hWndGameFrame;
		}

		//�ŵ�ģ��
		if (m_hShareMemory!=NULL)
		{
			ASSERT(m_pShareMemory->hWndGameServer!=NULL);
			IUnknownEx * pIUnknownEx=GET_MYSELF_INTERFACE(IUnknownEx);
			if (m_ChannelServiceHelper.CreateInstance()==false) return false;
			if (m_ChannelServiceHelper->SetChannelMessageSink(pIUnknownEx)==false) return false;
			if (m_ChannelServiceHelper->CreateChannel(m_pShareMemory->hWndGameServer)==false) return false;
		}
	}

	return true;
}

//�뿪״̬
bool __cdecl CClientKernel::GetExitFlags()
{
	if (m_bLookonMode==true) return true;
	if (m_pMeUserItem==NULL) return true;
	if (m_pMeUserItem->cbUserStatus!=US_PLAY) return true;
	return (m_bInquire==true)?false:true;
}

//��ȡ���
const tagUserData * __cdecl CClientKernel::GetUserInfo(WORD wChairID)
{
	if (wChairID>=MAX_CHAIR) return NULL;
	if (wChairID>=m_ServerAttribute.wChairCount) return NULL;
	return m_pUserItem[wChairID];
}

//��ʾ��Ϣ
int __cdecl CClientKernel::ShowMessageBox(LPCTSTR pszMessage, UINT nType)
{
	LPCTSTR pszTitle=m_ServerAttribute.szServerName;
	return ::MessageBox(AfxGetMainWnd()->GetSafeHwnd(),pszMessage,pszTitle,nType);
}


//���ͺ���
bool __cdecl CClientKernel::SendSocketData(WORD wMainCmdID, WORD wSubCmdID)
{
	//Ч��״̬
	if (m_ChannelServiceHelper.GetInterface()==NULL) return false;

	//��������
	IPC_SocketPackage SocketPackage;
	memset(&SocketPackage,0,sizeof(SocketPackage));
	SocketPackage.Command.wMainCmdID=wMainCmdID;
	SocketPackage.Command.wSubCmdID=wSubCmdID;

	//��������
	WORD wSendSize=sizeof(SocketPackage.Command);
	return m_ChannelServiceHelper->SendData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_SEND,&SocketPackage,wSendSize);
}

//���ͺ���
bool __cdecl CClientKernel::SendSocketData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//Ч��״̬
	if (m_ChannelServiceHelper.GetInterface()==NULL) return false;

	//Ч������
	ASSERT(wDataSize<=SOCKET_PACKAGE);
	if (wDataSize>SOCKET_PACKAGE) return false;

	//��������
	IPC_SocketPackage SocketPackage;
	memset(&SocketPackage,0,sizeof(SocketPackage));
	SocketPackage.Command.wMainCmdID=wMainCmdID;
	SocketPackage.Command.wSubCmdID=wSubCmdID;
	if (wDataSize>0)
	{
		ASSERT(pData!=NULL);
		CopyMemory(SocketPackage.cbBuffer,pData,wDataSize);
	}

	//��������
	WORD wSendSize=sizeof(SocketPackage.Command)+wDataSize;
	return m_ChannelServiceHelper->SendData(IPC_MAIN_SOCKET,IPC_SUB_SOCKET_SEND,&SocketPackage,wSendSize);
}

//��������
bool __cdecl CClientKernel::SendProcessData(WORD wMainCmdID, WORD wSubCmdID)
{
	if (m_ChannelServiceHelper.GetInterface()==NULL) return false;
	return m_ChannelServiceHelper->SendData(wMainCmdID,wSubCmdID);	
}

//��������
bool __cdecl CClientKernel::SendProcessData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)
{
	if (m_ChannelServiceHelper.GetInterface()==NULL) return false;
	return m_ChannelServiceHelper->SendData(wMainCmdID,wSubCmdID,pBuffer,wDataSize); 
}

//����׼��
bool __cdecl CClientKernel::SendUserReady(void * pBuffer, WORD wDataSize)
{
	return SendSocketData(MDM_GF_FRAME,SUB_GF_USER_READY,pBuffer,wDataSize);
}

//��������
bool __cdecl CClientKernel::SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor)
{
	//Ч��״̬
	if (m_pMeUserItem==NULL) return false;

	//��������
	CMD_GF_UserChat UserChat;
	UserChat.crFontColor=crFontColor;
	UserChat.dwTargetUserID=dwTargetUserID;
	UserChat.dwSendUserID=m_pMeUserItem->dwUserID;
	lstrcpyn(UserChat.szChatMessage,pszChatMessage,CountArray(UserChat.szChatMessage));
	UserChat.wChatLength=CountString(UserChat.szChatMessage);

	//��������
	WORD wSendSize=sizeof(UserChat)-sizeof(UserChat.szChatMessage)+UserChat.wChatLength;
	SendSocketData(MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wSendSize);

	return true;
}

//���ö�ʱ��
bool __cdecl CClientKernel::SetGameTimer(WORD wChairID, UINT nTimerID, UINT nElapse)
{
	//Ч�����
	ASSERT(m_hWnd!=NULL);
	ASSERT(m_pIClientKernelSink!=NULL);

	//�߼�����
	if (m_nTimerID!=0L) KillGameTimer(m_nTimerID);
	if ((wChairID<m_ServerAttribute.wChairCount)&&(nElapse>0L))
	{
		//���ñ���
		m_nTimerID=nTimerID;
		m_nElapseCount=nElapse;
		m_wTimerChairID=wChairID;

		//���ý���
		SetTimer(nTimerID,1000,NULL);
		m_pIClientKernelSink->OnEventTimer(m_wTimerChairID,nElapse,nTimerID);

		return true;
	}

	return false;
}

//ɾ����ʱ��
bool __cdecl CClientKernel::KillGameTimer(UINT nTimerID)
{
	//Ч�����
	ASSERT(m_hWnd!=NULL);
	ASSERT(m_pIClientKernelSink!=NULL);

	//�߼�����
	if ((m_nTimerID!=0)&&((m_nTimerID==nTimerID)||(nTimerID==0)))
	{
		//���ý���
		KillTimer(m_nTimerID);
		if (m_wTimerChairID!=INVALID_CHAIR)
		{
			ASSERT(m_wTimerChairID<m_ServerAttribute.wChairCount);
			ASSERT(m_pUserItem[m_wTimerChairID]!=NULL);
			m_pIClientKernelSink->OnEventTimerKilled(m_wTimerChairID,m_nTimerID);
		}

		//���ñ���
		m_nTimerID=0L;
		m_nElapseCount=0L;
		m_wTimerChairID=INVALID_CHAIR;

		return true;
	}

	return false;
}

//�ŵ����ݴ���
bool __cdecl CClientKernel::OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	//���⴦��
	ASSERT(m_pIClientKernelSink!=NULL);
	if (m_pIClientKernelSink->OnEventProcess(*pHead,pIPCBuffer,wDataSize,hWndSend)) return true;

	//Ĭ�ϴ���
	switch (pHead->wMainCmdID)
	{
	case IPC_MAIN_SOCKET:	//��������
		{
			bool bSuccess=OnIPCSocket(pHead,pIPCBuffer,wDataSize,hWndSend);
			if (bSuccess==false)
			{
				m_bInquire=false;
				m_pIClientKernelSink->CloseGameFrame(true);
			}
			return true;
		}
	case IPC_MAIN_CONFIG:	//������Ϣ
		{
			return OnIPCConfig(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_USER:		//�û���Ϣ
		{
			return OnIPCUser(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	case IPC_MAIN_CONCTROL:	//������Ϣ
		{
			return OnIPCControl(pHead,pIPCBuffer,wDataSize,hWndSend);
		}
	}

	return false;
}

//������Ϣ
bool CClientKernel::OnIPCSocket(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_SOCKET);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_SOCKET_RECV:	//���ݽ���
		{
			//Ч������
			ASSERT(wDataSize>=sizeof(CMD_Command));
			if (wDataSize<sizeof(CMD_Command)) return false;

			//��ȡ����
			WORD wPacketSize=wDataSize-sizeof(CMD_Command);
			IPC_SocketPackage * pSocketPackage=(IPC_SocketPackage *)pIPCBuffer;

			//���⴦��
			bool bHandle=false;
			ASSERT(m_pIClientKernelSink!=NULL);
			if (wPacketSize==0)	bHandle=m_pIClientKernelSink->OnEventSocket(pSocketPackage->Command,NULL,0);
			else bHandle=m_pIClientKernelSink->OnEventSocket(pSocketPackage->Command,pSocketPackage->cbBuffer,wPacketSize);
			
			//Ĭ�ϴ���
			if ((bHandle==false)&&(pSocketPackage->Command.wMainCmdID==MDM_GF_FRAME))
			{
				switch (pSocketPackage->Command.wSubCmdID)
				{
				case SUB_GF_OPTION:		//��Ϸ����
					{
						//Ч�����
						ASSERT(wPacketSize>=sizeof(CMD_GF_Option));

						//��Ϣ����
						CMD_GF_Option * pOption=(CMD_GF_Option *)pSocketPackage->cbBuffer;
						m_bGameStatus=pOption->bGameStatus;
						m_bAllowLookon=pOption->bAllowLookon?true:false;

						return true;
					}
				case SUB_GF_SCENE:		//��Ϸ����
					{
						bool ret;
						ret = m_pIClientKernelSink->OnEventGameScene(m_bGameStatus,m_bLookonMode,pSocketPackage->cbBuffer,wPacketSize);
						::SendMessage(g_GlobalOption.m_lFrameContorl,WM_USER+999,0,0);
						return ret;
					}
				case SUB_GF_USER_CHAT:	//������Ϣ
					{
						//Ч�����
						CMD_GF_UserChat * pUserChat=(CMD_GF_UserChat *)pSocketPackage->cbBuffer;
						ASSERT(wPacketSize>=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)));
						ASSERT(wPacketSize==(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength));
						if (wPacketSize<(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage))) return false;
						if (wPacketSize!=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength)) return false;

						//Ѱ���û�
						tagUserData * pRecvUserData=NULL;
						tagUserData * pSendUserData=SearchUserItem(pUserChat->dwSendUserID);
						if (pSendUserData==NULL) return true;
						if (pUserChat->dwTargetUserID!=0L) pRecvUserData=SearchUserItem(pUserChat->dwTargetUserID);

						//��Ϣ����
						if ((pUserChat->dwSendUserID!=m_dwUserID)&&(pSendUserData->cbCompanion==enCompanion_Detest)) return true;

						//��ʾ��Ϣ
						m_pIClientKernelSink->OnUserChatMessage(pSendUserData,pRecvUserData,pUserChat->szChatMessage,pUserChat->crFontColor);

						return true;
					}
				case SUB_GF_MESSAGE:	//ϵͳ��Ϣ
					{
						//Ч�����
						CMD_GF_Message * pMessage=(CMD_GF_Message *)pSocketPackage->cbBuffer;
						ASSERT(wDataSize>(sizeof(CMD_GF_Message)-sizeof(pMessage->szContent)));
						if (wDataSize<=(sizeof(CMD_GF_Message)-sizeof(pMessage->szContent))) return false;

						//��Ϣ����
						WORD wHeadSize=sizeof(CMD_GF_Message)-sizeof(pMessage->szContent);
						ASSERT(wPacketSize==(wHeadSize+pMessage->wMessageLength*sizeof(TCHAR)));
						if (wPacketSize!=(wHeadSize+pMessage->wMessageLength*sizeof(TCHAR))) return false;
						pMessage->szContent[pMessage->wMessageLength-1]=0;

						//�ж��ж�
						if (pMessage->wMessageType&SMT_CLOSE_GAME) 
						{
							KillGameTimer(m_nTimerID);
							m_ChannelServiceHelper->CloseChannel(true,true);
						}

						//��ʾ��Ϣ
						if (pMessage->wMessageType&SMT_INFO) m_pIMessageProxy->InsertSystemString(pMessage->szContent,MS_NORMAL);
						if (pMessage->wMessageType&SMT_EJECT) ShowMessageBox(pMessage->szContent,MB_ICONINFORMATION);

						//�رշ���
						if (pMessage->wMessageType&SMT_CLOSE_GAME) 
						{
							m_bInquire=false;
							m_pIClientKernelSink->CloseGameFrame(true);
						}

						return true;
					}
				}
			}

			return bHandle;
		}
	}

	return false;
}

//������Ϣ
bool CClientKernel::OnIPCConfig(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_CONFIG);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_SERVER_INFO:	//������Ϣ
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(IPC_GF_ServerInfo));
			if (wDataSize<sizeof(IPC_GF_ServerInfo)) return false;

			//��������
			IPC_GF_ServerInfo * pServerInfo=(IPC_GF_ServerInfo *)pIPCBuffer;
			m_wTableID=pServerInfo->wTableID;
			m_wChairID=pServerInfo->wChairID;
			m_dwUserID=pServerInfo->dwUserID;
			m_ServerAttribute.wKindID=pServerInfo->wKindID;
			m_ServerAttribute.wServerID=pServerInfo->wServerID;
			m_ServerAttribute.wGameGenre=pServerInfo->wGameGenre;
			m_ServerAttribute.wChairCount=pServerInfo->wChairCount;
			CopyMemory(&m_OptionBuffer,&pServerInfo->OptionBuffer,sizeof(m_OptionBuffer));
			lstrcpyn(m_ServerAttribute.szKindName,pServerInfo->szKindName,CountArray(m_ServerAttribute.szKindName));
			lstrcpyn(m_ServerAttribute.szServerName,pServerInfo->szServerName,CountArray(m_ServerAttribute.szServerName));

			//���ñ���
			TCHAR szTitle[128]=TEXT("");
			_snprintf(szTitle,sizeof(szTitle),TEXT("%s �� %s - �� %d �� ��"),m_ServerAttribute.szKindName,m_ServerAttribute.szServerName,m_wTableID+1);
			::SetWindowText(m_hWndGameFrame,szTitle);


			m_pIClientKernelSink->OnInitVideo( );
	
			
			
			return true;
		}
	}
	return false;
}



//�û�֪ͨ
bool CClientKernel::OnIPCUser(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_USER);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_USER_COME:		//�û���Ϣ
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(tagUserInfoHead));
			if (wDataSize<sizeof(tagUserInfoHead)) return false;

			
			//��ȡ������Ϣ
			tagUserData UserData;
			memset(&UserData,0,sizeof(UserData));
			tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)pIPCBuffer;
			strcpy( UserData.szFace, pUserInfoHead->szFace );
			//UserData.wFaceID=pUserInfoHead->wFaceID;
			UserData.wTableID=pUserInfoHead->wTableID;
			UserData.wChairID=pUserInfoHead->wChairID;
			UserData.wNetDelay=pUserInfoHead->wNetDelay;
			UserData.cbGender=pUserInfoHead->cbGender;
			UserData.cbMember=pUserInfoHead->cbMember;
			UserData.cbUserStatus=pUserInfoHead->cbUserStatus;
			UserData.dwUserID=pUserInfoHead->dwUserID;
			//UserData.szGroupName=pUserInfoHead->dwGroupID;
			//wcsncpy( UserData.szGroupName, pUserInfoHead->dwGroupID, NAME_LEN );
			UserData.dwUserRight=pUserInfoHead->dwUserRight;
			UserData.dwMasterRight=pUserInfoHead->dwMasterRight;
			UserData.lGold=pUserInfoHead->UserScoreInfo.lGold;
			UserData.lScore=pUserInfoHead->UserScoreInfo.lScore;
			UserData.lWinCount=pUserInfoHead->UserScoreInfo.lWinCount;
			UserData.lLostCount=pUserInfoHead->UserScoreInfo.lLostCount;
			UserData.lDrawCount=pUserInfoHead->UserScoreInfo.lDrawCount;
			UserData.lFleeCount=pUserInfoHead->UserScoreInfo.lFleeCount;
			UserData.lExperience=pUserInfoHead->UserScoreInfo.lExperience;

			//��ȡ��չ��Ϣ
			void * pDataBuffer=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pUserInfoHead+1,wDataSize-sizeof(tagUserInfoHead));
			while (true)
			{
				pDataBuffer=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_USER_ACCOUNTS:		//�û��ʻ�
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szName));
						if (DataDescribe.wDataSize<=sizeof(UserData.szName))
						{
							CopyMemory(&UserData.szName,pDataBuffer,DataDescribe.wDataSize);
							UserData.szName[CountArray(UserData.szName)-1]=0;
						}
						break;
					}
				case DTP_USER_GROUP_NAME:	//�û�����
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.szGroupName));
						if (DataDescribe.wDataSize<=sizeof(UserData.szGroupName))
						{
							CopyMemory(&UserData.szGroupName,pDataBuffer,DataDescribe.wDataSize);
							UserData.szGroupName[CountArray(UserData.szGroupName)-1]=0;
						}
						break;
					}
				case DTP_USER_COMPANION:	//�û���ϵ
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(UserData.cbCompanion));
						if (DataDescribe.wDataSize<=sizeof(UserData.cbCompanion))
						{
							CopyMemory(&UserData.cbCompanion,pDataBuffer,DataDescribe.wDataSize);
						}
						break;
					}
				}
			}

			//�����û�
			ActiveUserItem(&UserData);

			return true;
		}
	case IPC_SUB_USER_STATUS:	//�û�״̬
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(IPC_UserStatus));
			if (wDataSize<sizeof(IPC_UserStatus)) return false;

			//��Ϣ����
			IPC_UserStatus * pUserStatus=(IPC_UserStatus *)pIPCBuffer;
			if (pUserStatus->cbUserStatus<US_SIT) 
			{
				if (pUserStatus->dwUserID==m_dwUserID)
				{
					ResetClientKernel();
					return true;
				}
				else DeleteUserItem(pUserStatus->dwUserID);
			}
			else UpdateUserItem(pUserStatus->dwUserID,pUserStatus->cbUserStatus,pUserStatus->wNetDelay);

			return true;
		}
	case IPC_SUB_USER_SCORE:	//�û�����
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(IPC_UserScore));
			if (wDataSize<sizeof(IPC_UserScore)) return false;

			//��Ϣ����
			IPC_UserScore * pUserScore=(IPC_UserScore *)pIPCBuffer;
			UpdateUserItem(pUserScore->dwUserID,&pUserScore->UserScore);

			return true;
		}
	case IPC_SUB_GAME_START:	//��Ϸ��ʼ
		{
			//�����û�
			for (WORD i=0;i<m_ServerAttribute.wChairCount;i++)
			{
				if (m_pUserItem[i]!=NULL) 
				{
					m_pUserItem[i]->cbUserStatus=US_PLAY;
					m_pIClientKernelSink->OnEventUserStatus(m_pUserItem[i],m_pUserItem[i]->wChairID,false);
				}
			}

			return true;
		}
	case IPC_SUB_GAME_FINISH:	//��Ϸ����
		{
			//�����û�
			for (WORD i=0;i<m_ServerAttribute.wChairCount;i++)
			{
				if (m_pUserItem[i]!=NULL)
				{
					m_pUserItem[i]->cbUserStatus=US_SIT;
					m_pIClientKernelSink->OnEventUserStatus(m_pUserItem[i],m_pUserItem[i]->wChairID,false);
				}
			}

			return true;
		}
	}

	return false;
}

//����֪ͨ
bool CClientKernel::OnIPCControl(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)
{
	ASSERT(pHead->wMainCmdID==IPC_MAIN_CONCTROL);
	switch (pHead->wSubCmdID)
	{
	case IPC_SUB_START_FINISH:		//�������
		{
			//��ȡ����
			CMD_GF_Info Info;
			memset(&Info,0,sizeof(Info));
			Info.bAllowLookon=m_bAllowLookon?TRUE:FALSE;

			//������Ϣ
			SendSocketData(MDM_GF_FRAME,SUB_GF_INFO,&Info,sizeof(Info));

			return true;
		}
	case IPC_SUB_CLOSE_FRAME:		//�رտ��
		{
			m_bInquire=false;
			m_ChannelServiceHelper->CloseChannel(false,false);
			m_pIClientKernelSink->CloseGameFrame(true);
			return true;
		}
	}

	return false;
}

//�����û�
tagUserData * CClientKernel::SearchUserItem(DWORD dwUserID)
{
	//��������
	WORD wIndex=0;
	tagUserData * pUserData=NULL;

	//Ѱ���û�
	do
	{
		pUserData=m_UserItemFactory.EnumActiveObject(wIndex++);
		if (pUserData==NULL) break;
		if (pUserData->dwUserID==dwUserID) return pUserData;
	} while (true);

	return NULL;
}

//�����û�
bool CClientKernel::ActiveUserItem(const tagUserData * pUserData)
{
	//Ч�����
	ASSERT(pUserData!=NULL);
	ASSERT(pUserData->wTableID!=INVALID_TABLE);
	ASSERT(pUserData->wChairID<m_ServerAttribute.wChairCount);
	if (pUserData->wTableID==INVALID_TABLE) return false;
	if (pUserData->wChairID>=m_ServerAttribute.wChairCount) return false;

	//�����û�
	tagUserData * pActiveUserData=NULL;
	pActiveUserData=m_UserItemFactory.ActiveItem();
	ASSERT(pActiveUserData!=NULL);
	if (pActiveUserData==NULL) return false;
	CopyMemory(pActiveUserData,pUserData,sizeof(tagUserData));

	//���ñ���
	bool bLookonMode=(pActiveUserData->cbUserStatus==US_LOOKON);
	if (bLookonMode==false)
	{
		ASSERT(m_pUserItem[pUserData->wChairID]==NULL);
		m_pUserItem[pUserData->wChairID]=pActiveUserData;
	}
	else m_UserItemLookon.Add(pActiveUserData);

	//�ж��Լ�
	if (m_dwUserID==pActiveUserData->dwUserID)
	{
		m_bLookonMode=bLookonMode;
		m_pMeUserItem=pActiveUserData;
		m_wTableID=m_pMeUserItem->wTableID;
		m_wChairID=m_pMeUserItem->wChairID;
	}

	//֪ͨ�ı�
	ASSERT(m_pIClientKernelSink!=NULL);
	m_pIClientKernelSink->OnEventUserEnter(pActiveUserData,pActiveUserData->wChairID,bLookonMode);

	return true;
}

//ɾ���û�
bool CClientKernel::DeleteUserItem(DWORD dwUserID)
{
	//��Ϸ�û�
	tagUserData * pUserData=NULL;
	for (WORD i=0;i<m_ServerAttribute.wChairCount;i++)
	{
		pUserData=m_pUserItem[i];
		if ((pUserData!=NULL)&&(pUserData->dwUserID==dwUserID))
		{
			//���ñ���
			m_pUserItem[i]=NULL;
			m_UserItemFactory.FreeItem(pUserData);

			//֪ͨ�ı�
			ASSERT(m_pIClientKernelSink!=NULL);
			m_pIClientKernelSink->OnEventUserLeft(pUserData,pUserData->wChairID,false);

			return true;
		}
	}

	//�Թ��û�
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		pUserData=m_UserItemLookon[i];
		if (pUserData->dwUserID==dwUserID)
		{
			//���ñ���
			m_UserItemLookon.RemoveAt(i);
			m_UserItemFactory.FreeItem(pUserData);

			//�ж��Լ�
			if (m_dwUserID==dwUserID)
			{
				m_pMeUserItem=NULL;
				m_wTableID=INVALID_TABLE;
				m_wChairID=INVALID_CHAIR;
			}

			//֪ͨ�ı�
			ASSERT(m_pIClientKernelSink!=NULL);
			m_pIClientKernelSink->OnEventUserLeft(pUserData,pUserData->wChairID,true);

			return true;
		}
	}

	return false;
}

//�����û�
bool CClientKernel::UpdateUserItem(DWORD dwUserID, const tagUserScore * pUserScore)
{
	//Ѱ���û�
	tagUserData * pUserData=SearchUserItem(dwUserID);
	if (pUserData==NULL) return false;

	//��������
	pUserData->lGold=pUserScore->lGold;
	pUserData->lScore=pUserScore->lScore;
	pUserData->lWinCount=pUserScore->lWinCount;
	pUserData->lLostCount=pUserScore->lLostCount;
	pUserData->lDrawCount=pUserScore->lDrawCount;
	pUserData->lFleeCount=pUserScore->lFleeCount;
	pUserData->lExperience=pUserScore->lExperience;

	//֪ͨ�ı�
	ASSERT(m_pIClientKernelSink!=NULL);
	bool bLookonUser=(pUserData->cbUserStatus==US_LOOKON);
	m_pIClientKernelSink->OnEventUserScore(pUserData,pUserData->wChairID,bLookonUser);

	return true;
}

//�����û�
bool CClientKernel::UpdateUserItem(DWORD dwUserID, BYTE cbUserStatus, WORD wNetDelay)
{
	//Ѱ���û�
	tagUserData * pUserData=SearchUserItem(dwUserID);
	if (pUserData==NULL) return false;

	//��������
	pUserData->wNetDelay=wNetDelay;
	pUserData->cbUserStatus=cbUserStatus;

	//֪ͨ�ı�
	ASSERT(m_pIClientKernelSink!=NULL);
	bool bLookonUser=(cbUserStatus==US_LOOKON);
	m_pIClientKernelSink->OnEventUserStatus(pUserData,pUserData->wChairID,bLookonUser);

	return false;
}

//�ں�����
void CClientKernel::ResetClientKernel()
{
	//ɾ����ʱ��
	if (m_nTimerID!=0)
	{
		KillTimer(m_nTimerID);
		m_nTimerID=0L;
		m_nElapseCount=0L;
		m_wTimerChairID=INVALID_CHAIR;
	}

	//�Թ��û�
	for (INT_PTR i=0;i<m_UserItemLookon.GetCount();i++)
	{
		m_UserItemFactory.FreeItem(m_UserItemLookon[i]);
	}
	m_UserItemLookon.RemoveAll();

	//��Ϸ�û�
	for (WORD i=0;i<m_ServerAttribute.wChairCount;i++)
	{
		if (m_pUserItem[i]!=NULL)
		{
			m_UserItemFactory.FreeItem(m_pUserItem[i]);
			m_pUserItem[i]=NULL;
		}
	}

	//���ñ���
	m_bLookonMode=true;
	m_pMeUserItem=NULL;
	m_bGameStatus=US_FREE;
	m_wTableID=INVALID_TABLE;
	m_wChairID=INVALID_CHAIR;

	//��������
	m_pIClientKernelSink->ResetGameFrame();

	return;
}

//ʱ����Ϣ
void CClientKernel::OnTimer(UINT nIDEvent)
{
	//ʱ�䴦��
	if ((m_nTimerID==nIDEvent)&&(m_wTimerChairID!=INVALID_CHAIR))
	{
		//��Ϣ����
		if (m_nElapseCount>0) m_nElapseCount--;
		bool bSuccess=m_pIClientKernelSink->OnEventTimer(m_wTimerChairID,m_nElapseCount,m_nTimerID);
		if ((m_nElapseCount==0)||(bSuccess==false)) KillGameTimer(m_nTimerID);

		return;
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) void * __cdecl CreateClientKernel(const GUID & Guid, DWORD dwInterfaceVer)
{
	//��������
	CClientKernel * pClientKernel=NULL;
	try
	{
		pClientKernel=new CClientKernel();
		if (pClientKernel==NULL) throw TEXT("����ʧ��");
		void * pObject=pClientKernel->QueryInterface(Guid,dwInterfaceVer);
		if (pObject==NULL) throw TEXT("�ӿڲ�ѯʧ��");
		return pObject;
	}
	catch (...) {}

	//��������
	SafeDelete(pClientKernel);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////