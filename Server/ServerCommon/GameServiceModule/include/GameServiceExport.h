#ifndef GAME_SERVICE_EXPORT_HEAD_FILE
#define GAME_SERVICE_EXPORT_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ƽ̨ͷ�ļ�
#include "..\..\..\..\Common\CommonHead\Template\Template.h"
#include "..\..\..\..\Common\CommonHead\Common\GlobalDef.h"
#include "..\..\..\..\Common\CommonHead\Common\GlobalField.h"
#include "..\..\..\..\Common\CommonHead\Common\GlobalFrame.h"
#include "..\..\..\..\Common\CommonHead\Common\GlobalRight.h"
#include "..\..\..\..\Common\CommonHead\Common\GlobalService.h"

//�������ļ�
#include "..\..\..\..\Common\CommonHead\Message\CMD_Game.h"
#include "..\..\..\..\Common\CommonHead\Message\CMD_Plaza.h"
#include "..\..\..\..\Common\CommonHead\Message\CMD_Center.h"

//���ͷ�ļ�
#include "..\..\..\..\Common\FKLobby\include\ComService.h"
#include "..\..\..\..\Common\CommonHead\ModuleInterface\IUnknownEx.h"
#include "..\..\CoreServer\include\ServerKernel.h"

//////////////////////////////////////////////////////////////////////////
//�����궨��

//��������
#ifndef GAMESERVICEMODULE_CLASS
	#ifdef  GAMESERVICEMODULE_EXPORTS
		#define GAMESERVICEMODULE_CLASS _declspec(dllexport)
	#else
		#define GAMESERVICEMODULE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifdef _DEBUG
	#define GAMESERVICEMODULE_NAME	TEXT("GameServiceD.dll")				//��� DLL ����
#else
	#define GAMESERVICEMODULE_NAME	TEXT("GameService.dll")					//��� DLL ����
#endif

//�궨��
#define IDI_MAX_TIME_ID					30									//���޶�ʱ��

//����ԭ��
#define GER_NORMAL						0x00								//�������
#define GER_DISMISS						0x01								//��Ϸ��ɢ
#define GER_USER_LEFT					0x02								//�û��뿪

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//��ʼģʽ
enum enStartMode
{
	enStartMode_FullReady,				//���˿�ʼ
	enStartMode_AllReady,				//����׼��
	enStartMode_Symmetry,				//�Գƿ�ʼ
	enStartMode_TimeControl,			//ʱ�����
};

//��������
enum enScoreKind
{
	enScoreKind_Win,					//ʤ
	enScoreKind_Lost,					//��
	enScoreKind_Draw,					//��
	enScoreKind_Flee,					//��
};

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//������Ϣ
struct tagScoreInfo
{
	LONG								lScore;								//��Ϸ����
	enScoreKind							ScoreKind;							//��������
};

//�û�����
struct tagUserRule
{
	bool								bPassword;							//��������
	bool								bLimitWin;							//����ʤ��
	bool								bLimitFlee;							//��������
	bool								bLimitScore;						//���Ʒ���
	bool								bLimitDelay;						//������ʱ
	bool								bCheckSameIP;						//Ч���ַ
	WORD								wWinRate;							//ʤ������
	WORD								wFleeRate;							//��������
	WORD								wNetDelay;							//������ʱ
	LONG								lMaxScore;							//��߷��� 
	LONG								lLessScore;							//��ͷ���
	TCHAR								szPassword[PASS_LEN];				//��������
};

//�û���Ϣ�ṹ
struct tagServerUserData
{
	//������Ϣ
	TCHAR								szFace[128];						//ͷ��
	BYTE								cbMember;							//��Ա�ȼ�
	BYTE								cbGender;							//�û��Ա�
	DWORD								dwUserID;							//�û� I D
	DWORD								dwGroupID;							//��������
	DWORD								dwUserRight;						//�û��ȼ�
	DWORD								dwMasterRight;						//����Ȩ��
	TCHAR								szAccounts[NAME_LEN];				//�û��ʺ�
	TCHAR								szName[NAME_LEN];					//�û��ǳ�

	//״̬��Ϣ
	WORD								wTableID;							//���Ӻ���
	WORD								wChairID;							//����λ��
	WORD								wNetDelay;							//������ʱ
	BYTE								cbUserStatus;						//�û�״̬

	//������Ϣ
	tagUserScore						UserScoreInfo;						//�û�����
};

//���Ӳ����ṹ
struct tagTableFrameParameter
{
	IUnknownEx							* pIGameServiceFrame;				//������
	IUnknownEx							* pIGameServiceManager;				//�������
	const tagGameServiceAttrib			* pGameServiceAttrib;				//��������
	const tagGameServiceOption			* pGameServiceOption;				//���ò���
};

//��������ṹ
struct tagGameServiceParameter
{
	TCHAR								szModule[32];						//ģ������
	DWORD								dwCenterAddr;						//���ĵ�ַ
	tagDataBaseInfo						GameUserDBInfo;						//��������
	tagDataBaseInfo						GameScoreDBInfo;					//��������
	tagGameServiceOption				GameServiceOption;					//��������
};

//////////////////////////////////////////////////////////////////////////

#define VER_IServerUserItem INTERFACE_VERSION(2,1)
static const GUID IID_IServerUserItem={0x8fbde5db,0x99c5,0x4908,0x91,0xed,0x74,0xea,0xe4,0xc0,0xa7,0x8d};

//�������û��ӿ�
interface IServerUserItem : public IUnknownEx
{
	//��Ϣ�ӿ�
public:
	//�Ƿ񼤻�
	virtual bool __cdecl IsActive()=NULL;
	//�û�����
	virtual WORD __cdecl GetUserIndex()=NULL;
	//��ȡ��ַ
	virtual DWORD __cdecl GetClientIP()=NULL;
	//��ȡ����
	virtual LPCTSTR __cdecl GetPassword()=NULL;
	//��ȡ����
	virtual const tagUserRule * __cdecl GetUserRule()=NULL;
	//��ȡ����
	virtual const tagUserScore * __cdecl GetUserScore()=NULL;
	//��ȡ��Ϣ
	virtual tagServerUserData * __cdecl GetUserData()=NULL;

	//���Խӿ�
public:
	//�û�����
	virtual LPCTSTR __cdecl GetAccounts()=NULL;
	//�û� I D
	virtual DWORD __cdecl GetUserID()=NULL;
	//���Ӻ���
	virtual WORD __cdecl GetTableID()=NULL;
	//���Ӻ���
	virtual WORD __cdecl GetChairID()=NULL;
	//�û�״̬
	virtual BYTE __cdecl GetUserStatus()=NULL;
	//�û�ʤ��
	virtual WORD __cdecl GetUserWinRate()=NULL;
	//�û�����
	virtual WORD __cdecl GetUserLostRate()=NULL;
	//�û�����
	virtual WORD __cdecl GetUserDrawRate()=NULL;
	//�û�����
	virtual WORD __cdecl GetUserFleeRate()=NULL;

	//ͳ�ƽӿ�
public:
	//��Ϸʱ��
	virtual DWORD __cdecl GetPlayTimeCount()=NULL;
	//����ʱ��
	virtual DWORD __cdecl GetOnlineTimeCount()=NULL;
	//������Ϣ
	virtual bool __cdecl GetUserScoreInfo(tagUserScore & ScoreResultInfo, tagUserScore & ScoreModifyInfo)=NULL;

	//���ܽӿ�
public:
	//���ù���
	virtual bool __cdecl SetUserRule(tagUserRule & UserRule)=NULL;
	//�޸Ļ���
    virtual bool __cdecl WriteScore(tagScoreInfo & ScoreInfo, DWORD dwPlayTimeCount)=NULL;
	//����״̬
	virtual bool __cdecl SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IServerUserManager INTERFACE_VERSION(2,1)
static const GUID IID_IServerUserManager={0xa27d1d5d,0x5999,0x488e,0xaa,0x4a,0x30,0x8a,0x1,0x11,0xf4,0x9e};

//�������û�����ӿ�
interface IServerUserManager : public IUnknownEx
{
	//����ӿ�
public:
	//�����û�
	virtual bool __cdecl ResetUserManager()=NULL;
	//�л��û�
	virtual bool __cdecl SwitchOnLineUserItem(IServerUserItem * pIServerUserItem, DWORD dwClientIP, WORD wUserIndex)=NULL;
	//�����û�
	virtual bool __cdecl ActiveOffLineUserItem(IServerUserItem * pIServerUserItem, DWORD dwClientIP, WORD wUserIndex)=NULL;
	//�����û�
	virtual IServerUserItem * __cdecl ActiveUserItem(tagServerUserData & ServerUserData, DWORD dwClientIP, WORD wUserIndex, TCHAR szPassword[PASS_LEN])=NULL;
	//ɾ���û�
	virtual bool __cdecl DeleteUserItem(IServerUserItem * pIServerUserItem)=NULL;
	//ע�����
	virtual bool __cdecl RegOffLineUserItem(IServerUserItem * pIServerUserItem)=NULL;
	//ɾ������
	virtual bool __cdecl UnRegOffLineUserItem(IServerUserItem * pIServerUserItem)=NULL;

	//���ҽӿ�
public:
	//ö���û�
	virtual IServerUserItem * __cdecl EnumOnLineUser(WORD wEnumIndex)=NULL;
	//ö���û�
	virtual IServerUserItem * __cdecl EnumOffLineUser(WORD wEnumIndex)=NULL;
	//�����û�
	virtual IServerUserItem * __cdecl SearchOnLineUser(DWORD dwUserID)=NULL;
	//�����û�
	virtual IServerUserItem * __cdecl SearchOffLineUser(DWORD dwUserID)=NULL;

	//��Ϣ�ӿ�
public:
	//��������
	virtual DWORD __cdecl GetOnLineCount()=NULL;
	//��������
	virtual DWORD __cdecl GetOffLineCount()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITableFrame INTERFACE_VERSION(5,1)
static const GUID IID_ITableFrame={0x1aee9bf4,0x8629,0x45dc,0xb0,0xfa,0x2c,0xa7,0x4f,0xbe,0x18,0xc1};

//��Ϸ���ӽӿ�
interface ITableFrame : public IUnknownEx
{
	//��Ϣ�ӿ�
public:
	//���Ӻ���
	virtual WORD __cdecl GetTableID()=NULL;
	//��Ϸ����
	virtual WORD __cdecl GetChairCount()=NULL;
	//��Ϸʱ��
	virtual DWORD __cdecl GetPlayTimeCount()=NULL;

	//�û��ӿ�
public:
	//ö���û�
	virtual IServerUserItem * __cdecl EnumLookonUserItem(WORD wIndex)=NULL;
	//��ȡ�û�
	virtual IServerUserItem * __cdecl GetServerUserItem(WORD wChairID)=NULL;

	//״̬�ӿ�
public:
	//��ȡ״̬
	virtual BYTE __cdecl GetGameStatus()=NULL;
	//����״̬
	virtual void __cdecl SetGameStatus(BYTE bGameStatus)=NULL;
	//��ȡ����
	virtual const tagGameServiceOption * __cdecl GetGameServiceOption()=NULL;

	//��ʱ���ӿ�
public:
	//���ö�ʱ��
	virtual bool __cdecl SetGameTimer(WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)=NULL;
	//ɾ����ʱ��
	virtual bool __cdecl KillGameTimer(WORD wTimerID)=NULL;

	//����ӿ�
public:
	//��������
	virtual bool __cdecl SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)=NULL;
	//��������
	virtual bool __cdecl SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
	//��������
	virtual bool __cdecl SendTableData(WORD wChairID, WORD wSubCmdID)=NULL;
	//��������
	virtual bool __cdecl SendTableData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
	//��������
	virtual bool __cdecl SendLookonData(WORD wChairID, WORD wSubCmdID)=NULL;
	//��������
	virtual bool __cdecl SendLookonData(WORD wChairID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;

	//��Ϣ�ӿ�
public:
	//���ͷ�����Ϣ
	virtual bool __cdecl SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;
	//������Ϸ��Ϣ
	virtual bool __cdecl SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wMessageType)=NULL;

	//��Ϸ�ӿ�
public:
	//��ɢ��Ϸ
	virtual bool __cdecl DismissGame()=NULL;
	//������Ϸ
	virtual bool __cdecl ConcludeGame()=NULL;
	//д�����
	virtual bool __cdecl WriteUserScore(WORD wChairID, tagScoreInfo & ScoreInfo)=NULL;
	//д�����
	virtual bool __cdecl WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount, LONG lTaxScore)=NULL;
	//���ͳ���
	virtual bool __cdecl SendGameScene(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITableFrameManager INTERFACE_VERSION(2,1)
static const GUID IID_ITableFrameManager={0xc0994158,0xf4b4,0x462f,0xa8,0x84,0x85,0x13,0xe2,0xac,0xfe,0x31};

//��ܹ���ӿ�
interface ITableFrameManager : public ITableFrame
{
	//���ýӿ�
public:
	//��ʼ��
	virtual bool __cdecl InitTableFrame(WORD wTableID, tagTableFrameParameter * pTableFrameParameter)=NULL;

	//��Ϣ�ӿ�
public:
	//����״̬
	virtual bool __cdecl IsTableLocked()=NULL;
	//��Ϸ״̬
	virtual bool __cdecl IsGameStarted()=NULL;
	//��Ϸ״̬
	virtual bool __cdecl IsUserPlaying(IServerUserItem * pIServerUserItem)=NULL;

	//ִ�нӿ�
public:
	//���¶���
	virtual bool __cdecl PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//�Թ۶���
	virtual bool __cdecl PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//�뿪����
	virtual bool __cdecl PerformStandUpAction(IServerUserItem * pIServerUserItem)=NULL;

	//�û��¼�
public:
	//�����¼�
	virtual bool __cdecl OnUserOffLine(WORD wChairID)=NULL;
	//�ؽ��¼�
	virtual bool __cdecl OnUserReConnect(WORD wChairID)=NULL;
	//�������
	virtual bool __cdecl OnUserReqOffLine(WORD wChairID)=NULL;

	//ϵͳ�¼�
public:
	//��ʱ���¼�
	virtual bool __cdecl OnEventTimer(WORD wTimerID, WPARAM wBindParam)=NULL;
	//��Ϸ�¼�
	virtual bool __cdecl OnEventSocketGame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
	//����¼�
	virtual bool __cdecl OnEventSocketFrame(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITableFrameSink INTERFACE_VERSION(3,1)
static const GUID IID_ITableFrameSink={0x53d985c5,0x623a,0x41df,0x89,0x76,0x27,0xb1,0x8d,0xc1,0x97,0x37};

//��Ϸ���ӻص��ӿ�
interface ITableFrameSink : public IUnknownEx
{
	//����ӿ�
public:
	//��ʼ��
	virtual bool __cdecl InitTableFrameSink(IUnknownEx * pIUnknownEx)=NULL;
	//��λ����
	virtual void __cdecl RepositTableFrameSink()=NULL;

	//��Ϣ�ӿ�
public:
	//��ʼģʽ
	virtual enStartMode __cdecl GetGameStartMode()=NULL;
	//��Ϸ״̬
	virtual bool __cdecl IsUserPlaying(WORD wChairID)=NULL;

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool __cdecl OnEventGameStart()=NULL;
	//��Ϸ����
	virtual bool __cdecl OnEventGameEnd(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	//���ͳ���
	virtual bool __cdecl SendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE bGameStatus, bool bSendSecret)=NULL;

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool __cdecl OnTimerMessage(WORD wTimerID, WPARAM wBindParam)=NULL;
	//��Ϸ��Ϣ
	virtual bool __cdecl OnGameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
	//�����Ϣ
	virtual bool __cdecl OnFrameMessage(WORD wSubCmdID, const void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_ITableFrameEvent INTERFACE_VERSION(1,1)
static const GUID IID_ITableFrameEvent={0xb16717de,0xc243,0x4686,0xbb,0x8a,0xd0,0x37,0xb8,0xc2,0xc,0xf6};

//����¼��ӿ�
interface ITableFrameEvent : public IUnknownEx
{
	//�����¼�
public:
	//����ͬ��
	virtual bool __cdecl OnEventUserReqReady(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//�������
	virtual bool __cdecl OnEventUserReqOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//��������
	virtual bool __cdecl OnEventUserReqReConnect(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//��������
	virtual bool __cdecl OnEventUserReqSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon)=NULL;
	//��������
	virtual bool __cdecl OnEventUserReqStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bReqLookon)=NULL;

	//�����¼�
public:
	//�û�ͬ��
	virtual bool __cdecl OnEventUserReady(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//�û�����
	virtual bool __cdecl OnEventUserOffLine(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//�û�����
	virtual bool __cdecl OnEventUserReConnect(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//�û�����
	virtual bool __cdecl OnEventUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//�û�����
	virtual bool __cdecl OnEventUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IGameServiceFrame INTERFACE_VERSION(1,1)
static const GUID IID_IGameServiceFrame={0xafccfa53,0x6431,0x4b2a,0x98,0x29,0xf5,0xcf,0x15,0xdf,0xd5,0x3b};

//��Ϸ������
interface IGameServiceFrame : public IUnknownEx
{
	//״̬�ӿ�
public:
	//�رղ�ѯ
	virtual bool __cdecl IsShallClose()=NULL;
	//˽�Ĳ�ѯ
	virtual bool __cdecl IsAllowWisper()=NULL;
	//�����ѯ
	virtual bool __cdecl IsAllowRoomChat()=NULL;
	//�����ѯ
	virtual bool __cdecl IsAllowGameChat()=NULL;
	//�����ѯ
	virtual bool __cdecl IsAllowEnterRoom()=NULL;
	//�����ѯ
	virtual bool __cdecl IsAllowEnterGame()=NULL;

	//״̬֪ͨ
public:
	//����״̬
	virtual bool __cdecl SendTableStatus(WORD wTableID)=NULL;
	//���ͷ���
	virtual bool __cdecl SendUserScore(IServerUserItem * pIServerUserItem)=NULL;
	//����״̬
	virtual bool __cdecl SendUserStatus(IServerUserItem * pIServerUserItem)=NULL;

	//����ӿ�
public:
	//��������
	virtual bool __cdecl SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//��������
	virtual bool __cdecl SendData(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;

	//��ʱ���ӿ�
public:
	//���ö�ʱ��
	virtual bool __cdecl SetTableTimer(WORD wTableID, WORD wTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM wBindParam)=NULL; 
	//ɾ����ʱ��
	virtual bool __cdecl KillTableTimer(WORD wTableID, WORD wTimerID)=NULL;

	//����ӿ�
public:
	//ɾ���û�
	virtual bool __cdecl DeleteUserItem(IServerUserItem * pIServerUserItem)=NULL;
	//�����Ϣ
	virtual void __cdecl ExportInformation(LPCTSTR pszString, enTraceLevel TraceLevel)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IGameServiceManager INTERFACE_VERSION(2,1)
static const GUID IID_IGameServiceManager={0xda6b9efa,0x3c56,0x4ee1,0x96,0xce,0x90,0xe1,0x20,0x48,0xe5,0xe7};

//��Ϸ����������ӿ�
interface IGameServiceManager : public IUnknownEx
{
	//��������
	virtual void * __cdecl CreateTableFrameSink(const IID & Guid, DWORD dwQueryVer)=NULL;
	//��ȡ����
	virtual void __cdecl GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)=NULL;
	//�޸Ĳ���
	virtual bool __cdecl RectifyServiceOption(tagGameServiceOption * pGameServiceOption)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IGameService INTERFACE_VERSION(2,1)
static const GUID IID_IGameService={0x1239db23,0x4d2e,0x4f35,0xbb,0x4e,0x8c,0x4c,0x5d,0x39,0xe1,0xa0};

//��Ϸ����ӿ�
interface IGameService : public IUnknownEx
{
	//����״̬
	virtual bool __cdecl IsService()=NULL;
	//�����¼�
	virtual bool __cdecl SetEventService(IUnknownEx * pIEventService)=NULL;
	//��ʼ����
	virtual bool __cdecl StartService(tagGameServiceParameter * pGameServiceParameter)=NULL;
	//ֹͣ����
	virtual bool __cdecl StopService()=NULL;
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ�������������
class CGameServiceHelper : public CTempldateHelper<IGameService>
{
	//��������
public:
	//���캯��
	CGameServiceHelper(void) : CTempldateHelper<IGameService>(IID_IGameService,
		VER_IGameService,GAMESERVICEMODULE_NAME,TEXT("CreateGameService")) { }
};

//////////////////////////////////////////////////////////////////////////

#endif