#pragma once

#include "Stdafx.h"
#include "ScoreView.h"
#include "CardControl.h"

//////////////////////////////////////////////////////////////////////////
//��Ϣ����

#define IDM_START					(WM_USER+100)						//��ʼ��Ϣ
#define IDM_OUT_CARD				(WM_USER+101)						//������Ϣ
#define IDM_PASS_CARD				(WM_USER+102)						//��������
#define IDM_LAND_SCORE				(WM_USER+103)						//�з���Ϣ
#define IDM_AUTO_OUTCARD			(WM_USER+104)                       //��ʾ��Ϣ

//////////////////////////////////////////////////////////////////////////

//��Ϸ��ͼ
class CGameClientView : public CGameFrameView
{
	//��Ϸ����
protected:
	WORD							m_wLandUser;						//�����û�
	WORD							m_wBombTime;						//ը������
	LONG							m_lBaseScore;						//��������
	BYTE							m_cbLandScore;						//��������

	//���ñ���
protected:
	bool							m_bDeasilOrder;						//����˳��

	//״̬����
public:
	bool							m_bLandTitle;						//������־
	bool							m_bPass[GAME_PLAYER];				//��������
	BYTE							m_bScore[GAME_PLAYER];				//�û��з�
	BYTE							m_bCardCount[GAME_PLAYER];			//�˿���Ŀ

	//λ����Ϣ
public:
	CSize							m_LandSize;							//�ڿӱ�־
	CPoint							m_ptScore[GAME_PLAYER];				//�з�λ��
	CPoint							m_ptLand[GAME_PLAYER];				//����λ��

	//λͼ����
protected:
	CSkinImage						m_ImageBomb;						//ը��ͼƬ
	CSkinImage						m_ImageLand;						//ׯ��ͼƬ
	CSkinImage						m_ImageBack;						//������Դ
	CSkinImage						m_ImageScore;						//����ͼƬ
	CSkinImage						m_ImageCenter;						//������Դ
	CSkinImage						m_ImageLc;							//���ϽǱ�־
	CSkinImage						m_ImageHeadBg;						//ͷ�񱳾�
	//��ť�ؼ�
public:
	TransButton						m_btStart;							//��ʼ��ť
	TransButton						m_btOutCard;						//���ư�ť
	TransButton						m_btPassCard;						//������ť
	TransButton						m_btOneScore;
	TransButton						m_btTwoScore;
	TransButton						m_btThreeScore;						//3 �ְ�ť
	TransButton						m_btGiveUpScore;					//������ť
	TransButton						m_btAutoOutCard;					//��ʾ��ť

	//�˿˿ؼ�
public:
	CCardControl					m_HandCardControl;					//�����˿�
	CCardControl					m_BackCardControl;					//�����˿�
	CCardControl					m_UserCardControl[3];				//�˿���ͼ
	CCardControl					m_LeaveCardControl[2];				//�����˿�

	//�ؼ����
public:
	CScoreView						m_ScoreView;						//������ͼ

	//��������
public:
	//���캯��
	CGameClientView();
	//��������
	virtual ~CGameClientView();

	//�̳к���
private:
	//���ý���
	virtual void ResetGameView();
	//�����ؼ�
	virtual void RectifyGameView(int nWidth, int nHeight);
	//�滭����
	virtual void DrawGameView(CDC * pDC, int nWidth, int nHeight);

	//���ú���
public:
	//�û�˳��
	void SetUserOrder(bool bDeasilOrder);
	//��ȡ˳��
	bool IsDeasilOrder() { return m_bDeasilOrder; }

	//���ܺ���
public:
	//ը������
	void SetBombTime(WORD wBombTime);
	//��������
	void SetBaseScore(LONG lBaseScore);
	//��ʾ��ʾ
	void ShowLandTitle(bool bLandTitle);
	//���÷���
	void SetPassFlag(WORD wChairID, bool bPass);
	//�˿���Ŀ
	void SetCardCount(WORD wChairID, BYTE bCardCount);
	//�ڿӷ���
	void SetLandUser(WORD wChairID, BYTE bLandScore);
	//�ڿӷ���
	void SetLandScore(WORD wChairID, BYTE bLandScore);

	//��Ϣӳ��
protected:
	//��ʼ��ť
	afx_msg void OnStart();
	//���ư�ť
	afx_msg void OnOutCard();
	//������ť
	afx_msg void OnPassCard();
	//�зְ�ť
	afx_msg void OnOneScore();
	//�зְ�ť
	afx_msg void OnTwoScore();
	//�зְ�ť
	afx_msg void OnThreeScore();
	//������ť
	afx_msg void OnGiveUpScore();
	//������ʾ
	afx_msg void OnAutoOutCard();
	//��������
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//�����Ϣ
	afx_msg void OnRButtonUp(UINT nFlags, CPoint Point);
	//���˫��
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////