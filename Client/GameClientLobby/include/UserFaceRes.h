#ifndef USER_FACE_RES_HEAD_FILE
#define USER_FACE_RES_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "AtlImage.h"
#include "Companion.h"
#include <vector>
#include <string>
#include <afxmt.h>
#include <afxinet.h> 
#include  <vfw.h>
static CCriticalSection m_HeadCS;
std::vector<std::string> ImgFileList;
std::vector<std::string> TempFileList;

#define BMP_HEADER_MARKER	((WORD) ('M' << 8) | 'B')


inline UINT DownLoad(LPVOID pParam);
//�û�ͷ����Դ��
class CUserFaceRes : public IUserFaceRes
{
	//��������
protected:
	WORD								m_wCount;						//ͷ����Ŀ
	CSize								m_NormalSize;					//ͷ���С
	CSize								m_SmallSize;					//ͷ���С

	HDRAWDIB							m_hDrawDib;
	//��������
public:
	//���캯��
	CUserFaceRes(void);
	//��������
	virtual ~CUserFaceRes(void);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual bool __cdecl Release() { if (IsValid()) delete this; return true; }
	//�Ƿ���Ч
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CUserFaceRes))?true:false; }
	//�ӿڲ�ѯ
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//�ӿں���
public:
	//ͷ����Ŀ
	virtual WORD __cdecl GetFaceCount() { return m_wCount; }
	//��ȡ��С
	virtual const CSize & __cdecl GetSmallFaceSize() { return m_SmallSize; };
	//��ȡ��С
	virtual const CSize & __cdecl GetNormalFaceSize() { return m_NormalSize; }
	//�滭ͷ��
	virtual void __cdecl DrawFace(CDC * pDC, int nXPos, int nYPos, int xSize, int ySize, LPCTSTR pszFace, int cbGender);
	//�滭ͷ��
	virtual void __cdecl DrawSmallFace(CDC * pDC, int nXPos, int nYPos, LPCTSTR pszFace, int cbGender);
	//�滭ͷ��
	virtual void __cdecl DrawNormalFace(CDC * pDC, int nXPos, int nYPos, LPCTSTR pszFace, int cbGender);

	//�ڲ�����
protected:

	bool AlphaDrawImage(CDC * pDestDC, int xDest, int yDest, int cxDest, int cyDest, CDC * pScrDC, int xSrc, int ySrc);

};

//////////////////////////////////////////////////////////////////////////

#endif