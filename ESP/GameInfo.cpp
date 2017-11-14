#include "common.hpp"
#include "GameInfo.h"

using namespace std;

//Constructor
GameInfo::GameInfo(PGameMemHelper *pMem):pMem(pMem)
{
	
}

//Deconstructor
GameInfo::~GameInfo()
{
}

//GetCameraCache
FCameraCacheEntry GameInfo::GetCameraCache()
{
	FCameraCacheEntry cce;
	DWORD_PTR playercontroller = pMem->RPM<DWORD_PTR>(pLocalPlayer + 0x30, 8);
	cce = pMem->RPM<FCameraCacheEntry>(pMem->RPM<DWORD_PTR>(playercontroller + 0x438, 0x8) + 0x410, sizeof(FCameraCacheEntry));
	return cce;
}

//Uworld and Pworld maybe different
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

#ifdef DEBUG
	if (pUWorld != pPWorld) {
		cout << "Two different worlds" << endl;
		cout << "Uword:" << pUWorld << endl;
		cout << "Pword:" << pPWorld << endl;
	}
#endif

}

void GameInfo::GetLoot(DWORD_PTR entity)
{
	wchar_t entityname[64] = { NULL };
	ZeroMemory(entityname, sizeof(entityname));
}
void GameInfo::GetVehicle(DWORD_PTR entity, wchar_t *name)
{
	Entity vehicle = { 0 };
	wcscpy_s(vehicle.name, name);
	auto rootcomp = pMem->RPM<DWORD_PTR>(entity + 0x180, 8);
	vehicle.pos = pMem->RPM<Vector3>((DWORD_PTR)rootcomp + 0x174, 0xC);
	vehicle.distance = local.Distance(vehicle.pos) / 100.f;
	this->vehicles.push_back(vehicle);
}
void GameInfo::GetPlayer(DWORD_PTR entity)
{
	Entity player = { 0 };
	player.health = pMem->RPM<float>(entity + 0x107C, 0x4);
	auto rootcomp = pMem->RPM<DWORD_PTR>(entity + 0x180, 8);
	player.pos = pMem->RPM<Vector3>((DWORD_PTR)rootcomp + 0x174, 0xC);
	player.distance = local.Distance(player.pos) / 100.f;
	this->players.push_back(player);
}

void GameInfo::GetAllEntity()
{
	players.clear();
	loots.clear();
	vehicles.clear();

	DWORD_PTR enlist = pMem->RPM<DWORD_PTR>(pMem->RPM<DWORD_PTR>(pPWorld + 0x30, 0x8) + 0xA0, 0x8);
	int entitycount = pMem->RPM<int>(pMem->RPM<DWORD_PTR>(pPWorld + 0x30, 0x8) + 0xA8, 0x4);
	for (int i = 0; i < entitycount; i++)
	{
		auto entity = pMem->RPM<DWORD_PTR>(enlist + (i * 0x8), 0x8);
		if (!entity)
			continue;
		//Don't show local player
		if (entity == pMem->RPM<DWORD_PTR>(pMem->RPM<DWORD_PTR>(pLocalPlayer + 0x30, 0x8) + 0x418, 0x8))
			continue;

		int id = pMem->RPM<int>(entity + 0x18, 0x4);
		if (id == ActorIds[0] || id == ActorIds[1] || id == ActorIds[2] || id == ActorIds[3])
		{
			GetPlayer(entity);
		}
		else if (id == uaz[0] || id == uaz[1] || id == uaz[2] )
		{
			GetVehicle(entity,L"¼ªÆÕ");
		}
		else if (id == dacia[0] || id == dacia[1] || id == dacia[2] || id == dacia[3])
		{
			GetVehicle(entity, L"½Î³µ");
		}
		else if (id == motorbike[0] || id == motorbike[1] || id == motorbike[2] || id == motorbike[3])
		{
			GetVehicle(entity, L"Ä¦ÍÐ³µ");
		}
		else if (id == buggy[0] || id == buggy[1] || id == buggy[2])
		{
			GetVehicle(entity, L"É³Ä®³µ");
		}
		else if (id == uaz[0] || id == uaz[1] || id == uaz[2])
		{
			GetVehicle(entity, L"¼ªÆÕ");
		}
		else if (id ==item)
		{
			GetLoot(entity);
		}

	
	}
}

//update the game info
void GameInfo::Update() {
	if (pMem == NULL)
		return;
	UpdateAddress();
	GetLocalPlayerInfo();
	GetAllEntity();
}

//Get name
std::string GameInfo::GetNameFromId(int ID)
{
	DWORD_PTR fNamePtr = pMem->RPM<DWORD_PTR>(pMem->RPM_T<DWORD_PTR>(NAME_OFFSET, 0x8) + int(ID / 0x4000) * 8, 8);
	DWORD_PTR fName = pMem->RPM<DWORD_PTR>(fNamePtr + 8 * int(ID % 0x4000), 8);
	char name[64] = { NULL };
	pMem->Read((uint64_t)(fName + 16), (uint8_t*)name, sizeof(name));
	return std::string(name);
}

//Simply encript
void GameInfo::CacheNames()
{
	string name = "";
	for (int i = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0; i < 200000; i++)
	{
		if (c1 == 4 && c2 == 3 && c3 == 4 && c4 == 4 && c5 == 3 && boat != 0 && item != 0 && itemComponent!=0)
		{
			cout << "Get ID Done" << endl;
			return;
		}
		name = GetNameFromId(i);
		name = strEncrypt(name,2);

		if (name == NAME_PLAYER1 || name == NAME_PLAYER2 || name == NAME_PLAYER3 || name == NAME_PLAYER4)
		{
			cout << name << " " << i << endl;
			ActorIds[c1++] = i;
		}
		if (name == NAME_UAZ1 || name == NAME_UAZ2 || name == NAME_UAZ3)
		{
			uaz[c2++] = i;
			cout << name << " " << i << endl;
		}
		if (name == NAME_DACIA1 || name == NAME_DACIA2 || name == NAME_DACIA3 || name == NAME_DACIA4)
		{
			dacia[c3++] = i;
			cout << name << " " << i << endl;
		}
		if (name == NAME_MOTO1 || name == NAME_MOTO2 || name == NAME_MOTO3 || name == NAME_MOTO4)
		{
			cout << name << " " << i << endl;
			motorbike[c4++] = i;
		}
		if (name == NAME_BUGGY1 || name == NAME_BUGGY2 || name == NAME_BUGGY3)
		{
			cout << name << " " << i << endl;
			buggy[c5++] = i;
		}
		if (name == NAME_BOAT)
		{
			cout << name << " " << i << endl;
			boat = i;
		}
		//if (name == "DroppedItemInteractionComponent" || name == "DroppedItemGroup")
		if (name == NAME_ITEM)
		{
			cout << name << " " << i << endl;
			item = i;
		}
		if (name == NAME_ITEM_COMPONENT)
		{
			cout << name << " " << i << endl;
			itemComponent = i;
		}
	}
}

//GetLocalPlayerPos
void GameInfo::GetLocalPlayerInfo()
{
	local = pMem->RPM<Vector3>(pLocalPlayer + 0x70, 0xC);
}
