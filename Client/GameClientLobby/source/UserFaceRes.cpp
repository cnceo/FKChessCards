#include "Include/StdAfx.h"
#include "Resource.h"
#include "../Include/UserFaceRes.h"
#include  <algorithm>
#include <vfw.h>
#pragma comment(lib,"vfw32.lib")
//////////////////////////////////////////////////////////////////////////

//�궨��
#define COLOR_TRANS						RGB(255,0,255)				//͸����ɫ

//////////////////////////////////////////////////////////////////////////

inline UINT DownLoad(LPVOID pParam)
{
	while (1)
	{
		if(TempFileList.size()>0)
		{
			m_HeadCS.Lock();
			std::vector<std::string>::reverse_iterator rit = TempFileList.rbegin();
			LPCTSTR pszFace = (LPCTSTR)((std::string)*rit).c_str();;
			TempFileList.erase((++rit).base());
			m_HeadCS.Unlock();
			CString DATFile=TEXT("FaceImage.DAT");

			CFileFind	finder;
			BOOL	FF;

			FF=finder.FindFile(DATFile);
			if(!FF){
				CFile pwFile;
				pwFile.Open(DATFile,CFile::modeCreate);
				pwFile.Close();
			}
			CFile pwFile;
			pwFile.Open(DATFile,CFile::modeReadWrite| CFile::typeBinary);
			char FileNum[10];
			pwFile.Read(FileNum,9);
			sprintf(FileNum,"%d",atol(FileNum)+1);
			pwFile.SeekToBegin();
			pwFile.Write(FileNum,9);

			CInternetSession    is;
			CStdioFile        *psf;

			CString picUrl="http://www.17tzy.com/tzyBBS/avatars/common/0.bmp";

			psf = is.OpenURL( picUrl, 1, INTERNET_FLAG_TRANSFER_BINARY | INTERNET_FLAG_RELOAD);

			BITMAPFILEHEADER bmfh;

			// ����1����ȡ�ļ�ͷ
			int nCount = psf->Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));

			// �ж��Ƿ���BMP��ʽ��λͼ
			if(bmfh.bfType != BMP_HEADER_MARKER) 
			{
				continue;
			}

			// ������Ϣͷ���ϵ�ɫ��Ĵ�С�����ڴ�
			int nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
			LPBITMAPINFOHEADER m_pBMIH = (LPBITMAPINFOHEADER) new BYTE[nSize];

			// ����2����ȡ��Ϣͷ�͵�ɫ��
			nCount = psf->Read(m_pBMIH, nSize); 

			// ����3����ȡͼ������
			LPBYTE	m_pBits = (LPBYTE) new BYTE[m_pBMIH->biSizeImage];
			nCount = psf->Read(m_pBits, m_pBMIH->biSizeImage); 

			//д�뵽DAT
			pwFile.SeekToEnd();
			pwFile.Write(pszFace,128);
			// ����д����
			pwFile.Write((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));
			pwFile.Write((LPVOID) m_pBMIH,  nSize);
			pwFile.Write((LPVOID) m_pBits, m_pBMIH->biSizeImage);

			m_HeadCS.Lock();
			ImgFileList.push_back(pszFace);
			m_HeadCS.Unlock();
		}
		
		Sleep(1);
	}
	return 0;

}

//���캯��
CUserFaceRes::CUserFaceRes(void)
{
	//������Դ
	HINSTANCE hInstancle=GetModuleHandle(GAMECLIENTLOBBY_EXPORTS_NAME);
	
	//���ñ���
	m_NormalSize.SetSize(NOR_FACE_WIDTH,NOR_FACE_HEIGHT);
	m_SmallSize.SetSize(LIT_FACE_WIDTH,LIT_FACE_HEIGHT);
	ImgFileList.clear();
	TempFileList.clear();

	CString DATFile=TEXT("FaceImage.DAT");

	CFileFind	finder;
	BOOL	FF;
	BOOL    bHaveBmp=FALSE;

	FF=finder.FindFile(DATFile);
	if(!FF){
		return;
	}

	CFile pFile;
	pFile.Open(DATFile,CFile::modeRead);

	char FileNum[10];
	pFile.Read(FileNum,9);
	long NUM = atol(FileNum);

	for(long j=0;j<NUM;j++){	

		BITMAPFILEHEADER bmfh;

		// ����1����ȡ�ļ�ͷ
		char filename[128];
		pFile.Read(filename,127);

		int nCount = pFile.Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));

		// �ж��Ƿ���BMP��ʽ��λͼ
		if(bmfh.bfType != BMP_HEADER_MARKER) 
		{
			continue;
		}

		// ������Ϣͷ���ϵ�ɫ��Ĵ�С�����ڴ�
		int nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		LPBITMAPINFOHEADER m_pBMIH = (LPBITMAPINFOHEADER) new BYTE[nSize];

		// ����2����ȡ��Ϣͷ�͵�ɫ��
		nCount = pFile.Read(m_pBMIH, nSize); 

		// ����3����ȡͼ������
		LPBYTE	m_pBits = (LPBYTE) new BYTE[m_pBMIH->biSizeImage];
		nCount = pFile.Read(m_pBits, m_pBMIH->biSizeImage); 

		ImgFileList.push_back(filename);
		delete m_pBits;
		delete m_pBMIH;
	}
	pFile.Close();
	m_hDrawDib  = DrawDibOpen();
	return;
}

//��������
CUserFaceRes::~CUserFaceRes(void)
{
}

//�ӿڲ�ѯ
void * __cdecl CUserFaceRes::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IUserFaceRes,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IUserFaceRes,Guid,dwQueryVer);
	return NULL;
}
//�滭ͷ��
void __cdecl CUserFaceRes::DrawFace(CDC * pDC, int nXPos, int nYPos, int xSize, int ySize, LPCTSTR pszFace, int cbGender)
{
	m_HeadCS.Lock();
	int n=count(ImgFileList.begin(), ImgFileList.end(), pszFace);
	if (n<1)
	{
		int tn=count(TempFileList.begin(), TempFileList.end(), pszFace);
		if (tn<1 && strcmp(pszFace,"NoBmp")!=0)
		{

			TempFileList.push_back(pszFace);
		}
		m_HeadCS.Unlock();
		char ImgFile[128];
		if (cbGender==0)
		{
			sprintf(ImgFile,"PublicRes/male.bmp");
		}
		else
		{
			sprintf(ImgFile,"PublicRes/female.bmp");
		}
		CFile pFile;
		pFile.Open(ImgFile,CFile::modeRead);

		BITMAPFILEHEADER bmfh;

		// ����1����ȡ�ļ�ͷ

		int nCount = pFile.Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));

		// �ж��Ƿ���BMP��ʽ��λͼ
		if(bmfh.bfType != BMP_HEADER_MARKER) 
		{
			return;
		}

		// ������Ϣͷ���ϵ�ɫ��Ĵ�С�����ڴ�
		int nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		LPBITMAPINFOHEADER m_pBMIH = (LPBITMAPINFOHEADER) new BYTE[nSize];

		// ����2����ȡ��Ϣͷ�͵�ɫ��
		nCount = pFile.Read(m_pBMIH, nSize); 

		// ����3����ȡͼ������
		LPBYTE	m_pBits = (LPBYTE) new BYTE[m_pBMIH->biSizeImage];
		nCount = pFile.Read(m_pBits, m_pBMIH->biSizeImage); 

		int PaletteSize = 0;

		if(m_pBMIH->biClrUsed == 0) 	// ��ɫ��Ĵ�СΪ2��biBitCount�η�		
		{
			switch(m_pBMIH->biBitCount)
			{
			case 1:
				PaletteSize = 2;
				break;
			case 4:
				PaletteSize = 16;
				break;
			case 8:
				PaletteSize = 256;
				break;
			case 16:
			case 24:
			case 32:
				PaletteSize = 0;
				break;
			default:
				PaletteSize = 0;
			}
		}	
		else // ��ɫ��Ĵ�СΪʵ���õ�����ɫ��
		{
			PaletteSize = m_pBMIH->biClrUsed;
		}

		LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) 
			+ PaletteSize * sizeof(PALETTEENTRY)];

		// �����߼���ɫ�����Ϣ
		pLogPal->palVersion = 	0x300;
		pLogPal->palNumEntries = PaletteSize;

		// ����DIB�е���ɫ���߼���ɫ��
		LPRGBQUAD pDibQuad = (LPRGBQUAD) (LPVOID) (m_pBMIH + sizeof(BITMAPINFOHEADER));
		for(int i = 0; i < PaletteSize; ++i) 
		{
			pLogPal->palPalEntry[i].peRed = pDibQuad->rgbRed;
			pLogPal->palPalEntry[i].peGreen = pDibQuad->rgbGreen;
			pLogPal->palPalEntry[i].peBlue = pDibQuad->rgbBlue;
			pLogPal->palPalEntry[i].peFlags = 0;
			pDibQuad++;
		}

		HPALETTE            m_hPalette;			//��ɫ����	

		// ���ʹ�õ�ɫ�壬�򽫵�ɫ��ѡ���豸������
		m_hPalette = ::CreatePalette(pLogPal);

		::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);

		// ������ʾģʽ
		pDC->SetStretchBltMode(COLORONCOLOR);

		// ���豸��ptOriginλ���ϻ�����СΪszImage��ͼ��
		::StretchDIBits(pDC->GetSafeHdc(), 
			nXPos, nYPos,			//��ʼ��
			xSize,ySize,			//���Ϳ�
			0, 0, 
			m_pBMIH->biWidth, m_pBMIH->biHeight,
			m_pBits, (LPBITMAPINFO) m_pBMIH, 
			DIB_RGB_COLORS, SRCCOPY);
		delete m_pBits;
		delete m_pBMIH;
		pFile.Close();
		return;
	}

	m_HeadCS.Unlock();
	CString DATFile=TEXT("FaceImage.DAT");

	CFileFind	finder;
	BOOL	FF;
	BOOL    bHaveBmp=FALSE;

	FF=finder.FindFile(DATFile);
	if(!FF){
		return;
	}

	CFile pFile;
	pFile.Open(DATFile,CFile::modeRead);

	char FileNum[10];
	pFile.Read(FileNum,9);
	long NUM = atol(FileNum);
	for(long j=0;j<NUM;j++){	

		BITMAPFILEHEADER bmfh;

		// ����1����ȡ�ļ�ͷ
		char filename[128];
		pFile.Read(filename,127);

		int nCount = pFile.Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));

		// �ж��Ƿ���BMP��ʽ��λͼ
		if(bmfh.bfType != BMP_HEADER_MARKER) 
		{
			continue;
		}

		// ������Ϣͷ���ϵ�ɫ��Ĵ�С�����ڴ�
		int nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		LPBITMAPINFOHEADER m_pBMIH = (LPBITMAPINFOHEADER) new BYTE[nSize];

		// ����2����ȡ��Ϣͷ�͵�ɫ��
		nCount = pFile.Read(m_pBMIH, nSize); 

		// ����3����ȡͼ������
		LPBYTE	m_pBits = (LPBYTE) new BYTE[m_pBMIH->biSizeImage];
		nCount = pFile.Read(m_pBits, m_pBMIH->biSizeImage); 

		if (strcmp(filename,pszFace)==0)
		{

			int PaletteSize = 0;

			if(m_pBMIH->biClrUsed == 0) 	// ��ɫ��Ĵ�СΪ2��biBitCount�η�		
			{
				switch(m_pBMIH->biBitCount)
				{
				case 1:
					PaletteSize = 2;
					break;
				case 4:
					PaletteSize = 16;
					break;
				case 8:
					PaletteSize = 256;
					break;
				case 16:
				case 24:
				case 32:
					PaletteSize = 0;
					break;
				default:
					PaletteSize = 0;
				}
			}	
			else // ��ɫ��Ĵ�СΪʵ���õ�����ɫ��
			{
				PaletteSize = m_pBMIH->biClrUsed;
			}

			LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) 
				+ PaletteSize * sizeof(PALETTEENTRY)];

			// �����߼���ɫ�����Ϣ
			pLogPal->palVersion = 	0x300;
			pLogPal->palNumEntries = PaletteSize;

			// ����DIB�е���ɫ���߼���ɫ��
			LPRGBQUAD pDibQuad = (LPRGBQUAD) (LPVOID) (m_pBMIH + sizeof(BITMAPINFOHEADER));
			for(int i = 0; i < PaletteSize; ++i) 
			{
				pLogPal->palPalEntry[i].peRed = pDibQuad->rgbRed;
				pLogPal->palPalEntry[i].peGreen = pDibQuad->rgbGreen;
				pLogPal->palPalEntry[i].peBlue = pDibQuad->rgbBlue;
				pLogPal->palPalEntry[i].peFlags = 0;
				pDibQuad++;
			}

			HPALETTE            m_hPalette;			//��ɫ����	

			// ���ʹ�õ�ɫ�壬�򽫵�ɫ��ѡ���豸������
			m_hPalette = ::CreatePalette(pLogPal);

			::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);

			// ������ʾģʽ
			pDC->SetStretchBltMode(COLORONCOLOR);

			// ���豸��ptOriginλ���ϻ�����СΪszImage��ͼ��
			::StretchDIBits(pDC->GetSafeHdc(), 
				nXPos, nYPos,			//��ʼ��
				xSize,ySize,			//���Ϳ�
				0, 0, 
				m_pBMIH->biWidth, m_pBMIH->biHeight,
				m_pBits, (LPBITMAPINFO) m_pBMIH, 
				DIB_RGB_COLORS, SRCCOPY);
			delete m_pBits;
			delete m_pBMIH;
			bHaveBmp = TRUE;
			break;
		}
		delete m_pBits;
		delete m_pBMIH;

	}
	pFile.Close();
	return;
}
//�滭ͷ��
void __cdecl CUserFaceRes::DrawSmallFace(CDC * pDC, int nXPos, int nYPos, LPCTSTR pszFace, int cbGender)
{
	m_HeadCS.Lock();
	int n=count(ImgFileList.begin(), ImgFileList.end(), pszFace);
	if (n<1)
	{
		int tn=count(TempFileList.begin(), TempFileList.end(), pszFace);
		if (tn<1 && strcmp(pszFace,"NoBmp")!=0)
		{
			
			TempFileList.push_back(pszFace);
		}
		m_HeadCS.Unlock();
		char ImgFile[128];
		if (cbGender==0)
		{
			sprintf(ImgFile,"PublicRes/male.bmp");
		}
		else
		{
			sprintf(ImgFile,"PublicRes/female.bmp");
		}
		CFile pFile;
		pFile.Open(ImgFile,CFile::modeRead);

		BITMAPFILEHEADER bmfh;

		// ����1����ȡ�ļ�ͷ

		int nCount = pFile.Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));

		// �ж��Ƿ���BMP��ʽ��λͼ
		if(bmfh.bfType != BMP_HEADER_MARKER) 
		{
			return;
		}

		// ������Ϣͷ���ϵ�ɫ��Ĵ�С�����ڴ�
		int nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		LPBITMAPINFOHEADER m_pBMIH = (LPBITMAPINFOHEADER) new BYTE[nSize];

		// ����2����ȡ��Ϣͷ�͵�ɫ��
		nCount = pFile.Read(m_pBMIH, nSize); 

		// ����3����ȡͼ������
		LPBYTE	m_pBits = (LPBYTE) new BYTE[m_pBMIH->biSizeImage];
		nCount = pFile.Read(m_pBits, m_pBMIH->biSizeImage); 

		int PaletteSize = 0;

		if(m_pBMIH->biClrUsed == 0) 	// ��ɫ��Ĵ�СΪ2��biBitCount�η�		
		{
			switch(m_pBMIH->biBitCount)
			{
			case 1:
				PaletteSize = 2;
				break;
			case 4:
				PaletteSize = 16;
				break;
			case 8:
				PaletteSize = 256;
				break;
			case 16:
			case 24:
			case 32:
				PaletteSize = 0;
				break;
			default:
				PaletteSize = 0;
			}
		}	
		else // ��ɫ��Ĵ�СΪʵ���õ�����ɫ��
		{
			PaletteSize = m_pBMIH->biClrUsed;
		}

		LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) 
			+ PaletteSize * sizeof(PALETTEENTRY)];

		// �����߼���ɫ�����Ϣ
		pLogPal->palVersion = 	0x300;
		pLogPal->palNumEntries = PaletteSize;

		// ����DIB�е���ɫ���߼���ɫ��
		LPRGBQUAD pDibQuad = (LPRGBQUAD) (LPVOID) (m_pBMIH + sizeof(BITMAPINFOHEADER));
		for(int i = 0; i < PaletteSize; ++i) 
		{
			pLogPal->palPalEntry[i].peRed = pDibQuad->rgbRed;
			pLogPal->palPalEntry[i].peGreen = pDibQuad->rgbGreen;
			pLogPal->palPalEntry[i].peBlue = pDibQuad->rgbBlue;
			pLogPal->palPalEntry[i].peFlags = 0;
			pDibQuad++;
		}

		HPALETTE            m_hPalette;			//��ɫ����	

		// ���ʹ�õ�ɫ�壬�򽫵�ɫ��ѡ���豸������
		m_hPalette = ::CreatePalette(pLogPal);

		::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);

		// ������ʾģʽ
		pDC->SetStretchBltMode(COLORONCOLOR);

		// ���豸��ptOriginλ���ϻ�����СΪszImage��ͼ��
		::StretchDIBits(pDC->GetSafeHdc(), 
			nXPos, nYPos,			//��ʼ��
			m_SmallSize.cx,m_SmallSize.cy,			//���Ϳ�
			0, 0, 
			m_pBMIH->biWidth, m_pBMIH->biHeight,
			m_pBits, (LPBITMAPINFO) m_pBMIH, 
			DIB_RGB_COLORS, SRCCOPY);
		delete m_pBits;
		delete m_pBMIH;
		pFile.Close();
		return;
	}

	m_HeadCS.Unlock();
	CString DATFile=TEXT("FaceImage.DAT");

	CFileFind	finder;
	BOOL	FF;
	BOOL    bHaveBmp=FALSE;
	FF=finder.FindFile(DATFile);
	if(!FF){
		return;
	}

	CFile pFile;
	pFile.Open(DATFile,CFile::modeRead);

	char FileNum[10];
	pFile.Read(FileNum,9);
	long NUM = atol(FileNum);
	for(long j=0;j<NUM;j++){	

		BITMAPFILEHEADER bmfh;

		// ����1����ȡ�ļ�ͷ
		char filename[128];
		pFile.Read(filename,127);

		int nCount = pFile.Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));

		// �ж��Ƿ���BMP��ʽ��λͼ
		if(bmfh.bfType != BMP_HEADER_MARKER) 
		{
			continue;
		}

		// ������Ϣͷ���ϵ�ɫ��Ĵ�С�����ڴ�
		int nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		LPBITMAPINFOHEADER m_pBMIH = (LPBITMAPINFOHEADER) new BYTE[nSize];

		// ����2����ȡ��Ϣͷ�͵�ɫ��
		nCount = pFile.Read(m_pBMIH, nSize); 

		// ����3����ȡͼ������
		LPBYTE	m_pBits = (LPBYTE) new BYTE[m_pBMIH->biSizeImage];
		nCount = pFile.Read(m_pBits, m_pBMIH->biSizeImage); 

		if (strcmp(filename,pszFace)==0)
		{

			int PaletteSize = 0;

			if(m_pBMIH->biClrUsed == 0) 	// ��ɫ��Ĵ�СΪ2��biBitCount�η�		
			{
				switch(m_pBMIH->biBitCount)
				{
				case 1:
					PaletteSize = 2;
					break;
				case 4:
					PaletteSize = 16;
					break;
				case 8:
					PaletteSize = 256;
					break;
				case 16:
				case 24:
				case 32:
					PaletteSize = 0;
					break;
				default:
					PaletteSize = 0;
				}
			}	
			else // ��ɫ��Ĵ�СΪʵ���õ�����ɫ��
			{
				PaletteSize = m_pBMIH->biClrUsed;
			}

			LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) 
				+ PaletteSize * sizeof(PALETTEENTRY)];

			// �����߼���ɫ�����Ϣ
			pLogPal->palVersion = 	0x300;
			pLogPal->palNumEntries = PaletteSize;

			// ����DIB�е���ɫ���߼���ɫ��
			LPRGBQUAD pDibQuad = (LPRGBQUAD) (LPVOID) (m_pBMIH + sizeof(BITMAPINFOHEADER));
			for(int i = 0; i < PaletteSize; ++i) 
			{
				pLogPal->palPalEntry[i].peRed = pDibQuad->rgbRed;
				pLogPal->palPalEntry[i].peGreen = pDibQuad->rgbGreen;
				pLogPal->palPalEntry[i].peBlue = pDibQuad->rgbBlue;
				pLogPal->palPalEntry[i].peFlags = 0;
				pDibQuad++;
			}

			HPALETTE            m_hPalette;			//��ɫ����	

			// ���ʹ�õ�ɫ�壬�򽫵�ɫ��ѡ���豸������
			m_hPalette = ::CreatePalette(pLogPal);

			::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);

			// ������ʾģʽ
			pDC->SetStretchBltMode(COLORONCOLOR);

			// ���豸��ptOriginλ���ϻ�����СΪszImage��ͼ��
			::StretchDIBits(pDC->GetSafeHdc(), 
				nXPos, nYPos,			//��ʼ��
				m_SmallSize.cx,m_SmallSize.cy,			//���Ϳ�
				0, 0, 
				m_pBMIH->biWidth, m_pBMIH->biHeight,
				m_pBits, (LPBITMAPINFO) m_pBMIH, 
				DIB_RGB_COLORS, SRCCOPY);
			delete m_pBits;
			delete m_pBMIH;
			bHaveBmp = TRUE;
			break;
		}
		delete m_pBits;
		delete m_pBMIH;

	}
	pFile.Close();
	return;
}

//�滭ͷ��
void __cdecl CUserFaceRes::DrawNormalFace(CDC * pDC, int nXPos, int nYPos, LPCTSTR pszFace, int cbGender)
{
	m_HeadCS.Lock();
	int n=count(ImgFileList.begin(), ImgFileList.end(), pszFace);
	if (n<1)
	{
		int tn=count(TempFileList.begin(), TempFileList.end(), pszFace);
		if (tn<1 && strcmp(pszFace,"NoBmp")!=0)
		{
			TempFileList.push_back(pszFace);
		}
		m_HeadCS.Unlock();
		char ImgFile[128];
		if (cbGender==0)
		{
			sprintf(ImgFile,"PublicRes/male.bmp");
		}
		else
		{
			sprintf(ImgFile,"PublicRes/female.bmp");
		}
		CFile pFile;
		pFile.Open(ImgFile,CFile::modeReadWrite| CFile::typeBinary);

		BITMAPFILEHEADER bmfh;

		// ����1����ȡ�ļ�ͷ

		int nCount = pFile.Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));

		// �ж��Ƿ���BMP��ʽ��λͼ
		if(bmfh.bfType != BMP_HEADER_MARKER) 
		{
			return;
		}

		// ������Ϣͷ���ϵ�ɫ��Ĵ�С�����ڴ�
		int nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		LPBITMAPINFOHEADER m_pBMIH = (LPBITMAPINFOHEADER) new BYTE[nSize];

		// ����2����ȡ��Ϣͷ�͵�ɫ��
		nCount = pFile.Read(m_pBMIH, nSize); 

		// ����3����ȡͼ������
		LPBYTE	m_pBits = (LPBYTE) new BYTE[m_pBMIH->biSizeImage];
		nCount = pFile.Read(m_pBits, m_pBMIH->biSizeImage); 
		pFile.Close();
		
		::StretchDIBits(pDC->GetSafeHdc(),
			nXPos,nYPos,
			m_NormalSize.cx,m_NormalSize.cy,
			0,0,
			m_pBMIH->biWidth,m_pBMIH->biHeight,
			m_pBits,(LPBITMAPINFO) m_pBMIH,
			DIB_RGB_COLORS,SRCCOPY);

		delete m_pBits;
		delete m_pBMIH;
		
		return;
	}

	m_HeadCS.Unlock();
	CString DATFile=TEXT("FaceImage.DAT");

	CFileFind	finder;
	BOOL	FF;
	BOOL    bHaveBmp=FALSE;

	FF=finder.FindFile(DATFile);
	if(!FF){
		return;
	}

	CFile pFile;
	pFile.Open(DATFile,CFile::modeRead);

	char FileNum[10];
	pFile.Read(FileNum,9);
	long NUM = atol(FileNum);
	for(long j=0;j<NUM;j++){	

		BITMAPFILEHEADER bmfh;

		// ����1����ȡ�ļ�ͷ
		char filename[128];
		pFile.Read(filename,127);

		int nCount = pFile.Read((LPVOID) &bmfh, sizeof(BITMAPFILEHEADER));

		// �ж��Ƿ���BMP��ʽ��λͼ
		if(bmfh.bfType != BMP_HEADER_MARKER) 
		{
			continue;
		}

		// ������Ϣͷ���ϵ�ɫ��Ĵ�С�����ڴ�
		int nSize = bmfh.bfOffBits - sizeof(BITMAPFILEHEADER);
		LPBITMAPINFOHEADER m_pBMIH = (LPBITMAPINFOHEADER) new BYTE[nSize];

		// ����2����ȡ��Ϣͷ�͵�ɫ��
		nCount = pFile.Read(m_pBMIH, nSize); 

		// ����3����ȡͼ������
		LPBYTE	m_pBits = (LPBYTE) new BYTE[m_pBMIH->biSizeImage];
		nCount = pFile.Read(m_pBits, m_pBMIH->biSizeImage); 

		if (strcmp(filename,pszFace)==0)
		{

			int PaletteSize = 0;

			if(m_pBMIH->biClrUsed == 0) 	// ��ɫ��Ĵ�СΪ2��biBitCount�η�		
			{
				switch(m_pBMIH->biBitCount)
				{
				case 1:
					PaletteSize = 2;
					break;
				case 4:
					PaletteSize = 16;
					break;
				case 8:
					PaletteSize = 256;
					break;
				case 16:
				case 24:
				case 32:
					PaletteSize = 0;
					break;
				default:
					PaletteSize = 0;
				}
			}	
			else // ��ɫ��Ĵ�СΪʵ���õ�����ɫ��
			{
				PaletteSize = m_pBMIH->biClrUsed;
			}

			LPLOGPALETTE pLogPal = (LPLOGPALETTE) new char[2 * sizeof(WORD) 
				+ PaletteSize * sizeof(PALETTEENTRY)];

			// �����߼���ɫ�����Ϣ
			pLogPal->palVersion = 	0x300;
			pLogPal->palNumEntries = PaletteSize;

			// ����DIB�е���ɫ���߼���ɫ��
			LPRGBQUAD pDibQuad = (LPRGBQUAD) (LPVOID) (m_pBMIH + sizeof(BITMAPINFOHEADER));
			for(int i = 0; i < PaletteSize; ++i) 
			{
				pLogPal->palPalEntry[i].peRed = pDibQuad->rgbRed;
				pLogPal->palPalEntry[i].peGreen = pDibQuad->rgbGreen;
				pLogPal->palPalEntry[i].peBlue = pDibQuad->rgbBlue;
				pLogPal->palPalEntry[i].peFlags = 0;
				pDibQuad++;
			}

			HPALETTE            m_hPalette;			//��ɫ����	

			// ���ʹ�õ�ɫ�壬�򽫵�ɫ��ѡ���豸������
			m_hPalette = ::CreatePalette(pLogPal);

			::SelectPalette(pDC->GetSafeHdc(), m_hPalette, TRUE);

			// ������ʾģʽ
			pDC->SetStretchBltMode(COLORONCOLOR);

			// ���豸��ptOriginλ���ϻ�����СΪszImage��ͼ��
			::StretchDIBits(pDC->GetSafeHdc(), 
				nXPos, nYPos,			//��ʼ��
				m_NormalSize.cx,m_NormalSize.cy,			//���Ϳ�
				0, 0, 
				m_pBMIH->biWidth, m_pBMIH->biHeight,
				m_pBits, (LPBITMAPINFO) m_pBMIH, 
				DIB_RGB_COLORS, SRCCOPY);
			delete m_pBits;
			delete m_pBMIH;
			bHaveBmp = TRUE;
			break;
		}
		delete m_pBits;
		delete m_pBMIH;

	}
	pFile.Close();
	return;
}


//͸���滭
bool CUserFaceRes::AlphaDrawImage(CDC * pDestDC, int xDest, int yDest, int cxDest, int cyDest, CDC * pScrDC, int xSrc, int ySrc)
{
	//Ч�����
	ASSERT(pScrDC!=NULL);
	ASSERT(pDestDC!=NULL);

	//���� DC
	pScrDC->SetBkColor(COLOR_TRANS);
	pScrDC->SetBkMode(TRANSPARENT);

	//��������ͼ
	CDC DCMask;
	CBitmap BMPMask;
	BMPMask.CreateBitmap(cxDest,cyDest,1,1,NULL);
	DCMask.CreateCompatibleDC(pDestDC);
	DCMask.SelectObject(&BMPMask);
	DCMask.BitBlt(0,0,cxDest,cyDest,pScrDC,xSrc,ySrc,SRCCOPY);

	//�滭Ŀ��ͼ
	CDC DCDest;
	CBitmap BMPDest;
	DCDest.CreateCompatibleDC(pDestDC);
	BMPDest.CreateCompatibleBitmap(pDestDC,cxDest,cyDest);
	DCDest.SelectObject(&BMPDest);
	DCDest.BitBlt(0,0,cxDest,cyDest,pDestDC,xDest,yDest,SRCCOPY);
	DCDest.BitBlt(0,0,cxDest,cyDest,pScrDC,xSrc,ySrc,SRCINVERT);
	DCDest.BitBlt(0,0,cxDest,cyDest,&DCMask,0,0,SRCAND);
	DCDest.BitBlt(0,0,cxDest,cyDest,pScrDC,xSrc,ySrc,SRCINVERT);
	pDestDC->BitBlt(xDest,yDest,cxDest,cyDest,&DCDest,0,0,SRCCOPY);

	//������Դ
	DCMask.DeleteDC();
	DCDest.DeleteDC();
	BMPMask.DeleteObject();
	BMPDest.DeleteObject();

	return true;
}
//////////////////////////////////////////////////////////////////////////

//����������
extern "C" __declspec(dllexport) void * __cdecl CreateUserFaceRes(const GUID & Guid, DWORD dwInterfaceVer)
{
	//��������
	CUserFaceRes * pUserFaceRes=NULL;
	try
	{
		pUserFaceRes=new CUserFaceRes();
		if (pUserFaceRes==NULL) throw TEXT("����ʧ��");
		void * pObject=pUserFaceRes->QueryInterface(Guid,dwInterfaceVer);
		AfxBeginThread(DownLoad,NULL);
		if (pObject==NULL) throw TEXT("�ӿڲ�ѯʧ��");
		return pObject;
	}
	catch (...) {}

	//�������
	SafeDelete(pUserFaceRes);
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
