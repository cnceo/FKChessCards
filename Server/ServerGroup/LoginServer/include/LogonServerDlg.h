#pragma once

#include "stdafx.h"
#include "..\Resource.h"
#include "LogonService.h"

//////////////////////////////////////////////////////////////////////////

//���Ի���
class CLogonServerDlg : public CDialog
{
	//��������
protected:
	bool							m_bAskQuit;							//�˳�ѯ��

	//�������
public:
	CLogonService					m_LogonService;						//��¼����
	CEventServiceHelper				m_EventService;						//�¼�����

	//�ؼ�����
public:
	CRichEditCtrl					m_RichEditCtrl;						//�¼���¼

	//���캯��
public:
	//�������
	CLogonServerDlg();
	//��������
	virtual ~CLogonServerDlg();

	//���غ���
protected:
	//�ؼ����໯
	virtual void DoDataExchange(CDataExchange * pDX);
	//��ʼ������
	virtual BOOL OnInitDialog();
	//ȷ����Ϣ
	virtual void OnOK();
	//ȡ������
	virtual void OnCancel();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//��Ϣ����
protected:
	//��������
	afx_msg void OnBnClickedStartService();
	//ֹͣ����
	afx_msg void OnBnClickedStopService();
	//���÷���
	afx_msg void OnBnClickedInitService();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////