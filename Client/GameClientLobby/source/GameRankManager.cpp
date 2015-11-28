
#include "Include/StdAfx.h"
#include "../Include/GameRankManager.h"


typedef LPCTSTR (*PRANK)(LONG lScore);


CGameRankManager::CGameRankManager(void)
{
}

CGameRankManager::~CGameRankManager(void)
{
}

//
//�ӿڲ�ѯ
//
void * __cdecl CGameRankManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameRankManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameRankManager,Guid,dwQueryVer);
	return NULL;
}


//kindid
//1			1		��������Ϸ	Lord.exe		196610	100	1
//
//��ȡ�ȼ�
//

//
//�������
//

LPCTSTR __cdecl CGameRankManager::GetGameRank(WORD wKindID, 
											  LONG lScore, 
											  WORD wGameGenre)
{

	switch (wKindID)
	{
	case 1:
		{
			return GetLanddRank(lScore);
		}
	default:
		{
			return GetGoRank(lScore);
		}
	}

	return NULL;
}

LPCTSTR __cdecl CGameRankManager::GetCurrencyRank(LONG lScore)
{
/*
	LONG lScoreInfo[] = 
	{
	99L,		199L,		499L,		1499L,
	3499L,		7499L,		15499L,		47499L,
	111499L,	239499L,	495499L,	1007499L,
	2031499L,	4063499L,	6563499L,	9565499L,
	13563499L,	23563499L,	38563499L,	58563499L,
	83563499L,	93563499L,	103563499L
	};

	LPCTSTR pszOrderName[] =
	{
		_T("�Ա"),		_T("������鳤"),	_T("����鳤"),	_T("���������"),
		_T("�����"),		_T("֧�и��Ƴ�"),	_T("֧�пƳ�"),		_T("֧�и��г�"),
		_T("֧���г�"),		_T("���и�����"),	_T("���д���"),		_T("���и��г�"),
		_T("�����г�"),		_T("ʡ�и�����"),	_T("ʡ�д���"),		_T("ʡ�в����ܼ�"),
		_T("ʡ�и��г�"),	_T("ʡ���г�"),		_T("���и�����"),	_T("���в���"),
		_T("���в����ܼ�"),	_T("���и��г�"),	_T("�����г�")
	};

	for(int i=0; i<23; i++)	
	{
		if(lScore<lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return _T("��������");
*/
/*
���
����

<=2000
2000		-	3999
4000		-	7999
8000		-	19999

20000		-	39999
40000		-	79999
80000		-	149999
150000		-	299999

300000		-	499999
500000		-	999999
1000000		-	1999999
2000000		-	4999999

5000000		-	9999999
10000000	-	49999999
50000000	-	99999999
100000000	-	499999999

500000000	-	999999999
10��		-	4999999999 
50��		-	9999999999
100������
*/
	static __int64 lScoreInfo[19] = 
	{
	2000L,		4000,		8000,		20000,		
	40000L,		80000L,		150000L,		300000L,		
	500000L,		1000000L,	2000000L,	5000000L,	
	10000000L,	50000000L,	100000000L,	500000000L,	
	1000000000L,	5000000000L,	10000000000L
	};
	LPCTSTR pszOrderName[20] = 
	{
	_T("��ũ"),		_T("�軧"),		_T("�͹�"),		_T("������"),
	_T("ũ����"),	_T("����"),		_T("�����"),	_T("����"),
	_T("����"),		_T("�����"),	_T("����"),		_T("����"),
	_T("С����"),	_T("�����"),	_T("�Ĺ�"),		_T("����"),
	_T("����"),		_T("��ʥ"),		_T("����"),		_T("ְҵ����")
	};

	for(int i=0; i<19; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[19];
}

LPCTSTR __cdecl CGameRankManager::GetShowHandRank(LONG lScore)
{
	static __int64 lScoreInfo[19] = 
	{
	2000L,		4000,		8000,		20000,		
	40000L,		80000L,		150000L,		300000L,		
	500000L,		1000000L,	2000000L,	5000000L,	
	10000000L,	50000000L,	100000000L,	500000000L,	
	1000000000L,	5000000000L,	10000000000L
	};
	LPCTSTR pszOrderName[20] = 
	{
	_T("��ũ"),		_T("�軧"),		_T("�͹�"),		_T("������"),
	_T("ũ����"),	_T("����"),		_T("�����"),	_T("����"),
	_T("����"),		_T("�����"),	_T("����"),		_T("����"),
	_T("С����"),	_T("�����"),	_T("�Ĺ�"),		_T("����"),
	_T("����"),		_T("��ʥ"),		_T("����"),		_T("ְҵ����")
	};

	for(int i=0; i<19; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[19];
}

//
//240 ��
//
LPCTSTR __cdecl CGameRankManager::GetTFZGameRank(LONG lScore)
{
	static __int64 lScoreInfo[27] = 
	{
	51L,		101L,		151L,		251,		
	401,		651,		1151,		1801,		
	2951,		4751,		7701,		12451,	
	20151,		32601L,		52751L,		85351L,	
	138101L,		223451L,		361551L,		585001L,
	946551L,		1431551L,	2378101L,	3809651L,
	6187751L,	9997401L,	16185151L
	};
	LPCTSTR pszOrderName[28] = 
	{
	_T("��ҵ"),		_T("��Ա"),		_T("�Ƴ�����"),	_T("���Ƴ�"),
	_T("��һ����"),	_T("���Ƴ�"),	_T("��������"),	_T("������"),
	_T("��һ����"),	_T("������"),	_T("�ֳ�����"),	_T("���ֳ�"),
	_T("��һ����"),	_T("���ֳ�"),	_T("��������"),	_T("������"),
	_T("��һ����"),	_T("������"),	_T("��������"),	_T("������"),
	_T("��һ����"),	_T("������"),	_T("��������"),_T("������"),
	_T("��һ������"),_T("����"),	_T("����ϯ"),	_T("��ϯ")
	};

	for(int i=0; i<27; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[27];
}

//
//�����
//

LPCTSTR __cdecl CGameRankManager::GetSanDaHaRank(LONG lScore)
{
	static __int64 lScoreInfo[19] = 
	{
	500	,		501,		1001,		2001,		
	3001,		4001,		5001,		7501,		
	10001,		15001,		20001,		25001,	
	30001,		40001L,		60001L,		80001L,	
	100001L,		150001L,		200001L
	};
	LPCTSTR pszOrderName[20] = 
	{
	_T("ƽ��"),		_T("�Ӿ�ƷѲ��"),		_T("��Ʒ����"),		_T("�Ӱ�Ʒѵ��"),
	_T("��Ʒ��ة"),	_T("����Ʒ����"),		_T("��Ʒ֪��"),		_T("����Ʒ��ͬ"),
	_T("��Ʒͨ��"),	_T("����Ʒ֪��"),		_T("��Ʒ֪ͬ"),		_T("����Ʒ֪��"),
	_T("��Ʒ��Ա"),	_T("����Ʒ����ʹ"),		_T("��Ʒ����ʹ"),	_T("�Ӷ�ƷѲ��"),
	_T("��Ʒ�ܶ�"),	_T("��һƷ����ʷ"),		_T("һƷ��ѧʿ"),	_T("����Ԫ��")
	};

	for(int i=0; i<19; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[19];
}


//
//��ɳ�齫
//
LPCTSTR __cdecl CGameRankManager::GetSparrowCSRank(LONG lScore)
{
	static __int64 lScoreInfo[16] = 
	{
	100L,		200,		500,		1000,		
	2000,		5000,		10000,		20000,		
	30000,		70000L,		100000L,		200000L,	
	500000L,		700000L,		1000000L,	2000000L,	
	};
	LPCTSTR pszOrderName[17] = 
	{
	_T("��Ʒ����"),	_T("��Ʒ����"),		_T("��Ʒ����"),		_T("��Ʒ����"),
	_T("��Ʒ����"),	_T("��Ʒ����"),		_T("��Ʒ����"),		_T("��Ʒ����"),
	_T("��Ʒ����"),	_T("��Ʒ����"),		_T("��Ʒ����"),		_T("��Ʒ����"),
	_T("��Ʒ����"),	_T("��Ʒ����"),		_T("������"),		_T("�𹬵������"),
	_T("��ɳ������")
	};

	for(int i=0; i<16; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[16];
}


//
//����
//
LPCTSTR __cdecl CGameRankManager::GetUncoverPigRank(LONG lScore)
{
/*
����
����
<=-5000000
-4999999	-	-1000000
-999999		-	-200000
-199999		-	-100000
-99999		-	0
1			-	4999
5000		-	19999
20000		-	59999
60000		-	99999
100000		-	149999
150000		-	239999
240000		-	379999
380000		-	549999
550000		-	699999
700000		-	999999
1000000		-	2799999
2800000		-	4999999
5000000		-	9999999
10000000	-	19999999
20000000����
*/
	LONG lScoreInfo[19] = 
	{
	-5000000,	-1000000,	-200000,	-100000,
	0,			5000,		20000,		60000,
	100000,		150000,		240000,		380000,
	550000,		700000,		1000000,	2800000,
	5000000,	10000000,	20000000
	};
	LPCTSTR pszOrderName[20] = 
	{
	_T("������ʥ"),		_T("��������"),	_T("��������"),		_T("ũ������"),
	_T("��С��"),		_T("С����"),	_T("ҵ������Ա"),	_T("��������Ա"),
	_T("����רҵ��"),	_T("������"),	_T("����״Ԫ"),		_T("����ר��"),
	_T("�������"),		_T("ũ���鳤"),	_T("ũ������"),		_T("Сũ����"),
	_T("��ũ����"),		_T("С�ʱ���"),	_T("���ʱ���"),		_T("����һ��")
	};

	for(int i=0; i<19; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[19];
}

//
//����
//
LPCTSTR __cdecl CGameRankManager::GetUpgradeRank(LONG lScore)
{
/*
����
����

<=0
1		-	29
30		-	69
70		-	119

120		-	179
180		-	259
260		-	319
320		-	399

400		-	599
600		-	799
800		-	999
1000	-	1199

1200	-	1499
1500	-	1999
2000	-	2799
2800	-	3999

4000	-	7999
8000	-	14999
15000	-	29999
3�������
*/
	LONG lScoreInfo[19] = 
	{
	0,			30,			70,			120,
	180,		260,		320,		400,
	600,		800,		1000,		1200,
	1500,		2000,		2800,		4000,
	8000,		15000,		30000
	};

	LPCTSTR pszOrderName[20] = 
	{
	_T("ƽ��"),			_T("ͯ��"),		_T("���"),		_T("����"),
	_T("��ʿ"),			_T("̽��"),		_T("����"),		_T("״Ԫ"),
	_T("��Ʒ֥���"),	_T("��Ʒ��ة"),	_T("��Ʒ֪��"),	_T("��Ʒͨ��"),
	_T("��Ʒ֪��"),		_T("��Ʒ֪��"),	_T("��Ʒ��ʷ"),	_T("��Ʒ����"),
	_T("һƷ��ѧʿ"),	_T("����"),		_T("��ү"),		_T("�ʵ�")
	};

	for(int i=0; i<19; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[19];
}

//
//������
//
LPCTSTR __cdecl CGameRankManager::GetSDragonBallRank(LONG lScore)
{
/*
������
1 0 ���� 
2 50 ������ 
3 100 ���� 
4 200 ���� 
5 500 Ѽ���� 
6 800 ˫���� 
7 1200 Ѹ�� 
8 2000 ���� 
9 3000 ���� 
10 5000 ���� 
11 8000 ���� 
12 12000 �ƽ��� 
13 15000 ������ 
14 18000 ����ڤ�� 
15 23000 �Ϻ����� 
16 30000 ����ħ�� 
17 40000 �������� 
18 60000 �л�ʥ��  
*/
	static __int64 lScoreInfo[18] = 
	{
	0,		50,		100,	200,
	500,	800,	1200,	2000,
	3000,	5000,	8000,	12000,
	15000,	18000,	23000,	30000,
	40000,	60000
	};

	LPCTSTR pszOrderName[18] = 
	{
	_T("����"),			_T("������"),		_T("����"),		_T("����"),
	_T("Ѽ����"),		_T("˫����"),		_T("Ѹ��"),		_T("����"),
	_T("����"),			_T("����"),			_T("����"),		_T("�ƽ���"),
	_T("������"),		_T("����ڤ��"),		_T("�Ϻ�����"),	_T("����ħ��"),
	_T("��������"),		_T("�л�ʥ��")
	};

	for(int i=0; i<18; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[17];
}

//
//�ڿ�
//
LPCTSTR __cdecl CGameRankManager::GetCavernRank(LONG lScore)
{
/*
�ڿ�
����
<=0
1		-	29
30		-	59
60		-	99

100		-	179
180		-	339
340		-	659
660		-	1299

1300	-	2579
2580	-	5139
5140	-	10259
10260	-	20499

20500	-	40979
40980	-	81939
81940	-	163859
163860	-	327699

327700	-	655379
655380	-	1310739
1310740	-	2621459
2621460����
*/
	LONG lScoreInfo[19] = 
	{
	0,		30,		60,		100,	
	180,	340,	660,	1300,	
	2580,	5140,	10260,	20500,	
	40980,	81940,	163860,	327700,	
	655380,	1310740,2621460
	};
	LPCTSTR pszOrderName[20] = 
	{
	_T("����"),		_T("˶��"),			_T("����"),		_T("������"),
	_T("�����"),	_T("�ھ���"),		_T("�Խ���"),	_T("���¹�"),
	_T("��ͷ��"),	_T("��̽��"),		_T("������"),	_T("����������"),
	_T("�ھ��"),	_T("�����ھ��"),	_T("�꾮��"),	_T("�����꾮��"),
	_T("�ڿ����"),	_T("�ڿ�ר��"),		_T("�ڿӾ�Ӣ"),	_T("�ڿӴ�ʦ")
	};

	for(int i=0; i<19; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[19];
}

//
//�����
//
LPCTSTR __cdecl CGameRankManager::GetHoeRank(LONG lScore)
{
/*
�����
����
<=0 
1		-	99
100		-	399
400		-	899

900		-	1599
1600	-	2499
2500	-	3699
3700	-	5499

5500	-	7699
7700	-	10499
10500	-	15499
15500	-	22499

22500	-	31499
31500	-	46499
46500	-	66499
66500	-	106499

106500	-	186499
186500	-	286499
286500	-	572999
573000����
*/
	LONG lScoreInfo[19] = 
	{
	0,		100,	400,	900,	
	1600,	2500,	3700,	5500,	
	7700,	10500,	15500,	22500,	
	31500,	46500,	66500,	106500,	
	186500,	286500,	573000
	};

	LPCTSTR pszOrderName[20] = 
	{
	_T("����"),		_T("����"),		_T("����"),		_T("��ʥ"),
	_T("��ʿ"),		_T("���۾���"),	_T("������"),	_T("��ʹ"),
	_T("������"),	_T("ս��"),		_T("ʤ����"),	_T("̫����"),
	_T("����"),		_T("�����"),	_T("����"),		_T("����"),
	_T("ڤ��"),		_T("����"),		_T("�ռ�����"),	_T("��������")
	};

	for(int i=0; i<19; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[19];
}

//
//�й�����
//
LPCTSTR __cdecl CGameRankManager::GetCChessRank(LONG lScore)
{
/*
����
����
<=0
1		-	9
10		-	29
30		-	59

60		-	99
100		-	199
200		-	349
350		-	549

550		-	749
750		-	999
1000	-	1299
1300	-	1599

1600	-	1999
2000	-	2499
2500	-	2999
3000	-	3999

4000	-	4999
5000	-	5999
6000	-	7999
8000����
*/

	LONG lScoreInfo[19] = 
	{
	0,		10,		30,		60,
	100,	200,	350,	550,
	750,	1000,	1300,	1600,
	2000,	2500,	3000,	4000,	
	5000,	6000,	8000
	};
	LPCTSTR pszOrderName[20] = 
	{
	_T("����"),		_T("����ѧͽ"),	_T("�м�ѧͽ"),	_T("�߼�ѧͽ"),
	_T("ѧ�ɳ�ʦ"),	_T("�ż���ʿ"),	_T("�˼���ʿ"),	_T("�߼���ʿ"),
	_T("������ʿ"),	_T("�弶��ʿ"),	_T("�ļ���ʿ"),	_T("������ʿ"),
	_T("������ʿ"),	_T("һ����ʿ"),	_T("������ʦ"),	_T("������ʦ"),
	_T("һ����ʦ"),	_T("�ؼ���ʦ"),	_T("���Ҵ�ʦ"),	_T("���ʴ�ʦ")
	};

	for(int i=0; i<19; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[19];
}

//
//ɫ��
//
LPCTSTR __cdecl CGameRankManager::GetSeZhongRank(LONG lScore)	
{
	__int64 lScoreInfo[20] = 
	{
	1000,		2000,		4000,		8000,
	20000,		40000,		80000,		150000,
	300000,		500000,		1000000,	2000000,
	5000000,	10000000,	50000000,	100000000,
	500000000,	1000000000,	5000000000,	10000000000
	};
	LPCTSTR pszOrderName[20] = 
	{
	_T("��ѧ��"),	_T("ɢ��"),		_T("Ͷ����"),	_T("������"),
	_T("������"),	_T("�м�"),		_T("ר��"),		_T("��Ӣ"),
	_T("��ʦ"),		_T("���ɴ�ʦ"),	_T("��թ��ʦ"),	_T("������ʦ"),
	_T("�Ѱ�"),		_T("����"),		_T("����"),		_T("����"),
	_T("����"),		_T("��ʥ"),		_T("����"),		_T("����")
	};

	for(int i=0; i<19; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[19];
}

LPCTSTR __cdecl CGameRankManager::GetFourEnsignRank(LONG lScore)
{
	
	LONG lScoreInfo[21] = 
	{
	30,		120,	270,
	480,	750,	1080,	1470,
	1920,	2430,	3000,	3630,
	4320,	5070,	5880,	6750,	
	7680,	8670,	9720,	10830,
	12000,	13230
	};
	LPCTSTR pszOrderName[22] = 
	{
	_T("����"),		_T("���೤"),	_T("���೤"),	_T("���ų�"),
	_T("���ų�"),	_T("������"),	_T("������"),	_T("��Ӫ��"),
	_T("��Ӫ��"),	_T("���ų�"),	_T("���ų�"),	_T("���ó�"),
	_T("���ó�"),	_T("��ʦ��"),	_T("��ʦ��"),	_T("������"),
	_T("������"),	_T("��˾��"),	_T("��˾��"),	_T("��Ԫ˧"),
	_T("��Ԫ˧"),	_T("ͳ˧")
	};

	for(int i=0; i<21; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[21];
}
//
//������
//

LPCTSTR __cdecl CGameRankManager::GetDoShouQiRank(LONG lScore)
{
	
	LONG lScoreInfo[16] = 
	{
	10,		50,		80,		110,
	150,	200,	300,	400,
	500,	700,	800,    900,	
	1000,	1500,	2000,	3000
	};
	LPCTSTR pszOrderName[17] = 
	{
	_T("�汦��"),	   _T("�׳�"),	     _T("С�ɳ�"),	    _T("�ɳ�"),
	_T("���ǳ�"),	   _T("�����"),	 _T("�����"),      _T("�����"),
	_T("ս��"),  	   _T("���"),	     _T("�ҵ����"),	_T("������"),	
	_T("�������"),    _T("�������"),	 _T("���ͷ��"),	_T("�޵����"),
	_T("�޵����")	
	};

	for(int i=0; i<16; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[16];
}

//Χ��
LPCTSTR __cdecl CGameRankManager::GetGoRank(LONG lScore)
{
	
	LONG lScoreInfo[18] = 
	{
	-800,	-700,		-600,		-500,
    -400,	-300,	-200,	-100,
	0,	100,	200,	300,
	400,	500,	600,	700,	
	800,900
	};
	LPCTSTR pszOrderName[19] = 
	{
	_T("�ż�"),	_T("�˼�"),	_T("�߼�"),	_T("����"),
	_T("�弶"),	 _T("�ļ�"),	_T("����"),	_T("����"),
	_T("һ��"),	_T("һ��"),	_T("����"),	_T("����"),
	_T("�Ķ�"),	_T("���"),	_T("����"),	_T("�߶�"),
	_T("�˶�"),	_T("�Ŷ�"),_T("��ʥ")
	};

	for(int i=0; i<18; i++)	
	{
		if(lScore < lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[18];
}

//
//������
//
LPCTSTR __cdecl CGameRankManager::GetLanddRank(LONG lScore)
{
/*
������
����

<=0
1		-	49
50		-	99
100		-	149

150		-	199
200		-	299
300		-	499
500		-	799

800		-	1199
1200	-	1699
1700	-	2299
2300	-	2999

3000	-	3999
4000	-	5999
6000	-	8999
9000	-	11999

12000	-	19999
20000	-	49999
50000	-	99999
10�������
*/
	static __int64 lScoreInfo[19] = 
	{
	0,		50,		100,	150,
	200,	300,	500,	800,
	1200,	1700,	2300,	3000,
	4000,	6000,	9000,	12000,
	20000,	50000L,	100000L
	};

	LPCTSTR pszOrderName[20] = 
	{
	_T("��ؤ"),		_T("С��"),		_T("�̹�"),		_T("����"),
	_T("�軧"),		_T("ƶũ"),		_T("��ũ"),		_T("��ũ"),
	_T("��ũ"),		_T("С����"),	_T("�����"),	_T("����"),
	_T("����"),		_T("С�ʱ���"),	_T("���ʱ���"),	_T("���ڼ�"),
	_T("���м�"),	_T("����"),		_T("��������"),	_T("�׸�")
	};

	for(int i=0; i<19; i++)	
	{
		if(lScore <= lScoreInfo[i]) 
		{
			return pszOrderName[i];
		}
	}

	return pszOrderName[19];
}

LPCTSTR __cdecl CGameRankManager::GetRunFastRank(LONG lScore)
{
	return _T("");
}

LPCTSTR __cdecl CGameRankManager::GetDefaultRank(LONG lScore)
{
	return _T("");
}


//
//����������
//
extern "C" __declspec(dllexport) 
void * __cdecl CreateGameRankManager(const GUID & Guid, 
									 DWORD dwInterfaceVer)
{
	//��������
	CGameRankManager *pGameRankManager=NULL;
	try
	{
		pGameRankManager = new CGameRankManager();
		if(pGameRankManager==NULL) 
		{
			throw _T("����ʧ��");
		}

		void *pObject = pGameRankManager->QueryInterface(Guid,dwInterfaceVer);
		if(pObject==NULL) 
		{
			throw _T("�ӿڲ�ѯʧ��");
		}
		return pObject;
	}
	catch (...) 
	{
	}

	//�������
	SafeDelete(pGameRankManager);
	return NULL;
}


//-----------------------------------------------
//					the end
//-----------------------------------------------
