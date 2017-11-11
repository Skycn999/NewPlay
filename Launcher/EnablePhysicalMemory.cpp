#include "EnablePhysicalMemory.h"
#include "AMMAP64.h"

#include "Superfetch.h"
#include <iostream>

#ifndef _WIN64
#error This can only be compiled for 64bit
#endif


PACL				OldDacl_ = NULL;
HANDLE              hDriver = 0;
POBJECT_HEADER      g_obj_handle = 0;

static BOOLEAN MapPhysicalMemory(HANDLE PhysicalMemory, PDWORD64 Address, PSIZE_T Length, PDWORD64 VirtualAddress)
{
	NTSTATUS			ntStatus;
	PHYSICAL_ADDRESS	viewBase;

	*VirtualAddress = 0;
	viewBase.QuadPart = (ULONGLONG) (*Address);
	ntStatus = ZwMapViewOfSection
		(
			PhysicalMemory,
			GetCurrentProcess(),
			(PVOID *) VirtualAddress,
			0L,
			*Length,
			&viewBase,
			Length,
			ViewShare,
			0,
			PAGE_READWRITE
		);

	if(!NT_SUCCESS(ntStatus)) return false;
	*Address = viewBase.LowPart;
	return true;
}

static BOOLEAN UnMapmemory(PDWORD64 Address)
{
	if(!ZwUnmapViewOfSection(GetCurrentProcess(), (PVOID) Address))
		return true;
	else
		return false;
}


static BOOLEAN ChangeSecurityDescriptorPhysicalMemoryBack() {
	NTSTATUS				status;
	HANDLE					physmem;
	UNICODE_STRING			physmemString;
	OBJECT_ATTRIBUTES		attributes;
	
	WCHAR					physmemName[] = L"\\device\\physicalmemory";
	RtlInitUnicodeString(&physmemString, physmemName);
	InitializeObjectAttributes(&attributes, &physmemString, OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = ZwOpenSection(&physmem, WRITE_DAC | READ_CONTROL, &attributes);

	if (!NT_SUCCESS(status)) return false;

	SetSecurityInfo(physmem, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, OldDacl_, NULL);

	CloseHandle(physmem);
	return true;

}
static BOOLEAN ChangeSecurityDescriptorPhysicalMemory()
{
	EXPLICIT_ACCESS		Access;
	PACL				NewDacl = NULL;
	SECURITY_DESCRIPTOR security;
	ZeroMemory(&security, sizeof(SECURITY_DESCRIPTOR));

	PSECURITY_DESCRIPTOR	psecurity = &security;
	NTSTATUS				status;
	HANDLE					physmem;
	UNICODE_STRING			physmemString;
	OBJECT_ATTRIBUTES		attributes;
	WCHAR					physmemName[] = L"\\device\\physicalmemory";

	RtlInitUnicodeString(&physmemString, physmemName);

	InitializeObjectAttributes(&attributes, &physmemString, OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = ZwOpenSection(&physmem, WRITE_DAC | READ_CONTROL, &attributes);

	if(!NT_SUCCESS(status)) return false;

	GetSecurityInfo(physmem, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &OldDacl_, NULL, 0);

	Access.grfAccessPermissions = SECTION_ALL_ACCESS;
	Access.grfAccessMode = GRANT_ACCESS;
	Access.grfInheritance = NO_INHERITANCE;
	Access.Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	Access.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
	Access.Trustee.TrusteeType = TRUSTEE_IS_USER;
	Access.Trustee.ptstrName = L"CURRENT_USER";

	SetEntriesInAcl(1, &Access, OldDacl_, &NewDacl);

	SetSecurityInfo(physmem, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, NewDacl, NULL);

	CloseHandle(physmem);
	return true;
};


static HANDLE OpenPhysicalMemory()
{
	UNICODE_STRING		physmemString;
	OBJECT_ATTRIBUTES	attributes;
	WCHAR				physmemName[] = L"\\device\\physicalmemory";
	NTSTATUS			status;
	HANDLE				physmem;

	RtlInitUnicodeString(&physmemString, physmemName);

	InitializeObjectAttributes(&attributes, &physmemString, OBJ_CASE_INSENSITIVE, NULL, NULL);

	status = ZwOpenSection(&physmem, SECTION_ALL_ACCESS, &attributes);

	if(!NT_SUCCESS(status))
	{
		printf("%08x\n", status);
		return NULL;
	}

	return physmem;
}

int isAscii(int c)
{
	return((c >= 'A' && c <= 'z') || (c >= '0' && c <= '9') || c == 0x20 || c == '@' || c == '_' || c == '?');
}

int isPrintable(uint32_t uint32)
{
	if((isAscii((uint32 >> 24) & 0xFF)) && (isAscii((uint32 >> 16) & 0xFF)) && (isAscii((uint32 >> 8) & 0xFF)) &&
	   (isAscii((uint32) & 0xFF)))
		return true;
	else
		return false;
}


bool isInsidePhysicalRAM(uint64_t addr, SFMemoryInfo* mi, int nOfRange) {
	for (int i = 0; i < nOfRange; i++) 
		if (mi[i].Start <= addr && addr <= mi[i].End)
			return true;
	return false;
}


int patch() {
	std::cout << "Start Patching" << std::endl;
	system("start.bat");

	hDriver = OpenDriver();
	if (!hDriver || hDriver == (HANDLE)-1) {
		printf("Driver AMMAP64.sys is not running, launch the bat file...\n");
		system("pause");
		return 0;
	}

	if (!SFSetup()) {
		printf("You're not running with administrator privilege... relaunch with admin privileges to get access to required API.\n");
		system("pause");
	}

	SFMemoryInfo myRanges[32] = { 0 };
	int nOfRange = 0;
	SFGetMemoryInfo(myRanges, nOfRange);
	myRanges[nOfRange - 1].End -= 0x1000;
	IoCommand myIo = { 0 };
	myIo.offset = 0x0;

	myIo.read.QuadPart = 0x2000;

	bool bFound = false;
	if (DriverMapMemory(hDriver, &myIo)) {

		auto i = 0ULL;
		for (i = 0; i < myRanges[nOfRange - 1].End; i += 0x1000) {
			if (bFound)
				break;
			if (!isInsidePhysicalRAM(i, myRanges, nOfRange))
				continue;
			if (!DriverUnmapMemory(hDriver, &myIo))
				break;
			myIo.offset = i;
			if (!DriverMapMemory(hDriver, &myIo))
				break;
			uint8_t* lpCursor = (uint8_t*)(myIo.virtualmemory);
			uint32_t previousSize = 0;

			while (true) {
				auto pPoolHeader = (PPOOL_HEADER)lpCursor;
				auto blockSize = (pPoolHeader->BlockSize << 4);
				auto previousBlockSize = (pPoolHeader->PreviousSize << 4);

				if (previousBlockSize != previousSize ||
					blockSize == 0 ||
					blockSize >= 0xFFF ||
					!isPrintable(pPoolHeader->PoolTag & 0x7FFFFFFF))
					break;

				previousSize = blockSize;

				if (0x74636553 == pPoolHeader->PoolTag) {
					auto pObjectHeader = (POBJECT_HEADER)(lpCursor + 0x30);
					if(pObjectHeader->Flags == 0x16)
						printf("HandleCount %d  Flag %x\n", pObjectHeader->HandleCount, pObjectHeader->Flags);
					if (pObjectHeader->HandleCount >= 0 && pObjectHeader->HandleCount <= 3  && pObjectHeader->Flags == 0x16 )
					{
						printf("Get some Object\n");
						if (pObjectHeader->KernelOnlyAccess == 1 && pObjectHeader->KernelObject == 1) {
							printf("Found PhysicalMemory Object Header at %p\n", lpCursor += 0x30);
							pObjectHeader->KernelObject = 0;
							pObjectHeader->KernelOnlyAccess = 0;
							g_obj_handle = pObjectHeader;
							bFound = true;
						}
						else if (pObjectHeader->KernelOnlyAccess == 1 && pObjectHeader->KernelObject == 1) {
							printf("Found PhysicalMemory Object Header at %p\n", lpCursor += 0x30);
							g_obj_handle = pObjectHeader;
							bFound = true;
						}
						break;
					}
				}

				lpCursor += blockSize;
				if ((lpCursor - ((uint8_t*)myIo.virtualmemory)) >= 0x1000)
					break;

			}
		}
	}
	if (!bFound) {
		printf("Read %lld bytes without finding the physical address, did you already patch?\nTesting access... (requires administrator or it will fail.)\n", myIo.offset);

	}
	

	if (!ChangeSecurityDescriptorPhysicalMemory()) {
		printf("Failed to open hande on \\Device\\PhysicalMemory from usermode, either you don't have Administrator privilege or the exploit failed.\n");
		system("pause");
		return 0;
	}
	auto hMemory = OpenPhysicalMemory();
	if (hMemory && hMemory != (HANDLE)-1) {
		CloseHandle(hMemory);
		printf("Exploit success!\n");
		printf("You can now map \Device\PhysicalMemory from usermode, check https://github.com/waryas/UMPMLib/ as a guideline.\n");

	}
	else {
		printf("Exploit failed...\n");
	}
	
}

int unpatch() {
	if (g_obj_handle) {
		g_obj_handle->KernelObject = 1;
		g_obj_handle->KernelOnlyAccess = 1;
	}
	
	std::cout << "Unpatching " << std::endl;
	
	CloseDriver(hDriver);
	
	auto hMemory = OpenPhysicalMemory();
	if (hMemory && hMemory != (HANDLE)-1) {
		CloseHandle(hMemory);
		printf("Exploit success!\n");
		printf("You can now map \Device\PhysicalMemory from usermode, check https://github.com/waryas/UMPMLib/ as a guideline.\n");

	}
	else {
		printf("Exploit failed...\n");
	}
	system("stop.bat");

	return 0;
}


void start_esp() {
	std::cout << "Start the ESP" << std::endl;
	system("pause");
	
}
int main()
{
	
	patch();

	//Start ESP program
	start_esp();
	printf("HandleCount %d  Flag %x\n", g_obj_handle->HandleCount, g_obj_handle->Flags);

	unpatch();
	
	system("pause");
	return 0;
}
