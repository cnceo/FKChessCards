#include "Include/stdafx.h"
#include "../Include/SkinRichEdit.h"

//////////////////////////////////////////////////////////////////////////

#define ID_RICH_COPY					WM_USER+20
#define ID_RICH_SELECT_ALL				WM_USER+21
#define ID_RICH_CLEAR					WM_USER+22
#define ID_RICH_CLEAR_ALL				WM_USER+24
#define ID_RICH_SAVE					WM_USER+25

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinRichEdit, CRichEditCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONDOWN()
	ON_NOTIFY_REFLECT(EN_LINK, OnEnLink)
	ON_COMMAND(ID_RICH_COPY, OnCopy)
	ON_COMMAND(ID_RICH_CLEAR, OnClear)
	ON_COMMAND(ID_RICH_SAVE, OnSave)
	ON_COMMAND(ID_RICH_CLEAR_ALL, OnClearAll)
	ON_COMMAND(ID_RICH_SELECT_ALL, OnSelectAll)
	ON_NOTIFY_REFLECT(EN_LINK, OnEnLink)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CImageDataObject::CImageDataObject()
{
	m_ulRef=0;
	memset(&m_Stgmed,0,sizeof(m_Stgmed));
	memset(&m_Format,0,sizeof(m_Format));
}

//��������
CImageDataObject::~CImageDataObject()
{
	ReleaseStgMedium(&m_Stgmed);
}

//����ͼƬ
void CImageDataObject::SetBitmap(HBITMAP hBitmap)
{
	STGMEDIUM Stgm;
	FORMATETC Format;
	Stgm.tymed=TYMED_GDI;
	Stgm.hBitmap=hBitmap;
	Stgm.pUnkForRelease=NULL;
	Format.cfFormat=CF_BITMAP;
	Format.ptd=NULL;
	Format.dwAspect=DVASPECT_CONTENT;
	Format.lindex=-1;
	Format.tymed=TYMED_GDI;
	SetData(&Format,&Stgm,TRUE);

	return;
}

//��ȡ����
IOleObject * CImageDataObject::GetOleObject(IOleClientSite * pOleClientSite, IStorage * pStorage)
{
	IOleObject * pOleObject=NULL;
	::OleCreateStaticFromData(this,IID_IOleObject,OLERENDER_FORMAT,&m_Format,pOleClientSite,pStorage,(void **)&pOleObject);
	return pOleObject;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinRichEdit::CSkinRichEdit()
{
	m_bScroll=true;
	m_pIRichEditOle=NULL;
	m_strFontName=TEXT("����");
}

//��������
CSkinRichEdit::~CSkinRichEdit()
{
	if (m_pIRichEditOle!=NULL) m_pIRichEditOle->Release();
}

//�ؼ���
void CSkinRichEdit::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//��ʼ���ؼ�
	SetEventMask(ENM_LINK);
	SetAutoURLDetect(TRUE);
	SetOptions(ECOOP_OR,ECO_NOHIDESEL|ECO_AUTOWORDSELECTION);

	//��ȡ�ӿ�
	m_pIRichEditOle=GetIRichEditOle();

	return;
}

//������Ϣ
int CSkinRichEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//��ʼ���ؼ�
	SetEventMask(ENM_LINK);
	SetAutoURLDetect(TRUE);
	SetOptions(ECOOP_OR,ECO_NOHIDESEL);

	//��ȡ�ӿ�
	m_pIRichEditOle=GetIRichEditOle();

	return 0;
}

//�����Ļ
void CSkinRichEdit::CleanScreen()
{
	//�����Ļ
	SetSel(0,GetTextLength());
	ReplaceSel(TEXT(""));

	return;
}

//������Ϣ
bool CSkinRichEdit::LoadMessage(LPCTSTR pszFileName)
{
	//Ч�����
	ASSERT(pszFileName!=NULL);
	if (pszFileName==NULL) return false;

	//���ļ�
	CFile File;
	if (File.Open(pszFileName,CFile::modeRead,NULL)==FALSE) return false;

	//��ȡ�ļ�
	EDITSTREAM EditStream;
	EditStream.dwCookie=(DWORD)(&File);
	EditStream.pfnCallback=LoadCallBack;
    StreamIn(SF_RTF,EditStream);

	return true;
}

//������Ϣ
bool CSkinRichEdit::SaveMessage(LPCTSTR pszFileName)
{
	//Ч�����
	ASSERT(pszFileName!=NULL);
	if (pszFileName==NULL) return false;

	//���ļ�
	CFile File;
	if (File.Open(pszFileName,CFile::modeWrite|CFile::modeCreate,NULL)==FALSE) return false;

	//д���ļ�
	EDITSTREAM EditStream;
	EditStream.dwCookie=(DWORD)(&File);
	EditStream.pfnCallback=SaveCallBack;
    StreamOut(SF_RTF,EditStream);

	return true;
}

//����ͼƬ
bool CSkinRichEdit::InsertImage(CBitmap * pBitmap)
{
	//Ч��״̬
	if (pBitmap==NULL) return false;
	if (m_pIRichEditOle==NULL) return false;

	//������Ϣ
	CHARRANGE CharRange;
	GetSel(CharRange);

	//���ò����
	SetSel(GetWindowTextLength(),GetWindowTextLength());

	//����λͼ
	IStorage * pStorage=NULL;
	IOleObject * pOleObject=NULL;
	LPDATAOBJECT lpDataObject=NULL;
	IOleClientSite * pOleClientSite=NULL;
	CImageDataObject * pImageObject=NULL;
	try
	{
		//��ȡ��Ϣ
		BITMAP BitmapInfo;
		pBitmap->GetBitmap(&BitmapInfo);

		//��ȡ DC
		CWnd * pMainWnd=AfxGetMainWnd();
		CDC * pDC=pMainWnd->GetDC();

		//����λͼ
		CBitmap DestBitmap;
		CDC DCSource,DCDest;
		DCDest.CreateCompatibleDC(pDC);
		DCSource.CreateCompatibleDC(pDC);
		DestBitmap.CreateCompatibleBitmap(pDC,BitmapInfo.bmWidth,BitmapInfo.bmHeight);
		DCSource.SelectObject(pBitmap->GetSafeHandle());
		DCDest.SelectObject(&DestBitmap);
		DCDest.BitBlt(0,0,BitmapInfo.bmWidth,BitmapInfo.bmHeight,&DCSource,0,0,SRCCOPY);

		//������Դ
		DCDest.DeleteDC();
		DCSource.DeleteDC();
		pMainWnd->ReleaseDC(pDC);

		//�����¶���
		pImageObject=new CImageDataObject;
		pImageObject->QueryInterface(IID_IDataObject,(void * *)&lpDataObject);
		pImageObject->SetBitmap((HBITMAP)DestBitmap.Detach());
		m_pIRichEditOle->GetClientSite(&pOleClientSite);

		LPLOCKBYTES lpLockBytes=NULL;
		CreateILockBytesOnHGlobal(NULL,TRUE,&lpLockBytes);
		StgCreateDocfileOnILockBytes(lpLockBytes,STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE,0,&pStorage);
		pOleObject=pImageObject->GetOleObject(pOleClientSite,pStorage);
		OleSetContainedObject(pOleObject,TRUE);

		REOBJECT ReObject;
		ZeroMemory(&ReObject,sizeof(ReObject));
		ReObject.cbStruct=sizeof(ReObject);

		CLSID ClsID;
		pOleObject->GetUserClassID(&ClsID);
		ReObject.clsid=ClsID;
		ReObject.cp=REO_CP_SELECTION;
		ReObject.dvaspect=DVASPECT_CONTENT;
		ReObject.poleobj=pOleObject;
		ReObject.polesite=pOleClientSite;
		ReObject.pstg=pStorage;
		m_pIRichEditOle->InsertObject(&ReObject);
	}
	catch (...) { delete pImageObject; }
	if (pOleObject!=NULL) pOleObject->Release();
	if (pOleClientSite!=NULL) pOleClientSite->Release();
	if (pStorage!=NULL) pStorage->Release();
	if (lpDataObject!=NULL) lpDataObject->Release();

	//�ָ���Ϣ
	if (m_bScroll==false) SetSel(CharRange);

	return true;
}

//�����ַ�
bool CSkinRichEdit::InsertString(LPCTSTR pszString, COLORREF crColor, bool bLink, WORD wSize, LPCTSTR pszFont)
{
	//������Ϣ
	CHARRANGE CharRange;
	GetSel(CharRange);

	//���ø�ʽ
	CHARFORMAT2 CharFormat;
	memset(&CharFormat,0,sizeof(CharFormat));
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_COLOR;
	CharFormat.crTextColor=crColor;
	if (bLink==true)
	{
		CharFormat.dwMask|=CFM_LINK;
		CharFormat.dwEffects=CFE_LINK;
	}
	
	//��������
	LONG lTextLength=GetTextLengthEx(GTL_NUMCHARS);
	bool bResume=(CharRange.cpMax!=CharRange.cpMin);//||(lTextLength!=CharRange.cpMin));
	
	//������Ϣ
	SetSel(-1,-1);
	SetSelectionCharFormat(CharFormat);
	ReplaceSel(pszString);

	//�ָ���Ϣ
	if (bResume==true) SetSel(CharRange);

	return true;
}

//���ù��
BOOL CSkinRichEdit::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	if (nHitTest==HTCAPTION)
	{
		SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
		return TRUE;
	}
	return __super::OnSetCursor(pWnd,nHitTest,message);
}

//���ػص�
DWORD CALLBACK CSkinRichEdit::LoadCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	*pcb=((CFile *)dwCookie)->Read(pbBuff,cb);
	return 0;
}

//����ص�
DWORD CALLBACK CSkinRichEdit::SaveCallBack(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	*pcb=cb;
	((CFile *)dwCookie)->Write(pbBuff,cb);
	return 0;
}

//������Ϣ
void CSkinRichEdit::OnDestroy()
{
	__super::OnDestroy();

	//�ͷŽӿ�
	if (m_pIRichEditOle!=NULL)
	{
		m_pIRichEditOle->Release();
		m_pIRichEditOle=NULL;
	}

	return;
}

//�Ҽ���Ϣ
void CSkinRichEdit::OnRButtonDown(UINT nFlags, CPoint Point)
{
	SetFocus();
	CMenu PopMenu;

	//�����˵�
	PopMenu.CreatePopupMenu();
	PopMenu.AppendMenu(0,ID_RICH_COPY,TEXT("����(&C) \tCtrl+C"));
	PopMenu.AppendMenu(0,ID_RICH_SELECT_ALL,TEXT("ȫѡ(&A) \tCtrl+A"));
	PopMenu.AppendMenu(0,ID_RICH_CLEAR,TEXT("ɾ��(&D)"));
	PopMenu.AppendMenu(0,ID_RICH_CLEAR_ALL,TEXT("�����Ļ"));
	PopMenu.AppendMenu(0,MF_SEPARATOR);
	PopMenu.AppendMenu(0,ID_RICH_SAVE,TEXT("������Ϣ..."));

	//���ò˵�
	UINT nSel=((GetSelectionType()!=SEL_EMPTY)?0:MF_GRAYED);
	PopMenu.EnableMenuItem(ID_RICH_COPY,MF_BYCOMMAND|nSel);
	PopMenu.EnableMenuItem(ID_RICH_CLEAR,MF_BYCOMMAND|nSel);
	PopMenu.EnableMenuItem(ID_RICH_SELECT_ALL,MF_BYCOMMAND);

	//��ʾ�˵�
	ClientToScreen(&Point);
	PopMenu.TrackPopupMenu(TPM_RIGHTBUTTON,Point.x,Point.y,this);
	PopMenu.DestroyMenu();

	return;
}

//������Ϣ
afx_msg void CSkinRichEdit::OnSave()
{
	CFileDialog FileDlg(FALSE,"RTF",0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,"�ı��ļ�(*.RTF)|*.RTF||",this);
	if (FileDlg.DoModal()==IDOK)
	{
		CFile File;
		if (File.Open(FileDlg.GetPathName(),CFile::modeCreate|CFile::modeReadWrite)==TRUE)
		{
            EDITSTREAM EditStream;
			EditStream.dwCookie=(DWORD)&File;
			EditStream.pfnCallback=SaveCallBack; 
			StreamOut(SF_RTF,EditStream);
			File.Close();
		}
		else AfxMessageBox(TEXT("Ŀ��·������д����Ϣ����ʧ�ܣ�"));
	}
	return;
}

//��������
void CSkinRichEdit::OnEnLink(NMHDR * pNMHDR, LRESULT * pResult)
{
	ENLINK * pEnLink=reinterpret_cast<ENLINK *>(pNMHDR);

	//������Ϣ
	if ((pEnLink->msg==WM_LBUTTONDOWN)||(pEnLink->msg==WM_LBUTTONDBLCLK))
	{
		CString strBuffer;
		strBuffer.GetBufferSetLength((pEnLink->chrg.cpMax-pEnLink->chrg.cpMin)*2);
		strBuffer.ReleaseBuffer();
		GetTextRange(pEnLink->chrg.cpMin,pEnLink->chrg.cpMax,strBuffer);

		//��ҳ����
		LPCTSTR pszHttp[]=
		{
			TEXT("http://www.fox2046.cn"),
			TEXT("http://www.fox2046.net"),
			TEXT("http://www.fox2046.com"),
			TEXT("http://service.fox2046.cn"),
			TEXT("http://service.fox2046.net"),
			TEXT("http://service.fox2046.com"),
		};
		for (int i=0;i<CountArray(pszHttp);i++)
		{
			if (strBuffer.GetLength()>=lstrlen(pszHttp[i]))
			{
				if (memcmp((LPCTSTR)strBuffer,pszHttp[i],lstrlen(pszHttp[i]))==0)
				{
					ShellExecute(NULL,TEXT("open"),strBuffer,NULL,NULL,SW_SHOWDEFAULT);
					return;
				}
			}
		}
	}

	*pResult=0;
}

//////////////////////////////////////////////////////////////////////////
