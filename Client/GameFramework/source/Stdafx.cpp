// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// GameFrame.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "../Include/Stdafx.h"


#pragma comment( lib, "Winmm.lib" )
#ifdef _DEBUG
#pragma comment( lib, "..\\..\\Common\\FKUIModule\\bin\\FKUIModule_D.lib" )
#pragma comment( lib, "..\\GameClientLobby\\bin\\GameClientLobby_D.lib" )
#pragma comment( lib, "..\\..\\Common\\FKLobby\\bin\\FKLobby_D.lib" )
#else
#pragma comment( lib, "..\\..\\Common\\FKUIModule\\bin\\FKUIModule.lib" )
#pragma comment( lib, "..\\GameClientLobby\\bin\\GameClientLobby.lib" )
#pragma comment( lib, "..\\..\\Common\\FKLobby\\bin\\FKLobby.lib" )
#endif

