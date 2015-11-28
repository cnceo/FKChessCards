#include "Include/stdafx.h"
#include "../Include/TableFrame.h"

//////////////////////////////////////////////////////////////////////////

//��ʱ�� ID
#define IDI_OFFLINE					(IDI_MAX_TIME_ID+1)					//���߶�ʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrame::CTableFrame()
{
	//���Ա���
	m_wChairCount=0;
	m_wTableID=INVALID_TABLE;

	//״̬����
	m_bLocked=false;
	m_bStarted=false;
	m_dwTimeStart=0L;
	m_dwTableOwnerID=0L;
	m_bGameStatus=GS_FREE;
	m_szPassword[0]=0;

	//������Ϣ
	m_lGameTaxScore=0L;
	m_bWriteScore=false;
	memset(m_ScoreInfo,0,sizeof(m_ScoreInfo));
	memset(m_dwPlayerID,0,sizeof(m_dwPlayerID));

	//�û���Ϣ
	memset(m_pIUserItem,0,sizeof(m_pIUserItem));
	memset(m_bAllowLookon,0,sizeof(m_bAllowLookon));
	memset(m_wOffLineCount,0,sizeof(m_wOffLineCount));

	//���ñ���
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�ӿڱ���
	m_pITableFrameSink=NULL;
	m_pITableFrameEvent=NULL;
	m_pIGameServiceFrame=NULL;

	return;
}

//��������
CTableFrame::~CTableFrame()
{
	//�ͷŶ���
	if (m_pITableFrameSink!=NULL)
	{
		m_pITableFrameSink->Release();
		m_pITableFrameSink=NULL;
	}
	return;
}

//�ӿڲ�ѯ
void * __cdecl CTableFrame::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrame,Guid,dwQueryVer);
	QUERYINTERFACE(ITableFrameManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameManager,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��
bool __cdecl CTableFrame::InitTableFrame(WORD wTableID, tagTableFrameParameter * pTableFrameParameter)
{
	//Ч�����
	ASSERT(pTableFrameParameter!=NULL);

	//���ñ���
	m_wTableID=wTableID;
	m_pGameServiceAttrib=pTableFrameParameter->pGameServiceAttrib;
	m_pGameServiceOption=pTableFrameParameter->pGameServiceOption;
	m_wChairCount=pTableFrameParameter->pGameServiceAttrib->wChairCount;

	//��ѯ�ӿ�
	m_pIGameServiceFrame=GET_OBJECTPTR_INTERFACE(pTableFrameParameter->pIGameServiceFrame,IGameServiceFrame);
	ASSERT(m_pIGameServiceFrame!=NULL);
	if (m_pIGameServiceFrame==NULL) return false;

	//��������
	IGameServiceManager * pIGameServiceManager=GET_OBJECTPTR_INTERFACE(pTableFrameParameter->pIGameServiceManager,IGameServiceManager);
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink==NULL) return false;

	//���ýӿ�
	IUnknownEx * pIUnknownEx=GET_MYSELF_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->InitTableFrameSink(pIUnknownEx)==false) return false;

	//��չ�ӿ�
	m_pITableFrameEvent=GET_OBJECTPTR_INTERFACE(m_pITableFrameSink,ITableFrameEvent);

	return true;
}

//��Ϸ״̬
bool __cdecl CTableFrame::IsUserPlaying(IServerUserItem * pIServerUserItem)
{
	//��Ϸ״̬
	if (m_bStarted==false) return false;

	//�û�״̬
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	if ((cbUserStatus!=US_PLAY)&&(cbUserStatus!=US_OFFLINE)) return false;

	//�߼��ж�
	WORD wChairID=pIServerUserItem->GetChairID();
	return m_pITableFrameSink->IsUserPlaying(wChairID);
}

//�뿪����
bool __cdecl CTableFrame::PerformStandUpAction(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	ASSERT(pIServerUserItem->GetChairID()<m_wChairCount);

	//��������
	tagServerUserData * pUserData=pIServerUserItem->GetUserData();
	WORD wChairID=pUserData->wChairID;
	bool bLookonUser=(pIServerUserItem->GetUserStatus()==US_LOOKON);

	//�û�����
	if (bLookonUser==true)
	{
		IServerUserItem * pILookonUserItem=NULL;
		for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
		{
			pILookonUserItem=m_LookonUserItemPtr[i];
			ASSERT(pILookonUserItem!=NULL);
			if (pILookonUserItem==pIServerUserItem)
			{
				//��������
				if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqStandUp(wChairID,pIServerUserItem,false);

				//�����û�
				m_LookonUserItemPtr.RemoveAt(i);
				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);
				m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

				//��������
				if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserStandUp(wChairID,pIServerUserItem,true);

				return true;
			}
		}
	}
	else
	{
		//��������
		if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqStandUp(wChairID,pIServerUserItem,false);

		//������Ϸ
		if (IsUserPlaying(pIServerUserItem)==true)
		{
			m_pITableFrameSink->OnEventGameEnd(pUserData->wChairID,pIServerUserItem,GER_USER_LEFT);
		}

		//�뿪�ж�
		if ((pUserData->wTableID==INVALID_TABLE)||(pUserData->wChairID==INVALID_CHAIR)) return true;

		//�����û�
		m_pIUserItem[pUserData->wChairID]=NULL;
		pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);
		m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

		//��������
		if (m_dwTableOwnerID==pUserData->dwUserID) m_dwTableOwnerID=0L;

		//��������
		WORD wUserCount=0;
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL) wUserCount++;
		}
		if (wUserCount==0)
		{
			m_bLocked=false;
			m_szPassword[0]=0;
			m_pIGameServiceFrame->SendTableStatus(m_wTableID);
		}

		//���´���
		if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserStandUp(wChairID,pIServerUserItem,false);
		return true;
	}

	return true;
}

//�Թ۶���
bool __cdecl CTableFrame::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(wChairID<m_wChairCount);
	ASSERT(pIServerUserItem->GetTableID()==INVALID_TABLE);
	ASSERT(pIServerUserItem->GetChairID()==INVALID_CHAIR);

	//��������
	DWORD dwMasterRight=pIServerUserItem->GetUserData()->dwUserRight;

	//Ȩ���ж�
	if (CUserRight::CanLookon(dwMasterRight)==false)
	{
		//������Ϣ
		LPCTSTR pszMessage=TEXT("��Ǹ����û�н����Թ���Ϸ��Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��");
		SendSitFailedPacket(pIServerUserItem,pszMessage);

		return false;
	}

	//�رղ�ѯ
	if ((dwMasterRight==0L)&&(m_pIGameServiceFrame->IsShallClose()==true))
	{
		LPCTSTR pszDescribe=TEXT("���ڴ���Ϸ���伴����ͣ������Ҳ������ٽ�����Ϸ����");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//��ֹ��ѯ
	if ((dwMasterRight==0L)&&(m_pIGameServiceFrame->IsAllowEnterGame()==false))
	{
		LPCTSTR pszDescribe=TEXT("��Ǹ������Ϸ�����ڲ�������ҽ��룡");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//λ���ж�
	if (m_pIUserItem[wChairID]==NULL)
	{
		LPCTSTR pszDescribe=TEXT("�������Թ۵�λ���Ѿ�û������ˣ������Թۣ�");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//����Ч��
	if (m_bLocked==true)
	{
		const tagUserRule * pUserRule=pIServerUserItem->GetUserRule();
		if ((pUserRule->bPassword==false)||(lstrcmp(m_szPassword,pUserRule->szPassword)!=0))
		{
			LPCTSTR pszDescribe=TEXT("��Ϸ��Я��������󣬲����Թ���Ϸ��");
			SendSitFailedPacket(pIServerUserItem,pszDescribe);
			return false;
		}
	}

	//״̬�ж�
	if ((m_bStarted==false)&&(dwMasterRight==0L))
	{
		LPCTSTR pszDescribe=TEXT("��Ϸ��û�п�ʼ����ʱ�����Թۣ�");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//�Թ۴���
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqSitDown(wChairID,pIServerUserItem,true);

	//�������
	m_LookonUserItemPtr.Add(pIServerUserItem);
	pIServerUserItem->SetUserStatus(US_LOOKON,m_wTableID,wChairID);
	m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

	//�Թ۴���
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserSitDown(wChairID,pIServerUserItem,true);

	return true;
}

//���¶���
bool __cdecl CTableFrame::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(wChairID<m_wChairCount);
	ASSERT(pIServerUserItem->GetTableID()==INVALID_TABLE);
	ASSERT(pIServerUserItem->GetChairID()==INVALID_CHAIR);

	//��������
	const tagUserScore * pUserScore=pIServerUserItem->GetUserScore();;
	const tagUserRule * pUserRule=pIServerUserItem->GetUserRule(),* pTableUserRule=NULL;
	tagServerUserData * pUserData=pIServerUserItem->GetUserData(),* pTableUserData=NULL;

	//�رղ�ѯ
	if ((pUserData->dwMasterRight==0L)&&(m_pIGameServiceFrame->IsShallClose()==true))
	{
		LPCTSTR pszDescribe=TEXT("���ڴ���Ϸ���伴����ͣ������Ҳ������ٽ�����Ϸ����");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//��ֹ��ѯ
	if ((pUserData->dwMasterRight==0L)&&(m_pIGameServiceFrame->IsAllowEnterGame()==false))
	{
		LPCTSTR pszDescribe=TEXT("��Ǹ������Ϸ�����ڲ�������ҽ��룡");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//λ���ж�
	if (m_pIUserItem[wChairID]!=NULL)
	{
		TCHAR szDescribe[128]=TEXT("");
		pTableUserData=m_pIUserItem[wChairID]->GetUserData();
		_snprintf(szDescribe,sizeof(szDescribe),TEXT("�����Ѿ��� [ %s ] �����ȵ��ˣ��´ζ���Ҫ����ˣ�"),pTableUserData->szAccounts);
		SendSitFailedPacket(pIServerUserItem,szDescribe);
		return false;
	}

	//��Ϸ״̬
	if (m_bStarted==true)
	{
		LPCTSTR pszDescribe=TEXT("��Ϸ�Ѿ���ʼ�ˣ���ʱ���ܽ�����Ϸ����");
		SendSitFailedPacket(pIServerUserItem,pszDescribe);
		return false;
	}

	//Ȩ���ж�
	if (CUserRight::CanPlay(pUserData->dwUserRight)==false)
	{
		//������Ϣ
		LPCTSTR pszMessage=TEXT("��Ǹ����û�н�����Ϸ��Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��");
		SendSitFailedPacket(pIServerUserItem,pszMessage);

		return true;
	}

	//��������
	if (m_pGameServiceOption->lLessScore!=0L)
	{
		if (pUserData->UserScoreInfo.lScore<m_pGameServiceOption->lLessScore)
		{
			TCHAR szDescribe[128]=TEXT("");
			_snprintf(szDescribe,sizeof(szDescribe),TEXT("������Ϸ������Ҫ %ld ����Ϸ���֣���Ļ��ֲ��������ܼ��룡"),m_pGameServiceOption->lLessScore);
			SendSitFailedPacket(pIServerUserItem,szDescribe);
			return false;
		}
	}
	if (m_pGameServiceOption->lHighScore!=0L)
	{
		if (pUserData->UserScoreInfo.lScore>m_pGameServiceOption->lHighScore)
		{
			TCHAR szDescribe[128]=TEXT("");
			_snprintf(szDescribe,sizeof(szDescribe),TEXT("�����Ϸ���ָ��� %ld�����ܼ�����Ϸ��"),m_pGameServiceOption->lHighScore);
			SendSitFailedPacket(pIServerUserItem,szDescribe);
			return false;
		}
	}

	//����Ч��
	if (m_bLocked==true)
	{
		if ((pUserRule->bPassword==false)||(lstrcmp(m_szPassword,pUserRule->szPassword)!=0))
		{
			LPCTSTR pszDescribe=TEXT("Я��������󣬲��ܼ�����Ϸ��");
			SendSitFailedPacket(pIServerUserItem,pszDescribe);
			return false;
		}
	}

	//���ַ�Χ
	WORD wWinRate=pIServerUserItem->GetUserWinRate();
	WORD wFleeRate=pIServerUserItem->GetUserFleeRate();
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			//��ȡ����
			pTableUserRule=m_pIUserItem[i]->GetUserRule();

			//����Ч��
			if ((pTableUserRule->bLimitFlee)&&(wFleeRate>pTableUserRule->wFleeRate))
			{
				TCHAR szDescribe[128]=TEXT("");
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("���������̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),m_pIUserItem[i]->GetAccounts());
				SendSitFailedPacket(pIServerUserItem,szDescribe);
				return false;
			}

			//ʤ��Ч��
			if ((pTableUserRule->bLimitWin)&&(wWinRate<pTableUserRule->wWinRate))
			{
				TCHAR szDescribe[128]=TEXT("");
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("���ʤ��̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),m_pIUserItem[i]->GetAccounts());
				SendSitFailedPacket(pIServerUserItem,szDescribe);
				return false;
			}

			//��ʱЧ��
			if ((pTableUserRule->bLimitDelay)&&(pUserData->wNetDelay>pTableUserRule->wNetDelay))
			{
				TCHAR szDescribe[128]=TEXT("");
				_snprintf(szDescribe,sizeof(szDescribe),TEXT("���������ʱ̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),m_pIUserItem[i]->GetAccounts());
				SendSitFailedPacket(pIServerUserItem,szDescribe);
				return false;
			}

			//����Ч��
			if (pTableUserRule->bLimitScore==true)
			{
				if (pUserScore->lScore>pTableUserRule->lMaxScore)
				{
					TCHAR szDescribe[128]=TEXT("");
					_snprintf(szDescribe,sizeof(szDescribe),TEXT("��Ļ���̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��"),m_pIUserItem[i]->GetAccounts());
					SendSitFailedPacket(pIServerUserItem,szDescribe);
					return false;
				}
				if (pUserScore->lScore<pTableUserRule->lLessScore)
				{
					TCHAR szDescribe[128]=TEXT("");
					_snprintf(szDescribe,sizeof(szDescribe),TEXT("��Ļ���̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��"),m_pIUserItem[i]->GetAccounts());
					SendSitFailedPacket(pIServerUserItem,szDescribe);
					return false;
				}
			}
		}
	}

	//��ַЧ��
	bool bCheckSameIP=pUserRule->bCheckSameIP;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			pTableUserRule=m_pIUserItem[i]->GetUserRule();
			if (pTableUserRule->bCheckSameIP==true) 
			{
				bCheckSameIP=true;
				break;
			}
		}
	}
	if (bCheckSameIP==true)
	{
		DWORD dwUserIP=pIServerUserItem->GetClientIP();
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if ((m_pIUserItem[i]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==dwUserIP))
			{
				if (pUserRule->bCheckSameIP)
				{
					LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ���������� IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
					SendSitFailedPacket(pIServerUserItem,pszDescribe);
					return false;
				}
				else
				{
					LPCTSTR pszDescribe=TEXT("���������˲�����ͬ IP ��ַ�������Ϸ������Ϸ���������� IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
					SendSitFailedPacket(pIServerUserItem,pszDescribe);
					return false;
				}
			}
		}
		for (WORD i=0;i<m_wChairCount-1;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				for (WORD j=i+1;j<m_wChairCount;j++)
				{
					if ((m_pIUserItem[j]!=NULL)&&(m_pIUserItem[i]->GetClientIP()==m_pIUserItem[j]->GetClientIP()))
					{
						LPCTSTR pszDescribe=TEXT("�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ������ IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
						SendSitFailedPacket(pIServerUserItem,pszDescribe);
						return false;
					}
				}
			}
		}
	}

	//���´���
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqSitDown(wChairID,pIServerUserItem,false);

	//��������
	WORD wUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL) wUserCount++;
	}
	if (wUserCount==0)
	{
		m_bLocked=pUserRule->bPassword;
		m_dwTableOwnerID=pUserData->dwUserID;
		if (m_bLocked==true) lstrcpyn(m_szPassword,pUserRule->szPassword,CountArray(m_szPassword));
	}

	//�������
	m_pIUserItem[wChairID]=pIServerUserItem;
	pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,wChairID);
	m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

	//���´���
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserSitDown(wChairID,pIServerUserItem,false);

	return true;
}

//�����¼�
bool __cdecl CTableFrame::OnUserOffLine(WORD wChairID)
{
	//Ч��״̬
	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);

	//����״̬
	m_wOffLineCount[wChairID]++;
	m_pIUserItem[wChairID]->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);
	m_pIGameServiceFrame->SendUserStatus(m_pIUserItem[wChairID]);

	//���ߴ���
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserOffLine(wChairID,m_pIUserItem[wChairID]);

	//���ö�ʱ��
	WORD wTimerID=IDI_OFFLINE+wChairID;
	WPARAM wBindParam=m_pIUserItem[wChairID]->GetUserID();
	m_pIGameServiceFrame->SetTableTimer(m_wTableID,wTimerID,90000L,1,wBindParam);

	return true;
}

//�ؽ��¼�
bool __cdecl CTableFrame::OnUserReConnect(WORD wChairID)
{
	//Ч��״̬
	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);

	//���봦��
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqReConnect(wChairID,m_pIUserItem[wChairID]);

	//ɾ����ʱ��
	m_pIGameServiceFrame->KillTableTimer(m_wTableID,IDI_OFFLINE+wChairID);

	//�����û�
	m_pIUserItem[wChairID]->SetUserStatus(US_PLAY,m_wTableID,wChairID);
	m_pIGameServiceFrame->SendUserStatus(m_pIUserItem[wChairID]);

	//���봦��
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReConnect(wChairID,m_pIUserItem[wChairID]);

	return true;
}

//�������
bool __cdecl CTableFrame::OnUserReqOffLine(WORD wChairID)
{
	//Ч�����
	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);
	if (wChairID>=m_wChairCount) return false;
	if (m_pIUserItem[wChairID]==NULL) return false;

	//���ߴ���
	if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqOffLine(wChairID,m_pIUserItem[wChairID]);

	return m_wOffLineCount[wChairID]<3;
}

//��ʱ���¼�
bool __cdecl CTableFrame::OnEventTimer(WORD wTimerID, WPARAM wBindParam)
{
	//���Ӷ�ʱ��
	if (wTimerID>IDI_MAX_TIME_ID)
	{
		switch (wTimerID)
		{
		case IDI_OFFLINE:
		case IDI_OFFLINE+1:
		case IDI_OFFLINE+2:
		case IDI_OFFLINE+3:
		case IDI_OFFLINE+4:
		case IDI_OFFLINE+5:
		case IDI_OFFLINE+6:
		case IDI_OFFLINE+7:		//���߶�ʱ��
			{
				//Ч�����
				WORD wChairID=wTimerID-IDI_OFFLINE;
				ASSERT(wChairID<m_wChairCount);

				//Ч��״̬
				if (m_bStarted==false) return false;

				//���ߴ���
				if (m_pIUserItem[wChairID]!=NULL)
				{
					tagServerUserData * pUserData=m_pIUserItem[wChairID]->GetUserData();
					if (pUserData->dwUserID!=wBindParam) return false;
					if (pUserData->cbUserStatus!=US_OFFLINE) return false;
					m_pITableFrameSink->OnEventGameEnd(wChairID,m_pIUserItem[wChairID],GER_USER_LEFT);

					return true;
				}

				return false;
			}
		}

		return false;
	}

	//��Ϸ��ʱ��
	return m_pITableFrameSink->OnTimerMessage(wTimerID,wBindParam);
}

//��Ϸ�¼�����
bool __cdecl CTableFrame::OnEventSocketGame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//��Ϣ����
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pDataBuffer,wDataSize,pIServerUserItem);
}

//����¼�����
bool __cdecl CTableFrame::OnEventSocketFrame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//��Ϣ����
	bool bSuccess=m_pITableFrameSink->OnFrameMessage(wSubCmdID,pDataBuffer,wDataSize,pIServerUserItem);
	if (bSuccess==false)
	{
		switch (wSubCmdID)
		{
		case SUB_GF_INFO:	//��Ϸ��Ϣ
			{
				//Ч�����
				ASSERT(wDataSize==sizeof(CMD_GF_Info));
				if (wDataSize<sizeof(CMD_GF_Info)) return false;

				//��������
				CMD_GF_Info * pInfo=(CMD_GF_Info *)pDataBuffer;
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				bool bLookonUser=(pUserData->cbUserStatus==US_LOOKON);

				//Ч��״̬
				ASSERT(pUserData->wChairID<m_wChairCount);
				if (pUserData->wChairID>=m_wChairCount) return false;

				//���ñ���
				if (bLookonUser==false) m_bAllowLookon[pUserData->wChairID]=pInfo->bAllowLookon?true:false;

				//��������
				CMD_GF_Option Option;
				Option.bGameStatus=m_bGameStatus;
				Option.bAllowLookon=m_bAllowLookon[pUserData->wChairID]?TRUE:FALSE;
				m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_OPTION,&Option,sizeof(Option));

				//���ͳ���
				bool bSendSecret=((bLookonUser==false)||(m_bAllowLookon[pUserData->wChairID]==true));
				return m_pITableFrameSink->SendGameScene(pUserData->wChairID,pIServerUserItem,m_bGameStatus,bSendSecret);
			}
		case SUB_GF_USER_READY:	//�û�ͬ��
			{
				//��������
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				bool bLookonUser=(pUserData->cbUserStatus==US_LOOKON);

				//״̬Ч��
				ASSERT(bLookonUser==false);
				ASSERT(m_pIUserItem[pUserData->wChairID]==pIServerUserItem);
				if (bLookonUser==true) return false;
				if (pUserData->cbUserStatus>=US_PLAY) return true;

				//ͬ�⴦��
				if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReqReady(pUserData->wChairID,m_pIUserItem[pUserData->wChairID]);

				//�¼�����
				if ((m_pITableFrameEvent!=NULL)&&(m_pITableFrameEvent->OnEventUserReqReady(
					pUserData->wChairID,pIServerUserItem)==false)) return true;

				//���ñ���
				pUserData->cbUserStatus=US_READY;

				//ͬ�⴦��
				if (m_pITableFrameEvent!=NULL) m_pITableFrameEvent->OnEventUserReady(pUserData->wChairID,m_pIUserItem[pUserData->wChairID]);

				//����״̬
				m_pIGameServiceFrame->SendUserStatus(pIServerUserItem);

				return true;
			}
		case SUB_GF_USER_CHAT:	//�û�����
			{
				//Ч�����
				CMD_GF_UserChat * pUserChat=(CMD_GF_UserChat *)pDataBuffer;
				ASSERT(wDataSize>=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)));
				ASSERT(wDataSize==(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength));
				if (wDataSize<(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage))) return false;
				if (wDataSize!=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatMessage)+pUserChat->wChatLength)) return false;

				//�������
				const tagServerUserData * pUserData=pIServerUserItem->GetUserData();
				if (pUserData->dwMasterRight!=0L)
				{
					//��Ϸ��ɢ
					if (lstrcmp(pUserChat->szChatMessage,TEXT("/:DismissGame"))==0)
					{
						DismissGame();
						return true;
					}
				}

				//Ȩ���ж�
				if (CUserRight::CanGameChat(pIServerUserItem->GetUserData()->dwUserRight)==false)
				{
					LPCTSTR pszMessage=TEXT("����ʱû�з��ͷ�����Ϣ��Ȩ�ޣ�ֻ�������Ա˽�ģ�");
					SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
					return true;
				}

				//״̬��ѯ
				if ((pUserData->dwMasterRight==0L)&&(m_pIGameServiceFrame->IsAllowGameChat()==false))
				{
					LPCTSTR pszMessage=TEXT("��Ǹ������Ϸ���䲻��������������Ϣ��");
					SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO);
					return true;
				}

				//��Ϸ���
				for (WORD i=0;i<m_wChairCount;i++)
				{
					if (m_pIUserItem[i]!=NULL)
						m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_FRAME,SUB_GF_USER_CHAT,pUserChat,wDataSize);
				}

				//�Թ����
				for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
				{
					m_pIGameServiceFrame->SendData(m_LookonUserItemPtr[i],MDM_GF_FRAME,SUB_GF_USER_CHAT,pUserChat,wDataSize);
				}

				return true;
			}
		}
	}

	return bSuccess;
}

//ö���û�
IServerUserItem * __cdecl CTableFrame::EnumLookonUserItem(WORD wIndex)
{
	if (wIndex>=m_LookonUserItemPtr.GetCount()) return NULL;
	return m_LookonUserItemPtr[wIndex];
}

//��ȡ�û�
IServerUserItem * __cdecl CTableFrame::GetServerUserItem(WORD wChairID)
{
	ASSERT(wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;
	return m_pIUserItem[wChairID];
}

//���ö�ʱ��
bool __cdecl CTableFrame::SetGameTimer(WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)
{
	//Ч�����
	ASSERT(wTimerID<=IDI_MAX_TIME_ID);
	if (wTimerID>IDI_MAX_TIME_ID) return false;

	//���ö�ʱ��
	return m_pIGameServiceFrame->SetTableTimer(m_wTableID,wTimerID,dwElapse,dwRepeat,wBindParam);
}

//ɾ����ʱ��
bool __cdecl CTableFrame::KillGameTimer(WORD wTimerID)
{
	//Ч�����
	ASSERT(wTimerID<=IDI_MAX_TIME_ID);
	if (wTimerID>IDI_MAX_TIME_ID) return false;

	//ɾ����ʱ��
	return m_pIGameServiceFrame->KillTableTimer(m_wTableID,wTimerID);
}

//��������
bool __cdecl CTableFrame::SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID);
	return true;
}

//��������
bool __cdecl CTableFrame::SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
	return true;
}

//��������
bool __cdecl CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID)
{
	//Ⱥ���û�
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL) 
			{
				m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_GAME,wSubCmdID);
			}
		}
		return true;
	}

	//��һ����
	ASSERT(wChairID<m_wChairCount);
	if ((wChairID<m_wChairCount)&&(m_pIUserItem[wChairID]!=NULL))
	{
		m_pIGameServiceFrame->SendData(m_pIUserItem[wChairID],MDM_GF_GAME,wSubCmdID);
		return true;
	}

	return false;
}

//��������
bool __cdecl CTableFrame::SendTableData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	//Ⱥ���û�
	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL) 
			{
				m_pIGameServiceFrame->SendData(m_pIUserItem[i],MDM_GF_GAME,wSubCmdID,pData,wDataSize);
			}
		}
		return true;
	}

	//��һ����
	ASSERT(wChairID<m_wChairCount);
	if ((wChairID<m_wChairCount)&&(m_pIUserItem[wChairID]!=NULL))
	{
		m_pIGameServiceFrame->SendData(m_pIUserItem[wChairID],MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		return true;
	}

	return false;
}

//��������
bool __cdecl CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID)
{
	tagServerUserData * pUserData=NULL;
	IServerUserItem * pIServerUserItem=NULL;
	for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
	{
		pIServerUserItem=m_LookonUserItemPtr[i];
		ASSERT(pIServerUserItem!=NULL);
		if (wChairID<m_wChairCount)
		{
			pUserData=pIServerUserItem->GetUserData();
			if (pUserData->wChairID==wChairID) m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID);
		}
		else m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID);
	}

	return true;
}

//��������
bool __cdecl CTableFrame::SendLookonData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)
{
	tagServerUserData * pUserData=NULL;
	IServerUserItem * pIServerUserItem=NULL;
	for (INT_PTR i=0;i<m_LookonUserItemPtr.GetCount();i++)
	{
		pIServerUserItem=m_LookonUserItemPtr[i];
		ASSERT(pIServerUserItem!=NULL);
		if (wChairID<m_wChairCount)
		{
			pUserData=pIServerUserItem->GetUserData();
			if (pUserData->wChairID==wChairID) m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
		}
		else m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
	}

	return true;
}

//���ͷ�����Ϣ
bool __cdecl CTableFrame::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	//�������ݰ�
	CMD_GR_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountString(Message.szContent);

	//��������
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_SYSTEM,SUB_GR_MESSAGE,&Message,wSendSize);

	return true;
}

//������Ϸ��Ϣ
bool __cdecl CTableFrame::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)
{
	//�������ݰ�
	CMD_GF_Message Message;
	Message.wMessageType=wMessageType;
	lstrcpyn(Message.szContent,lpszMessage,CountArray(Message.szContent));
	Message.wMessageLength=CountString(Message.szContent);

	//��������
	WORD wSendSize=sizeof(Message)-sizeof(Message.szContent)+Message.wMessageLength*sizeof(TCHAR);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_MESSAGE,&Message,wSendSize);

	return true;
}

//��ɢ��Ϸ
bool __cdecl CTableFrame::DismissGame()
{
	//״̬�ж�
	if (m_bStarted==false) return false;

	//��ɢ��Ϸ
	m_pITableFrameSink->OnEventGameEnd(INVALID_CHAIR,NULL,GER_DISMISS);

	return true;
}

//������Ϸ
bool __cdecl CTableFrame::ConcludeGame()
{
	//���ñ���
	m_bStarted=false;
	m_dwTimeStart=0L;
	m_lGameTaxScore=0L;
	m_bWriteScore=false;
	m_bGameStatus=GS_FREE;
	memset(m_ScoreInfo,0,sizeof(m_ScoreInfo));
	memset(m_dwPlayerID,0,sizeof(m_dwPlayerID));
	memset(m_wOffLineCount,0,sizeof(m_wOffLineCount));

	//��������
	m_pITableFrameSink->RepositTableFrameSink();

	//�����û�
	tagServerUserData * pUserData=NULL;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			pUserData=m_pIUserItem[i]->GetUserData();
			if (pUserData->cbUserStatus!=US_OFFLINE) pUserData->cbUserStatus=US_SIT;
		}
	}

	//���ߴ���
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=m_pIUserItem[i];
			if (pIServerUserItem->GetUserStatus()!=US_OFFLINE) continue;
			
			//ɾ����ʱ��
			WORD wTimerID=IDI_OFFLINE+i;
			m_pIGameServiceFrame->KillTableTimer(m_wTableID,wTimerID);

			//�û��뿪
			PerformStandUpAction(pIServerUserItem);

			//�����û�����
			m_pIGameServiceFrame->DeleteUserItem(pIServerUserItem);
		}
	}

	//������
	LONG lHighScore=m_pGameServiceOption->lHighScore;
	LONG lLessScore=m_pGameServiceOption->lLessScore;
	if ((lLessScore!=0L)||(lHighScore!=0))
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=m_pIUserItem[i];
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();

				//�жϻ���
				if ((lLessScore!=0L)&&(pUserData->UserScoreInfo.lScore<lLessScore))
				{
					TCHAR szDescribe[256]=TEXT("");
					_snprintf(szDescribe,sizeof(szDescribe),TEXT("�����Ϸ�������� %ld�����ܼ�����Ϸ��"),lLessScore);
					SendGameMessage(pIServerUserItem,szDescribe,SMT_EJECT|SMT_INFO|SMT_CLOSE_GAME);
					PerformStandUpAction(pIServerUserItem);
				}
				if ((lHighScore!=0L)&&(pUserData->UserScoreInfo.lScore<lHighScore))
				{
					TCHAR szDescribe[256]=TEXT("");
					_snprintf(szDescribe,sizeof(szDescribe),TEXT("�����Ϸ���ָ��� %ld�����ܼ�����Ϸ��"),lHighScore);
					SendGameMessage(pIServerUserItem,szDescribe,SMT_EJECT|SMT_INFO|SMT_CLOSE_GAME);
					PerformStandUpAction(pIServerUserItem);
				}
			}
		}
	}

	//�ر��ж�
	if ((m_pIGameServiceFrame->IsAllowEnterGame()==false)||(m_pIGameServiceFrame->IsShallClose()==true))
	{
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_pIUserItem[i]!=NULL)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=m_pIUserItem[i];
				tagServerUserData * pUserData=pIServerUserItem->GetUserData();

				//�û��ж�
				if (pUserData->dwMasterRight==0L)
				{
					//�û�����
					LPCTSTR pszMessage=TEXT("��ע�⣬��Ϸ���伴���رջ��߲�������ҽ��룬�����뿪��Ϸ���ӣ�");
					SendGameMessage(pIServerUserItem,pszMessage,SMT_EJECT|SMT_INFO|SMT_CLOSE_GAME);
					PerformStandUpAction(pIServerUserItem);

					continue;
				}
			}
		}
	}

	//����״̬
	m_pIGameServiceFrame->SendTableStatus(m_wTableID);

	return true;
}

//���ͳ���
bool __cdecl CTableFrame::SendGameScene(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)
{
	//ϵͳ��Ϣ
	TCHAR szMessage[256]=TEXT("");

	_snprintf(szMessage,sizeof(szMessage),TEXT("�װ����û�����ӭ��������%s������ӭ����ᱦ���飡"),m_pGameServiceAttrib->szKindName);
	SendGameMessage(pIServerUserItem,szMessage,SMT_INFO);

	//���ͳ���
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GF_FRAME,SUB_GF_SCENE,pData,wDataSize);

	return true;
}

//д�����
bool __cdecl CTableFrame::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount, LONG lTaxScore)
{
	//Ч��״̬
	ASSERT(m_bStarted==true);
	ASSERT(m_bWriteScore==false);
	if (m_bStarted==false) return false;
	if (m_bWriteScore==true) return false;

	//Ч�����
	ASSERT(wScoreCount==m_wChairCount);
	if (wScoreCount!=m_wChairCount) return false;

	//Ч�����
	if (m_pGameServiceOption->wServerType==GAME_GENRE_POINT)
	{
		LONG lAllScore=m_lGameTaxScore+lTaxScore;
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (m_dwPlayerID[i]!=0)
			{
				lAllScore+=m_ScoreInfo[i].lScore;
				lAllScore+=ScoreInfoArray[i].lScore;
			}
		}
		ASSERT(lAllScore==0L);
		if (lAllScore!=0L) return false;
	}

	//��Ϸʱ��
	DWORD dwPlayTimeCount=(DWORD)time(NULL)-m_dwTimeStart;

	//д�����
	m_bWriteScore=true;
	m_lGameTaxScore+=lTaxScore;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			m_ScoreInfo[i]=ScoreInfoArray[i];
			m_pIUserItem[i]->WriteScore(ScoreInfoArray[i],dwPlayTimeCount);
			m_pIGameServiceFrame->SendUserScore(m_pIUserItem[i]);
		}
	}

	return true;
}

//д�����
bool __cdecl CTableFrame::WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo)
{
	//Ч��״̬
	ASSERT(m_bStarted==true);
	ASSERT(m_bWriteScore==false);
	if (m_bStarted==false) return false;
	if (m_bWriteScore==true) return false;

	//Ч�����
	ASSERT(ScoreInfo.lScore<=0);
	ASSERT(wChairID<m_wChairCount);
	ASSERT(m_pIUserItem[wChairID]!=NULL);
	//�޸Ĺؼ��֣���¼���֣���¼Ӯ���䰡��
	//if (ScoreInfo.lScore>0) return false;
	if (wChairID>=m_wChairCount) return false;
	if (m_pIUserItem[wChairID]==NULL) return false;

	//�޸Ļ���
	m_pIUserItem[wChairID]->WriteScore(ScoreInfo,(DWORD)time(NULL)-m_dwTimeStart);
	m_pIGameServiceFrame->SendUserScore(m_pIUserItem[wChairID]);

	//��¼��Ϣ
	m_ScoreInfo[wChairID]=ScoreInfo;

	return true;
}

//��ʼ��Ϸ
void CTableFrame::StartGame()
{
	//Ч��״̬
	ASSERT(m_bStarted==false);
	if (m_bStarted==true) return;

	//���ñ���
	m_bStarted=true;
	m_dwTimeStart=(DWORD)time(NULL);
	memset(m_wOffLineCount,0,sizeof(m_wOffLineCount));

	//��¼����
	m_lGameTaxScore=0L;
	m_bWriteScore=false;
	memset(m_ScoreInfo,0,sizeof(m_ScoreInfo));
	memset(m_dwPlayerID,0,sizeof(m_dwPlayerID));

	//�������
	tagServerUserData * pUserData=NULL;
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_pIUserItem[i]!=NULL)
		{
			pUserData=m_pIUserItem[i]->GetUserData();
			pUserData->cbUserStatus=US_PLAY;
			m_dwPlayerID[i]=pUserData->dwUserID;
		}
	}

	//����״̬
	m_pIGameServiceFrame->SendTableStatus(m_wTableID);

	//֪ͨ�¼�
	m_pITableFrameSink->OnEventGameStart();

	return;
}

//�жϿ�ʼ
bool CTableFrame::StartVerdict()
{
	//�����ж�
	if (m_bStarted==true) return false;

	//ʱ��ģʽ
	enStartMode StartMode=m_pITableFrameSink->GetGameStartMode();
	if (StartMode==enStartMode_TimeControl) return false;

	//׼������
	WORD wReadyUserCount=0;
	tagServerUserData * pUserData=NULL;
	for (WORD i=0;i<m_wChairCount;i++) 
	{
		if (m_pIUserItem[i]!=NULL)
		{
			wReadyUserCount++;
			pUserData=m_pIUserItem[i]->GetUserData();
			if (pUserData->cbUserStatus!=US_READY) return false;
		}
	}

	//�����ж�
	if (wReadyUserCount>1L)
	{
		if (StartMode==enStartMode_Symmetry)
		{
			if ((wReadyUserCount%2)!=0) return false;
			if (wReadyUserCount==m_wChairCount) return true;
			WORD wHalfCount=m_wChairCount/2;
			for (WORD i=0;i<wHalfCount;i++)
			{
				if ((m_pIUserItem[i]==NULL)&&(m_pIUserItem[i+wHalfCount]!=NULL)) return false;
				if ((m_pIUserItem[i]!=NULL)&&(m_pIUserItem[i+wHalfCount]==NULL)) return false;
			}
			return true;
		}
		else 
		{
			if (StartMode==enStartMode_FullReady) return (wReadyUserCount==m_wChairCount);
			if (StartMode==enStartMode_AllReady) return true;
		}
	}

	return false;
}

//��������ʧ��
void CTableFrame::SendSitFailedPacket(IServerUserItem * pIServerUserItem, LPCTSTR pszFailedDescribe)
{
	//��������
	CMD_GR_SitFailed SitFailed;
	memset(&SitFailed,0,sizeof(SitFailed));
	lstrcpyn(SitFailed.szFailedDescribe,pszFailedDescribe,CountArray(SitFailed.szFailedDescribe));

	//��������
	WORD wSendSize=sizeof(SitFailed)-sizeof(SitFailed.szFailedDescribe)+CountString(SitFailed.szFailedDescribe);
	m_pIGameServiceFrame->SendData(pIServerUserItem,MDM_GR_USER,SUB_GR_SIT_FAILED,&SitFailed,wSendSize);

	return;
}

//////////////////////////////////////////////////////////////////////////