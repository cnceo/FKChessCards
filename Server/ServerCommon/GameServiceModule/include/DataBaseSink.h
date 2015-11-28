#ifndef DATA_BASE_SINK_HEAD_FILE
#define DATA_BASE_SINK_HEAD_FILE

#pragma once

#include "stdafx.h"
#include "GameServiceExport.h"

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//���ݿ����
struct tagDataBaseSinkParameter
{
	tagDataBaseInfo						* pGameUserDBInfo;				//������Ϣ
	tagDataBaseInfo						* pGameScoreDBInfo;				//������Ϣ
	tagGameServiceAttrib				* pGameServiceAttrib;			//��������
	tagGameServiceOption				* pGameServiceOption;			//������Ϣ
};

//////////////////////////////////////////////////////////////////////////
//���ݿ��������ݰ�

//���ݿ������ʶ
#define	DBR_GR_LOGON_BY_ACCOUNTS		1								//�ʻ���½
#define	DBR_GR_LOGON_BY_USERID			2								//I D ��½
#define DBR_GR_WRITE_SCORE				3								//�޸Ļ���
#define DBR_GR_LIMIT_ACCOUNTS			4								//�����ʻ�
#define DBR_GR_SET_USER_RIGHT			5								//����Ȩ��

//���ݿ������ʶ
#define DBR_GR_LOGON_SUCCESS			100								//��½�ɹ�
#define DBR_GR_LOGON_ERROR				101								//��½ʧ��

//�ʻ���½
struct DBR_GR_LogonByAccounts
{
	DWORD								dwClientIP;						//���ӵ�ַ
	TCHAR								szAccounts[NAME_LEN];			//��¼�ʺ�
	TCHAR								szPassWord[PASS_LEN];			//��¼����
};

//ID ��¼
struct DBR_GR_LogonByUserID
{
	DWORD								dwClientIP;						//���ӵ�ַ
	DWORD								dwUserID;						//�û� I D
	TCHAR								szPassWord[PASS_LEN];			//��¼����
};

//�����ʻ�
struct DBR_GR_LimitAccounts
{
	DWORD								dwUserID;						//�û� I D
	DWORD								dwMasterUserID;					//���� I D
	DWORD								dwMasterClientIP;				//���ӵ�ַ
};

//����Ȩ��
struct DBR_GR_SetUserRight
{
	//Ȩ�޷�Χ
	BYTE								cbGame;							//������Ϸ
	BYTE								cbAccounts;						//�����ʺ�

	//Ȩ����Ϣ
	DWORD								dwUserID;						//�û� I D
	DWORD								dwUserRight;					//�û�Ȩ��
	DWORD								dwMasterUserID;					//���� I D
	DWORD								dwMasterClientIP;				//���ӵ�ַ
};

//��½�ɹ�
struct DBR_GR_LogonSuccess
{
	//������Ϣ
	BYTE								cbGender;						//�û��Ա�
	BYTE								cbMember;						//��Ա�ȼ�
	TCHAR								szFace[128];					//ͷ��
	DWORD								dwUserID;						//�û� I D
	DWORD								dwUserRight;					//�û��ȼ�
	DWORD								dwMasterRight;					//����Ȩ��
	TCHAR								szAccounts[NAME_LEN];			//��¼�ʺ�
	TCHAR								szName[NAME_LEN];				//�û��ǳ�
	TCHAR								szPassWord[PASS_LEN];			//��¼����

	//������Ϣ
	LONG								lTzPoint;						//�û���ȯ
	LONG								lScore;							//�û�����
	LONG								lWinCount;						//ʤ������
	LONG								lLostCount;						//ʧ������
	LONG								lDrawCount;						//�;�����
	LONG								lFleeCount;						//������Ŀ
};

//��½ʧ��
struct DBR_GR_LogonError
{
	LONG								lErrorCode;						//�������
	TCHAR								szErrorDescribe[128];			//������Ϣ
};

//�޸Ļ���
struct DBR_GR_WriteScore
{
	DWORD								dwUserID;						//�û���ַ
	DWORD								dwClientIP;						//���ӵ�ַ
	DWORD								dwPlayTimeCount;				//��Ϸʱ��
	DWORD								dwOnlineTimeCount;				//����ʱ��
	TCHAR								szAccounts[NAME_LEN];			//�ʻ�����
	tagUserScore						ScoreBornInfo;					//ԭ������
	tagUserScore						ScoreModifyInfo;				//�޸Ļ���
	tagUserScore						ScoreResultInfo;				//�û�����
};

//////////////////////////////////////////////////////////////////////////

//���ݿ����湳��
class CDataBaseSink : public IDataBaseSink
{
	//���ñ���
protected:
	tagDataBaseInfo						* m_pGameUserDBInfo;			//������Ϣ
	tagDataBaseInfo						* m_pGameScoreDBInfo;			//������Ϣ
	tagGameServiceAttrib				* m_pGameServiceAttrib;			//��������
	tagGameServiceOption				* m_pGameServiceOption;			//������Ϣ

	//�ӿڱ���
protected:
	IEventService						* m_pIEventService;				//�¼�����

	//�������
protected:
	CDataBaseHelper						m_GameUserDB;					//�û����ݿ�
	CDataBaseHelper						m_GameScoreDB;					//��Ϸ���ݿ�
	CQueueServiceEvent					m_AttemperEvent;				//����֪ͨ

	//��������
public:
	//���캯��
	CDataBaseSink();
	//��������
	virtual ~CDataBaseSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//�Ƿ���Ч
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CDataBaseSink))?true:false; }
	//�ӿڲ�ѯ
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//���Ƚӿ�
public:
	//����ģ������
	virtual bool __cdecl StartService(IUnknownEx * pIUnknownEx);
	//����ģ��ر�
	virtual bool __cdecl StopService(IUnknownEx * pIUnknownEx);
	//���ݲ�������
	virtual bool __cdecl OnDataBaseRequest(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);

	//���ú��� 
public:
	//�����¼�
	bool SetEventService(IUnknownEx * pIUnknownEx);
	//���ú���
	bool InitDataBaseSink(tagDataBaseSinkParameter * pDataBaseSinkParameter, IUnknownEx * pIUnknownEx);

	//������
private:
	//��¼����
	bool OnRequestLogon(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);
	//д������
	bool OnRequestWriteScore(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);
	//�����ʻ�
	bool OnRequestLimitAccounts(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);
	//����Ȩ��
	bool OnRequestSetUserRight(const NTY_DataBaseEvent & DataBaseEvent, void * pDataBuffer, WORD wDataSize);

	//�洢����
protected:
	//I D �洢����
	LONG SPLogonByUserID(DWORD dwUserID, LPCTSTR pszPassword, DWORD dwClientIP);
	//�ʺŴ洢����
	LONG SPLogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszPassword, DWORD dwClientIP);
	//д�ִ洢����
	LONG SPWriteUserScore(DWORD dwUserID, DWORD dwPlayTimeCount, DWORD dwOnLineTimeCount, DWORD dwClientIP, tagUserScore & UserScore);
	//���Ŵ洢����
	LONG SPCongealAccounts(DWORD dwUserID, DWORD dwMasterUserID, DWORD dwClientIP);
	//Ȩ�޴洢����
	LONG SPSetUserGameRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP);
	//Ȩ�޴洢����
	LONG SPSetUserAccountsRight(DWORD dwUserID, DWORD dwUserRight, DWORD dwMasterUserID, DWORD dwClientIP);
};

//////////////////////////////////////////////////////////////////////////

#endif