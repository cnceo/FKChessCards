#pragma once

#ifndef SKIN_BUTTON_HEAD_FILE
#define SKIN_BUTTON_HEAD_FILE

#include "SkinImage.h"
#include "SkinControls.h"
#include "SkinAttribute.h"

//////////////////////////////////////////////////////////////////////////

//��������
#define SKIN_BUTTON_CRTEXTCOLOR			TEXT("��ť������ɫ")			// COLORREF ���� ����д��
#define SKIN_BUTTON_STRIMAGEPATH		TEXT("��ť����ͼ")				// CString ���� ����д��

//////////////////////////////////////////////////////////////////////////

//��ť������
class CSkinButtonAttribute : public CSkinAttribute
{
	friend class CSkinButton;

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
	CSkinButtonAttribute();
	//��������
	virtual ~CSkinButtonAttribute();

	//�ӿں���
public:
	//��������
	virtual bool __cdecl LoadSkinOption();
	//��������
	virtual bool __cdecl SaveSkinOption();
};

//////////////////////////////////////////////////////////////////////////

//��ť��
class FKUIMODULE_CLASS CSkinButton : public CButton
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
	static CSkinButtonAttribute			m_SkinAttribute;				//��ť����

	//��������
public:
	//���캯��
	CSkinButton();
	//��������
	virtual ~CSkinButton();

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

	//��ѯ����
public:
	//��ȡͼ��
	HICON GetButtonIcon() { return m_hIcon; }
	//��ȡ��ɫ
	COLORREF GetButtonColor() { return m_crTextColor; }
	//��ȡλͼ����
	bool GetButtonLoadInfo(tagImageLoadInfo & LoadInfo);
	//��ȡ���Խӿ�
	static CSkinButtonAttribute * GetSkinAttribute() { return &m_SkinAttribute; }

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
	DECLARE_DYNAMIC(CSkinButton)
};

//////////////////////////////////////////////////////////////////////////

//����ť��
class FKUIMODULE_CLASS CRgnButton : public CButton
{
	//��������
protected:
	bool								m_bHovering;					//������־
	CRgn								m_ButtonRgn;					//��������
	COLORREF							m_crTrans;						//͸����ɫ
	COLORREF							m_crTextColor;					//������ɫ
	CSkinImage							m_ImageBack;					//����λͼ

	//�ӿ�ָ��
protected:
	IMouseEvent							* m_pIMouseEvent;				//����¼�

	//��������
public:
	//���캯��
	CRgnButton();
	//��������
	virtual ~CRgnButton();

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
	//����λͼ
	bool LoadRgnImage(LPCTSTR pszFileName, COLORREF crTrans);
	//����λͼ
	bool LoadRgnImage(HINSTANCE hInstance, UINT uBitmapID, COLORREF crTrans);
	//��ȡ���ص��ӿ�
	IMouseEvent * GetMouseEventCallBack() { return m_pIMouseEvent; }
	//�������ص��ӿ�
	void SetMouseEventCallBack(IMouseEvent * pIMouseEvent) { m_pIMouseEvent=pIMouseEvent; };

	//�ڲ����� 
private:
	//��������
	bool CreateControlRgn(COLORREF crTrans);

	//��Ϣ����
protected:
	//������Ϣ
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//������Ϣ
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//����ƶ�
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//����뿪
	afx_msg LRESULT OnMouseLeave(WPARAM wparam, LPARAM lparam);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CRgnButton)
};

//////////////////////////////////////////////////////////////////////////

#endif