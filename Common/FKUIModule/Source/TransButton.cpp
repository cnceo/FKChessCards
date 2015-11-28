#include "Include/stdafx.h"
#include "../Resource.h"
#include "../Include/TransButton.h"

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(TransButton, CButton)

BEGIN_MESSAGE_MAP(TransButton, CButton)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//�궨��
#define DEF_TEXT_COLOR				RGB(10,10,10)							//Ĭ����ɫ
#define DEF_TEXT_COLOR_EX			RGB(10,10,10)							//Ĭ����ɫ

//��̬����
TransButtonAttribute				TransButton::m_SkinAttribute;			//��ť����

//////////////////////////////////////////////////////////////////////////

//���캯��
TransButtonAttribute::TransButtonAttribute() : CSkinAttribute(TEXT("TransButton"))
{
	//��ʼ������
	m_crTextColor=DEF_TEXT_COLOR;

	//ע������
	RegisterAttribute(SKIN_BUTTON_CRTEXTCOLOR,&m_crTextColor,false);
	RegisterAttribute(SKIN_BUTTON_STRIMAGEPATH,&m_strImagePath,false);

	return;
}

//��������
TransButtonAttribute::~TransButtonAttribute() 
{
}

//��������
bool __cdecl TransButtonAttribute::LoadSkinOption()
{
	//��ȡ������ɫ
	m_crTextColor=ReadColor(SKIN_BUTTON_CRTEXTCOLOR,DEF_TEXT_COLOR);
	m_ImageBack.SetLoadInfo(IDB_SKIN_BUTTON_BACK,GetModuleHandle(FKUIMODULE_DLL_NAME));

	return true;
}

//��������
bool __cdecl TransButtonAttribute::SaveSkinOption()
{
	WriteColor(SKIN_BUTTON_CRTEXTCOLOR,m_crTextColor);
	WriteString(SKIN_BUTTON_STRIMAGEPATH,m_strImagePath);
	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
TransButton::TransButton()
{
	m_hIcon=NULL;
	m_bExpand=true;
	m_bHovering=false;
	m_pIMouseEvent=NULL;
	m_crTextColor=m_SkinAttribute.m_crTextColor;
}

//��������
TransButton::~TransButton()
{
}

//���󸽼ӵ����д���
void TransButton::PreSubclassWindow()
{
	__super::PreSubclassWindow();
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SendMessage(WM_SETFONT,(WPARAM)m_SkinAttribute.GetDefaultFont(),TRUE);
	return;
}

//������Ϣ
int TransButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SendMessage(WM_SETFONT,(WPARAM)m_SkinAttribute.GetDefaultFont(),TRUE);
	return 0;
}

//������ɫ
bool TransButton::SetTextColor(COLORREF crTextColor)
{
	m_crTextColor=crTextColor;
	if (GetSafeHwnd()) Invalidate(FALSE);
	return true;
}

//����ͼ��
bool TransButton::SetButtonIcon(HICON hIcon)
{
	m_hIcon=hIcon;
	if (GetSafeHwnd()) Invalidate(FALSE);
	return true;
}

//����λͼ
bool TransButton::SetButtonImage(LPCTSTR pszFileName, bool bExpandImage)
{
	//Ч�����
	ASSERT(pszFileName);
	if (pszFileName==NULL) return false;

	//����λͼ
	m_bExpand=bExpandImage;
	m_ImageBack.SetLoadInfo(pszFileName);

	//����λ��
	if (m_bExpand==false) FixButtonSize();
	if (GetSafeHwnd()) Invalidate(FALSE);

	return true;
}

//����λͼ
bool TransButton::SetButtonImage(UINT uBitmapID, HINSTANCE hInstance, bool bExpandImage)
{
	//����λͼ
	ASSERT(uBitmapID!=0);
	if (uBitmapID==0) return false;
	m_bExpand=bExpandImage;
	m_ImageBack.SetLoadInfo(uBitmapID,hInstance);

	//����λ��
	if (m_bExpand==false) FixButtonSize();
	if (GetSafeHwnd()) Invalidate(FALSE);

	return true;
}

//��ȡλͼ����
bool TransButton::GetButtonLoadInfo(tagImageLoadInfo & LoadInfo)
{
	if (m_ImageBack.IsSetLoadInfo()) return m_ImageBack.GetLoadInfo(LoadInfo);
	return m_SkinAttribute.m_ImageBack.GetLoadInfo(LoadInfo);
}

//Ĭ�ϰ�ť
void TransButton::SetDefaultButton(bool bResetIcon, bool bResetColor, bool bResetImage)
{
	//�ָ�Ĭ��
	if (bResetImage) 
	{
		m_bExpand=true;
		m_ImageBack.RemoveLoadInfo();
	}
	if (bResetIcon)	m_hIcon=NULL;
	if (bResetColor) m_crTextColor=m_SkinAttribute.m_crTextColor;

	//ˢ�½���
	if (GetSafeHwnd()) Invalidate(FALSE);

	return;
}

//����λ��
bool TransButton::FixButtonSize()
{
	if (m_ImageBack.IsSetLoadInfo()&&GetSafeHwnd())
	{
		CImageHandle ImageHandle(&m_ImageBack);
		SetWindowPos(NULL,0,0,ImageHandle->GetWidth()/5,ImageHandle->GetHeight(),SWP_NOMOVE);
		return true;
	}
	return false;
}

//����ƶ���Ϣ
void TransButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bHovering==false)
	{
		//ע����Ϣ
		m_bHovering=true;
		Invalidate(FALSE);
		TRACKMOUSEEVENT TrackMouseEvent;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.hwndTrack=GetSafeHwnd();
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		_TrackMouseEvent(&TrackMouseEvent);
	}
	__super::OnMouseMove(nFlags, point);
}

//����뿪��Ϣ
LRESULT TransButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	m_bHovering=false;
	Invalidate(FALSE);
	return 0;
}

//��������
BOOL TransButton::OnEraseBkgnd(CDC * pDC)
{
	Invalidate(FALSE);
	UpdateWindow();
	return TRUE;
}

//����滭����
void TransButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//�������
	CRect ClientRect;
	GetClientRect(&ClientRect);
	bool bDisable=((lpDrawItemStruct->itemState&ODS_DISABLED)!=0);
	bool bButtonDown=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0);

	//���� DC
	HDC hDC=lpDrawItemStruct->hDC;
	///////////////////////////////////////////////////////////////////////

	CDC* pDC=CDC::FromHandle(lpDrawItemStruct->hDC);

	//Ŀǰm_ImageBack��û���õ��Ĳ���
	//��SetupRgn����GetButtonLoadInfo(buttonInfo)�õ��˰�ť��λͼ��Ϣ
	SetupRgn(pDC,m_ImageBack,RGB(255,0,255));

	//CBitmap cBitmap;
	//cBitmap.LoadBitmap(IDB_SKIN_MASK);
	//��IDB_SKIN_MASKΪ�ֶ��ü���һ����ť��1/5.Ŀǰ��ť͸��û�����⡣
	//���ܽ���CPP�е�m_ImageBack�ü���1/5����͸����ť�ɹ���

	///////////////////////////////////////////////////////////////////////
	//��ȡ����
	CString strText;
	GetWindowText(strText);

	//���ر���ͼ
	CImageHandle ImageHandle;
	if (m_ImageBack.IsSetLoadInfo()) ImageHandle.AttachResource(&m_ImageBack);
	else ImageHandle.AttachResource(&m_SkinAttribute.m_ImageBack);
	if (ImageHandle.IsResourceValid())
	{
		//����λͼλ��
		int iPartWidth=ImageHandle->GetWidth()/5,iDrawPos=0;
		if (bDisable) iDrawPos=iPartWidth*4;
		else if (bButtonDown) iDrawPos=iPartWidth;
		else if (m_bHovering) iDrawPos=iPartWidth*3;
		else if (lpDrawItemStruct->itemState&ODS_FOCUS)	iDrawPos=iPartWidth*2;

		//�滭����ͼ
		if (m_bExpand==false) 
		{
			//ImageHandle->BitBlt(hDC,0,0,ClientRect.Width(),ClientRect.Height(),iDrawPos,0,SRCCOPY);
			ImageHandle->AlphaDrawImage(pDC,0,0,ClientRect.Width(),ClientRect.Height(),iDrawPos,0,RGB(255,0,255));
		}
		else 
			ImageHandle->StretchBlt(hDC,0,0,ClientRect.Width(),ClientRect.Height(),iDrawPos,0,iPartWidth,ImageHandle->GetHeight(),SRCCOPY);
	}
	else
	{
		//�滭Ĭ�Ͻ���
		CDC * pDC=CDC::FromHandle(hDC);
		pDC->FillSolidRect(&ClientRect,GetSysColor(COLOR_BTNFACE));
		if (bButtonDown) pDC->Draw3dRect(&ClientRect,GetSysColor(COLOR_WINDOWFRAME),GetSysColor(COLOR_3DHILIGHT));
		else pDC->Draw3dRect(&ClientRect,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_WINDOWFRAME));
	}

	//�滭ͼ��
	if (bButtonDown) ClientRect.top+=2;
	if (m_hIcon)
	{
		DrawIconEx(hDC,ClientRect.left+6,ClientRect.top+(ClientRect.Height()-16)/2+1,m_hIcon,16,16,0,NULL,DI_NORMAL);
		ClientRect.left+=22;
	}

	//�滭����
	ClientRect.top+=1;
	::SetBkMode(hDC,TRANSPARENT);
	if (bDisable) ::SetTextColor(hDC,GetSysColor(COLOR_GRAYTEXT));
	else ::SetTextColor(hDC,m_crTextColor);
	DrawText(hDC,strText,strText.GetLength(),ClientRect,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	return;
}
void TransButton::SetupRgn(CDC *pDC,				//�����DCָ��
						   //CBitmap &cBitmap,		//���д�����״��λͼ����
						   CSkinImage bImage,
						   COLORREF TransColor)	//͸��ɫ
{
	///////////////////////////////////////////////////////////////
	//bimage�п��ܲ�����IDB��Ϣ����������·��ͨ
	//�ܷ�Ѱ�ҵ�cskinimage��bmp��ת��?
	////��CSkinImage�еõ���ԴIDB
	//tagImageLoadInfo imageIDB;
	//bImage.GetLoadInfo(imageIDB.uResourceID);
	//��IDB�еõ�BMP
	//CBitmap cBitmap;
	//cBitmap.LoadBitmap(imageIDB);

	//int IWidth,IHeight;
	//IWidth=bImage.GetWidth()/5;
	//IHeight=bImage.GetHeight();

	//CBitmap cBitmap;
	//cBitmap.LoadBitmap(IDB_SKIN_MASK);

	//�Ӱ�ťװ����Ϣ����ťͼƬ����BMP������BMPȷ��͸������
	tagImageLoadInfo buttonInfo;
	GetButtonLoadInfo(buttonInfo);

	CBitmap cBitmap;
	if((buttonInfo.strFileName)!="")
		cBitmap.LoadBitmap(buttonInfo.strFileName);
	else if((buttonInfo.uResourceID)!=0)
		cBitmap.LoadBitmap(buttonInfo.uResourceID);

	//////////////////////////////////////////////////////////////////


	CDC memDC;
	//�����봫��DC���ݵ���ʱDC
	memDC.CreateCompatibleDC(pDC);

	CBitmap *pOldMemBmp=NULL;
	//��λͼѡ����ʱDC
	pOldMemBmp=memDC.SelectObject(&cBitmap);

	CRgn wndRgn;
	//�����ܵĴ������򣬳�ʼregionΪ0
	wndRgn.CreateRectRgn(0,0,0,0);


	BITMAP bit;   
	cBitmap.GetBitmap (&bit);//ȡ��λͼ����������Ҫ�õ�λͼ�ĳ��Ϳ�     
	int y;
	for(y=0;y<=bit.bmHeight  ;y++)
	{
		CRgn rgnTemp; //������ʱregion

		int iX = 0;
		do
		{
			//����͸��ɫ�ҵ���һ����͸��ɫ�ĵ�.
			while (iX <= bit.bmWidth/5  && memDC.GetPixel(iX, y) == TransColor)
				iX++;

			//��ס�����ʼ��
			int iLeftX = iX;

			//Ѱ���¸�͸��ɫ�ĵ�
			while (iX <= bit.bmWidth/5  && memDC.GetPixel(iX, y) != TransColor)
				++iX;

			//����һ������������ص���Ϊ1���ص���ʱ��region��
			rgnTemp.CreateRectRgn(iLeftX, y, iX, y+1);

			//�ϲ�����"region".
			wndRgn.CombineRgn(&wndRgn, &rgnTemp, RGN_OR);

			//ɾ����ʱ"region",�����´δ���ʱ�ͳ���
			rgnTemp.DeleteObject();
		}while(iX <bit.bmWidth/5 );
		iX = 0;
	}
	if(pOldMemBmp)
		memDC.SelectObject(pOldMemBmp);
	CWnd * pWnd = pDC->GetWindow();
	pWnd->SetWindowRgn(wndRgn,TRUE);    
	//pWnd->SetForegroundWindow();    
}

//Ĭ�ϻص�����
LRESULT TransButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (m_pIMouseEvent)
	{
		switch (message)
		{
		case WM_MOUSELEAVE:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventMouseLeft(uButtonID,wParam,lParam);
				break;
			}
		case WM_MOUSEMOVE:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventMouseMove(uButtonID,wParam,lParam);
				break;
			}
		case WM_LBUTTONDOWN:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventLButtonDown(uButtonID,wParam,lParam);
				break;
			}
		case WM_LBUTTONUP:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventLButtonUp(uButtonID,wParam,lParam);
				break;
			}
		case WM_RBUTTONDOWN:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventRButtonDown(uButtonID,wParam,lParam);
				break;
			}
		case WM_RBUTTONUP:
			{
				UINT uButtonID=GetWindowLong(GetSafeHwnd(),GWL_ID);
				m_pIMouseEvent->OnEventRButtonUp(uButtonID,wParam,lParam);
				break;
			}
		}
	}
	return __super::DefWindowProc(message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
