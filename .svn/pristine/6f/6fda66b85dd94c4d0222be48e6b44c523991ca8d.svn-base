// GetCapacity.h : main header file for the GetCapacity DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#ifdef _GETCAPACITY_EXPORTS
	#define GETCAPACITYDLL extern "C"__declspec(dllexport)
	#pragma message("Geneate GetCapacity export dll")
#else
	#define GETCAPACITYDLL extern "C"__declspec(dllimport)
	#pragma message("Geneate GetCapacity import dll")
#endif

// CGetCapacityApp
// See GetCapacity.cpp for the implementation of this class
//

class CGetCapacityApp : public CWinApp
{
public:
	CGetCapacityApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

GETCAPACITYDLL __int64 getDriveTotalSector(char *chDriveName);