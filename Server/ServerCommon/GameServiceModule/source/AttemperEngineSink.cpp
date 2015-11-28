#include "Include/stdafx.h"
#include "../Include/TableFrame.h"
#include "../Include/DataBaseSink.h"
#include "../Include/AttemperEngineSink.h"

//��Ϣͷ�ļ�
#include "../../../../Common/CommonHead/Message/CMD_Game.h"
#include "../../../../Common/CommonHead/Common/GlobalField.h"
#include "../../../../Common/CommonHead/Common/GlobalFrame.h"

//////////////////////////////////////////////////////////////////////////

//�궨��
#define TIME_RECONNECT						15000L						//����ʱ��
#define TIME_UPDATE_LIST					30000L						//����ʱ��
#define TIME_UPDATE_ONLINE					30000L						//��������

//��ʱ����ʶ
#define IDI_CONNECT_CENTER_SERVER			1							//���ӷ�����
#define IDI_UPDATE_SERVER_LIST				2							//�����б�
#define IDI_UPDATE_ONLINE_COUNT				3							//��������
#define IDI_SEND_SYSTEM_MESSAGE				4							//ϵͳ��Ϣ

//��������
#define IDI_TABLE_BEGIN						100							//��ʼ��ʶ
#define TABLE_TIME_RANGE					50							//��ʱ����Χ

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//��Ϸ����
	m_wMaxUserItem=0;
	m_pTableFrame=NULL;
	m_pConnectItemInfo=NULL;

	//���ñ���
	m_dwCenterServer=0L;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//���Ʊ���
	m_bShallClose=false;
	m_bAllowWisper=true;
	m_bAllowRoomChat=true;
	m_bAllowGameChat=true;
	m_bAllowEnterRoom=true;
	m_bAllowEnterGame=true;

	//���ķ�����
	m_pCenterSocket=NULL;
	m_SocketState=SocketState_NoConnect;

	//��������
	m_wColumnInfoSize=0;
	m_wOnLineInfoSize=0;
	memset(&m_ColumnInfo,0,sizeof(m_ColumnInfo));
	memset(&m_OnLineCountInfo,0,sizeof(m_OnLineCountInfo));

	//��Ϸ���
	m_hDllInstance=NULL;
	m_pIGameServiceManager=NULL;

	//�ӿڱ���
	m_pITimerEngine=NULL;
	m_pIEventService=NULL;
	m_pITCPSocketEngine=NULL;

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
	//ɾ���ڴ�
	SafeDeleteArray(m_pTableFrame);
	SafeDeleteArray(m_pConnectItemInfo);

	return;
}

//�ӿڲ�ѯ
void * __cdecl CAttemperEngineSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceFrame,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//����ģ������
bool __cdecl CAttemperEngineSink::StartService(IUnknownEx * pIUnknownEx)
{
	//Ч�����
	ASSERT(pIUnknownEx!=NULL);

	//���ñ���
	m_wColumnInfoSize=0;
	m_wOnLineInfoSize=0;
	memset(&m_ColumnInfo,0,sizeof(m_ColumnInfo));
	memset(&m_OnLineCountInfo,0,sizeof(m_OnLineCountInfo));

	//������������
	m_wMaxUserItem=m_pGameServiceOption->wMaxConnect;
	m_pConnectItemInfo=new tagConnectItemInfo [m_wMaxUserItem];
	if (m_pConnectItemInfo==NULL) return false;
	memset(m_pConnectItemInfo,0,sizeof(tagConnectItemInfo)*m_wMaxUserItem);

	//������Ϸ���
	if (m_pIGameServiceManager->RectifyServiceOption(m_pGameServiceOption)==false) return false;

	//������Ϸ����
	ASSERT(m_pGameServiceOption->wTableCount!=0);
	m_pTableFrame=new CTableFrame [m_pGameServiceOption->wTableCount];
	if (m_pTableFrame==NULL) return false;

	//��ʼ������
	bool bSuccess=false;
	tagTableFrameParameter TableFrameParameter;
	TableFrameParameter.pGameServiceAttrib=m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption=m_pGameServiceOption;
	TableFrameParameter.pIGameServiceFrame=GET_MYSELF_INTERFACE(IUnknownEx);
	TableFrameParameter.pIGameServiceManager=GET_OBJECTPTR_INTERFACE(m_pIGameServiceManager,IUnknownEx);
	for (WORD i=0;i<m_pGameServiceOption->wTableCount;i++)
	{
		bSuccess=(m_pTableFrame+i)->InitTableFrame(i,&TableFrameParameter);
		if (bSuccess==false) return false;
	}

	//�����б����
	tagColumnItem ColumnItem[]=
	{
		115,DTP_USER_ACCOUNTS,TEXT("�û���"),
		115,DTP_USER_NAME,TEXT("�ǳ�"),
		60,DTP_USER_POINT,TEXT("��Ϸ��ȯ"),
		40,DTP_USER_TABLE,TEXT("����"),
		85,DTP_GAME_LEVEL,TEXT("��Ϸ����"),
		50,DTP_USER_SCORE,TEXT("����"),
		60,DTP_WIN_RATE,TEXT("ʤ��"),
		60,DTP_FLEE_RATE,TEXT("����"),
		60,DTP_USER_PLAY_COUNT,TEXT("�ܾ�"),
		60,DTP_USER_WIN,TEXT("Ӯ��"),
		60,DTP_USER_LOST,TEXT("���"),
		60,DTP_USER_DRAW,TEXT("�;�"),
		60,DTP_USER_FLEE,TEXT("�Ӿ�"),
	};
	m_ColumnInfo.wColumnCount=CountArray(ColumnItem);
	CopyMemory(m_ColumnInfo.ColumnItem,ColumnItem,sizeof(ColumnItem));
	WORD wColumnInfoHandSize=sizeof(m_ColumnInfo)-sizeof(m_ColumnInfo.ColumnItem);
	m_wColumnInfoSize=wColumnInfoHandSize+m_ColumnInfo.wColumnCount*sizeof(m_ColumnInfo.ColumnItem[0]);

	//�������
	srand((DWORD)time(NULL));

	//�������ķ�����
	m_pCenterSocket->Connect(m_dwCenterServer,PORT_CENTER_SERVER);

	//ϵͳ��Ϣ
	m_pITimerEngine->SetTimer(IDI_SEND_SYSTEM_MESSAGE,200000L,(DWORD)(-1),0);

	return true;
}

//����ģ��ر�
bool __cdecl CAttemperEngineSink::StopService(IUnknownEx * pIUnknownEx)
{
	//ע������
	if (m_SocketState==SocketState_Connected)
	{
		CMD_CS_UnRegGameServer UnRegGameServer;
		memset(&UnRegGameServer,0,sizeof(UnRegGameServer));
		UnRegGameServer.wKindID=m_pGameServiceAttrib->wKindID;
		UnRegGameServer.wServerID=m_pGameServiceOption->wServerID;
		m_pCenterSocket->SendData(MDM_CS_SERVER_MANAGER,SUB_CS_UNREG_GAME_SERVER,&UnRegGameServer,sizeof(UnRegGameServer));
	}

	//�ر����
	m_pCenterSocket->CloseSocket(false);
	m_ServerUserManager.ResetUserManager();

	//ɾ������
	m_wMaxUserItem=0;
	SafeDeleteArray(m_pTableFrame);
	SafeDeleteArray(m_pConnectItemInfo);

	//���Ʊ���
	m_bShallClose=false;
	m_bAllowWisper=true;
	m_bAllowRoomChat=true;
	m_bAllowGameChat=true;
	m_bAllowEnterRoom=true;
	m_bAllowEnterGame=true;

	//���ñ���
	m_wColumnInfoSize=0;
	m_wOnLineInfoSize=0;
	memset(&m_ColumnInfo,0,sizeof(m_ColumnInfo));
	memset(&m_OnLineCountInfo,0,sizeof(m_OnLineCountInfo));

	return true;
}

//�¼�����ӿ�
bool __cdecl CAttemperEngineSink::OnAttemperEvent(WORD wIdentifier, void * pDataBuffer, WORD wDataSize, DWORD dwInsertTime)
{
	switch (wIdentifier)
	{
	case EVENT_CENTER_CONNECT:	//���������¼�
		{
			//Ч���С
			ASSERT(wDataSize==sizeof(NTY_CenterConnect));
			if (wDataSize!=sizeof(NTY_CenterConnect)) return false;

			//��Ϣ����
			NTY_CenterConnect * pCenterSocketConnect=(NTY_CenterConnect *)pDataBuffer;

			return OnEventCenterSocketConnect(pCenterSocketConnect->iErrorCode);
		}
	case EVENT_CENTER_READ:		//���Ķ�ȡ�¼�
		{
			//Ч���С
			NTY_CenterRead * pCenterSocketRead=(NTY_CenterRead *)pDataBuffer;
			ASSERT(wDataSize>=((sizeof(NTY_CenterRead)-sizeof(pCenterSocketRead->cbReadBuffer))));
			ASSERT(wDataSize==(sizeof(NTY_CenterRead)-sizeof(pCenterSocketRead->cbReadBuffer)+pCenterSocketRead->wDataSize));
			if (wDataSize<(sizeof(NTY_CenterRead)-sizeof(pCenterSocketRead->cbReadBuffer))) return false;
			if (wDataSize!=sizeof(NTY_CenterRead)-sizeof(pCenterSocketRead->cbReadBuffer)+pCenterSocketRead->wDataSize) return false;

			//������Ϣ
			bool bSuccess=false;
			try 
			{ 
				bSuccess=OnEventCenterSocketRead(pCenterSocketRead->Command,pCenterSocketRead->cbReadBuffer,pCenterSocketRead->wDataSize);
			}
			catch (...)	{ }
			if (bSuccess==false) m_pCenterSocket->CloseSocket(true);

			return true;
		}
	case EVENT_CENTER_CLOSE:		//���Ĺر��¼�
		{
			//Ч���С
			ASSERT(wDataSize==sizeof(NTY_CenterClose));
			if (wDataSize!=sizeof(NTY_CenterClose)) return false;

			//��Ϣ����
			NTY_CenterClose * pCenterSocketClose=(NTY_CenterClose *)pDataBuffer;
			OnEventCenterSocketClose(pCenterSocketClose->bCloseByServer);

			return true;
		}
	}

	return false;
}

//����״̬
bool __cdecl CAttemperEngineSink::SendTableStatus(WORD wTableID)
{
	//Ч�����
	ASSERT(wTableID<m_pGameServiceOption->wTableCount);
	if (wTableID>=m_pGameServiceOption->wTableCount) return false;

	//�������
	CMD_GR_TableStatus TableStatus;
	memset(&TableStatus,0,sizeof(TableStatus));
	TableStatus.wTableID=wTableID;
	TableStatus.bPlayStatus=(m_pTableFrame+wTableID)->IsGameStarted()?TRUE:FALSE;

	//��������
	m_pITCPSocketEngine->SendDataBatch(MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	return true;
}

//���ͷ���
bool __cdecl CAttemperEngineSink::SendUserScore(IServerUserItem * pIServerUserItem)
{
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//��������
	CMD_GR_UserScore UserScore;
	UserScore.dwUserID=pUserData->dwUserID;
	UserScore.UserScore=pUserData->UserScoreInfo;
	m_pITCPSocketEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	return true;
}

//����״̬
bool __cdecl CAttemperEngineSink::SendUserStatus(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsActive()==true);
	if (pIServerUserItem->IsActive()==false) return false;

	//��������
	CMD_GR_UserStatus UserStatus;
	memset(&UserStatus,0,sizeof(UserStatus));
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//��������
	UserStatus.dwUserID=pUserData->dwUserID;
	UserStatus.wTableID=pUserData->wTableID;
	UserStatus.wChairID=pUserData->wChairID;
	UserStatus.wNetDelay=pUserData->wNetDelay;
	UserStatus.cbUserStatus=pUserData->cbUserStatus;

	//��������
	m_pITCPSocketEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));

	return true;
}

//ɾ���û�
bool __cdecl CAttemperEngineSink::DeleteUserItem(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//Ч��״̬
	ASSERT(pIServerUserItem->GetTableID()==INVALID_TABLE);
	ASSERT(pIServerUserItem->GetChairID()==INVALID_CHAIR);

	//д�����
	DBR_GR_WriteScore WriteScore;
	memset(&WriteScore,0,sizeof(WriteScore));
	WriteScore.dwUserID=pIServerUserItem->GetUserID();
	WriteScore.dwClientIP=pIServerUserItem->GetClientIP();
	WriteScore.dwPlayTimeCount=pIServerUserItem->GetPlayTimeCount();
	WriteScore.dwOnlineTimeCount=pIServerUserItem->GetOnlineTimeCount();
	pIServerUserItem->GetUserScoreInfo(WriteScore.ScoreResultInfo,WriteScore.ScoreModifyInfo);
	lstrcpyn(WriteScore.szAccounts,pIServerUserItem->GetAccounts(),CountArray(WriteScore.szAccounts));
	m_DataBaseEvent.PostDataBaseEvent(DBR_GR_WRITE_SCORE,0,0,&WriteScore,sizeof(WriteScore));

	//����״̬
	CMD_GR_UserStatus UserStatus;
	memset(&UserStatus,0,sizeof(UserStatus));
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	UserStatus.dwUserID=pIServerUserItem->GetUserID();
	UserStatus.wTableID=INVALID_TABLE;
	UserStatus.wChairID=INVALID_CHAIR;
	UserStatus.cbUserStatus=US_NULL;
	m_pITCPSocketEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));

	//ɾ���û�
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex<m_wMaxUserItem) (m_pConnectItemInfo+wIndex)->pIServerUserItem=NULL;
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);

	return true;
}

//��������
bool __cdecl CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsActive()==true);
	if (pIServerUserItem->IsActive()==false) return false;

	//��������
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=0xFFFF)
	{
		WORD wRoundID=(m_pConnectItemInfo+wIndex)->wRoundID;
		m_pITCPSocketEngine->SendData(wIndex,wRoundID,wMainCmdID,wSubCmdID);
	}

	return true;
}

//��������
bool __cdecl CAttemperEngineSink::SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->IsActive()==true);
	if (pIServerUserItem->IsActive()==false) return false;

	//��������
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=0xFFFF)
	{
		WORD wRoundID=(m_pConnectItemInfo+wIndex)->wRoundID;
		m_pITCPSocketEngine->SendData(wIndex,wRoundID,wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//���ö�ʱ��
bool __cdecl CAttemperEngineSink::SetTableTimer(WORD wTableID, WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)
{
	//Ч�����
	ASSERT(wTimerID<TABLE_TIME_RANGE);
	ASSERT(wTableID<m_pGameServiceOption->wTableCount);

	//���ö�ʱ��
	if (wTableID>=m_pGameServiceOption->wTableCount) return false;
	WORD wTableTimeID=IDI_TABLE_BEGIN+wTableID*TABLE_TIME_RANGE+wTimerID;
	return m_pITimerEngine->SetTimer(wTableTimeID,dwElapse,dwRepeat,wBindParam);
}

//ɾ����ʱ��
bool __cdecl CAttemperEngineSink::KillTableTimer(WORD wTableID, WORD wTimerID)
{
	//Ч�����
	ASSERT(wTimerID<TABLE_TIME_RANGE);
	ASSERT(wTableID<m_pGameServiceOption->wTableCount);

	//ɾ����ʱ��
	if (wTableID>=m_pGameServiceOption->wTableCount) return false;
	WORD wTableTimeID=IDI_TABLE_BEGIN+wTableID*TABLE_TIME_RANGE+wTimerID;
	return m_pITimerEngine->KillTimer(wTableTimeID);
}

//�����Ϣ
void __cdecl CAttemperEngineSink::ExportInformation(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pszString,TraceLevel);
	return;
}

//�����¼�
bool CAttemperEngineSink::SetEventService(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIEventService=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IEventService);
	ASSERT(m_pIEventService!=NULL);

	return (m_pIEventService!=NULL);
}

//���ú���
bool CAttemperEngineSink::InitAttemperSink(tagAttemperSinkParameter * pAttemperSinkParameter, IUnknownEx * pIUnknownEx)
{
	//Ч�����
	ASSERT(pIUnknownEx!=NULL);
	ASSERT(pAttemperSinkParameter!=NULL);

	//���ñ���
	m_pCenterSocket=pAttemperSinkParameter->pCenterSocket;
	m_dwCenterServer=pAttemperSinkParameter->dwCenterServer;
	m_pGameServiceAttrib=pAttemperSinkParameter->pGameServiceAttrib;
	m_pGameServiceOption=pAttemperSinkParameter->pGameServiceOption;
	m_pIGameServiceManager=pAttemperSinkParameter->pIGameServiceManager;

	//��������
	IServiceEngine * pIServiceEngine=(IServiceEngine *)pIUnknownEx->QueryInterface(IID_IServiceEngine,VER_IServiceEngine);
	if (pIServiceEngine==NULL)
	{
		ExportInformation(TEXT("��ȡ�����������ʧ��"),Level_Exception);
		return false;
	}

	//��ʱ������
	m_pITimerEngine=(ITimerEngine *)pIServiceEngine->GetTimerEngine(IID_ITimerEngine,VER_ITimerEngine);
	if (m_pITimerEngine==NULL)
	{
		ExportInformation(TEXT("��ȡ��ʱ���������ʧ��"),Level_Exception);
		return false;
	}

	//��������
	m_pITCPSocketEngine=(ITCPSocketEngine *)pIServiceEngine->GetTCPSocketEngine(IID_ITCPSocketEngine,VER_ITCPSocketEngine);
	if (m_pITCPSocketEngine==NULL)
	{
		ExportInformation(TEXT("��ȡ�����������ʧ��"),Level_Exception);
		return false;
	}

	//���ݿ�����
	IDataBaseEngine * pIDataBaseEngine=(IDataBaseEngine *)pIServiceEngine->GetDataBaseEngine(IID_IDataBaseEngine,VER_IDataBaseEngine);
	if (pIDataBaseEngine==NULL)
	{
		ExportInformation(TEXT("��ȡ���ݿ��������ʧ��"),Level_Exception);
		return false;
	}

	//���ݿⴥ��
	IUnknownEx * pIQueueService=(IUnknownEx *)pIDataBaseEngine->GetQueueService(IID_IUnknownEx,VER_IUnknownEx);
	if (m_DataBaseEvent.SetQueueService(pIQueueService)==false)
	{
		ExportInformation(TEXT("��ȡ���ݿⴥ������ʧ��"),Level_Exception);
		return false;
	}

	return true;
}

//���͵�½ʧ��
bool CAttemperEngineSink::SendLogonFailed(WORD wIndex, WORD wRoundID, LPCTSTR pszMessage, LONG lErrorCode)
{
	//��������
	CMD_GR_LogonError LogonError;
	LogonError.lErrorCode=lErrorCode;
	lstrcpyn(LogonError.szErrorDescribe,pszMessage,sizeof(LogonError.szErrorDescribe));
	WORD wDescribeSize=CountString(LogonError.szErrorDescribe);

	//��������
	WORD wSendSize=sizeof(LogonError)-sizeof(LogonError.szErrorDescribe)+wDescribeSize;
	m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GR_LOGON,SUB_GR_LOGON_ERROR,&LogonError,wSendSize);

	return true;
}

//���ͷ�����Ϣ
bool CAttemperEngineSink::SendRoomMessage(WORD wIndex, WORD wRoundID, LPCTSTR lpszMessage, WORD wMessageType)
{
	//�������ݰ�
	CMD_GR_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountString(Message.szContent);

	//��������
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	if (wIndex==INDEX_ALL_SOCKET) m_pITCPSocketEngine->SendDataBatch(MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);
	else m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);

	return true;
}

//������Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(WORD wIndex, WORD wRoundID, LPCTSTR lpszMessage, WORD wMessageType)
{
	//�������ݰ�
	CMD_GF_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountString(Message.szContent);

	//��������
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	if (wIndex==INDEX_ALL_SOCKET) m_pITCPSocketEngine->SendDataBatch(MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);
	else m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);

	return true;
}

//���ͷ�����Ϣ
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	//Ч���û�
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=0xFFFF)
	{
		WORD wRoundID=(m_pConnectItemInfo+wIndex)->wRoundID;
		SendRoomMessage(wIndex,wRoundID,lpszMessage,wMessageType);

		return true;
	}

	return false;
}

//������Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	//Ч���û�
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û�����
	if ((pIServerUserItem->GetTableID()==INVALID_TABLE)||(pIServerUserItem->GetChairID()==INVALID_CHAIR)) return false;

	//��������
	WORD wIndex=pIServerUserItem->GetUserIndex();
	if (wIndex!=0xFFFF)
	{
		WORD wRoundID=(m_pConnectItemInfo+wIndex)->wRoundID;
		SendGameMessage(wIndex,wRoundID,lpszMessage,wMessageType);

		return true;
	}

	return false;
}

//��ʱ���¼�
bool __cdecl CAttemperEngineSink::OnEventTimer(WORD wTimerID, WPARAM wBindParam)
{
	//�ں˶�ʱ��
	if (wTimerID<IDI_TABLE_BEGIN)
	{
		switch (wTimerID)
		{
		case IDI_CONNECT_CENTER_SERVER:		//���ӷ�����
			{
				m_pCenterSocket->Connect(m_dwCenterServer,PORT_CENTER_SERVER);
				return true;
			}
		case IDI_UPDATE_SERVER_LIST:		//�����б�
			{
				if (m_SocketState==SocketState_Connected)
				{
					m_pCenterSocket->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);
				}
				return true;
			}
		case IDI_UPDATE_ONLINE_COUNT:		//��������
			{
				if (m_SocketState==SocketState_Connected)
				{
					CMD_CS_ServerOnLineCount ServerOnLineCount;
					ServerOnLineCount.wKindID=m_pGameServiceOption->wKindID;
					ServerOnLineCount.wServerID=m_pGameServiceOption->wServerID;
					ServerOnLineCount.dwOnLineCount=(m_ServerUserManager.GetOnLineCount()+m_ServerUserManager.GetOffLineCount());
					m_pCenterSocket->SendData(MDM_CS_SERVER_MANAGER,SUB_CS_SERVER_ONLINE_COUNT,&ServerOnLineCount,sizeof(ServerOnLineCount));
				}

				return true;
			}
		case IDI_SEND_SYSTEM_MESSAGE:		//ϵͳ��Ϣ
			{
				//��ȡĿ¼
				TCHAR szPath[MAX_PATH]=TEXT("");
				GetCurrentDirectory(sizeof(szPath),szPath);

				//��ȡ����
				TCHAR szFileName[MAX_PATH],szMessage[1024];
				_snprintf(szFileName,sizeof(szFileName),TEXT("%s\\GameService.ini"),szPath);

				//ϵͳ��Ϣ
				GetPrivateProfileString(TEXT("Message"),TEXT("RoomMessage"),TEXT(""),szMessage,sizeof(szMessage),szFileName);
				if (szMessage[0]!=0) SendRoomMessage(INDEX_ALL_SOCKET,0,szMessage,SMT_INFO);

				//��Ϸ��Ϣ
				GetPrivateProfileString(TEXT("Message"),TEXT("GameMessage"),TEXT(""),szMessage,sizeof(szMessage),szFileName);
				if (szMessage[0]!=0) SendGameMessage(INDEX_ALL_SOCKET,0,szMessage,SMT_INFO);

				return true;
			}
		}
		return false;
	}

	//��������
	WORD wTableID=(wTimerID-IDI_TABLE_BEGIN)/TABLE_TIME_RANGE;
	WORD wTableTimeID=(wTimerID-IDI_TABLE_BEGIN)%TABLE_TIME_RANGE;

	//��ʱ������
	ASSERT(wTableID<m_pGameServiceOption->wTableCount);
	return (m_pTableFrame+wTableID)->OnEventTimer(wTableTimeID,wBindParam);
}

//���ݿ��¼�
bool __cdecl CAttemperEngineSink::OnEventDataBase(void * pDataBuffer, WORD wDataSize, NTY_DataBaseEvent * pDataBaseEvent)
{
	switch (pDataBaseEvent->wRequestID)
	{
	case DBR_GR_LOGON_SUCCESS:		//��¼�ɹ�
		{
			return OnDBLogonSuccess(pDataBuffer,wDataSize,pDataBaseEvent);
		}
	case DBR_GR_LOGON_ERROR:		//��¼ʧ��
		{
			return OnDBLogonError(pDataBuffer,wDataSize,pDataBaseEvent);
		}
	}

	return false;
}

//�û���¼�ɹ�
bool CAttemperEngineSink::OnDBLogonSuccess(void * pDataBuffer, WORD wDataSize, NTY_DataBaseEvent * pDataBaseEvent)
{
	//Ч�����
	ASSERT(pDataBaseEvent->wRequestID==DBR_GR_LOGON_SUCCESS);
	ASSERT(wDataSize==sizeof(DBR_GR_LogonSuccess));
	if (wDataSize!=sizeof(DBR_GR_LogonSuccess)) return false;

	//�ж�����
	ASSERT(pDataBaseEvent->wIndex<m_wMaxUserItem);
	tagConnectItemInfo * pConnectItemInfo=(m_pConnectItemInfo+pDataBaseEvent->wIndex);
	if (pConnectItemInfo->bConnect==false) return true;
	if (pConnectItemInfo->wRoundID!=pDataBaseEvent->wRoundID) return true;
	ASSERT(pConnectItemInfo->pIServerUserItem==NULL);

	//��Ϣ����
	DBR_GR_LogonSuccess * pLogonSuccess=(DBR_GR_LogonSuccess *)pDataBuffer;

	//�����ж�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchOnLineUser(pLogonSuccess->dwUserID);
	if (pIServerUserItem!=NULL) return false;

	//��������
	WORD wIndex=pDataBaseEvent->wIndex;
	WORD wRoundID=pDataBaseEvent->wRoundID;

	//״̬�ж�
	if ((m_bAllowEnterRoom==false)&&(pLogonSuccess->dwMasterRight==0L))
	{
		//������Ϣ
		LPCTSTR pszMessage=TEXT("��Ǹ������ϵͳά����ԭ�򣬱���Ϸ���䲻�����κ���ҵ�¼���룡");
		SendRoomMessage(wIndex,wRoundID,pszMessage,SMT_EJECT|SMT_INFO|SMT_INTERMIT_LINE|SMT_CLOSE_ROOM);

		//�������
		DBR_GR_WriteScore WriteScore;
		ZeroMemory(&WriteScore,sizeof(WriteScore));
		WriteScore.dwUserID=pLogonSuccess->dwUserID;
		WriteScore.dwClientIP=pConnectItemInfo->dwClientIP;
		lstrcpyn(WriteScore.szAccounts,pLogonSuccess->szAccounts,CountArray(WriteScore.szAccounts));
		m_DataBaseEvent.PostDataBaseEvent(DBR_GR_WRITE_SCORE,0,0,&WriteScore,sizeof(WriteScore));

		return true;
	}

	//�ж���Ŀ
	DWORD dwUserCount=m_ServerUserManager.GetOnLineCount()+m_ServerUserManager.GetOffLineCount();
	if ((pLogonSuccess->cbMember==0)&&(pLogonSuccess->dwMasterRight==0L))
	{
		//��ȡ��Ŀ
		DWORD dwUserCount=m_ServerUserManager.GetOnLineCount()+m_ServerUserManager.GetOffLineCount();

		//��Ŀ�ж�
		if ((dwUserCount+50L)>=m_pGameServiceOption->wMaxConnect)
		{
			//������Ϣ
			LPCTSTR pszMessage=TEXT("��Ǹ������Ϸ�����Ѿ����ˣ���ֹ�ǻ�Ա��ҵ�¼���룡");
			SendRoomMessage(wIndex,wRoundID,pszMessage,SMT_EJECT|SMT_INFO|SMT_INTERMIT_LINE|SMT_CLOSE_ROOM);

			//�������
			DBR_GR_WriteScore WriteScore;
			ZeroMemory(&WriteScore,sizeof(WriteScore));
			WriteScore.dwUserID=pLogonSuccess->dwUserID;
			WriteScore.dwClientIP=pConnectItemInfo->dwClientIP;
			lstrcpyn(WriteScore.szAccounts,pLogonSuccess->szAccounts,CountArray(WriteScore.szAccounts));
			m_DataBaseEvent.PostDataBaseEvent(DBR_GR_WRITE_SCORE,0,0,&WriteScore,sizeof(WriteScore));

			return true;
		}
	}

	//�����û���Ϣ
	tagServerUserData ServerUserData;
	memset(&ServerUserData,0,sizeof(ServerUserData));
	lstrcpyn(ServerUserData.szFace,pLogonSuccess->szFace,CountArray(ServerUserData.szFace));
	ServerUserData.cbMember=pLogonSuccess->cbMember;
	ServerUserData.cbGender=pLogonSuccess->cbGender;
	ServerUserData.dwUserID=pLogonSuccess->dwUserID;
	ServerUserData.dwUserRight=pLogonSuccess->dwUserRight;
	ServerUserData.dwMasterRight=pLogonSuccess->dwMasterRight;
	ServerUserData.UserScoreInfo.lGold=pLogonSuccess->lTzPoint;
	ServerUserData.UserScoreInfo.lScore=pLogonSuccess->lScore;
	ServerUserData.UserScoreInfo.lWinCount=pLogonSuccess->lWinCount;
	ServerUserData.UserScoreInfo.lLostCount=pLogonSuccess->lLostCount;
	ServerUserData.UserScoreInfo.lDrawCount=pLogonSuccess->lDrawCount;
	ServerUserData.UserScoreInfo.lFleeCount=pLogonSuccess->lFleeCount;
	lstrcpyn(ServerUserData.szAccounts,pLogonSuccess->szAccounts,CountArray(ServerUserData.szAccounts));
	lstrcpyn(ServerUserData.szName,pLogonSuccess->szName,CountArray(ServerUserData.szName));

	//״̬��Ϣ
	ServerUserData.cbUserStatus=US_FREE;
	ServerUserData.wTableID=INVALID_TABLE;
	ServerUserData.wChairID=INVALID_CHAIR;

	//�����û�
	pConnectItemInfo->wRoundID=wRoundID;
	pConnectItemInfo->pIServerUserItem=m_ServerUserManager.ActiveUserItem(ServerUserData,pConnectItemInfo->dwClientIP,wIndex,pLogonSuccess->szPassWord);

	//���ͷ�����Ϣ
	SendGameServerInfo(pConnectItemInfo->pIServerUserItem,wIndex,wRoundID);

	//�㲥�û���Ϣ
	SendUserItem(pConnectItemInfo->pIServerUserItem,INDEX_ALL_SOCKET,0xFFFF);

	//������Ϣ
	LPCTSTR pszMessage=TEXT("�װ����û�����ӭ����ᱦ���飡");
	SendRoomMessage(wIndex,wRoundID,pszMessage,SMT_INFO);

	//������Ϣ
	if (m_bShallClose==true)
	{
		LPCTSTR pszMessage=TEXT("��ע�⣬������Ҫ������Ϸ����ά�����¹���������Ϸ���伴����ͣ����");
		SendRoomMessage(wIndex,wRoundID,pszMessage,SMT_INFO);
	}

	//����Ⱥ��
	m_pITCPSocketEngine->AllowBatchSend(wIndex,wRoundID,true);

	return true;
}

//�û���¼ʧ��
bool CAttemperEngineSink::OnDBLogonError(void * pDataBuffer, WORD wDataSize, NTY_DataBaseEvent * pDataBaseEvent)
{
	//Ч�����
	ASSERT(pDataBaseEvent->wRequestID==DBR_GR_LOGON_ERROR);
	ASSERT(wDataSize==sizeof(DBR_GR_LogonError));

	//�ж�����
	ASSERT(pDataBaseEvent->wIndex<m_wMaxUserItem);
	tagConnectItemInfo * pConnectItemInfo=(m_pConnectItemInfo+pDataBaseEvent->wIndex);
	if (pConnectItemInfo->bConnect==false) return true;
	if (pConnectItemInfo->wRoundID!=pDataBaseEvent->wRoundID) return true;

	//������Ϣ
	DBR_GR_LogonError * pLogonError=(DBR_GR_LogonError *)pDataBuffer;
	pLogonError->szErrorDescribe[CountString(pLogonError->szErrorDescribe)-1]=0;
	SendLogonFailed(pDataBaseEvent->wIndex,pDataBaseEvent->wRoundID,pLogonError->szErrorDescribe,pLogonError->lErrorCode);
	m_pITCPSocketEngine->ShutDownSocket(pDataBaseEvent->wIndex,pDataBaseEvent->wRoundID);

	return true;
}

//����Ӧ���¼�
bool __cdecl CAttemperEngineSink::OnEventSocketAccept(NTY_SocketAcceptEvent * pSocketAcceptEvent)
{
	//Ч�����
	ASSERT(pSocketAcceptEvent->wIndex<m_wMaxUserItem);
	ASSERT((m_pConnectItemInfo+pSocketAcceptEvent->wIndex)->pIServerUserItem==NULL);

	//��������
	tagConnectItemInfo * pConnectItemInfo=m_pConnectItemInfo+pSocketAcceptEvent->wIndex;
	pConnectItemInfo->bConnect=true;
	pConnectItemInfo->wRoundID=pSocketAcceptEvent->wRoundID;
	pConnectItemInfo->dwClientIP=pSocketAcceptEvent->dwClientIP;

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventSocketRead(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GR_LOGON:	//��¼��Ϣ
		{
			return OnSocketMainLogon(Command,pDataBuffer,wDataSize,pSocketReadEvent);
		}
	case MDM_GR_USER:	//�û���Ϣ
		{
			return OnSocketMainUser(Command,pDataBuffer,wDataSize,pSocketReadEvent);
		}
	case MDM_GR_MANAGER://������Ϣ
		{
			return OnSocketMainManager(Command,pDataBuffer,wDataSize,pSocketReadEvent);
		}
	case MDM_GF_FRAME:	//�����Ϣ
		{
			return OnSocketMainFrame(Command,pDataBuffer,wDataSize,pSocketReadEvent);
		}
	case MDM_GF_GAME:	//��Ϸ��Ϣ
		{
			return OnSocketMainGame(Command,pDataBuffer,wDataSize,pSocketReadEvent);
		}
	}

	return false;
}

//����ر��¼�
bool CAttemperEngineSink::OnEventSocketClose(NTY_SocketCloseEvent * pSocketCloseEvent)
{
	//Ч�����
	ASSERT(pSocketCloseEvent->wIndex<m_wMaxUserItem);

	//�û�����
	tagConnectItemInfo * pConnectItemInfo=m_pConnectItemInfo+pSocketCloseEvent->wIndex;
	IServerUserItem * pIServerUserItem=pConnectItemInfo->pIServerUserItem;
	if (pIServerUserItem!=NULL)
	{
		//�ж���Ϸ
		bool bOffLine=false;
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			//��������
			CTableFrame * pTableFrame=(m_pTableFrame+pIServerUserItem->GetTableID());

			//����ע��
			WORD wChairID=pIServerUserItem->GetChairID();
			if ((pTableFrame->IsUserPlaying(pIServerUserItem)==true)&&(pTableFrame->OnUserReqOffLine(wChairID)==true))
			{
				bOffLine=m_ServerUserManager.RegOffLineUserItem(pIServerUserItem);
			}

			//���ߴ���
			if (bOffLine==true) pTableFrame->OnUserOffLine(wChairID);
			else pTableFrame->PerformStandUpAction(pIServerUserItem);
		}

		//ɾ���û�
		if (bOffLine==false) DeleteUserItem(pIServerUserItem);
	}

	//��������
	pConnectItemInfo->wRoundID++;
	pConnectItemInfo->dwClientIP=0;
	pConnectItemInfo->bConnect=false;
	pConnectItemInfo->pIServerUserItem=NULL;

	return true;
}

//���������¼�
bool __cdecl CAttemperEngineSink::OnEventCenterSocketConnect(int iErrorCode)
{
	//�����ж�
	if (iErrorCode!=0)
	{
		m_SocketState=SocketState_NoConnect;
		m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER_SERVER,TIME_RECONNECT,1,NULL);
		return true;
	}

	//���ñ���
	m_SocketState=SocketState_Connected;

	//ע�᷿��
	CMD_CS_RegGameServer RegGameServer;
	memset(&RegGameServer,0,sizeof(RegGameServer));
	RegGameServer.GameServer.wSortID=0;
	RegGameServer.GameServer.wKindID=m_pGameServiceOption->wKindID;
	RegGameServer.GameServer.wServerID=m_pGameServiceOption->wServerID;
	RegGameServer.GameServer.wStationID=m_pGameServiceOption->wStationID;
	RegGameServer.GameServer.wServerPort=m_pGameServiceOption->wServerPort;
	RegGameServer.GameServer.dwServerAddr=m_pGameServiceOption->dwServerAddr;
	RegGameServer.GameServer.dwOnLineCount=(m_ServerUserManager.GetOnLineCount()+m_ServerUserManager.GetOffLineCount())*2;
	lstrcpyn(RegGameServer.GameServer.szServerName,m_pGameServiceOption->szGameRoomName,CountArray(RegGameServer.GameServer.szServerName));
	m_pCenterSocket->SendData(MDM_CS_SERVER_MANAGER,SUB_CS_REG_GAME_SERVER,&RegGameServer,sizeof(RegGameServer));

	//��ȡ�б�
	m_pCenterSocket->SendData(MDM_CS_SERVER_LIST,SUB_CS_GET_SERVER_LIST,NULL,0);

	//���ö�ʱ��
	m_pITimerEngine->SetTimer(IDI_UPDATE_SERVER_LIST,TIME_UPDATE_LIST,TIMER_REPEAT_TIMER,NULL);
	m_pITimerEngine->SetTimer(IDI_UPDATE_ONLINE_COUNT,TIME_UPDATE_ONLINE,TIMER_REPEAT_TIMER,NULL);

	return true;
}

//���Ķ�ȡ�¼�
bool __cdecl CAttemperEngineSink::OnEventCenterSocketRead(CMD_Command Command, void * pDataBuffer, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CS_SERVER_LIST:	//�б���Ϣ
		{
			return OnCenterMainServerList(Command,pDataBuffer,wDataSize);
		}
	}
	return true;
}

//���Ĺر��¼�
bool __cdecl CAttemperEngineSink::OnEventCenterSocketClose(bool bCloseByServer)
{
	//���ñ���
	m_SocketState=SocketState_NoConnect;

	//��������
	m_pITimerEngine->KillTimer(IDI_UPDATE_SERVER_LIST);
	m_pITimerEngine->KillTimer(IDI_UPDATE_ONLINE_COUNT);
	m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER_SERVER,TIME_RECONNECT,1,NULL);

	return true;
}

//��¼��Ϣ����
bool CAttemperEngineSink::OnSocketMainLogon(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent)
{
	ASSERT(Command.wMainCmdID==MDM_GR_LOGON);
	switch (Command.wSubCmdID)
	{
	case SUB_GR_LOGON_USERID:		//ID ��¼
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(CMD_GR_LogonByUserID));
			if (wDataSize<sizeof(CMD_GR_LogonByUserID)) return false;

			//������Ϣ
			CMD_GR_LogonByUserID * pLogonByUserID=(CMD_GR_LogonByUserID *)pDataBuffer;
			pLogonByUserID->szPassWord[CountArray(pLogonByUserID->szPassWord)-1]=0;

			//�ظ��ж�
			tagConnectItemInfo * pConnectItemInfo=(m_pConnectItemInfo+pSocketReadEvent->wIndex);
			IServerUserItem * pIServerUserItem=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItem!=NULL) return false;

			//�����ж�
			pIServerUserItem=m_ServerUserManager.SearchOnLineUser(pLogonByUserID->dwUserID);
			if (pIServerUserItem!=NULL)
			{
				//Ч������
				if (lstrcmp(pIServerUserItem->GetPassword(),pLogonByUserID->szPassWord)!=0)
				{
					TCHAR szMessage[]=TEXT("�ʻ������������֤���ٴγ��Ե�¼��");
					SendLogonFailed(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID,szMessage,0);
					m_pITCPSocketEngine->ShutDownSocket(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID);
					return true;
				}

				//����֪ͨ
				WORD wSourceIndex=pIServerUserItem->GetUserIndex();
				WORD wSourceRoundID=(m_pConnectItemInfo+wSourceIndex)->wRoundID;
				LPCTSTR pszMessage=TEXT("��ע�⣬����ʺ�����һ�ط��������Ϸ�����ˣ��㱻���뿪��");
				SendRoomMessage(wSourceIndex,wSourceRoundID,pszMessage,SMT_INFO|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

				//�Ͽ�ԭ�û�
				(m_pConnectItemInfo+wSourceIndex)->pIServerUserItem=NULL;
				m_pITCPSocketEngine->ShutDownSocket(wSourceIndex,wSourceRoundID);

				//�������û�
				pConnectItemInfo->pIServerUserItem=pIServerUserItem;
				pConnectItemInfo->wRoundID=pSocketReadEvent->wRoundID;
				m_ServerUserManager.SwitchOnLineUserItem(pIServerUserItem,pConnectItemInfo->dwClientIP,pSocketReadEvent->wIndex);

				//���ͷ�����Ϣ
				SendGameServerInfo(pIServerUserItem,pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID);

				//����Ⱥ��
				m_pITCPSocketEngine->AllowBatchSend(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID,true);

				return true;
			}

			//�����ж�
			pIServerUserItem=m_ServerUserManager.SearchOffLineUser(pLogonByUserID->dwUserID);
			if (pIServerUserItem!=NULL)
			{
				//Ч������
				if (lstrcmp(pIServerUserItem->GetPassword(),pLogonByUserID->szPassWord)!=0)
				{
					TCHAR szMessage[]=TEXT("�ʻ������������֤���ٴγ��Ե�¼��");
					SendLogonFailed(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID,szMessage,0);
					m_pITCPSocketEngine->ShutDownSocket(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID);
					return true;
				}

				//�����û�
				pConnectItemInfo->pIServerUserItem=pIServerUserItem;
				pConnectItemInfo->wRoundID=pSocketReadEvent->wRoundID;
				m_ServerUserManager.ActiveOffLineUserItem(pIServerUserItem,pConnectItemInfo->dwClientIP,pSocketReadEvent->wIndex);

				//��������
				WORD wTableID=pIServerUserItem->GetTableID();
				WORD wChairID=pIServerUserItem->GetChairID();
				(m_pTableFrame+wTableID)->OnUserReConnect(wChairID);

				//���ͷ�����Ϣ
				SendGameServerInfo(pIServerUserItem,pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID);

				//����Ⱥ��
				m_pITCPSocketEngine->AllowBatchSend(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID,true);

				return true;
			}

			//������Ϣ
			ASSERT(pSocketReadEvent->wIndex<m_wMaxUserItem);
			DWORD dwClientAddr=(m_pConnectItemInfo+pSocketReadEvent->wIndex)->dwClientIP;

			//��������
			DBR_GR_LogonByUserID LogonByUserID;
			memset(&LogonByUserID,0,sizeof(LogonByUserID));
			LogonByUserID.dwClientIP=dwClientAddr;
			LogonByUserID.dwUserID=pLogonByUserID->dwUserID;
			lstrcpyn(LogonByUserID.szPassWord,pLogonByUserID->szPassWord,sizeof(LogonByUserID.szPassWord));

			//Ͷ�����ݿ�
			m_DataBaseEvent.PostDataBaseEvent(DBR_GR_LOGON_BY_USERID,pSocketReadEvent->wIndex,
				pSocketReadEvent->wRoundID,&LogonByUserID,sizeof(LogonByUserID));

			return true;
		}
	case SUB_GR_LOGON_ACCOUNTS:
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(CMD_GR_LogonByAccounts));
			if (wDataSize<sizeof(CMD_GR_LogonByAccounts)) return false;

			//������Ϣ
			CMD_GR_LogonByAccounts * pLogonByAccounts=(CMD_GR_LogonByAccounts *)pDataBuffer;
			pLogonByAccounts->szAccounts[CountArray(pLogonByAccounts->szAccounts)-1]=0;
			pLogonByAccounts->szPassWord[CountArray(pLogonByAccounts->szPassWord)-1]=0;

			//�ظ��ж�
			tagConnectItemInfo * pConnectItemInfo=(m_pConnectItemInfo+pSocketReadEvent->wIndex);
			IServerUserItem * pIServerUserItem=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItem!=NULL) return false;

			//�����ж�
			pIServerUserItem=m_ServerUserManager.SearchOnLineUser(pLogonByAccounts->szAccounts);
			if (pIServerUserItem!=NULL)
			{
				//����֪ͨ
				WORD wSourceIndex=pIServerUserItem->GetUserIndex();
				WORD wSourceRoundID=(m_pConnectItemInfo+wSourceIndex)->wRoundID;
				LPCTSTR pszMessage=TEXT("��ע�⣬����ʺ�����һ�ط��������Ϸ�����ˣ��㱻���뿪��");
				SendRoomMessage(wSourceIndex,wSourceRoundID,pszMessage,SMT_INFO|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);

				//�Ͽ�ԭ�û�
				(m_pConnectItemInfo+wSourceIndex)->pIServerUserItem=NULL;
				m_pITCPSocketEngine->ShutDownSocket(wSourceIndex,wSourceRoundID);

				//�������û�
				pConnectItemInfo->pIServerUserItem=pIServerUserItem;
				pConnectItemInfo->wRoundID=pSocketReadEvent->wRoundID;
				m_ServerUserManager.SwitchOnLineUserItem(pIServerUserItem,pConnectItemInfo->dwClientIP,pSocketReadEvent->wIndex);

				//���ͷ�����Ϣ
				SendGameServerInfo(pIServerUserItem,pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID);

				//����Ⱥ��
				m_pITCPSocketEngine->AllowBatchSend(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID,true);

				return true;
			}

			//�����ж�
			pIServerUserItem=m_ServerUserManager.SearchOffLineUser(pLogonByAccounts->szAccounts);
			if (pIServerUserItem!=NULL)
			{
				//Ч������
				if (lstrcmp(pIServerUserItem->GetPassword(),pLogonByAccounts->szPassWord)!=0)
				{
					TCHAR szMessage[]=TEXT("�ʻ������������֤���ٴγ��Ե�¼��");
					SendLogonFailed(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID,szMessage,0);
					m_pITCPSocketEngine->ShutDownSocket(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID);
					return true;
				}

				//�����û�
				pConnectItemInfo->pIServerUserItem=pIServerUserItem;
				pConnectItemInfo->wRoundID=pSocketReadEvent->wRoundID;
				m_ServerUserManager.ActiveOffLineUserItem(pIServerUserItem,pConnectItemInfo->dwClientIP,pSocketReadEvent->wIndex);

				//��������
				WORD wTableID=pIServerUserItem->GetTableID();
				WORD wChairID=pIServerUserItem->GetChairID();
				(m_pTableFrame+wTableID)->OnUserReConnect(wChairID);

				//���ͷ�����Ϣ
				SendGameServerInfo(pIServerUserItem,pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID);

				//����Ⱥ��
				m_pITCPSocketEngine->AllowBatchSend(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID,true);

				return true;
			}

			//������Ϣ
			ASSERT(pSocketReadEvent->wIndex<m_wMaxUserItem);
			DWORD dwClientAddr=(m_pConnectItemInfo+pSocketReadEvent->wIndex)->dwClientIP;

			//��������
			DBR_GR_LogonByAccounts LogonByAccounts;
			memset(&LogonByAccounts,0,sizeof(LogonByAccounts));
			LogonByAccounts.dwClientIP=dwClientAddr;
			lstrcpyn(LogonByAccounts.szAccounts,pLogonByAccounts->szAccounts,sizeof(LogonByAccounts.szAccounts));
			lstrcpyn(LogonByAccounts.szPassWord,pLogonByAccounts->szPassWord,sizeof(LogonByAccounts.szPassWord));

			//Ͷ�����ݿ�
			m_DataBaseEvent.PostDataBaseEvent(DBR_GR_LOGON_BY_ACCOUNTS,pSocketReadEvent->wIndex,
				pSocketReadEvent->wRoundID,&LogonByAccounts,sizeof(LogonByAccounts));

			return true;
		}
	}

	return false;
}

//�û���Ϣ����
bool CAttemperEngineSink::OnSocketMainUser(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent)
{
	//Ч�����
	ASSERT(Command.wMainCmdID==MDM_GR_USER);

	switch (Command.wSubCmdID)
	{
	case SUB_GR_USER_SIT_REQ:		//��������
	case SUB_GR_USER_LOOKON_REQ:	//�Թ�����
		{
			//��������
			CMD_GR_UserSitReq * pUserSitReq=(CMD_GR_UserSitReq *)pDataBuffer;
			WORD wHeadSize=sizeof(CMD_GR_UserSitReq)-sizeof(pUserSitReq->szTablePass);
				
			//Ч������
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize<=sizeof(CMD_GR_UserSitReq)));
			if ((wDataSize<wHeadSize)||(wDataSize>sizeof(CMD_GR_UserSitReq))) return false;
			ASSERT((wHeadSize+pUserSitReq->cbPassLen)==wDataSize);
			if ((wHeadSize+pUserSitReq->cbPassLen)!=wDataSize) return false;

			//�ж�λ��
			if (pUserSitReq->wTableID>=m_pGameServiceOption->wTableCount) return false;
			if (pUserSitReq->wChairID>=m_pGameServiceAttrib->wChairCount) return false;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItem==NULL) return false;

			//�������
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if ((pUserData->wTableID==pUserSitReq->wTableID)&&(pUserData->wChairID==pUserSitReq->wChairID)) return true;

			//������Ϣ
			pUserData->wNetDelay=pUserSitReq->wNetTimelag;

			//�뿪λ��
			if (pUserData->wTableID!=INVALID_TABLE)
			{
				CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
				if (pTableFrame->IsUserPlaying(pIServerUserItem)==true)
				{
					//��������
					CMD_GR_SitFailed SitFailed;
					memset(&SitFailed,0,sizeof(SitFailed));
					_snprintf(SitFailed.szFailedDescribe,sizeof(SitFailed.szFailedDescribe),TEXT("�����ڵ� [ %d ] ����Ϸ����Ϸ����ʱ�����뿪λ�ã�"),pUserData->wTableID+1);

					//��������
					WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountString(SitFailed.szFailedDescribe);
					SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);

					return true;
				}
				pTableFrame->PerformStandUpAction(pIServerUserItem);
			}

			//����λ��
			CTableFrame * pTableFrame=(m_pTableFrame+pUserSitReq->wTableID);
			if (Command.wSubCmdID==SUB_GR_USER_SIT_REQ)	
			{
				pTableFrame->PerformSitDownAction(pUserSitReq->wChairID,pIServerUserItem);
			}
			else
			{
				pTableFrame->PerformLookonAction(pUserSitReq->wChairID,pIServerUserItem);
			}

			return true;
		}
	case SUB_GR_USER_STANDUP_REQ:	//��������
	case SUB_GR_USER_LEFT_GAME_REQ:	//ǿ������
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItem==NULL) return false;

			//�������
			tagServerUserData * pUserData=pIServerUserItem->GetUserData();
			if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

			//����λ��
			CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
			if ((Command.wSubCmdID==SUB_GR_USER_STANDUP_REQ)&&(pTableFrame->IsUserPlaying(pIServerUserItem)==true))
			{
				TCHAR szMessage[256]=TEXT("");
				_snprintf(szMessage,sizeof(szMessage),TEXT("�����ڵ� [ %d ] ����Ϸ����Ϸ����ʱ�����뿪λ�ã�"),pUserData->wTableID+1);
				SendRoomMessage(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID,szMessage,SMT_EJECT);
				return true;
			}
			pTableFrame->PerformStandUpAction(pIServerUserItem);

			return true;
		}
	case SUB_GR_USER_CHAT:			//������Ϣ
		{
			//Ч�����
			CMD_GR_UserChat * pUserChat=(CMD_GR_UserChat *)pDataBuffer;
			ASSERT(wDataSize>=(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage))) return false;
			if (wDataSize!=(sizeof(CMD_GR_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength)) return false;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItem==NULL) return false;

			//Ѱ���û�
			IServerUserItem * pIServerUserItemRecv=NULL;
			if (pUserChat->dwTargetUserID!=0) 
			{
				pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pUserChat->dwTargetUserID);
				if (pIServerUserItemRecv==NULL) return true;
			}

			//״̬�ж�
			if (m_bAllowRoomChat==false)
			{
				LPCTSTR pszMessage=TEXT("��Ǹ������Ϸ���䲻������д������ģ�");
				SendRoomMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
				return true;
			}

			//Ȩ���ж�
			if (CUserRight::CanRoomChat(pIServerUserItem->GetUserData()->dwUserRight)==false)
			{
				LPCTSTR pszMessage=TEXT("��Ǹ����û�д������ĵ�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��");
				SendRoomMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
				return true;
			}

			//ת����Ϣ
			m_pITCPSocketEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_CHAT,pUserChat,wDataSize);

			return true;
		}
	case SUB_GR_USER_WISPER:		//˽����Ϣ
		{
			//Ч�����
			CMD_GR_Wisper * pWisper=(CMD_GR_Wisper *)pDataBuffer;
			ASSERT(wDataSize>=(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)+pWisper->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage))) return false;
			if (wDataSize!=(sizeof(CMD_GR_Wisper)-sizeof(pWisper->szChatMessage)+pWisper->wChatLength)) return false;

			//��ȡ�û�
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItemSend==NULL) return false;
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pWisper->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//״̬�ж�
			DWORD dwMasterRightSend=pIServerUserItemSend->GetUserData()->dwMasterRight;
			DWORD dwMasterRightRecv=pIServerUserItemRecv->GetUserData()->dwMasterRight;
			if ((m_bAllowWisper==false)&&(dwMasterRightSend==0L)&&(dwMasterRightRecv==0L))
			{
				LPCTSTR pszMessage=TEXT("��Ǹ������Ϸ���䲻�ܽ������˽�ģ�");
				SendRoomMessage(pIServerUserItemSend,pszMessage,SMT_EJECT|SMT_INFO);
				return true;
			}

			//Ȩ���ж�
			DWORD dwUserRightSend=pIServerUserItemSend->GetUserData()->dwUserRight;
			if ((dwMasterRightRecv==0L)&&(CUserRight::CanWisper(dwUserRightSend)==false))
			{
				TCHAR szMessage[256]=TEXT("����ʱû������ҷ���˽����Ϣ��Ȩ�ޣ�ֻ�������Ա˽�ģ�");
				SendRoomMessage(pSocketReadEvent->wIndex,pSocketReadEvent->wRoundID,szMessage,SMT_EJECT);
				return true;
			}

			//ת����Ϣ
			SendData(pIServerUserItemSend,MDM_GR_USER,SUB_GR_USER_WISPER,pWisper,wDataSize);
			SendData(pIServerUserItemRecv,MDM_GR_USER,SUB_GR_USER_WISPER,pWisper,wDataSize);

			return true;
		}
	case SUB_GR_USER_RULE:		//�û�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GR_UserRule));
			if (wDataSize!=sizeof(CMD_GR_UserRule)) return false;

			//��Ϣ����
			CMD_GR_UserRule * pUserRule=(CMD_GR_UserRule *)pDataBuffer;
			pUserRule->szPassword[CountArray(pUserRule->szPassword)-1]=0;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItem==NULL) return false;

			//�������
			tagUserRule UserRule;
			UserRule.bLimitWin=pUserRule->bLimitWin;
			UserRule.bPassword=pUserRule->bPassword;
			UserRule.bLimitFlee=pUserRule->bLimitFlee;
			UserRule.bLimitScore=pUserRule->bLimitScore;
			UserRule.bLimitDelay=pUserRule->bLimitDelay;
			UserRule.bCheckSameIP=pUserRule->bCheckSameIP;
			UserRule.wWinRate=pUserRule->wWinRate;
			UserRule.wFleeRate=pUserRule->wFleeRate;
			UserRule.wNetDelay=pUserRule->wNetDelay;
			UserRule.lMaxScore=pUserRule->lMaxScore;
			UserRule.lLessScore=pUserRule->lLessScore;
			lstrcpyn(UserRule.szPassword,pUserRule->szPassword,CountArray(UserRule.szPassword));

			//���ù���
			pIServerUserItem->SetUserRule(UserRule);

			return true;
		}

	case SUB_GR_USER_INVITE_REQ:		//�����û�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_UserInviteReq));
			if (wDataSize!=sizeof(CMD_GR_UserInviteReq)) return false;

			//��Ϣ����
			CMD_GR_UserInviteReq * pUserInviteReq=(CMD_GR_UserInviteReq *)pDataBuffer;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItem==NULL) return false;

			//Ч��״̬
			if (pUserInviteReq->wTableID==INVALID_TABLE) return true;
			if (pIServerUserItem->GetTableID()!=pUserInviteReq->wTableID) return true;

			//Ŀ���û�
			IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchOnLineUser(pUserInviteReq->dwUserID);
			if (pITargetUserItem==NULL) return true;

			//������Ϣ
			CMD_GR_UserInvite UserInvite;
			memset(&UserInvite,0,sizeof(UserInvite));
			UserInvite.wTableID=pUserInviteReq->wTableID;
			UserInvite.dwUserID=pIServerUserItem->GetUserID();
			SendData(pITargetUserItem,MDM_GR_USER,SUB_GR_USER_INVITE,&UserInvite,sizeof(UserInvite));

			return true;
		}
	}

	return true;
}

//������Ϣ����
bool CAttemperEngineSink::OnSocketMainManager(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent)
{
	//Ч�����
	ASSERT(Command.wMainCmdID==MDM_GR_MANAGER);

	//��Ϣ����
	switch (Command.wSubCmdID)
	{
	case SUB_GR_SEND_WARNING:	//������Ϣ
		{
			//Ч�����
			CMD_GR_SendWarning * pSendWarning=(CMD_GR_SendWarning *)pDataBuffer;
			ASSERT(wDataSize>=(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage)+pSendWarning->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage))) return false;
			if (wDataSize!=(sizeof(CMD_GR_SendWarning)-sizeof(pSendWarning->szWarningMessage)+pSendWarning->wChatLength)) return false;

			//�����û�
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItemSend==NULL) return false;

			//Ȩ���ж�
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanSendWarning(dwMasterRight)==false) return true;

			//Ŀ���û�
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pSendWarning->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//������Ϣ
			SendGameMessage(pIServerUserItemRecv,pSendWarning->szWarningMessage,SMT_INFO);
			SendRoomMessage(pIServerUserItemRecv,pSendWarning->szWarningMessage,SMT_INFO|SMT_EJECT);

			return true;
		}
	case SUB_GR_LOOK_USER_IP:	//�鿴��ַ
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_LookUserIP));
			if (wDataSize!=sizeof(CMD_GR_LookUserIP)) return false;

			//��Ϣ����
			CMD_GR_LookUserIP * pLookUserIP=(CMD_GR_LookUserIP *)pDataBuffer;

			//�����û�
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItemSend==NULL) return false;

			//Ȩ���ж�
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanSeeUserIP(dwMasterRight)==false) return true;

			//Ŀ���û�
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pLookUserIP->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//��������
			DWORD dwUserIP=pIServerUserItemRecv->GetClientIP();

			//������Ϣ
			TCHAR szMessage[128]=TEXT("");
			BYTE * pcbUserIP=(BYTE *)(&dwUserIP);
			_snprintf(szMessage,CountArray(szMessage),TEXT("[ %s ]��%d.%d.%d.%d"),pIServerUserItemRecv->GetAccounts(),
				pcbUserIP[0],pcbUserIP[1],pcbUserIP[2],pcbUserIP[3]);

			//������Ϣ
			SendRoomMessage(pIServerUserItemSend,szMessage,SMT_INFO);

			return true;
		}
	case SUB_GR_KILL_USER:		//�߳��û�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_KillUser));
			if (wDataSize!=sizeof(CMD_GR_KillUser)) return false;

			//��Ϣ����
			CMD_GR_KillUser * pKillUser=(CMD_GR_KillUser *)pDataBuffer;

			//�����û�
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItemSend==NULL) return false;

			//Ȩ���ж�
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanCutUser(dwMasterRight)==false) return true;

			//Ŀ���û�
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pKillUser->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//������Ϣ
			LPCTSTR szMessage=TEXT("��Ǹ����������Ա�߳����䣬�����κ����ʣ�����ϵ��Ϸ�ͷ���");
			SendRoomMessage(pIServerUserItemRecv,szMessage,SMT_INFO|SMT_EJECT|SMT_INTERMIT_LINE);

			return true;
		}
	case SUB_GR_LIMIT_ACCOUNS:	//�����ʻ�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_LimitAccounts));
			if (wDataSize!=sizeof(CMD_GR_LimitAccounts)) return false;

			//��Ϣ����
			CMD_GR_LimitAccounts * pLimitAccounts=(CMD_GR_LimitAccounts *)pDataBuffer;

			//�����û�
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItemSend==NULL) return false;

			//Ȩ���ж�
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanForbidAccounts(dwMasterRight)==false) return true;

			//Ŀ���û�
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pLimitAccounts->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//Ͷ�����ݿ�
			DBR_GR_LimitAccounts LimitAccounts;
			LimitAccounts.dwUserID=pLimitAccounts->dwTargetUserID;
			LimitAccounts.dwMasterUserID=pIServerUserItemSend->GetUserID();
			LimitAccounts.dwMasterClientIP=pIServerUserItemSend->GetClientIP();
			m_DataBaseEvent.PostDataBaseEvent(DBR_GR_LIMIT_ACCOUNTS,0,0,&LimitAccounts,sizeof(LimitAccounts));

			//������Ϣ
			LPCTSTR szMessage=TEXT("��Ǹ�������ʺű������ˣ������κ����ʣ�����ϵ��Ϸ�ͷ���");
			SendRoomMessage(pIServerUserItemRecv,szMessage,SMT_INFO|SMT_EJECT|SMT_INTERMIT_LINE);

			return true;
		}
	case SUB_GR_SET_USER_RIGHT:	//�û�Ȩ��
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_SetUserRight));
			if (wDataSize!=sizeof(CMD_GR_SetUserRight)) return false;

			//��Ϣ����
			CMD_GR_SetUserRight * pSetUserRight=(CMD_GR_SetUserRight *)pDataBuffer;

			//�����û�
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItemSend==NULL) return false;

			//Ŀ���û�
			IServerUserItem * pIServerUserItemRecv=m_ServerUserManager.SearchOnLineUser(pSetUserRight->dwTargetUserID);
			if (pIServerUserItemRecv==NULL) return true;

			//Ȩ���ж�
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (dwMasterRight==0L) return true;

			//��ȡȨ��
			tagServerUserData * pUserData=pIServerUserItemRecv->GetUserData();

			//Ȩ������
			if (CUserRight::CanLimitPlay(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitPlayGame==TRUE) pUserData->dwUserRight|=UR_CANNOT_PLAY;
				else pUserData->dwUserRight&=~UR_CANNOT_PLAY;
			}
			if (CUserRight::CanLimitLookon(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitLookonGame==TRUE) pUserData->dwUserRight|=UR_CANNOT_LOOKON;
				else pUserData->dwUserRight&=~UR_CANNOT_LOOKON;
			}
			if (CUserRight::CanLimitWisper(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitSendWisper==TRUE) pUserData->dwUserRight|=UR_CANNOT_WISPER;
				else pUserData->dwUserRight&=~UR_CANNOT_WISPER;
			}
			if (CUserRight::CanLimitRoomChat(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitRoomChat==TRUE) pUserData->dwUserRight|=UR_CANNOT_ROOM_CHAT;
				else pUserData->dwUserRight&=~UR_CANNOT_ROOM_CHAT;
			}
			if (CUserRight::CanLimitGameChat(dwMasterRight)==true)
			{
				if (pSetUserRight->cbLimitGameChat==TRUE) pUserData->dwUserRight|=UR_CANNOT_GAME_CHAT;
				else pUserData->dwUserRight&=~UR_CANNOT_GAME_CHAT;
			}

			//����Ȩ��
			DBR_GR_SetUserRight SetUserRight;
			SetUserRight.dwUserRight=pUserData->dwUserRight;
			SetUserRight.dwUserID=pSetUserRight->dwTargetUserID;
			SetUserRight.dwMasterUserID=pIServerUserItemSend->GetUserID();
			SetUserRight.dwMasterClientIP=pIServerUserItemSend->GetClientIP();
			SetUserRight.cbGame=(CUserRight::CanBindGame(dwMasterRight)==true)?TRUE:FALSE;
			SetUserRight.cbAccounts=(CUserRight::CanBindGlobal(dwMasterRight)==true)?TRUE:FALSE;

			//Ͷ������
			if ((SetUserRight.cbAccounts==TRUE)||(SetUserRight.cbGame==TRUE))
			{
				m_DataBaseEvent.PostDataBaseEvent(DBR_GR_SET_USER_RIGHT,0,0,&SetUserRight,sizeof(SetUserRight));
			}

			return true;
		}
	case SUB_GR_SEND_MESSAGE:	//������Ϣ
		{
			//Ч�����
			CMD_GR_SendMessage * pSendMessage=(CMD_GR_SendMessage *)pDataBuffer;
			ASSERT(wDataSize>=(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)));
			ASSERT(wDataSize==(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+pSendMessage->wChatLength));
			if (wDataSize<(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage))) return false;
			if (wDataSize!=(sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szSystemMessage)+pSendMessage->wChatLength)) return false;

			//�����û�
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItemSend==NULL) return false;

			//Ȩ���ж�
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanIssueMessage(dwMasterRight)==false) return true;

			//������Ϣ
			if (pSendMessage->cbGame==TRUE) SendGameMessage(INDEX_ALL_SOCKET,0,pSendMessage->szSystemMessage,SMT_INFO);
			if (pSendMessage->cbRoom==TRUE) SendRoomMessage(INDEX_ALL_SOCKET,0,pSendMessage->szSystemMessage,SMT_INFO);

			return true;
		}
	case SUB_GR_OPTION_SERVER:	//��������
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_OptionServer));
			if (wDataSize!=sizeof(CMD_GR_OptionServer)) return false;

			//��Ϣ����
			CMD_GR_OptionServer * pOptionServer=(CMD_GR_OptionServer *)pDataBuffer;

			//�����û�
			IServerUserItem * pIServerUserItemSend=GetServerUserItem(pSocketReadEvent->wIndex);
			if (pIServerUserItemSend==NULL) return false;

			//Ȩ���ж�
			DWORD dwMasterRight=pIServerUserItemSend->GetUserData()->dwMasterRight;
			if (CUserRight::CanServerOption(dwMasterRight)==false) return true;

			//��������
			if (pOptionServer->cbOptionFlags==OSF_ROOM_CHAT)
			{
				m_bAllowRoomChat=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//��Ϸ����
			if (pOptionServer->cbOptionFlags==OSF_GAME_CHAT)
			{
				m_bAllowGameChat=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//����˽��
			if (pOptionServer->cbOptionFlags==OSF_ROOM_WISPER)
			{
				m_bAllowWisper=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//������Ϸ
			if (pOptionServer->cbOptionFlags==OSF_ENTER_GAME)
			{
				m_bAllowEnterGame=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//���뷿��
			if (pOptionServer->cbOptionFlags==OSF_ENTER_ROOM)
			{
				m_bAllowEnterRoom=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//�����ر�
			if (pOptionServer->cbOptionFlags==OSF_SHALL_CLOSE)
			{
				m_bShallClose=(pOptionServer->cbOptionValue==TRUE)?true:false;
			}

			//������Ϣ
			LPCTSTR pszMessage=TEXT("�������������ò���ѡ�����óɹ���");
			SendRoomMessage(pIServerUserItemSend,pszMessage,SMT_INFO);

			return true;
		}
	}

	return false;
}

//�����Ϣ����
bool CAttemperEngineSink::OnSocketMainFrame(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent)
{
	//Ч�����
	ASSERT(Command.wMainCmdID==MDM_GF_FRAME);

	//��ȡ�û�
	IServerUserItem * pIServerUserItem=GetServerUserItem(pSocketReadEvent->wIndex);
	if (pIServerUserItem==NULL) return false;

	//�������
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
	return pTableFrame->OnEventSocketFrame(Command.wSubCmdID,pDataBuffer,wDataSize,pIServerUserItem);
}

//��Ϸ��Ϣ����
bool CAttemperEngineSink::OnSocketMainGame(CMD_Command Command, void * pDataBuffer, WORD wDataSize, NTY_SocketReadEvent * pSocketReadEvent)
{
	//Ч�����
	ASSERT(Command.wMainCmdID==MDM_GF_GAME);

	//��ȡ�û�
	IServerUserItem * pIServerUserItem=GetServerUserItem(pSocketReadEvent->wIndex);
	if (pIServerUserItem==NULL) return false;

	//�������
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrame * pTableFrame=(m_pTableFrame+pUserData->wTableID);
	return pTableFrame->OnEventSocketGame(Command.wSubCmdID,pDataBuffer,wDataSize,pIServerUserItem);
}

//��ȡ�û�
IServerUserItem * CAttemperEngineSink::GetServerUserItem(WORD wIndex)
{
	ASSERT(wIndex<m_wMaxUserItem);
	return (m_pConnectItemInfo+wIndex)->pIServerUserItem;
}

//�����û�
bool CAttemperEngineSink::SendUserItem(IServerUserItem * pIServerUserItem, WORD wTargetIndex, WORD wTargetRoundID)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);

	//��������
	BYTE cbBuffer[SOCKET_PACKAGE];
	tagUserInfoHead * pUserInfoHead=(tagUserInfoHead *)cbBuffer;
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();

	//��д����
	memset(pUserInfoHead,0,sizeof(tagUserInfoHead));
	pUserInfoHead->wTableID=pUserData->wTableID;
	pUserInfoHead->wChairID=pUserData->wChairID;
	pUserInfoHead->wNetDelay=pUserData->wNetDelay;
	pUserInfoHead->cbGender=pUserData->cbGender;
	pUserInfoHead->cbMember=pUserData->cbMember;
	pUserInfoHead->cbUserStatus=pUserData->cbUserStatus;
	pUserInfoHead->dwUserID=pUserData->dwUserID;
	pUserInfoHead->dwUserRight=pUserData->dwUserRight;
	pUserInfoHead->dwMasterRight=pUserData->dwMasterRight;
	pUserInfoHead->cbGender=pUserData->cbGender;
	pUserInfoHead->UserScoreInfo=pUserData->UserScoreInfo;

	//��������
	CSendPacketHelper SendPacket(cbBuffer+sizeof(tagUserInfoHead),sizeof(cbBuffer)-sizeof(tagUserInfoHead));
	SendPacket.AddPacket(pUserData->szAccounts,CountString(pUserData->szAccounts),DTP_USER_ACCOUNTS);
	if (pUserData->szName[0]!=0)
		SendPacket.AddPacket(pUserData->szName,CountString(pUserData->szName),DTP_USER_NAME);
	SendPacket.AddPacket(pUserData->szFace,CountString(pUserData->szFace),DTP_USER_FACE);

	//��������
	WORD wSendLength=sizeof(tagUserInfoHead)+SendPacket.GetDataSize();
	if (wTargetIndex!=INDEX_ALL_SOCKET) 
	{
		ASSERT(wTargetIndex<m_wMaxUserItem);
		m_pITCPSocketEngine->SendData(wTargetIndex,wTargetRoundID,MDM_GR_USER,SUB_GR_USER_COME,cbBuffer,wSendLength);
	}
	else m_pITCPSocketEngine->SendDataBatch(MDM_GR_USER,SUB_GR_USER_COME,cbBuffer,wSendLength);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::SendGameServerInfo(IServerUserItem * pIServerUserItem, WORD wIndex, WORD wRoundID)
{
	//���͵�¼�ɹ�
	CMD_GR_LogonSuccess LogonSuccess;
	memset(&LogonSuccess,0,sizeof(LogonSuccess));
	LogonSuccess.dwUserID=pIServerUserItem->GetUserID();
	m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GR_LOGON,SUB_GR_LOGON_SUCCESS,&LogonSuccess,sizeof(LogonSuccess));

	//����������Ϣ
	BYTE cbBuffer[SOCKET_PACKAGE];
	CMD_GR_ServerInfo * pServerInfo=(CMD_GR_ServerInfo *)cbBuffer;
	memset(pServerInfo,0,sizeof(CMD_GR_ServerInfo));
	pServerInfo->wKindID=m_pGameServiceAttrib->wKindID;
	pServerInfo->wGameGenre=m_pGameServiceOption->wServerType;
	pServerInfo->wTableCount=m_pGameServiceOption->wTableCount;
	pServerInfo->wChairCount=m_pGameServiceAttrib->wChairCount;
	m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GR_INFO,SUB_GR_SERVER_INFO,cbBuffer,sizeof(CMD_GR_ServerInfo));

	//�����б����
	m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GR_INFO,SUB_GR_COLUMN_INFO,&m_ColumnInfo,m_wColumnInfoSize);

	//�����������
	m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GR_INFO,SUB_GR_CONFIG_FINISH);

	//�����Լ���Ϣ
	SendUserItem(pIServerUserItem,wIndex,wRoundID);

	//���������û�
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItemSend=NULL;
	while (true)
	{
		pIServerUserItemSend=m_ServerUserManager.EnumOnLineUser(wEnumIndex++);
		if (pIServerUserItemSend==NULL) break;
		if (pIServerUserItemSend==pIServerUserItem) continue;
		SendUserItem(pIServerUserItemSend,wIndex,wRoundID);
	}

	//���Ͷ����û�
	wEnumIndex=0;
	while (true)
	{
		pIServerUserItemSend=m_ServerUserManager.EnumOffLineUser(wEnumIndex++);
		if (pIServerUserItemSend==NULL) break;
		if (pIServerUserItemSend==pIServerUserItem) continue;
		SendUserItem(pIServerUserItemSend,wIndex,wRoundID);
	}

	//����������Ϣ
	CMD_GR_TableInfo TableInfo;
	CTableFrame * pTableFrame=NULL;
	TableInfo.wTableCount=m_pGameServiceOption->wTableCount;
	ASSERT(TableInfo.wTableCount<CountArray(TableInfo.TableStatus));
	for (WORD i=0;i<TableInfo.wTableCount;i++)
	{
		pTableFrame=(m_pTableFrame+i);
		TableInfo.TableStatus[i].bTableLock=pTableFrame->IsTableLocked()?TRUE:FALSE;
		TableInfo.TableStatus[i].bPlayStatus=pTableFrame->IsGameStarted()?TRUE:FALSE;
	}
	WORD wSendSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatus)+TableInfo.wTableCount*sizeof(TableInfo.TableStatus[0]);
	m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);

	//�������
	m_pITCPSocketEngine->SendData(wIndex,wRoundID,MDM_GR_LOGON,SUB_GR_LOGON_FINISH);

	return true;
}

//�б���Ϣ����
bool CAttemperEngineSink::OnCenterMainServerList(CMD_Command Command, void * pDataBuffer, WORD wDataSize)
{
	ASSERT(Command.wMainCmdID==MDM_CS_SERVER_LIST);
	switch (Command.wSubCmdID)
	{
	case SUB_CS_LIST_INFO:		//�б���Ϣ
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_CS_ListInfo));
			if (wDataSize!=sizeof(CMD_CS_ListInfo)) return false;

			//��Ϣ����
			m_ServerList.ResetServerListBuffer();

			return true;
		}
	case SUB_CS_LIST_TYPE:		//������Ϣ
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(tagGameType));
			ASSERT(wDataSize%sizeof(tagGameType)==0);

			//��Ϣ����
			DWORD dwCount=wDataSize/sizeof(tagGameType);
			tagGameType * pGameType=(tagGameType *)pDataBuffer;
			m_ServerList.AppendGameTypeBuffer(pGameType,dwCount);

			return true;
		}
	case SUB_CS_LIST_KIND:		//������Ϣ
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(tagGameKind));
			ASSERT(wDataSize%sizeof(tagGameKind)==0);

			//��Ϣ����
			DWORD dwCount=wDataSize/sizeof(tagGameKind);
			tagGameKind * pGameKind=(tagGameKind *)pDataBuffer;
			m_ServerList.AppendGameKindBuffer(pGameKind,dwCount);

			return true;
		}
	case SUB_CS_LIST_STATION:	//վ����Ϣ
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(tagGameStation));
			ASSERT(wDataSize%sizeof(tagGameStation)==0);

			//��Ϣ����
			DWORD dwCount=wDataSize/sizeof(tagGameStation);
			tagGameStation * pGameStation=(tagGameStation *)pDataBuffer;
			m_ServerList.AppendGameStationBuffer(pGameStation,dwCount);

			return true;
		}
	case SUB_CS_LIST_SERVER:	//������Ϣ
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(tagGameServer));
			ASSERT(wDataSize%sizeof(tagGameServer)==0);

			//��Ϣ����
			DWORD dwCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pDataBuffer;
			m_ServerList.AppendGameServerBuffer(pGameServer,dwCount);

			return true;
		}
	case SUB_CS_LIST_FINISH:	//�������
		{
			//�����
			m_ServerList.ActiveServerListBuffer();

			//��������
			WORD wInfoCount=m_ServerList.GetOnLineCountInfo(m_OnLineCountInfo,CountArray(m_OnLineCountInfo));
			m_wOnLineInfoSize=wInfoCount*sizeof(tagOnLineCountInfo);

			//������Ϣ
			if (m_wOnLineInfoSize>0)
			{
				ASSERT(m_pITCPSocketEngine!=NULL);
				m_pITCPSocketEngine->SendDataBatch(MDM_GR_SERVER_INFO,SUB_GR_ONLINE_COUNT_INFO,&m_OnLineCountInfo,m_wOnLineInfoSize);
			}

			return true;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
