#pragma once
#include "../Resource.h"
#include "afxwin.h"
#include "GameFrame.h"
#include "afxcmn.h"

// CGameFrameVideo �Ի���

class CVideoClient;
class CGameFrameVideo : public CDialog
{
	DECLARE_DYNAMIC(CGameFrameVideo)

public:
	CGameFrameVideo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CGameFrameVideo();

	BOOL m_bMe; //��ʾ�Լ�

// �Ի�������
	enum { IDD = IDD_FRAME_VIDEO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel(){ return; };
	virtual void OnOK(){ return; };
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

protected:
	CSkinImage							m_ImageLeftViewLM;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewMM;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewRM;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewLTM;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewRTM;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewLT;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewMT;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewRT;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewLB;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewMB;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewRB;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewLBM;			//��ԴͼƬ
	CSkinImage							m_ImageLeftViewRBM;			//��ԴͼƬ

public:
	virtual BOOL OnInitDialog();
	CVideoClient* m_pVideoClient;
	void SetMe();

	CSkinButton		m_btVisibleVideo; //��ʾ��������Ƶ
	CProgressCtrl m_ProgressSoundPop;
	CSkinButton m_btVideoSetup;
	afx_msg void OnBnClickedButtonVideoSetup();
	afx_msg void OnBnClickedButtonVisible();
	
	//����ͼ��
	void DrawLeftViewFrame(CDC * pDC);
	afx_msg void OnPaint();
	CStatic m_video;
	afx_msg void OnStnClickedStaticVideo();
};
