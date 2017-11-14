#pragma once

#include "common.hpp"
#include "PhysicalMemory.h"
#include "PMemHelper.h"

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
	{

	}
	~Vector3()
	{

	}

	float x;
	float y;
	float z;

	inline float Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline float Distance(Vector3 v)
	{
		return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}
};

struct FMinimalViewInfo
{
	class Vector3                                     Location;                                                 // 0x0000(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	class Vector3                                      Rotation;                                                 // 0x000C(0x000C) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              FOV;                                                      // 0x0018(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoWidth;                                               // 0x001C(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoNearClipPlane;                                       // 0x0020(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              OrthoFarClipPlane;                                        // 0x0024(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
	float                                              AspectRatio;                                              // 0x0028(0x0004) (CPF_Edit, CPF_BlueprintVisible, CPF_ZeroConstructor, CPF_IsPlainOldData)
};

struct FCameraCacheEntry
{
	float                                              TimeStamp;                                                // 0x0000(0x0004) (CPF_ZeroConstructor, CPF_IsPlainOldData)
	unsigned char                                      UnknownData00[0xC];                                       // 0x0004(0x000C) MISSED OFFSET
	struct FMinimalViewInfo                            POV;                                                      // 0x0010(0x0530)
};


struct Entity {
	wchar_t name[64];
	int health;
	int distance;
	Vector3 pos;
};

class GameInfo
{
public:
	GameInfo(PGameMemHelper *pMem);
	~GameInfo();
	void CacheNames();

	void Update();

private:
	inline void UpdateAddress();
	inline FCameraCacheEntry GetCameraCache();
	inline void GetLocalPlayerInfo();
	inline void GetAllEntity();
	inline void GetPlayer(DWORD_PTR entity);
	inline void GetLoot(DWORD_PTR entity);
	inline void GetVehicle(DWORD_PTR entity, wchar_t *name);

	std::string GetNameFromId(int ID);
	
public:
	vector<Entity> players;
	vector<Entity> loots;
	vector<Entity> vehicles;
	FCameraCacheEntry cameracache = { NULL };

private:
	PGameMemHelper *pMem;
	int ActorIds[4] = {0x10580,0x10581,0x10585,0x10586};
	int uaz[3];
	int dacia[4];
	int motorbike[4];
	int buggy[3];
	int boat = 0;
	int item = 0;
	int itemComponent = 0;
 
	DWORD_PTR pUWorld;
	DWORD_PTR pGameInstance;
	DWORD_PTR pULocalPlayer;
	DWORD_PTR pLocalPlayer;
	DWORD_PTR pViewportClient;
	DWORD_PTR pPWorld;
	DWORD_PTR pUlevel;
	Vector3 local;
};

