#ifndef GLOBAL_DEF_HEAD_FILE
#define GLOBAL_DEF_HEAD_FILE

#pragma once

/////////////////////////////////////////////////////////////////////////////////////////
//��������

const TCHAR	szProductName[]=TEXT("FK����ƽ̨");							//��Ʒ����
const TCHAR szSystemRegKey[]=TEXT("FKChessCards");							//ע������
const TCHAR szStationPage[]=TEXT("hi.baidu.com/FreedomKnightDuzhi");					    //��Ϸ��ҳ

/////////////////////////////////////////////////////////////////////////////////////////
//��Ϸ��ʶ

//ͨ����Ϸ
#define ID_PLAZA						0									//����
#define ID_LAND							1									//������
/////////////////////////////////////////////////////////////////////////////////////////
//�궨��

#define MAX_CHAIR						8									//�����Ϸ��
#define MAX_CHAT_LEN					512									//���쳤��
#define INVALID_TABLE					((WORD)(-1))						//��Ч���Ӻ�
#define INVALID_CHAIR					((WORD)(-1))						//��Ч���Ӻ�
#define MAIN_DLG_CLASSNAME				TEXT("FKChessCards")				//����ע����

//�˿ڶ���
#define PORT_LOGON_SERVER				8888								//��½������
#define PORT_CENTER_SERVER				8889								//���ķ�����

//�������ݶ���
#define SOCKET_VER						68									//���ݰ��汾
#define SOCKET_PACKAGE					2046								//��������
#define SOCKET_BUFFER					(sizeof(CMD_Head)+SOCKET_PACKAGE+2*sizeof(DWORD))

/////////////////////////////////////////////////////////////////////////////////////////

//���ݰ��ṹ��Ϣ
struct CMD_Info
{
	WORD								wDataSize;							//���ݴ�С
	BYTE								cbCheckCode;						//Ч���ֶ�
	BYTE								cbMessageVer;						//�汾��ʶ
};

//���ݰ�������Ϣ
struct CMD_Command
{
	WORD								wMainCmdID;							//��������
	WORD								wSubCmdID;							//��������
};

//���ݰ����ݰ�ͷ
struct CMD_Head
{
	CMD_Info							CmdInfo;							//�����ṹ
	CMD_Command							CommandInfo;						//������Ϣ
};

//�������ݰ�����
struct CMD_Buffer
{
	CMD_Head							Head;								//���ݰ�ͷ
	BYTE								cbBuffer[SOCKET_PACKAGE];			//���ݻ���
};

//�ں�������
#define MDM_KN_COMMAND					0									//�ں�����
#define SUB_KN_DETECT_SOCKET			1									//�������

//���ṹ��Ϣ
struct CMD_KN_DetectSocket
{
	DWORD								dwSendTickCount;					//����ʱ��
	DWORD								dwRecvTickCount;					//����ʱ��
};

/////////////////////////////////////////////////////////////////////////////////////////

//IPC ���ݶ���
#define IPC_VER							0x0001								//IPC �汾
#define IPC_IDENTIFIER					0x0001								//��ʶ����
#define IPC_PACKAGE						4096								//��� IPC ��
#define IPC_BUFFER						(sizeof(IPC_Head)+IPC_PACKAGE)		//���峤��

//IPC ���ݰ�ͷ
struct IPC_Head
{
	WORD								wVersion;							//IPC �汾
	WORD								wDataSize;							//���ݴ�С
	WORD								wMainCmdID;							//��������
	WORD								wSubCmdID;							//��������
};

//IPC ����ṹ
struct IPC_Buffer
{
	IPC_Head							Head;								//���ݰ�ͷ
	BYTE								cbBuffer[IPC_PACKAGE];				//���ݻ���
};

//////////////////////////////////////////////////////////////////////////

//���Ⱥ궨��
#define TYPE_LEN						32									//���೤��
#define KIND_LEN						32									//���ͳ���
#define STATION_LEN						32									//վ�㳤��
#define SERVER_LEN						32									//���䳤��
#define MODULE_LEN						32									//���̳���
#define SERVERNAME_LEN					64									//�������ֳ���

//��Ϸ����
#define GAME_GENRE_SCORE				0x0001								//��ֵ����
#define GAME_GENRE_POINT				0x0002								//��ȯ����

//��Ϸ���ͽṹ
struct tagGameType
{
	WORD								wSortID;							//�������
	WORD								wTypeID;							//�������
	TCHAR								szTypeName[TYPE_LEN];				//��������
};

struct tagLogonServer
{
	TCHAR								szName[SERVERNAME_LEN];					//����������
	TCHAR								szHost[SERVERNAME_LEN];					//����������
	TCHAR								szIP[15];							    //IP

};

//��Ϸ���ƽṹ
struct tagGameKind
{
	WORD								wSortID;							//�������
	WORD								wTypeID;							//���ͺ���
	WORD								wKindID;							//���ƺ���
	DWORD								dwMaxVersion;						//���°汾
	DWORD								dwOnLineCount;						//������Ŀ
	TCHAR								szKindName[KIND_LEN];				//��Ϸ����
	TCHAR								szProcessName[MODULE_LEN];			//��������
};

//��Ϸ���̽ṹ
struct tagGameProcess
{
	WORD								wSortID;							//�������
	WORD								wTypeID;							//���ͺ���
	WORD								wKindID;							//���ƺ���
	WORD								wServerPort;						//����˿�
	DWORD								dwServerAddr;						//�����ַ
	DWORD								dwMaxVersion;						//���°汾
	DWORD								dwOnLineCount;						//������Ŀ
	TCHAR								szKindName[KIND_LEN];				//��Ϸ����
	TCHAR								szProcessName[MODULE_LEN];			//��������
};

//��Ϸվ��ṹ
struct tagGameStation
{
	WORD								wSortID;							//�������
	WORD								wKindID;							//���ƺ���
	WORD								wJoinID;							//�ҽӺ���
	WORD								wStationID;							//վ�����
	TCHAR								szStationName[STATION_LEN];			//վ������
};

//��Ϸ�����б�ṹ
struct tagGameServer
{
	WORD								wSortID;							//�������
	WORD								wKindID;							//���ƺ���
	WORD								wServerID;							//�������
	WORD								wStationID;							//վ�����
	WORD								wServerPort;						//����˿�
	DWORD								dwServerAddr;						//�����ַ
	DWORD								dwOnLineCount;						//��������
	TCHAR								szServerName[SERVER_LEN];			//��������
};

//////////////////////////////////////////////////////////////////////////

//�û�״̬����
#define US_NULL							0x00								//û��״̬
#define US_FREE							0x01								//վ��״̬
#define US_SIT							0x02								//����״̬
#define US_READY						0x03								//ͬ��״̬
#define US_LOOKON						0x04								//�Թ�״̬
#define US_PLAY							0x05								//��Ϸ״̬
#define US_OFFLINE						0x06								//����״̬

//���Ⱥ궨��
#define NAME_LEN						32									//���ֳ���
#define PASS_LEN						33									//���볤��

//�û�������Ϣ
struct tagUserScore
{
	LONG								lGold;							//�û���ȯ
	LONG								lScore;								//�û�����
	LONG								lWinCount;							//ʤ������
	LONG								lLostCount;							//ʧ������
	LONG								lDrawCount;							//�;�����
	LONG								lFleeCount;							//������Ŀ
	LONG								lExperience;						//�û�����
};

//�û�״̬��Ϣ
struct tagUserStatus
{
	WORD								wTableID;							//���Ӻ���
	WORD								wChairID;							//����λ��
	WORD								wNetDelay;							//������ʱ
	BYTE								cbUserStatus;						//�û�״̬
};

//�û�������Ϣ�ṹ
struct tagUserInfoHead
{
	//�û�����
	TCHAR								szFace[128];						//ͷ��
	BYTE								cbGender;							//�û��Ա�
	BYTE								cbMember;							//��Ա�ȼ�
	DWORD								dwUserID;							//�û� I D
	DWORD								dwUserRight;						//�û��ȼ�
	DWORD								dwMasterRight;						//����Ȩ��
	DWORD								dwGroupID;							//���� I D

	//�û�״̬
	WORD								wTableID;							//���Ӻ���
	WORD								wChairID;							//����λ��
	WORD								wNetDelay;							//������ʱ
	BYTE								cbUserStatus;						//�û�״̬

	//�û�����
	tagUserScore						UserScoreInfo;						//������Ϣ
};

//�û���Ϣ�ṹ
struct tagUserData
{
	//�û�����
	TCHAR								szFace[128];						//ͷ��
	BYTE								cbGender;							//�û��Ա�
	BYTE								cbMember;							//��Ա�ȼ�
	DWORD								dwUserID;							//�û� I D
	DWORD								dwUserRight;						//�û��ȼ�
	DWORD								dwMasterRight;						//����Ȩ��
	TCHAR								szAccounts[NAME_LEN];				//�û��˺�
	TCHAR								szName[NAME_LEN];					//�û��ǳ�
	TCHAR								szGroupName[NAME_LEN];				//��������

	//�û�����
	LONG								lGold;							//�û���ȯ
	LONG								lScore;								//�û�����
	LONG								lWinCount;							//ʤ������
	LONG								lLostCount;							//ʧ������
	LONG								lDrawCount;							//�;�����
	LONG								lFleeCount;							//������Ŀ
	LONG								lExperience;						//�û�����

	//�û�״̬
	WORD								wTableID;							//���Ӻ���
	WORD								wChairID;							//����λ��
	WORD								wNetDelay ;							//������ʱ
	BYTE								cbUserStatus;						//�û�״̬

	//������Ϣ
	BYTE								cbCompanion;						//�û���ϵ
};

//////////////////////////////////////////////////////////////////////////

//�������кŽṹ
struct tagClientSerial
{
	DWORD								dwComputerID[3];					//��������
};

//���û���ṹ
struct tagOptionBuffer
{
	BYTE								cbBufferLen;						//���ݳ���
	BYTE								cbOptionBuf[32];					//���û���
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif