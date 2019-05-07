#include "nvse/PluginAPI.h"
#include "nvse/GameAPI.h"
#include "nvse/SafeWrite.h"


IDebugLog		gLog("NOTAGSREDONE.log");
int g_bRemoveRedOutline = 1;
int g_bAddPercSymbol = 0;
int g_iRemoveTags = 1;

HMODULE noTagsHandle;
bool InitINI();
bool VersionCheck(const NVSEInterface *nvse);

extern "C" {
//Getting the HMODULE from dllmain, shoud be nicr
BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
	switch (dwReason)
	{
		case (DLL_PROCESS_ATTACH):
			noTagsHandle = (HMODULE) hDllHandle;
			break;
	}
	return TRUE;
}


bool NVSEPlugin_Query(const NVSEInterface * nvse, PluginInfo * info)
{	
	info->infoVersion = PluginInfo::kInfoVersion;
	info->name = "NO TAGS REDONE";
	info->version = 1;
	return (InitINI() && VersionCheck(nvse));
}

bool NVSEPlugin_Load(const NVSEInterface * nvse)
{
	if (g_bRemoveRedOutline)
	{
		SafeWriteBuf(0x765026, "\xE9\xD5\x00\x00\x00\x90", 0x06);
	}

	switch (g_iRemoveTags)
	{
		case 1:
			SafeWriteBuf(0x7639ED,"\xE9\x45\x03\x00\x00\x90", 0x06);
			break;
		case 2:
			SafeWriteBuf(0x763C87, "\xEB\x76", 0x02);
			if(g_bAddPercSymbol) SafeWriteBuf(0x0763B8A, "\x90\x90\x90\x90\x90", 0x05); 
			else SafeWriteBuf(0x763A2F, "\xE9\x7C\x01\x00\x00\x90", 0x06);
		break;
		default:
			_MESSAGE("Tags not removed");
			break;
	}
	return true;
}

};

//Checks here

bool InitINI()
{
	char filename[MAX_PATH];
	GetModuleFileNameA(noTagsHandle, filename, MAX_PATH);
	strcpy((char *)(strrchr(filename, '\\') + 1), "notagsredone.ini");
	_MESSAGE("ini location: %s", filename);
	g_bRemoveRedOutline = GetPrivateProfileIntA("Main", "bRemoveRedOutlines", 1, filename);
	g_iRemoveTags = GetPrivateProfileIntA("Main", "iRemoveTags", 1, filename);
	g_bAddPercSymbol = GetPrivateProfileIntA("Main", "bAddPercSymbol", 0, filename);
	if (!(g_bRemoveRedOutline || g_iRemoveTags))return false;
	else return true;
}

bool VersionCheck(const NVSEInterface *nvse)
{
	if (nvse->isEditor) return false;
	if (nvse->nvseVersion < NVSE_VERSION_INTEGER)
	{
		_ERROR("NVSE version too old (got %08X expected at least %08X)", nvse->nvseVersion, NVSE_VERSION_INTEGER);
		return false;
	}
	if (nvse->isNogore)
	{
		_ERROR("The NoGore version is not supported");
		return false;

	}
	if (nvse->runtimeVersion < RUNTIME_VERSION_1_4_0_525)
	{
		_ERROR("Unsupported NV exe version");
		return false;
	}
	// version checks pass

	return true;

}