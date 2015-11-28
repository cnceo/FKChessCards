#include "Include/stdafx.h"
#include "Afxinet.h"
#include "../Include/DataBaseSink.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseSink::CDataBaseSink()
{
	//���ñ���
	m_pIEventService=NULL;
	m_pGameUserDBInfo=NULL;
	m_pGameScoreDBInfo=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//��������
CDataBaseSink::~CDataBaseSink()
{
}

//�ӿڲ�ѯ
void * __cdecl CDataBaseSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseSink,Guid,dwQueryVer);
	return NULL;
}

//����ģ������
bool __cdecl CDataBaseSink::StartService(IUnknownEx * pIUnknownEx)
{
	//Ч�����
	ASSERT(m_pGameUserDBInfo!=NULL);
	ASSERT(m_pGameScoreDBInfo!=NULL);
	ASSERT(m_pGameServiceOption!=NULL);
	ASSERT(m_AttemperEvent.IsValid()==true);

	//����ʵ��
	if ((m_GameUserDB.GetInterface()==NULL)&&(m_GameUserDB.CreateInstance()==false))
	{
		m_pIEventService->ShowEventNotify(TEXT("�û����ݿ���󴴽�ʧ��"),Level_Exception);
		return false;
	}

	//����ʵ��
	if ((m_GameScoreDB.GetInterface()==NULL)&&(m_GameScoreDB.CreateInstance()==false))
	{
		m_pIEventService->ShowEventNotify(TEXT("��Ϸ���ݿ���󴴽�ʧ��"),Level_Exception);
		return false;
	}

	try
	{
		//��������
		BYTE * pcbAddr=NULL;
		TCHAR szDataBaseAddr[16]=TEXT("");

		//�����û����ݿ�
		pcbAddr=(BYTE *)&m_pGameUserDBInfo->dwDataBaseAddr;
		_snprintf(szDataBaseAddr,sizeof(szDataBaseAddr),TEXT("%d.%d.%d.%d"),pcbAddr[0],pcbAddr[1],pcbAddr[2],pcbAddr[3]);
		m_GameUserDB->SetConnectionInfo(szDataBaseAddr,m_pGameUserDBInfo->szDataBaseName,
			m_pGameUserDBInfo->szDataBaseUser,m_pGameUserDBInfo->szDataBasePass);
		m_GameUserDB->OpenConnection();

		//������Ϸ���ݿ�
		pcbAddr=(BYTE *)&m_pGameScoreDBInfo->dwDataBaseAddr;
		_snprintf(szDataBaseAddr,sizeof(szDataBaseAddr),TEXT("%d.%d.%d.%d"),pcbAddr[0],pcbAddr[1],pcbAddr[2],pcbAddr[3]);
		m_GameScoreDB->SetConnectionInfo(szDataBaseAddr,m_pGameScoreDBInfo->szDataBaseName,
			m_pGameScoreDBInfo->szDataBaseUser,m_pGameScoreDBInfo->szDataBasePass);
		m_GameScoreDB->OpenConnection();

		return true;
	}
	catch (IADOError * pIADOError)
	{
		m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return false;
}

//����ģ��ر�
bool __cdecl CDataBaseSink::StopService(IUnknownEx * pIUnknownEx)
{
	try
	{
		//�ر�����
		if (m_GameUserDB.GetInterface()) m_GameUserDB->CloseConnection();
		if (m_GameScoreDB.GetInterface()) m_GameScoreDB->CloseConnection();

		return true;
	}
	catch (IADOError * pIADOError)
	{
		m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return false;
}

//���ݲ�������
bool __cdecl CDataBaseSink::OnDataBaseRequest(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize)
{
	switch (DataBaseEvent.wRequestID)
	{
	case DBR_GR_LOGON_BY_ACCOUNTS:		//�ʻ���¼
		{
			return OnRequestLogon(DataBaseEvent,pDataBuffer,wDataSize);
		}
	case DBR_GR_LOGON_BY_USERID:		//I D ��¼
		{
			return OnRequestLogon(DataBaseEvent,pDataBuffer,wDataSize);
		}
	case DBR_GR_WRITE_SCORE:			//д�ֲ���
		{
			return OnRequestWriteScore(DataBaseEvent,pDataBuffer,wDataSize);
		}
	case DBR_GR_LIMIT_ACCOUNTS:			//�����ʺ�
		{
			return OnRequestLimitAccounts(DataBaseEvent,pDataBuffer,wDataSize);
		}
	case DBR_GR_SET_USER_RIGHT:			//����Ȩ��
		{
			return OnRequestSetUserRight(DataBaseEvent,pDataBuffer,wDataSize);
		}
	}

	return false;
}

//�����¼�
bool CDataBaseSink::SetEventService(IUnknownEx * pIUnknownEx)
{
	ASSERT(pIUnknownEx!=NULL);
	m_pIEventService=GET_OBJECTPTR_INTERFACE(pIUnknownEx,IEventService);
	ASSERT(m_pIEventService!=NULL);
	return (m_pIEventService!=NULL);
}

//���ú���
bool CDataBaseSink::InitDataBaseSink(tagDataBaseSinkParameter * pDataBaseSinkParameter, IUnknownEx * pIUnknownEx)
{
	//Ч�����
	ASSERT(pIUnknownEx!=NULL);
	ASSERT(pDataBaseSinkParameter!=NULL);

	//���ñ���
	m_pGameUserDBInfo=pDataBaseSinkParameter->pGameUserDBInfo;
	m_pGameScoreDBInfo=pDataBaseSinkParameter->pGameScoreDBInfo;
	m_pGameServiceAttrib=pDataBaseSinkParameter->pGameServiceAttrib;
	m_pGameServiceOption=pDataBaseSinkParameter->pGameServiceOption;

	//��ѯ�ӿ�
	IServiceEngine * pIServiceEngine=(IServiceEngine *)pIUnknownEx->QueryInterface(IID_IServiceEngine,VER_IServiceEngine);
	ASSERT(pIServiceEngine!=NULL);
	if (pIServiceEngine==NULL) throw TEXT("��������ӿڲ�ѯʧ��");

	//��ȡ�߼�����
	IAttemperEngine * pIAttemperEngine=(IAttemperEngine *)pIServiceEngine->GetAttemperEngine(IID_IAttemperEngine,VER_IAttemperEngine);
	ASSERT(pIAttemperEngine!=NULL);
	if (pIAttemperEngine==NULL) throw TEXT("��������ӿڲ�ѯʧ��");

	//����֪ͨ���
	IUnknownEx * pIQueueService=(IUnknownEx *)pIAttemperEngine->GetQueueService(IID_IQueueService,VER_IQueueService);
	if (m_AttemperEvent.SetQueueService(pIQueueService)==false) throw TEXT("�߼�����֪ͨ�ӿ�����ʧ��");

	return true;
}

//��¼������
bool CDataBaseSink::OnRequestLogon(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize)
{
	try
	{
		//��¼����
		LONG lReturnValue=-1;
		LPCTSTR pszPassword=NULL;
		switch (DataBaseEvent.wRequestID)
		{
		case DBR_GR_LOGON_BY_ACCOUNTS:		//�ʻ���¼
			{
				//Ч�����
				ASSERT(wDataSize==sizeof(DBR_GR_LogonByAccounts));
				if (wDataSize!=sizeof(DBR_GR_LogonByAccounts)) return false;

				//ִ�в�ѯ
				DBR_GR_LogonByAccounts * pLogonByAccounts=(DBR_GR_LogonByAccounts *)pDataBuffer;
				lReturnValue=SPLogonByAccounts(pLogonByAccounts->szAccounts,pLogonByAccounts->szPassWord,pLogonByAccounts->dwClientIP);
				pszPassword=pLogonByAccounts->szPassWord;

				break;
			}
		case DBR_GR_LOGON_BY_USERID:		//I D ��¼
			{
				//Ч�����
				ASSERT(wDataSize==sizeof(DBR_GR_LogonByUserID));
				if (wDataSize!=sizeof(DBR_GR_LogonByUserID)) return false;

				//ִ�в�ѯ
				DBR_GR_LogonByUserID * pLogonByUserID=(DBR_GR_LogonByUserID *)pDataBuffer;
				lReturnValue=SPLogonByUserID(pLogonByUserID->dwUserID,pLogonByUserID->szPassWord,pLogonByUserID->dwClientIP);
				pszPassword=pLogonByUserID->szPassWord;

				break;
			}
		default :	//Ĭ�ϴ���
			{
				ASSERT(FALSE); 
				return false;
			}
		}

		//��¼ʧ��
		if (lReturnValue!=0L)
		{
			DBR_GR_LogonError LogonError;
			LogonError.lErrorCode=lReturnValue;
			m_GameScoreDB->GetFieldValue(TEXT("ErrorDescribe"),LogonError.szErrorDescribe,sizeof(LogonError.szErrorDescribe));
			m_AttemperEvent.PostDataBaseEvent(DBR_GR_LOGON_ERROR,DataBaseEvent.wIndex,DataBaseEvent.wRoundID,&LogonError,sizeof(LogonError));
			m_GameScoreDB->CloseRecordset();
			return true;
		}

		//��������
		DBR_GR_LogonSuccess LogonSuccess;
		memset(&LogonSuccess,0,sizeof(LogonSuccess));
		CopyMemory(LogonSuccess.szPassWord,pszPassword,sizeof(LogonSuccess.szPassWord));

		//��ȡ�û���Ϣ
		m_GameScoreDB->GetFieldValue(TEXT("UserID"),LogonSuccess.dwUserID);
		m_GameScoreDB->GetFieldValue(TEXT("Gender"),LogonSuccess.cbGender);
		m_GameScoreDB->GetFieldValue(TEXT("MemberOrder"),LogonSuccess.cbMember);
		m_GameScoreDB->GetFieldValue(TEXT("Face"),LogonSuccess.szFace,sizeof(LogonSuccess.szFace));
		m_GameScoreDB->GetFieldValue(TEXT("TZPoint"),LogonSuccess.lTzPoint);
		m_GameScoreDB->GetFieldValue(TEXT("UserRight"),LogonSuccess.dwUserRight);
		m_GameScoreDB->GetFieldValue(TEXT("MasterRight"),LogonSuccess.dwMasterRight);
		m_GameScoreDB->GetFieldValue(TEXT("Accounts"),LogonSuccess.szAccounts,sizeof(LogonSuccess.szAccounts));
		m_GameScoreDB->GetFieldValue(TEXT("Name"),LogonSuccess.szName,sizeof(LogonSuccess.szName));

		//��ȡ��Ϸ��Ϣ
		m_GameScoreDB->GetFieldValue(TEXT("Score"),LogonSuccess.lScore);
		m_GameScoreDB->GetFieldValue(TEXT("WinCount"),LogonSuccess.lWinCount);
		m_GameScoreDB->GetFieldValue(TEXT("LostCount"),LogonSuccess.lLostCount);
		m_GameScoreDB->GetFieldValue(TEXT("DrawCount"),LogonSuccess.lDrawCount);
		m_GameScoreDB->GetFieldValue(TEXT("FleeCount"),LogonSuccess.lFleeCount);

		//Ͷ�ݵ���֪ͨ
		m_AttemperEvent.PostDataBaseEvent(DBR_GR_LOGON_SUCCESS,DataBaseEvent.wIndex,DataBaseEvent.wRoundID,
			&LogonSuccess,sizeof(LogonSuccess));
	}
	catch (IADOError * pIADOError)
	{
		//�������
		if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);

		//����ʧ��
		DBR_GR_LogonError LogonError;
		LogonError.lErrorCode=-1;
		lstrcpyn(LogonError.szErrorDescribe,TEXT("�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��Ϸ��������"),sizeof(LogonError.szErrorDescribe));
		m_AttemperEvent.PostDataBaseEvent(DBR_GR_LOGON_ERROR,DataBaseEvent.wIndex,DataBaseEvent.wRoundID,
			&LogonError,sizeof(LogonError));
	}

	//�رռ�¼��
	m_GameScoreDB->CloseRecordset();

	return true;
}

//д������
bool CDataBaseSink::OnRequestWriteScore(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_WriteScore));
		if (wDataSize!=sizeof(DBR_GR_WriteScore)) return false;

		//ִ�в�ѯ
		DBR_GR_WriteScore * pWriteScore=(DBR_GR_WriteScore *)pDataBuffer;
		LONG lReturnValue=SPWriteUserScore(pWriteScore->dwUserID,pWriteScore->dwPlayTimeCount,pWriteScore->dwOnlineTimeCount,pWriteScore->dwClientIP,pWriteScore->ScoreModifyInfo);

		//�������
		if (lReturnValue!=0L)
		{
		}
	}
	catch (IADOError * pIADOError)
	{
		//�������
		if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return true;
}

//�����ʻ�
bool CDataBaseSink::OnRequestLimitAccounts(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_LimitAccounts));
		if (wDataSize!=sizeof(DBR_GR_LimitAccounts)) return false;

		//ִ�в�ѯ
		DBR_GR_LimitAccounts * pLimitAccounts=(DBR_GR_LimitAccounts *)pDataBuffer;
		LONG lReturnValue=SPCongealAccounts(pLimitAccounts->dwUserID,pLimitAccounts->dwMasterUserID,pLimitAccounts->dwMasterClientIP);

		return true;
	}
	catch (IADOError * pIADOError)
	{
		//�������
		if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return true;
}

//����Ȩ��
bool CDataBaseSink::OnRequestSetUserRight(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_SetUserRight));
		if (wDataSize!=sizeof(DBR_GR_SetUserRight)) return false;

		//ִ�в�ѯ
		DBR_GR_SetUserRight * pSetUserRight=(DBR_GR_SetUserRight *)pDataBuffer;

		//��ϷȨ��
		if (pSetUserRight->cbGame==TRUE) 
		{
			SPSetUserGameRight(pSetUserRight->dwUserID,pSetUserRight->dwUserRight,pSetUserRight->dwMasterUserID,pSetUserRight->dwMasterClientIP);
		}

		//�ʺ�Ȩ��
		if (pSetUserRight->cbAccounts==TRUE) 
		{
			SPSetUserAccountsRight(pSetUserRight->dwUserID,pSetUserRight->dwUserRight,pSetUserRight->dwMasterUserID,pSetUserRight->dwMasterClientIP);
		}

		return true;
	}
	catch (IADOError * pIADOError)
	{
		//�������
		if (m_pIEventService!=NULL) m_pIEventService->ShowEventNotify(pIADOError->GetErrorDescribe(),Level_Exception);
	}

	return true;
}

//I D �洢����
LONG CDataBaseSink::SPLogonByUserID(DWORD dwUserID, LPCTSTR pszPassword, DWORD dwClientIP)
{
	//Ч�����
	ASSERT(dwUserID!=0L);
	ASSERT(pszPassword!=NULL);

	//ת����ַ
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//ִ�д洢����
	m_GameScoreDB->ClearAllParameters();
	m_GameScoreDB->SetSPName("GSP_GR_LogonByUserID");
	m_GameScoreDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameScoreDB->AddParamter(TEXT("@dwUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserID));
	m_GameScoreDB->AddParamter(TEXT("@strPassword"),adParamInput,adChar,lstrlen(pszPassword),_variant_t(pszPassword));
	m_GameScoreDB->AddParamter(TEXT("@wKindID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceAttrib->wKindID));
	m_GameScoreDB->AddParamter(TEXT("@wServerID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceOption->wServerID));
	m_GameScoreDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameScoreDB->ExecuteCommand(true);

	return m_GameScoreDB->GetReturnValue();
}

//�ʺŴ洢����
LONG CDataBaseSink::SPLogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, DWORD dwClientIP)
{
	//Ч�����
	ASSERT(pszAccounts!=NULL);
	ASSERT(pszPassword!=NULL);

	//ת����ַ
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//ִ�д洢����
	m_GameScoreDB->ClearAllParameters();
	m_GameScoreDB->SetSPName("GSP_GR_LogonByAccounts");
	m_GameScoreDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameScoreDB->AddParamter(TEXT("@strAccounts"),adParamInput,adVarChar,lstrlen(pszAccounts),_variant_t(pszAccounts));
	m_GameScoreDB->AddParamter(TEXT("@strPassword"),adParamInput,adChar,lstrlen(pszPassword),_variant_t(pszPassword));
	m_GameScoreDB->AddParamter(TEXT("@wKindID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceAttrib->wKindID));
	m_GameScoreDB->AddParamter(TEXT("@wServerID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceOption->wServerID));
	m_GameScoreDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameScoreDB->ExecuteCommand(true);

	return 0;
}

//д�ִ洢����
LONG CDataBaseSink::SPWriteUserScore(DWORD dwUserID, DWORD dwPlayTimeCount, DWORD dwOnLineTimeCount, DWORD dwClientIP, tagUserScore & UserScore)
{
	//Ч�����
	ASSERT(dwUserID!=0L);

	//ת����ַ
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//ִ�д洢����
	m_GameScoreDB->ClearAllParameters();
	m_GameScoreDB->SetSPName("GSP_GR_WriteGameScore");
	m_GameScoreDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameScoreDB->AddParamter(TEXT("@dwUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserID));
	m_GameScoreDB->AddParamter(TEXT("@lScore"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lScore));
	m_GameScoreDB->AddParamter(TEXT("@lWinCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lWinCount));
	m_GameScoreDB->AddParamter(TEXT("@lLostCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lLostCount));
	m_GameScoreDB->AddParamter(TEXT("@lDrawCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lDrawCount));
	m_GameScoreDB->AddParamter(TEXT("@lFleeCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lFleeCount));
	m_GameScoreDB->AddParamter(TEXT("@lTzPoint"),adParamInput,adInteger,sizeof(long),_variant_t((long)UserScore.lGold));
	m_GameScoreDB->AddParamter(TEXT("@dwPlayTimeCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwPlayTimeCount));
	m_GameScoreDB->AddParamter(TEXT("@dwOnLineTimeCount"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwOnLineTimeCount));
	m_GameScoreDB->AddParamter(TEXT("@wKindID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceAttrib->wKindID));
	m_GameScoreDB->AddParamter(TEXT("@wServerID"),adParamInput,adChar,sizeof(long),_variant_t((long)m_pGameServiceOption->wServerID));
	m_GameScoreDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameScoreDB->ExecuteCommand(false);

	return m_GameScoreDB->GetReturnValue();
}

//���Ŵ洢����
LONG CDataBaseSink::SPCongealAccounts(DWORD dwUserID, DWORD dwMasterUserID, DWORD dwClientIP)
{
	//Ч�����
	ASSERT(dwUserID!=0L);

	//ת����ַ
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//ִ�д洢����
	m_GameUserDB->ClearAllParameters();
	m_GameUserDB->SetSPName("GSP_GR_CongealAccounts");
	m_GameUserDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameUserDB->AddParamter(TEXT("@dwUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserID));
	m_GameUserDB->AddParamter(TEXT("@dwMasterUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwMasterUserID));
	m_GameUserDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameUserDB->ExecuteCommand(false);

	return m_GameUserDB->GetReturnValue();
}

//Ȩ�޴洢����
LONG CDataBaseSink::SPSetUserGameRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP)
{
	//Ч�����
	ASSERT(dwUserID!=0L);

	//ת����ַ
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//ִ�д洢����
	m_GameScoreDB->ClearAllParameters();
	m_GameScoreDB->SetSPName("GSP_GR_SetUserRight");
	m_GameScoreDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameScoreDB->AddParamter(TEXT("@dwUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserID));
	m_GameScoreDB->AddParamter(TEXT("@dwUserRight"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserRight));
	m_GameScoreDB->AddParamter(TEXT("@dwMasterUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwMasterUserID));
	m_GameScoreDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameScoreDB->ExecuteCommand(false);

	return m_GameScoreDB->GetReturnValue();
}

//Ȩ�޴洢����
LONG CDataBaseSink::SPSetUserAccountsRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP)
{
	//Ч�����
	ASSERT(dwUserID!=0L);

	//ת����ַ
	TCHAR szClientIP[16]=TEXT("");
	BYTE * pClientIP=(BYTE *)&dwClientIP;
	_snprintf(szClientIP,sizeof(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

	//ִ�д洢����
	m_GameUserDB->ClearAllParameters();
	m_GameUserDB->SetSPName("GSP_GR_SetUserRight");
	m_GameUserDB->AddParamter(TEXT("RETURN_VALUE"),adParamReturnValue,adInteger,sizeof(long),_variant_t((long)0));
	m_GameUserDB->AddParamter(TEXT("@dwUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserID));
	m_GameUserDB->AddParamter(TEXT("@dwUserRight"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwUserRight));
	m_GameUserDB->AddParamter(TEXT("@dwMasterUserID"),adParamInput,adInteger,sizeof(long),_variant_t((long)dwMasterUserID));
	m_GameUserDB->AddParamter(TEXT("@strClientIP"),adParamInput,adVarChar,lstrlen(szClientIP),_variant_t(szClientIP));
	m_GameUserDB->ExecuteCommand(false);

	return m_GameUserDB->GetReturnValue();
}

//////////////////////////////////////////////////////////////////////////
