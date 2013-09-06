//========== IV:Multiplayer - https://github.com/IVMultiplayer/IVMultiplayer ==========
//
// File: CDirectInput8Hook.cpp
// Project: Client.Core
// Author: FRi<FRi.developing@gmail.com>
// License: See LICENSE in root directory
//
//==============================================================================

#include "CDirectInput8Hook.h"
#include <CCore.h>
#include <dinput.h>

extern	CCore					* g_pCore;
bool							CDirectInput8Hook::m_bInstalled = false;
DirectInput8Create_t			CDirectInput8Hook::m_pDirectInput8Create = NULL;

HRESULT WINAPI DirectInput8Create__Hook( HINSTANCE hInst, DWORD dwVersion, REFIID riidltf, LPVOID * ppvOut, LPUNKNOWN punkOuter )
{
	// Call the real DirectInput8Create
	HRESULT hResult = CDirectInput8Hook::m_pDirectInput8Create( hInst, dwVersion, riidltf, ppvOut, punkOuter );

	// Did the call succeed?
	if( SUCCEEDED( hResult ) )
	{
		// Get the directinput8 pointer
		IDirectInput8 * pInput = (IDirectInput8 *)*ppvOut;

		// Create the proxy
		*ppvOut = new CDirectInput8Proxy( pInput );
	}

	return hResult;
}

bool CDirectInput8Hook::Install( )
{
	// Is the proxy already installed?
	if( m_bInstalled )
		return false;

	// Install the detour patch
	m_pDirectInput8Create = (DirectInput8Create_t)DetourFindFunction("dinput8.dll", "DirectInput8Create");
	DetourTransactionBegin();
	DetourAttach(&(PVOID&)m_pDirectInput8Create, DirectInput8Create__Hook);
	DetourTransactionCommit();

	// Mark as installed
	m_bInstalled = true;

	return true;
}

void CDirectInput8Hook::Uninstall( )
{
	// Is the proxy not installed?
	if( !m_bInstalled )
		return;

	// Uninstall the detour patch
	if(m_pDirectInput8Create)
	{
		DetourTransactionBegin();
		DetourDetach(&(PVOID&)m_pDirectInput8Create, DirectInput8Create__Hook);
		DetourTransactionCommit();
	}

	// Mark as not installed
	m_bInstalled = false;
}