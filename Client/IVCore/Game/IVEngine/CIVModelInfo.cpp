//================ IV:Multiplayer - http://github.com/IVMultiplayer/Ivmultiplayer ================
//
// File: CIVModelInfo.cpp
// Project: Client.Core
// Author: FRi<FRi.developing@gmail.com>
// License: See LICENSE in root directory
//
//==============================================================================

#include "CIVModelInfo.h"
#include <CCore.h>
#include "CIVModelManager.h"

extern CCore * g_pCore;

CIVModelInfo::CIVModelInfo( )
{
	// Reset the model index
	m_iModelIndex = -1;
}

CIVModelInfo::CIVModelInfo( int iModelIndex )
{
	// Set the model index
	SetIndex( iModelIndex );
}

CIVModelInfo::~CIVModelInfo( )
{

}

IVBaseModelInfo * CIVModelInfo::GetModelInfo( )
{
	if( m_iModelIndex >= 0 && m_iModelIndex < NUM_ModelInfos )
		return *(IVBaseModelInfo **)(COffsets::ARRAY_ModelInfos + (m_iModelIndex * 4));

	return NULL;
}

BYTE CIVModelInfo::GetType( )
{
	IVBaseModelInfo * pModelInfo = GetModelInfo( );
	if( pModelInfo )
	{
		DWORD dwFunctionAddress = pModelInfo->m_VFTable->GetType;
		BYTE byteType = 0;

		_asm	mov ecx, pModelInfo;
		_asm	call dwFunctionAddress;
		_asm	mov byteType, al;

		return byteType;
	}

	return 0;
}

void CIVModelInfo::Load( bool bWaitForLoad )
{
	// Is the model invalid?
	if( !IsValid() )
		return;

	// Are we already loaded
	if( IsLoaded() )
		return;

	// Request the model
	CIVModelManager::RequestModel( m_iModelIndex );

	// Should we wait for it to load?
	if( bWaitForLoad )
	{
		// Load all requested models
		CIVModelManager::LoadRequestedModels( );

		// Loop until the model loads
		while( !IsLoaded() )
			Sleep(10);
	}
}

bool CIVModelInfo::IsLoaded( )
{
	if( IsValid() )
		return CIVModelManager::HasModelLoaded( m_iModelIndex );

	return false;
}

DWORD CIVModelInfo::GetHash( )
{
	IVBaseModelInfo * pModelInfo = GetModelInfo( );

	if( pModelInfo )
		return pModelInfo->dwHash;

	return NULL;
}

DWORD CIVModelInfo::GetReferenceCount( )
{
	IVBaseModelInfo * pModelInfo = GetModelInfo( );

	if( pModelInfo )
		return pModelInfo->dwRefCount;

	return NULL;
}

WORD CIVModelInfo::GetAnimIndex( )
{
	IVBaseModelInfo * pModelInfo = GetModelInfo( );

	if( pModelInfo )
		return pModelInfo->wAnimIndex;

	return NULL;
}

void CIVModelInfo::AddReference( bool bWaitForLoad )
{
	// Increase the ref count
	m_dwReferenceCount++;

	// Is this the first reference?
	if( m_dwReferenceCount == 1 )
	{
		// Load the model
		Load( bWaitForLoad );
	}
}

void CIVModelInfo::RemoveReference( )
{
	// Is there a reference to this model?
	if( m_dwReferenceCount > 0 )
	{
		// Decrease the reference count
		m_dwReferenceCount--;
	}
}