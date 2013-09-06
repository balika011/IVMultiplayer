//========== IV:Multiplayer - https://github.com/IVMultiplayer/IVMultiplayer ==========
//
// File: CDirect3D9Hook.cpp
// Project: Client.Core
// Author: FRi<FRi.developing@gmail.com>
// License: See LICENSE in root directory
//
//==============================================================================

#include "CDirect3D9Hook.h"
#include "IDirect3D9Proxy.h"
#include <Detours\detours.h>

CDirect3D9Hook::Direct3DCreate9_t CDirect3D9Hook::m_pfnDirect3DCreate9;

CDirect3D9Hook::CDirect3D9Hook()
{
	m_bHookInstalled = false;
	m_pfnDirect3DCreate9 = NULL;
}

CDirect3D9Hook::~CDirect3D9Hook()
{

}

bool CDirect3D9Hook::Install()
{
	if(!m_bHookInstalled)
	{      
		m_pfnDirect3DCreate9 = (Direct3DCreate9_t)DetourFindFunction("D3D9.DLL", "Direct3DCreate9");
		DetourTransactionBegin();
		DetourAttach(&(PVOID&)m_pfnDirect3DCreate9, Direct3DCreate9_Hook);
		DetourTransactionCommit();
			
		m_bHookInstalled = true;
        return true;
    }
    return false;
}

void CDirect3D9Hook::Uninstall()
{
	if(m_bHookInstalled)
    {
        if ( m_pfnDirect3DCreate9 )
		{
			DetourTransactionBegin();
			DetourDetach(&(PVOID&)m_pfnDirect3DCreate9, Direct3DCreate9_Hook);
			DetourTransactionCommit();
		}
                        
        m_bHookInstalled = false;
    }
}

IDirect3D9 * WINAPI CDirect3D9Hook::Direct3DCreate9_Hook(UINT SDKVersion)
{
    IDirect3D9 * pD3D = m_pfnDirect3DCreate9(SDKVersion);
    return (!pD3D ? NULL : new IDirect3D9Proxy(pD3D));
}