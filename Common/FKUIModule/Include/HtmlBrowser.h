#ifndef HTML_BROWSER_HEAD_FILE
#define HTML_BROWSER_HEAD_FILE

#pragma once

#include "SkinControls.h"
//////////////////////////////////////////////////////////////////////////

//�������ͼ
class FKUIMODULE_CLASS CHtmlBrowerB : public CHtmlView
{
	//��������
protected:
	bool							m_bEnableBrowser;					//�Ƿ�����
	CString							m_strNavigateUrl;					//ҳ���ַ

	//��������
public:
	//���캯��
	CHtmlBrowerB();
	//��������
	virtual ~CHtmlBrowerB();
	afx_msg void OnDestroy(); //���ٷ���

	//���ܺ���
public:
	//ת����ҳ
	void Navigate(LPCTSTR pszUrl);
	//������ҳ
	bool EnableBrowser(bool bEnableBrowser);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif