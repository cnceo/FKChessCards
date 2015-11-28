#pragma once

#ifndef TRANS_BUTTON_HEAD_FILE
#define TRANS_BUTTON_HEAD_FILE

#include "SkinImage.h"
#include "SkinControls.h"
#include "SkinAttribute.h"

//////////////////////////////////////////////////////////////////////////

//��������
#define SKIN_BUTTON_CRTEXTCOLOR			TEXT("��ť������ɫ")			// COLORREF ���� ����д��
#define SKIN_BUTTON_STRIMAGEPATH		TEXT("��ť����ͼ")				// CString ���� ����д��

//////////////////////////////////////////////////////////////////////////

//��ť������
class TransButtonAttribute : public CSkinAttribute
{
	friend class TransButton;

	//���ò���
public:
	COLORREF							m_crTextColor;					//������ɫ
	CString								m_strImagePath;					//λͼ·��

	//GDI ����
public:
	CSkinImage							m_ImageBack;					//��ťλͼ

	//��������
protected:
	//���캯��
	TransButtonAttribute();
	//��������
	virtual ~TransButtonAttribute();

	//�ӿں���
public:
	//��������
	virtual bool __cdecl LoadSkinOption();
	//��������
	virtual bool __cdecl SaveSkinOption();
};

//////////////////////////////////////////////////////////////////////////

//��ť��
class FKUIMODULE_CLASS TransButton : public CButton
{
	friend class CRgnButton;

	//��������
protected:
	bool								m_bExpand;						//�Ƿ�����
	bool								m_bHovering;					//������־
	HICON								m_hIcon;						//��ťͼ��
	COLORREF							m_crTextColor;					//������ɫ
	CSkinImage							m_ImageBack;					//��ťλͼ

	//�ӿ�ָ��
protected:
	IMouseEvent							* m_pIMouseEvent;				//����¼�

	//��ť������
public:
	static TransButtonAttribute			m_SkinAttribute;				//��ť����

	//��������
public:
	//���캯��
	TransButton();
	//��������
	virtual ~TransButton();

	//���غ���
protected:
	//�ؼ����໯
	virtual void PreSubclassWindow();
	//����滭����
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//Ĭ�ϻص�����
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//���ܺ���
public:
	//��ȡ���ص��ӿ�
	IMouseEvent * GetMouseEventCallBack() { return m_pIMouseEvent; }
	//�������ص��ӿ�
	void SetMouseEventCallBack(IMouseEvent * pIMouseEvent) { m_pIMouseEvent=pIMouseEvent; };

	//���ú���
public:
	//����λ��
	bool FixButtonSize();
	//����ͼ��
	bool SetButtonIcon(HICON hIcon);
	//������ɫ
	bool SetTextColor(COLORREF crTextColor);
	//����λͼ
	bool SetButtonImage(LPCTSTR pszFileName, bool bExpandImage);
	//����λͼ
	bool SetButtonImage(UINT uBitmapID, HINSTANCE hInstance, bool bExpandImage);
	//Ĭ�ϰ�ť
	void SetDefaultButton(bool bResetIcon=true, bool bResetColor=true, bool bResetImage=true);

	void SetupRgn(CDC *pDC,				//�����DCָ��
		//CBitmap &cBitmap,		//���д�����״��λͼ����
		CSkinImage bImage,
		COLORREF TransColor);	//͸��ɫ

	//��ѯ����
public:
	//��ȡͼ��
	HICON GetButtonIcon() { return m_hIcon; }
	//��ȡ��ɫ
	COLORREF GetButtonColor() { return m_crTextColor; }
	//��ȡλͼ����
	bool GetButtonLoadInfo(tagImageLoadInfo & LoadInfo);
	//��ȡ���Խӿ�
	static TransButtonAttribute * GetSkinAttribute() { return &m_SkinAttribute; }

	//��Ϣ����
protected:
	//������Ϣ
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//����ƶ�
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//����뿪
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);
	//������Ϣ
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(TransButton)
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif