#pragma once

#include "GameFrameVideo.h"
#include "videoclient.h"

class GAMEFRAMEWORK_CLASS CGameVideo
{
private:
	int m_WindowNum; //������
	CWnd* m_pParent; //������
	CGameFrameVideo **m_pGameFrameVideo; //����ָ��
	//CVideoClient* m_pVideoClient; //��Ƶ�ͻ���
	
public:
	CVideoClient	*m_pVideoClient;
	CGameVideo(void);
	virtual ~CGameVideo(void);

	//��ʼ��
	void Init( int WindowNum, CWnd* pParent );
	void r() {return ;} ;
	//�ͷ�
	void Release();
	//���ô���
	void SetWindowPos( int Pos, int x, int y, int cx = 0, int cy = 0 );
	void ShowWindow( int nCmdShow, int Pos=-1 );
	void SetMe( int Pos ){ m_pGameFrameVideo[Pos]->SetMe(); };
	//��½
	BOOL Login( char* IP/*������IP*/, int Port/*�������˿�*/, DWORD RoomID, DWORD DeskID, DWORD Pos );
};
