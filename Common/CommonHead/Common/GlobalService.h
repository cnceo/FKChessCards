#ifndef GLOBAL_SERVICE_HEAD_FILE
#define GLOBAL_SERVICE_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//���ݿ�����
const TCHAR szGameUserDB[]=TEXT("TZUserDB");							//�û����ݿ�
const TCHAR szServerInfoDB[]=TEXT("TZServerInfoDB");					//�������ݿ�

//���ݿ����
const TCHAR szTableScore[]=TEXT("GameScore");							//��Ϸ���ֱ�
const TCHAR szTableAccounts[]=TEXT("UserAccounts");						//�û��ʻ���

//////////////////////////////////////////////////////////////////////////

//���ݿ���Ϣ
struct tagDataBaseInfo
{
	DWORD							dwDataBaseAddr;						//���ݿ��ַ
	TCHAR							szDataBaseUser[32];					//���ݿ��û�
	TCHAR							szDataBasePass[32];					//���ݿ�����
	TCHAR							szDataBaseName[32];					//���ݿ�����
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ��������
struct tagGameServiceAttrib
{
	WORD							wKindID;							//���ƺ���
	WORD							wChairCount;						//������Ŀ
	WORD							wSupporType;						//֧������
	DWORD							dwServerVersion;					//�汾��Ϣ
	DWORD							dwMaxClientVersion;					//�汾��Ϣ
	DWORD							dwLessClientVersion;				//�汾��Ϣ
	TCHAR							szDataBaseName[32];					//��Ϸ����
	TCHAR							szDescription[128];					//ģ������
	TCHAR							szKindName[KIND_LEN];				//��������
	TCHAR							szServerModuleName[MODULE_LEN];		//ģ������
	TCHAR							szClientModuleName[MODULE_LEN];		//ģ������
};

//��Ϸ��������
struct tagGameServiceOption
{
	//��������
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wServerID;							//�������
	WORD							wStationID;							//վ�����
	WORD							wTableCount;						//������Ŀ
	WORD							wServerType;						//��������
	WORD							wServerPort;						//����˿�
	DWORD							dwServerAddr;						//�����ַ
	TCHAR							szGameRoomName[SERVER_LEN];			//��������

	//��������
	LONG							lCellScore;							//��λ����
	LONG							lHighScore;							//��߻���
	LONG							lLessScore;							//��ͻ���

	//��չ����
	WORD							wMaxConnect;						//�������
	DWORD							dwUserRight;						//����Ȩ��
	tagOptionBuffer					OptionBuffer;						//���û���

	//��������
	LONG							lMatchDraw;							//��������
	BYTE							cbLimitDraw;						//���ƾ���
	BYTE							cbControlStart;						//���ƿ�ʼ
};

//////////////////////////////////////////////////////////////////////////

#endif