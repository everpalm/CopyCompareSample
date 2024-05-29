// DLL_CopyCore.h : main header file for the DLL_CopyCore DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols



#ifdef COPYCORE_EXPORTS
	#define COPYCOREDLL extern "C"__declspec(dllexport)
	#pragma message("Geneate CopyCore export dll")
#else
	#define COPYCOREDLL extern "C"__declspec(dllimport)
	#pragma message("Geneate CopyCore import dll")
#endif

#define TOTAL_DRIVE_NUMBER 		26
#define SET 						1
#define CLR 						0

#define NOMINAL_SPEED_NUMBER 	65
#define PATTERN_BUFFER_SIZE 		512
#define SECTOR_SIZE 				512
#define ALIGN_UNIT 				0x40	
	
#define WRITE_TIME_LIMIT 			60
#define READ_TIME_LIMIT 			60	
#define SET_DATA_TIME_LIMIT 		60

enum
{
	PHASE_INITIAL,
	PHASE_SEARCH,
	PHASE_SET_DATA,
	PHASE_WRITE,
	PHASE_READ,
	PHASE_COMPARE,
};


enum	
{	
	MODE_STOP = 0,		
	MODE_INITIAL,		// 1
	MODE_SEARCH,		// 2
	MODE_SET_DATA,		// 3
	MODE_WRITE,			// 4	
	MODE_READ,			// 5
	MODE_COMPARE,		// 6
	MODE_FORMAT,		// 7
	MODE_PAUSE,		// 8
};

enum
{
	PATTERN_ASCEND,
	PATTERN_FIX,		
	PATTERN_RANDOM,
	PATTERN_CUSTOM,
};

enum
{
	PHASE_OK,
	PHASE_BUSY,
	PHASE_ERROR,
	PHASE_STOP,
};

enum
{
	FORMAT_PHASE_OK,
	FORMAT_PHASE_BUSY,
	FORMAT_PHASE_ERROR,
	FORMAT_PHASE_STOP,
};

enum
{
	SET_DATA_OK,
	SET_DATA_BUSY,
	SET_DATA_ERROR,
	SET_DATA_STOP,
};



enum
{
	SEARCH_PHASE_OK,
	SEARCH_PHASE_BUSY,
	SEARCH_PHASE_ERROR,
	SEARCH_PHASE_STOP,
};

enum
{
	READ_PHASE_OK,
	READ_PHASE_BUSY,
	READ_PHASE_ERROR,
	READ_PHASE_STOP,
};

enum
{
	WRITE_PHASE_OK,
	WRITE_PHASE_BUSY,
	WRITE_PHASE_ERROR,
	WRITE_PHASE_STOP,
};

enum
{
	ERROR_OK_END = 0,	
	ERROR_BUSY_END,			// 1
	ERROR_ID_MISMATCH,		// 2
	ERROR_ID_READ_ERROR,	// 3
	ERROR_WRITE_LENGTH,		// 4
	ERROR_WRITE_ACCROSS,	// 5	
	ERROR_DATA_COMPARE,	// 6	
	ERROR_BUFFER_FULL,		// 7
	ERROR_PATTERN,			// 8
	ERROR_OPEN_FILE,			// 9
	ERROR_START_ADDRESS,	// 10
	ERROR_END_ADDRESS,		// 11
	ERROR_READ_LENGTH,		// 12
	ERROR_TIME_OUT,			// 13
	ERROR_INVALID_COMMAND,// 14
	ERROR_HANDLE,			// 15	
	ERROR_GET_GEOMETRY,		// 16
	ERROR_TIMER_SETTING		// 17
};

typedef struct
{
	union
	{
		unsigned int VALUE;
		struct
		{
			unsigned char Symptom;
			unsigned char Status;
			unsigned char Mode;
			unsigned char Preserved;
		}SYNOPSIS;
	}PARSE;
}ErrorCode;

typedef struct
{
	UINT nSecond;
	UINT nMinute;
	UINT nHour;
	UINT nDay;
}CompareTime;

typedef struct
{
	int nPatternMode;
	int nPattern;
	CString szCustomFile;
} PatternData;

typedef struct
{
	UINT Mode;
	int RetryCount;
	ULONGLONG BufferStart;
	ULONGLONG BufferEnd;
	union
	{
		unsigned char BYTE;
		struct
		{
			unsigned char Enable:1;			// 1: Enable drive, 0: Disable drive
			unsigned char ErrorStop:1;		// 1: Error stop, 0: Error continue
			unsigned char HideWarning:1;		// 1: Hide warning, 0: Warning
			unsigned char ATACommand:1;	// 1: ATA, 0: SCSI
			unsigned char Beep:1;			// 1: Beep, 0: None
			unsigned char ReadEachWrite:1;	// 1: Read each write, 0: Read after write complete
			unsigned char Sequential:1;		// 1: Sequential, 0: Randomize
			unsigned char FormatAfterward:1;	// 1: Format after copy compare, 0: None
		}BIT;
	}Option;

	union
	{
		unsigned char BYTE;
		struct
		{
			unsigned char Removable:1;		// 1: Removable, 0: Fix disk
			unsigned char WriteVerify:1;		// 1: Write and verify, 0: Normal write
			unsigned char ListControl:1;		// 1: List control, 0: No list control
			unsigned char EraseResidue:1;
			unsigned char :1;
			unsigned char :1;
			unsigned char :1;
			unsigned char :1;
		}BIT;
	}Option2;
	
	union
	{
		unsigned char BYTE;
		struct
		{
			unsigned char Processing:1;		// 1: Processing, 0: In halt
			unsigned char ErrorOccurred:1;		// 1: Error occurred, 0: Normal
			unsigned char Reading:1;			// 1: Reading, 0: Not reading
			unsigned char ForceStop:1;		// 1: Force stop, 0: No force stop
			unsigned char Looping:1;		// 1: Looping, 0: Loop end
			unsigned char Patterning:1;	// 1: Patterning, 0: Pattern stop
			unsigned char Formating:1;	// 1: Formating, 0: not formating
			unsigned char LogicalWrite:1;	// 1: Logical write, 0: Physical write
		}BIT;
		
	}Status;
}DriveControl;

typedef struct
{
	bool fEnable;
	bool fLastPartition;
	char *chDriveName;	
	CString szDriveLabel;
	CString szDriveFormat;
	CString szCustomFile;
	CString szPhysicalName;
	DWORD dwHiddenSectors;
	UINT nMode;	
	UINT nDriveType;
	UINT nPartitionNumber;

	unsigned __int64 i64StartSector;
	unsigned __int64 i64EndSector;
	unsigned __int64 i64TotalSector;
	unsigned __int64 AccessNumber;
	unsigned __int64 SubStep;
	unsigned __int64 i64PhysicalEnd;
	unsigned __int64 i64LogicalEnd;
	LARGE_INTEGER llStartingOffset;
	LARGE_INTEGER llPartitionLength;
	
	
	int nDriveIndex;
	int nCopySize;	
	int nEditNumber;
	int nLoopNumber;
	int nLoopCount;
	int nPercentage;
	int nPatternCount;
	
	PatternData myPatternData[PATTERN_BUFFER_SIZE];
	unsigned char* pWriteMemory;
	unsigned char* pReadMemory;
	HANDLE DeviceHandle;
} DriveInform;

// CDLL_CopyCoreApp
// See DLL_CopyCore.cpp for the implementation of this class
//

class  CDLL_CopyCoreApp : public CWinApp
{
public:
	CDLL_CopyCoreApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

COPYCOREDLL UINT deviceMain(LPVOID pParam);

COPYCOREDLL int g_nSectionCount[TOTAL_DRIVE_NUMBER];
COPYCOREDLL volatile DriveControl g_DriveControl[TOTAL_DRIVE_NUMBER];
COPYCOREDLL int g_nLoopCount[TOTAL_DRIVE_NUMBER];
COPYCOREDLL unsigned __int64 g_lStartSector[TOTAL_DRIVE_NUMBER];
COPYCOREDLL unsigned __int64 g_lEndSector[TOTAL_DRIVE_NUMBER];
COPYCOREDLL unsigned __int64 g_lSectorSum[TOTAL_DRIVE_NUMBER];	
COPYCOREDLL unsigned __int64 g_lAccessCount[TOTAL_DRIVE_NUMBER];
COPYCOREDLL unsigned __int64 g_lSubSector[TOTAL_DRIVE_NUMBER];
COPYCOREDLL unsigned __int64 g_lSumWrite[TOTAL_DRIVE_NUMBER];
COPYCOREDLL unsigned __int64 g_lSumRead[TOTAL_DRIVE_NUMBER];		

COPYCOREDLL DriveInform *g_pDriveInform[TOTAL_DRIVE_NUMBER];
COPYCOREDLL int g_nTempCopySize[TOTAL_DRIVE_NUMBER];
COPYCOREDLL int g_nTempSubStep[TOTAL_DRIVE_NUMBER];
COPYCOREDLL ErrorCode g_ErrorCode[TOTAL_DRIVE_NUMBER];

COPYCOREDLL ULONGLONG g_dwBufferStart[TOTAL_DRIVE_NUMBER];
COPYCOREDLL ULONGLONG g_dwBufferEnd[TOTAL_DRIVE_NUMBER];	

COPYCOREDLL int g_nRetryCount[TOTAL_DRIVE_NUMBER];	
COPYCOREDLL bool  g_pauseStart;	
COPYCOREDLL double g_dbStartTime[TOTAL_DRIVE_NUMBER];
COPYCOREDLL double g_dbEndTime[TOTAL_DRIVE_NUMBER];
COPYCOREDLL double g_dbWriteLength[TOTAL_DRIVE_NUMBER];	
COPYCOREDLL double g_dbWriteTime[TOTAL_DRIVE_NUMBER];		
COPYCOREDLL double g_dbReadLength[TOTAL_DRIVE_NUMBER];
COPYCOREDLL double g_dbReadTime[TOTAL_DRIVE_NUMBER];

COPYCOREDLL int g_nPercentage[TOTAL_DRIVE_NUMBER];
COPYCOREDLL int g_nPercentageWrite[TOTAL_DRIVE_NUMBER];
COPYCOREDLL int g_nPercentageRead[TOTAL_DRIVE_NUMBER];

COPYCOREDLL double g_dbProcessLength[TOTAL_DRIVE_NUMBER];
COPYCOREDLL CompareTime g_myTimer;

COPYCOREDLL ULONGLONG g_dwErrorLogOffset;	
COPYCOREDLL int g_nWritePhase[TOTAL_DRIVE_NUMBER];
COPYCOREDLL int g_nReadPhase[TOTAL_DRIVE_NUMBER];
//COPYCOREDLL unsigned char *g_pBuffer[TOTAL_DRIVE_NUMBER];