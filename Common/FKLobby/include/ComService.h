#ifndef COM_SERVICE_HEAD_FILE
#define COM_SERVICE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ϵͳͷ�ļ�
#include "AfxHtml.h"

//ƽ̨ͷ�ļ�
#include "..\..\CommonHead\Template\Template.h"
#include "..\..\CommonHead\Common\GlobalDef.h"
//ϵͳͷ�ļ�
#include <GdiPlus.h>
#include <AtlImage.h>
using namespace Gdiplus;

//ƽ̨ͷ�ļ�
#include "..\..\CommonHead\ModuleInterface\IUnknownEx.h"
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//�����궨��

//��������
#define PROXY_NONE					0									//û�д���
#define PROXY_SOCKS4				1									//SOCKS4 ����
#define PROXY_SOCKS5				2									//SOCKS5 ����
#define PROXY_HTTP					3									//HTTP ����


//��������
#ifndef FKLOBBY_DLL_CLASS
	#ifdef  FKLOBBY_EXPORTS
		#define FKLOBBY_DLL_CLASS _declspec(dllexport)
	#else
		#define FKLOBBY_DLL_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifdef _DEBUG
	#define FKLOBBY_DLL_NAME	TEXT("FKLobby_D.dll")			//��� DLL ����
#else
	#define FKLOBBY_DLL_NAME	TEXT("FKLobby.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////

//������ͷ�ļ�
#include "Encrypt.h"
#include "WebBrowser.h"
#include "HtmlBrowser.h"
#include "WinFileInfo.h"
#include "DataStorage.h"
#include "ServiceThread.h"


//����״̬����
enum enSocketState
{
	SocketState_NoConnect,			//û������
	SocketState_Connecting,			//��������
	SocketState_Connected,			//�ɹ�����
};

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//������Ϣ�ṹ
struct tagProxyInfo
{
	BYTE							cbProxyType;						//��������
	WORD							wProxyPort;							//����˿�
	CString							strProxyName;						//�����û���
	CString							strProxyPass;						//��������
	CString							strProxyServer;						//�����ַ
	CString							strHttpDomain;						//��������
};

//////////////////////////////////////////////////////////////////////////

#define VER_IClientSocket INTERFACE_VERSION(1,1)
static const GUID IID_IClientSocket={0x8b0e3918,0x5139,0x48da,0xbf,0x40,0xba,0x95,0xad,0xce,0x59,0x64};

//��������ӿ�
interface IClientSocket : public IUnknownEx
{
	//ȡ������
	virtual void __cdecl CancelProxyServer()=NULL;
	//���������
	virtual void __cdecl SetProxyServer(tagProxyInfo & ProxyInfo)=NULL;
	//���ýӿ�
	virtual bool __cdecl SetSocketSink(IUnknownEx * pIUnknownEx)=NULL;
	//��ȡ�ӿ�
	virtual void  * __cdecl GetSocketSink(const IID & Guid, DWORD dwQueryVer)=NULL;
	//��ȡ���ͼ��
	virtual DWORD __cdecl GetLastSendTick()=NULL;
	//��ȡ���ռ��
	virtual DWORD __cdecl GetLastRecvTick()=NULL;
	//��ȡ������Ŀ
	virtual DWORD __cdecl GetSendPacketCount()=NULL;
	//��ȡ������Ŀ
	virtual DWORD __cdecl GetRecvPacketCount()=NULL;
	//��ȡ״̬
	virtual enSocketState __cdecl GetConnectState()=NULL;
	//���ӷ�����
	virtual bool __cdecl Connect(DWORD dwServerIP, WORD wPort)=NULL;
	//���ӷ�����
	virtual bool __cdecl Connect(const LPCTSTR szServerIP, WORD wPort)=NULL;
	//���ͺ���
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID)=NULL;
	//���ͺ���
	virtual bool __cdecl SendData(WORD wMainCmdID, WORD wSubCmdID, void * pData, WORD wDataSize)=NULL;
	//�ر�����
	virtual bool __cdecl CloseSocket(bool bNotify)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#define VER_IClientSocketSink INTERFACE_VERSION(1,1)
static const GUID IID_IClientSocketSink={0x55fcf8aa,0x526a,0x44c2,0x9a,0xd1,0x4,0xe,0x7,0xd6,0x8a,0x2a};

//���繳�ӽӿ�
interface IClientSocketSink : public IUnknownEx
{
	//����������Ϣ
	virtual bool __cdecl OnSocketConnect(int iErrorCode, LPCTSTR pszErrorDesc, IClientSocket * pIClientSocke)=NULL;
	//�����ȡ��Ϣ
	virtual bool __cdecl OnSocketRead(CMD_Command Command, void * pBuffer, WORD wDataSize, IClientSocket * pIClientSocke)=NULL;
	//����ر���Ϣ
	virtual bool __cdecl OnSocketClose(IClientSocket * pIClientSocke, bool bCloseByServer)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//�������������
class CClientSocketHelper : public CTempldateHelper<IClientSocket>
{
	//��������
public:
	//���캯��
	CClientSocketHelper(void) : CTempldateHelper<IClientSocket>(IID_IClientSocket,
		VER_IClientSocket,FKLOBBY_DLL_NAME,TEXT("CreateClientSocket"))
	{
	}
};




#endif