#ifndef HTML_BROWSER_HEAD_FILE
#define HTML_BROWSER_HEAD_FILE

#pragma once

#include "ComService.h"

//////////////////////////////////////////////////////////////////////////

//�������ͼ
class FKLOBBY_DLL_CLASS CHtmlBrower : public CHtmlView
{
	//��������
protected:
	bool							m_bEnableBrowser;					//�Ƿ�����
	CString							m_strNavigateUrl;					//ҳ���ַ

	//��������
public:
	//���캯��
	CHtmlBrower();
	//��������
	virtual ~CHtmlBrower();
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