// JudgeOS.h : main header file for the JudgeOS DLL
//
// Copyright(c) 2014 Jaron Cheng.
// All right reserved
//
// Description
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#ifdef JUDGEOS_EXPORTS
	#define JUDGEOSDLL extern "C"__declspec(dllexport)
	#pragma message("Geneate judge OS export dll")
#else
	#define JUDGEOSDLL extern "C"__declspec(dllimport)
	#pragma message("Geneate judge OS import dll")
#endif

// CJudgeOSApp
// See JudgeOS.cpp for the implementation of this class
//

class CJudgeOSApp : public CWinApp
{
public:
	CJudgeOSApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

typedef struct 
{
int OSIndex;
//DWORD PlatformID;
DWORD MajorVersion;
DWORD MinorVersion;
} WindowsVersion;

const WindowsVersion VersionTable[]=
{
//	Index  Major Minor		
	{	1,		4,		10	},	// Windows 98
	{	2,		4,		90	},	// Windows Me
	{	3,		4,		0		},	// Windows NT 4.0, Windows 2000
	{	4,		5,		1		},	// Windows XP
	{	5,		5,		2		},	// Windows XP Professional x64 Edition, Windows Server 2003, Windows Server 2003 R2
	{	6,		6,		0		},	// Windows Vista, Windows 2008
	{	7,		6,		1		},	// Windows 7, Windows Server 2008 R2
	{	8,		6,		2		},	// Windows 8, Windows Server 2012
	{	9,		6,		3		},	// Windows 8.1 Preview, Windows Server 2012 R2 Preview
};

enum
{
	WINDOWS_98 = 1,
	WINDOWS_ME,
	WINDOWS_NT_4,
	WINDOWS_XP,
	WINDOWS_XP_PROFESSIONAL_X64,
	WINDOWS_VISTA,
	WINDOWS_7,
	WINDOWS_8,
	WINDOWS_8_1
};

JUDGEOSDLL int JudgeOS();