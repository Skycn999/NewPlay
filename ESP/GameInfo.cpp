#include "common.hpp"
#include "GameInfo.h"

using namespace std;


GameInfo::GameInfo(PGameMemHelper *pMem):pMem(pMem),min_distance(1000)
{
	
}
FCameraCacheEntry GameInfo::GetCameraCache()
{
	FCameraCacheEntry cce;
	auto playercontroller = pMem->RPM<DWORD_PTR>(pLocalPlayer + 0x30, 8);
	cce = pMem->RPM<FCameraCacheEntry>(pMem->RPM<DWORD_PTR>(playercontroller + 0x438, 0x8) + 0x410, sizeof(FCameraCacheEntry));
	return cce;
}

void GameInfo::UpdateAddress()
{
	pUWorld = pMem->RPM_T<DWORD_PTR>(WORLD_OFFSET, 8);
	pGameInstance = pMem->RPM<DWORD_PTR>(pUWorld + 0x140, 8);
	pULocalPlayer = pMem->RPM<DWORD_PTR>(pGameInstance + 0x38, 8);
	pLocalPlayer = pMem->RPM<DWORD_PTR>(pULocalPlayer + 0x0, 8);
	pViewportClient = pMem->RPM<DWORD_PTR>(pLocalPlayer + 0x58, 8);

	pPWorld = pMem->RPM<DWORD_PTR>(pViewportClient + 0x80, 8);
	pGameInstance = pMem->RPM<DWORD_PTR>(pPWorld + 0x140, 8);
	pULocalPlayer = pMem->RPM<DWORD_PTR>(pGameInstance + 0x38, 8);
	pLocalPlayer = pMem->RPM<DWORD_PTR>(pULocalPlayer + 0x0, 8);
	cameracache = GetCameraCache();
}

void GameInfo::update() {
	if (pMem == NULL)
		return;
	UpdateAddress();
	local = GetLocalPlayerPos();

	wchar_t entityname[64] = { NULL };
	DWORD_PTR enlist = pMem->RPM<DWORD_PTR>(pMem->RPM<DWORD_PTR>(pPWorld + 0x30, 0x8) + 0xA0, 0x8);
	int entitycount = pMem->RPM<int>(pMem->RPM<DWORD_PTR>(pPWorld + 0x30, 0x8) + 0xA8, 0x4);

	this->player_num = 0;
	for (int i = 0; i < entitycount; i++)
	{
		ZeroMemory(entityname, sizeof(entityname));
		auto entity = pMem->RPM<DWORD_PTR>(enlist + (i * 0x8), 0x8);
		
		if (!entity)
			continue;

		if (entity == pMem->RPM<DWORD_PTR>(pMem->RPM<DWORD_PTR>(pLocalPlayer + 0x30, 0x8) + 0x418, 0x8))
			continue;

		int id = pMem->RPM<int>(entity + 0x18, 0x4);
		if (id == ActorIds[0] || id == ActorIds[1] || id == ActorIds[2] || id == ActorIds[3])
		{
			player[player_num].health = pMem->RPM<float>(entity + 0x107C, 0x4);
			auto rootcomp = pMem->RPM<DWORD_PTR>(entity + 0x180, 8);
			player[player_num].pos = pMem->RPM<Vector3>((DWORD_PTR)rootcomp + 0x174, 0xC);
			player[player_num].distance = local.Distance(player[player_num].pos) / 100.f;
			player_num++;
		}
	}

	if (pUWorld != pPWorld) {
		cout << "Two different worlds" << endl;
		cout << "Uword:" << pUWorld << endl;
		cout << "Pword:" << pPWorld << endl;
	}
}

GameInfo::~GameInfo()
{
}

std::string GameInfo::GetNameFromId(int ID)
{
	DWORD_PTR fNamePtr = pMem->RPM<DWORD_PTR>(pMem->RPM_T<DWORD_PTR>(NAME_OFFSET, 0x8) + int(ID / 0x4000) * 8, 8);
	DWORD_PTR fName = pMem->RPM<DWORD_PTR>(fNamePtr + 8 * int(ID % 0x4000), 8);
	
	char name[64] = { NULL };
	pMem->Read((uint64_t)(fName + 16), (uint8_t*)name, sizeof(name));
	
	return std::string(name);
}

#if 1
void GameInfo::CacheNames()
{
	string name = "";
	for (int i = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0, c6 = 0, c7 = 1; i < 200000; i++)
	{
		if (c1 == 4 && c2 == 3 && c3 == 4 && c4 == 4 && c5 == 3 && boat != 0 && c6 == 1 && c7 == 2)
		{
			cout << "Get ID Done" << endl;
			return;
		}
		name = GetNameFromId(i);
	
		if (name == "PlayerFemale_A" || name == "PlayerFemale_A_C" || name == "PlayerMale_A" || name == "PlayerMale_A_C")
		{
			cout << name << " " << i << endl;
			ActorIds[c1++] = i;
		}
		if (name == "Uaz_A_01_C" || name == "Uaz_B_01_C" || name == "Uaz_C_01_C")
		{
			uaz[c2++] = i;
			cout << name << " " << i << endl;
		}
		if (name == "Dacia_A_01_C" || name == "Dacia_A_02_C" || name == "Dacia_A_03_C" || name == "Dacia_A_04_C")
		{
			dacia[c3++] = i;
			cout << name << " " << i << endl;
		}
		if (name == "ABP_Motorbike_03_C" || name == "ABP_Motorbike_04_C" || name == "BP_Motorbike_03_C" || name == "BP_Motorbike_04_C")
		{
			cout << name << " " << i << endl;
			motorbike[c4++] = i;
		}
		if (name == "Buggy_A_01_C" || name == "Buggy_A_02_C" || name == "Buggy_A_03_C")
		{
			cout << name << " " << i << endl;
			buggy[c5++] = i;
		}
		if (name == "Boat_PG117_C")
		{
			cout << name << " " << i << endl;
			boat = i;
		}
		//if (name == "DroppedItemInteractionComponent" || name == "DroppedItemGroup")
		if (name == "DroppedItemGroup")
		{
			cout << name << " " << i << endl;
			itemtype[c6++] = i;
		}
		if (name == "DroppedItemInteractionComponent")
		{
			cout << name << " " << i << endl;
			itemtype[c7++] = i;
		}
	}
}
#else
void GameInfo::CacheNames()
{
}
#endif

Vector3 GameInfo::GetLocalPlayerPos()
{
	Vector3 pos;
	pos = pMem->RPM<Vector3>(pLocalPlayer + 0x70, 0xC);
	return pos;
}
