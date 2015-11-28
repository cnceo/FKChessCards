#ifndef GAME_FRAME_CONTROL_HEAD_FILE
#define GAME_FRAME_CONTROL_HEAD_FILE

#pragma once

#include "GameFrame.h"
#include "UserListView.h"
#include "GameFrameDlg.h"

//��˵��
class CClientKernelSink;

//////////////////////////////////////////////////////////////////////////

//��ܿ�����ͼ
class GAMEFRAMEWORK_CLASS CGameFrameControl : public CDialog
{
	//��������
protected:
	bool								m_bRectify;						//������־
	bool								m_bInitDialog;					//������־

	//�������
protected:
	CHtmlBrower							* m_pAdBrower;					//��洰��
	CGameFrameDlg						* m_pGameFrameDlg;				//�Ի���
	IClientKernel						* m_pIClientKernel;				//�ں˽ӿ�

	//����ͼ��
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

	//����ؼ�
public:
	CEdit								m_ChatInput;					//��������
	CComboBox							m_ChatObject;					//�������
	CSkinButton							m_btPhrase;						//���ﰴť
	CSkinButton							m_btColorSet;					//��ɫ����
	CSkinButton							m_btCleanScreen;				//������ť
	CSkinButton							m_btSendChat;					//���Ͱ�ť
	CSkinButton							m_btExpression;					//���鰴ť
	CSkinButton							m_btGameRule;					//��Ϸ����
	CExpression							m_Expression;					//���鴰��
	CSkinRichEdit						m_ChatMessage;					//�������

	//��������
public:
	CSkinButton							m_btQuitGame;					//�˳���Ϸ
	CSkinButton							m_btGameOption;					//��Ϸ����
	CToolTipCtrl						m_ToolTipCtrl;					//��ʾ�ؼ�
	CUserListView						m_UserListView;					//�û��б�

	//��������
public:
	//���캯��
	CGameFrameControl(CGameFrameDlg * pGameFrameDlg); 
	//��������
	virtual ~CGameFrameControl();

	//���غ���
protected:
	//�ؼ���
	virtual void DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ������
	virtual void OnOK() { return; };
	//ȡ������
	virtual void OnCancel() { return; };
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//���ú���
public:
	//�����ں�
	void SetClientKernel(IClientKernel * pIClientKernel);
	//�����û���ϢIE
	LRESULT SetUserInfo(WPARAM wParam, LPARAM lParam);

	//���ܺ���
public:
	//���ÿ���
	void ResetGameFrameControl();
	//�����û�
	bool InsertUserItem(tagUserData * pUserData);
	//�����û�
	bool UpdateUserItem(tagUserData * pUserData);
	//ɾ���û�
	bool DeleteUserItem(tagUserData * pUserData);

	//�ڲ�����
protected:
	//����ͼ��
	void DrawLeftViewFrame(CDC * pDC);
	//�����ؼ�
	void RectifyControl(int nWidth, int nHeight);

	//��Ϣ����
protected:
	//�滭��Ϣ
	afx_msg void OnPaint();
	//��Ϸ����
	afx_msg void OnBnClickedOption();
	//��Ϸ����
	afx_msg void OnBnClickedRule();
	//�˳���ť
	afx_msg void OnBnClickedQuitGame();
	//�滭����
	afx_msg BOOL OnEraseBkgnd(CDC * pDC);
	//λ����Ϣ
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//���Ͱ�ť
	afx_msg void OnBnClickedSendChat();
	//���鰴ť
	afx_msg void OnBnClickedExpression();
	//��ɫ��ť
	afx_msg void OnBnClickedColorSet();
	//������ť
	afx_msg void OnBnClickedCleanScreen();
	//������Ϣ
	LRESULT OnHitExpression(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif