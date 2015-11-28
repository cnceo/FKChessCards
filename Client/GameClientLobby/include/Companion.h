#ifndef COMPANION_HEAD_FILE
#define COMPANION_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ƽ̨ͷ�ļ�
#include "../../../Common/CommonHead/Template/Template.h"

//ȫ��ͷ�ļ�
#include "../../../Common/CommonHead/Common/GlobalDef.h"

//���ͷ�ļ�
#include "../../../Common/CommonHead/ModuleInterface/IUnknownEx.h"
#include "../../../Common/FKUIModule/Include/SkinControls.h"
#include "../../../Common/FKLobby/include/ComService.h"

//�궨��
#define LIT_FACE_WIDTH						16							//ͷ����
#define LIT_FACE_HEIGHT						16							//ͷ��߶�
#define NOR_FACE_WIDTH						32							//ͷ����
#define NOR_FACE_HEIGHT						32							//ͷ��߶�

//////////////////////////////////////////////////////////////////////////
//�����궨��

//��������
#ifndef GAMECLIENTLOBBY_CLASS
	#ifdef  GAMECLIENTLOBBY_EXPORTS
		#define GAMECLIENTLOBBY_CLASS _declspec(dllexport)
	#else
		#define GAMECLIENTLOBBY_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifdef _DEBUG
	#define GAMECLIENTLOBBY_EXPORTS_NAME	TEXT("GameClientLobby_D.dll")					//��� DLL ����
#else
	#define GAMECLIENTLOBBY_EXPORTS_NAME	TEXT("GameClientLobby.dll")					//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//��ϵ����
enum enCompanion
{
	enCompanion_UnKnow,				//δ֪��ϵ
	enCompanion_Friend,				//���ѹ�ϵ
	enCompanion_Detest,				//����ϵ
};

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//���ȶ���
#define CP_NOTE_LEN					128									//��ע����

//�û���¼�ṹ
struct tagCompanionItem
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szFace[128];						//�û�ͷ��
	enCompanion						Companion;							//�û���ϵ
	TCHAR							szNote[CP_NOTE_LEN];				//�û���ע
	TCHAR							szAccounts[NAME_LEN];				//�û��˺�
	TCHAR							szName[NAME_LEN];					//�û��ǳ�
};

//////////////////////////////////////////////////////////////////////////

#define VER_ICompanionManager INTERFACE_VERSION(2,1)
static const GUID IID_ICompanionManager={0x7bef6deb,0x5915,0x4021,0xa1,0x4f,0xa9,0xa3,0xec,0xad,0x74,0xf3};

//��ϵ����ӿ�
interface ICompanionManager : public IUnknownEx
{
	//����ӿ�
public:
	//������Ϣ
	virtual bool __cdecl LoadCompanion()=NULL;
	//�����ϵ
	virtual bool __cdecl SaveCompanion()=NULL;
	//��ʾ����
	virtual bool __cdecl ShowCompanionManager()=NULL;
	//��ϵ��Ŀ
	virtual INT_PTR __cdecl GetCompanionItemCount()=NULL;

	//���ܽӿ�
public:
	//�����ϵ
	virtual const tagCompanionItem * __cdecl InsertCompanionItem(tagCompanionItem & CompanionItem)=NULL;
	//���¹�ϵ
	virtual const tagCompanionItem * __cdecl UpdateCompanionItem(tagCompanionItem & CompanionItem)=NULL;
	//���¹�ϵ
	virtual const tagCompanionItem * __cdecl UpdateCompanionItem(DWORD dwUserID, LPCTSTR pszNote)=NULL;
	//���¹�ϵ
	virtual const tagCompanionItem * __cdecl UpdateCompanionItem(DWORD dwUserID, LPCTSTR pszAccounts, LPCTSTR pszFace)=NULL;
	//ɾ����ϵ
	virtual bool __cdecl DeleteCompanionItem(DWORD dwUserID)=NULL;

	//���ҽӿ�
public:
	//ö�ٹ�ϵ
	virtual const tagCompanionItem * __cdecl EmunCompanionItem(WORD wIndex)=NULL;
	//Ѱ�ҹ�ϵ
	virtual const tagCompanionItem * __cdecl SearchCompanionItem(DWORD dwUserID)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//��ϵ��������
class CCompanionManagerHelper : public CTempldateHelper<ICompanionManager>
{
	//��������
public:
	//���캯��
	CCompanionManagerHelper(void) : CTempldateHelper<ICompanionManager>(IID_ICompanionManager,
		VER_ICompanionManager,GAMECLIENTLOBBY_EXPORTS_NAME,TEXT("CreateCompanionManager")) { }
};

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//�ṹ�嶨��

//��������
struct tagDownLoadRequest
{
	bool							bDisplay;							//�Ƿ���ʾ
	TCHAR							szDescribe[128];					//������Ϣ
	TCHAR							szFileName[MAX_PATH];				//�����ļ�
	TCHAR							szLocalPath[MAX_PATH];				//����·��
	TCHAR							szDownLoadUrl[MAX_PATH];			//����·��
};

//////////////////////////////////////////////////////////////////////////

#define VER_IDownLoadService INTERFACE_VERSION(3,1)
static const GUID IID_IDownLoadService={0xe0b37184,0x6fbe,0x44ac,0xa4,0x83,0xab,0xa6,0x89,0x9b,0x33,0xd4};

//���ط���ӿ�
interface IDownLoadService : public IUnknownEx
{
	//��ȡ��Ŀ
	virtual INT_PTR __cdecl GetDownLoadMissionCount()=NULL;
	//���ýӿ�
	virtual bool __cdecl SetDownLoadServiceSink(IUnknownEx * pIUnknownEx)=NULL;
	//��������
	virtual DWORD __cdecl AddDownLoadRequest(DWORD dwDownLoadType, tagDownLoadRequest * pDownLoadRequest)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IDownLoadServiceSink INTERFACE_VERSION(1,1)
static const GUID IID_IDownLoadServiceSink={0xb339b588,0xd328,0x4482,0x96,0xde,0xc4,0x12,0xdd,0x78,0x81,0xec};

//���ط���ص��ӿ�
interface IDownLoadServiceSink : public IUnknownEx
{
};

//////////////////////////////////////////////////////////////////////////

//���ط�������
class CDownLoadServiceHelper : public CTempldateHelper<IDownLoadService>
{
	//��������
public:
	//���캯��
	CDownLoadServiceHelper(void) : CTempldateHelper<IDownLoadService>(IID_IDownLoadService,
		VER_IDownLoadService,GAMECLIENTLOBBY_EXPORTS_NAME,TEXT("CreateDownLoadService"))
	{
	}
};



#define VER_IUserFaceRes INTERFACE_VERSION(1,1)
static const GUID IID_IUserFaceRes={0xd9c22bd3,0x986b,0x4513,0x83,0x5d,0x52,0xb4,0x13,0x20,0x76,0x34};

//ͷ������ӿ�
interface IUserFaceRes : public IUnknownEx
{
	//��ȡ��Ŀ
	virtual WORD __cdecl GetFaceCount()=NULL;
	//��ȡ��С
	virtual const CSize & __cdecl GetSmallFaceSize()=NULL;
	//��ȡ��С
	virtual const CSize & __cdecl GetNormalFaceSize()=NULL;
	//�滭ͷ��
	virtual void __cdecl DrawFace(CDC * pDC, int nXPos, int nYPos, int xSize, int ySize, LPCTSTR pszFace, int cbGender)=NULL;
	//�滭ͷ��
	virtual void __cdecl DrawSmallFace(CDC * pDC, int nXPos, int nYPos, LPCTSTR pszFace, int cbGender)=NULL;
	//�滭ͷ��
	virtual void __cdecl DrawNormalFace(CDC * pDC, int nXPos, int nYPos, LPCTSTR pszFace, int cbGender)=NULL;
	//�滭ͷ��
//	virtual void __cdecl DrawNormal3DFace(CDC * pDC, int nXPos, int nYPos, WORD wFaceID)=NULL;
	
};

//////////////////////////////////////////////////////////////////////////

#define VER_IFaceControl INTERFACE_VERSION(1,1)
static const GUID IID_IFaceControl={0xd2c483c,0xbdfa,0x46a6,0x8e,0x60,0xf8,0xc8,0x77,0xed,0x59,0xc3};

//ͷ��ؼ��ӿ�
interface IFaceControl : public IUnknownEx
{
	//��Ϣ�ӿ�
public:
	//ͷ����
	virtual int __cdecl GetFaceWidth()=NULL;
	//ͷ��߶�
	virtual int __cdecl GetFaceHeight()=NULL;
	//ͷ����Ŀ
	virtual WORD __cdecl GetFaceItemCount()=NULL;

	//�滭�ӿ�
public:
	//�滭ͷ��
	virtual void __cdecl DrawFaceItem(CDC * pDC, int nXPos, int nYPos, WORD wFaceID)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//ͷ�����������
class CUserFaceResHelper : public CTempldateHelper<IUserFaceRes>
{
	//��������
public:
	//���캯��
	CUserFaceResHelper(void) : CTempldateHelper<IUserFaceRes>(IID_IUserFaceRes,
		VER_IUserFaceRes,GAMECLIENTLOBBY_EXPORTS_NAME,TEXT("CreateUserFaceRes"))
	{
	}
};

//////////////////////////////////////////////////////////////////////////

//ͷ��ؼ�������
class CFaceControlHelper : public CTempldateHelper<IFaceControl>
{
	//��������
public:
	//���캯��
	CFaceControlHelper(void) : CTempldateHelper<IFaceControl>(IID_IFaceControl,
		VER_IFaceControl,GAMECLIENTLOBBY_EXPORTS_NAME,TEXT("CreateFaceControl"))
	{
	}
};


//////////////////////////////////////////////////////////////////////////
//�ṹ�嶨��

//��Ϣ����
struct tagMessageOption
{
	COLORREF							crName;							//������ɫ
	COLORREF							crSystemHead;					//ϵͳ��ɫ
	COLORREF							crSystemString;					//ϵͳ��ɫ
	TCHAR								szDefaultFont[32];				//Ĭ������
};

//��Ϣ��С
#define MS_NORMAL						0								//�����С
#define MS_MIDDLING						1								//�еȴ�С
#define MS_LARGE						2								//�������

//////////////////////////////////////////////////////////////////////////

#define VER_IMessageProxy INTERFACE_VERSION(1,1)
static const GUID IID_IMessageProxy={0xe8bd2387,0x9c2a,0x4346,0xb3,0x2c,0x17,0xb7,0xb3,0x51,0x9a,0x2f};

//��Ϣ����ӿ�
interface IMessageProxy : public IUnknownEx
{
	//���ýӿ�
public:
	//���þ��
	virtual bool __cdecl SetRichEditHwnd(CSkinRichEdit * pSkinRichEdit)=NULL;
	//������ɫ
	virtual bool __cdecl SetMessageOption(tagMessageOption & MessageOption)=NULL;

	//���ƽӿ�
public:
	//�����Ļ
	virtual bool __cdecl CleanScreen()=NULL;
	//���ù���
	virtual bool __cdecl SetAutoScroll(bool bScroll)=NULL;
	//������Ϣ
	virtual bool __cdecl LoadMessage(LPCTSTR pszFileName)=NULL;
	//������Ϣ
	virtual bool __cdecl SaveMessage(LPCTSTR pszFileName)=NULL;

	//��Ϣ�ӿ�
public:
	//����ͼƬ
	virtual bool __cdecl InsertImage(CBitmap * pBitmap)=NULL;
	//����ʱ��
	virtual bool __cdecl InsertSystemTime(COLORREF crColor)=NULL;
	//�û�����
	virtual bool __cdecl InsertUserName(LPCTSTR pszUserName)=NULL;
	//ϵͳ��Ϣ
	virtual bool __cdecl InsertSystemString(LPCTSTR pszString, UINT uSize)=NULL;
	//��ɫ��Ϣ
	virtual bool __cdecl InsertShtickString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine)=NULL;
	//��ͨ��Ϣ
	virtual bool __cdecl InsertGeneralString(LPCTSTR pszString, COLORREF crColor, UINT uSize, bool bNewLine)=NULL;
	//�û�����
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszString, COLORREF crColor, UINT uSize)=NULL;
	//�û�����
	virtual bool __cdecl InsertUserChat(LPCTSTR pszSendName, LPCTSTR pszRecvName, LPCTSTR pszString, COLORREF crColor, UINT uSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//��Ϣ��������
class CMessageProxyHelper : public CTempldateHelper<IMessageProxy>
{
	//��������
public:
	//���캯��
	CMessageProxyHelper(void) : CTempldateHelper<IMessageProxy>(IID_IMessageProxy,
		VER_IMessageProxy,GAMECLIENTLOBBY_EXPORTS_NAME,TEXT("CreateMessageProxy")) { }
};

//////////////////////////////////////////////////////////////////////////
//������ͷ�ļ�

#include "Expression.h"



//�ں���������
#define IPC_MIAN_IPC_KERNEL					0							//�ں�����

//�ں���������
#define IPC_SUB_IPC_CLIENT_CONNECT			1							//����֪ͨ
#define IPC_SUB_IPC_SERVER_ACCEPT			2							//Ӧ��֪ͨ
#define IPC_SUB_IPC_CLIENT_CLOSE			3							//�ر�֪ͨ

//////////////////////////////////////////////////////////////////////////

#define VER_IChannelMessageSink INTERFACE_VERSION(1,1)
static const GUID IID_IChannelMessageSink={0x7a672dca,0x141e,0x45a6,0x95,0xc4,0x43,0x79,0x23,0x7f,0x29,0x73};

//�ŵ���Ϣ���ӽӿ�
interface IChannelMessageSink : public IUnknownEx
{
	//�ŵ����ݴ���
	virtual bool __cdecl OnChannelMessage(const IPC_Head * pHead, const void * pIPCBuffer, WORD wDataSize, HWND hWndSend)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IChannelEventSink INTERFACE_VERSION(1,1)
static const GUID IID_IChannelEventSink={0xb87334af,0x3f52,0x4e51,0x9d,0xc6,0x51,0x93,0x24,0x66,0x9e,0x2e};

//�ŵ��¼����ӽӿ�
interface IChannelEventSink : public IUnknownEx
{
	//������Ϣ
	virtual bool __cdecl OnChannelConnect(HWND hWndSend)=NULL;
	//Ӧ����Ϣ
	virtual bool __cdecl OnChannelAccept(HWND hWndSend)=NULL;
	//�Ͽ���Ϣ
	virtual bool __cdecl OnChannelClose(HWND hWndSend, bool bInitiative)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IChannelService INTERFACE_VERSION(1,1)
static const GUID IID_IChannelService={0x4d0151e9,0xbd65,0x4a4c,0xa6,0xf8,0x25,0x74,0x68,0x17,0x6e,0x74};

//�ŵ�ģ��ӿ�
interface IChannelService : public IUnknownEx
{
	//�����¼�
	virtual bool __cdecl SetChannelEventSink(IUnknownEx * pIUnknownEx)=NULL;
	//������Ϣ
	virtual bool __cdecl SetChannelMessageSink(IUnknownEx * pIUnknownEx)=NULL;
	//��ȡ�¼�
	virtual void * __cdecl GetChannelEventSink(const IID & Guid, DWORD dwQueryVer)=NULL;
	//��ȡ��Ϣ
	virtual void * __cdecl GetChannelMessageSink(const IID & Guid, DWORD dwQueryVer)=NULL;
	//�Ƿ�����
	virtual bool __cdecl IsConnect(bool bEfficacy)=NULL;
	//��������
	virtual bool __cdecl CreateChannel(HWND hWndServer)=NULL;
	//�رպ���
	virtual bool __cdecl CloseChannel(bool bNotifyServer, bool bNotifySink)=NULL;
	//��������
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//��������
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, void * pBuffer, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//�ŵ���������
class CChannelServiceHelper : public CTempldateHelper<IChannelService>
{
	//��������
public:
	//���캯��
	CChannelServiceHelper(void) : CTempldateHelper<IChannelService>(IID_IChannelService,
		VER_IChannelService,GAMECLIENTLOBBY_EXPORTS_NAME,TEXT("CreateChannelService"))
	{
	}
};

//////////////////////////////////////////////////////////////////////////

//��������ļ�
#include "IPCControls.h"

//////////////////////////////////////////////////////////////////////////



#define VER_IGameRankManager INTERFACE_VERSION(1,1)
static const GUID IID_IGameRankManager={0x850e5617,0x4eab,0x4f2e,0xae,0x11,0xef,0xa3,0x22,0x65,0xe0,0xc2};

//��Ϸ�ȼ��ӿ�
interface IGameRankManager : public IUnknownEx
{
	//��ȡ�ȼ�
	virtual LPCTSTR __cdecl GetGameRank(WORD wKindID, LONG lScore, WORD wGameGenre)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//��ϵ��������
class CGameRankManagerHelper : public CTempldateHelper<IGameRankManager>
{
	//��������
public:
	//���캯��
	CGameRankManagerHelper(void) : CTempldateHelper<IGameRankManager>(IID_IGameRankManager,
		VER_IGameRankManager,GAMECLIENTLOBBY_EXPORTS_NAME,TEXT("CreateGameRankManager")) { }
};

static CString FindExtension(const CString& name)
{
	int len = name.GetLength();
	int i;
	for (i = len-1; i >= 0; i--)
	{
		if (name[i] == '.')
		{
			return name.Mid(i+1);
		}
	}
	return CString("");
}

#endif