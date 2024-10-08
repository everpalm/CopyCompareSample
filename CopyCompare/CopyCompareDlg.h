
// CopyCompareDlg.h : header file

// Copyright(c) Jaron Cheng.
// All right reserved
//
// Description
//
#pragma once

#include "Switch.h"	
#include "afxwin.h"	
#include "DLL_CopyCore.h"
#include "afxcmn.h"
#if(DEBUG_TAB_CONTROL == TRUE)
#include "Page1.h"
#include "Page2.h"
#endif

#ifdef DEBUG_VENDOR_MODE
#define START_CAPACITY 			7847000LL
#else
#define START_CAPACITY 			0
#endif

#define CHECKBOX_OFFSET			999

#define START_EDITBOX			1028	
#define VENDOR_SPEED_1024K 		3
#define REMOVABLE_SPEED_1024K 	64
#define FIX_SPEED_128K 			64
#define FIX_SPEED_1024K 			65
#define NOMINAL_SPEED_NUMBER 	65

#define WRITE_TIME_LIMIT 			60	
#define READ_TIME_LIMIT 			60	
#define SET_DATA_TIME_LIMIT 		60	
#define PATTERN_BUFFER_SIZE 		512
#define SECTOR_SIZE				512

#ifdef DEBUG_SELECT_FILE
#define COMBOBOX2_RANDOM 	0
#define COMBOBOX2_CUSTOM 		2
#endif	

#define SECONDS_PER_MINUTE		60
#define MINUTES_PER_HOUR		60
#define HOURS_PER_DAY			24
#define SECONDS_PER_HOUR		3600
#define SECONDS_PER_DAY			86400U

#define SECTORS_PER_MBYTE		2048
#define START_DRIVE_A			0
#define START_DRIVE_C			3

#define SET 						1
#define CLR 						0

#define PHYSICAL_DRIVE L"\\\\.\\PhysicalDrive"
#define GUID_LENGTH				30
// Tentative
typedef void (*callBack)();

enum
{	// Enumerate items in control list of patterns
	LIST_INDEX = 0,
	LIST_PATTERN,
	LIST_PATTERN_CONTENT,
};

enum
{
	DLG_COMBOBOX,
	DLG_CBUTTON,
	DLG_RADIO,
	DLG_EDIT,
};

typedef struct
{
	CString ItemName;
	CString Description;
	int DlgType;
	int ID;
	int Value;
}IniTable;

typedef struct
{
	WCHAR  GUID[MAX_PATH];
	DWORD LastPartition;
	UINT PhysicalNumber;
	__int64 DiskSize;
}PhysicalInformation;

class CFlashDevice
{
public:
	// Constructor
	CFlashDevice(char* pbDriveName, __int64 i64StartSector, __int64 i64EndSector, int nCopySize)
	{
		m_pbDriveName = pbDriveName;
		m_i64StartSector = i64StartSector;
		m_i64EndSector = i64EndSector;
		m_nCopySize = nCopySize;
	}	

	CFlashDevice()
	{
		m_pbDriveName = "D";
		m_i64StartSector = m_i64EndSector = 0;
		m_nCopySize = 1024;
	}	

	//Destructor
	//~CFlashDevice();

private:
	bool m_fEnable;
	bool m_fLastPartition;
	char* m_pbDriveName;
	UINT m_nMode;
	UINT m_nDriveType;
	UINT m_nPartitionNumber;
	UINT m_nPhysicalNumber;
	int m_nDriveIndex;	
	int m_nCopySize;
	int m_nPatternCount;
	int m_nLoopCount;	
	int m_nLoopNumber;
	int m_nEditBoxNumber;
	int m_nCheckBoxNumber;
	int m_nPercentage;	
	__int64 m_i64TotalSector;
	__int64 m_i64StartSector;
	__int64 m_i64EndSector;
	__int64 m_i64PhysicalEnd;
	__int64 m_i64LogicalEnd;
	LARGE_INTEGER m_llStartingOffset;
	LARGE_INTEGER m_llPartitionLength;
	
	unsigned long m_dwBufferStart;
	unsigned long m_dwBufferEnd;
	CString m_szDriveLabel;
	CString m_szDriveFormat;
	CString m_szPhysicalName;
	DWORD m_dwHiddenSectors;
 	PatternData m_patternData[PATTERN_BUFFER_SIZE];
 	unsigned char* m_pReadMemory;
	
public:
	void setPartitionLength(LARGE_INTEGER PartitionLength)
	{
		m_llPartitionLength= PartitionLength;
	}
	
	LARGE_INTEGER getPartitionLength()
	{
		return m_llPartitionLength;
	}
	
	void setLastPartition(bool Flag)
	{
		m_fLastPartition = Flag;
	}
	
	bool checkLastPartition()
	{
		return m_fLastPartition;
	}
	
	void setPhysicalNumber(UINT PhysicalNumber)
	{
		m_nPhysicalNumber = PhysicalNumber;
	}
	
	UINT getPhysicalNumber()
	{
		return m_nPhysicalNumber;
	}

	void setPartitionNumber(UINT PartitionNumber)
	{
		m_nPartitionNumber = PartitionNumber;
	}
	
	UINT getPartitionNumber()
	{
		return m_nPartitionNumber;
	}
	
	void setPhysicalEnd(__int64 PhysicalEnd)
	{
		m_i64PhysicalEnd = PhysicalEnd;
	}
	
	__int64 getPhysicalEnd()
	{
		return m_i64PhysicalEnd;
	}

	void setLogicalEnd(__int64 LogicalEnd)
	{
		m_i64LogicalEnd = LogicalEnd;
	}
	
	__int64 getLogicalEnd()
	{
		return m_i64LogicalEnd;
	}
	
	void setStartingOffset(LARGE_INTEGER StartingOffset)
	{
		m_llStartingOffset = StartingOffset;
	}
	
	LARGE_INTEGER getStartingOffset()
	{
		return m_llStartingOffset;
	}

	void setHiddenSectors(DWORD HiddenSectors)
	{
		m_dwHiddenSectors = HiddenSectors;
	}

	DWORD getHiddenSectors()
	{
		return m_dwHiddenSectors;
	}
	
	void setPhysicalName(CString PhysicalName)
	{
		m_szPhysicalName = PhysicalName;
	}

	CString getPhysicalName()
	{
		return m_szPhysicalName;
	}
	
	void setCheckBoxNumber(int nCheckBoxNumber)
	{
		m_nCheckBoxNumber = nCheckBoxNumber;
	}

	int getCheckBoxNumber()
	{
		return m_nCheckBoxNumber;
	}

	void setDriveIndex(int nDriveIndex)
	{
		m_nDriveIndex = nDriveIndex;
	}

	int getDriveIndex()
	{
		return m_nDriveIndex;
	}

	void setLoopCount(int nLoopCount)
	{
		m_nLoopCount = nLoopCount;
	}

	int getLoopCount()
	{
		return m_nLoopCount;
	}

	void setPercentage(int nPercentage)
	{
		m_nPercentage = nPercentage;
	}

	int getPercentage()
	{
		return m_nPercentage;
	}
	
	void setMode(int nMode)
	{
		m_nMode = nMode;
	}

	int getMode()
	{
		return m_nMode;
	}

	void setBufferStartAddress(int nAddress)
	{
		m_dwBufferStart = nAddress;
	}

	void setBufferEndAddress(int nAddress)
	{
		m_dwBufferEnd = nAddress;
	}
	
	void setEnable()
	{
		m_fEnable = true;
	}

	void setDisable()
	{
		m_fEnable = false;
	}
	
	bool getStatus()
	{
		return m_fEnable;
	}

	void setDriveName(char *bDriveName)
	{
		m_pbDriveName = bDriveName;
	}
	
	char* getDriveName()
	{
		return m_pbDriveName;
	}

	void setDriveType(int nDriveType)
	{
		m_nDriveType = nDriveType;
	}

	int getDriveType()
	{
		return m_nDriveType;
	}

	void setStartAddress(__int64 int64Address)
	{
		m_i64StartSector = int64Address;
	}

	void setEndAddress(__int64 int64Address)
	{
		m_i64EndSector = int64Address;
	}

	__int64 getStartAddress()
	{
		return m_i64StartSector;
	}

	void setCopySize(int nCopySize)
	{
		m_nCopySize = nCopySize;
	}

	int getCopySize()
	{
		return m_nCopySize;
	}

	__int64 getTotalSector()
	{
		__int64 Difference = (m_i64EndSector - m_i64StartSector);
		/*
		__int64 Mod = m_nCopySize / SECTOR_SIZE;
		__int64 Residue = Difference%Mod;
		__int64 Product = Difference/Mod;
		return Residue ? Product * Mod : Difference;
		*/
		return Difference;
	}

	__int64 getEndAddress()
	{
		/*
		__int64 Difference = (m_i64EndSector - m_i64StartSector);
		__int64 Mod = m_nCopySize / SECTOR_SIZE;
		__int64 Residue = Difference%Mod;
		__int64 Product = Difference/Mod;
		return Residue ? (m_i64StartSector + Product * Mod) : m_i64EndSector;
		*/
		return m_i64EndSector;
	}
	
	void setPatternMode(int nIndex, int nPatternMode)
	{
		m_patternData[nIndex].nPatternMode = nPatternMode;
	}
	
	int getPatternMode(int nPatternIndex)
	{
		return m_patternData[nPatternIndex].nPatternMode;
	}

	void setPatternCount(int nPatternCount)
	{
		m_nPatternCount = nPatternCount;
	}
	
	int getPatternCount()
	{
		return m_nPatternCount;
	}
	
	void setPattern(int nIndex, int nPattern)
	{
		m_patternData[nIndex].nPattern = nPattern;
	}
	
	int getPattern(int nIndex)
	{
		return m_patternData[nIndex].nPattern;
	}

	void setCustomFile(int nIndex, CString szCustomFile)
	{
		m_patternData[nIndex].szCustomFile = szCustomFile;
	}
	CString getCustomFile(int nIndex)
	{
		return m_patternData[nIndex].szCustomFile;
	}

	void setLoopNumber(int nLoopNumber)
	{
		m_nLoopNumber = nLoopNumber;
	}
	
	int getLoopNumber()
	{
		return m_nLoopNumber;
	}

	void setEditBoxNumber(int nEditBoxNumber)
	{
		m_nEditBoxNumber = nEditBoxNumber;
	}
	
	int getEditBoxNumber()
	{
		return m_nEditBoxNumber;
	}

	void setDriveLabel(CString szDriveLabel)
	{
		m_szDriveLabel = szDriveLabel;
	}

	CString getDriveLabel()
	{
		return m_szDriveLabel;
	}

	void setDriveFormat(CString DriveFormat)
	{
		m_szDriveFormat = DriveFormat;
	}
	
	CString getDriveFormat()
	{
		return m_szDriveFormat;
	}

	void initialDevice(void);
	
	//static UINT deviceMain(LPVOID pParam);
	/*
	static UINT writeDevice(HANDLE deviceHandle, int nDriveIndex,	
						unsigned long lStartSector,
						unsigned long lEndSector,
						int nCopySize,
						unsigned char *pWriteMemory);
	
	static UINT readDevice(HANDLE deviceHandle, int nDriveIndex,
						unsigned long lStartSector,
						unsigned long lEndSector,
						int nCopySize,
						unsigned char *pReadMemory);
	*/
};

// CCopyCompareDlg dialog
class CCopyCompareDlg : public CDialogEx
{
// Construction
public:
	CCopyCompareDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_COPYCOMPARE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
		
public:
#ifdef DEBUG_PRELOAD_INI
	void screenDisk();
#endif

#ifdef DEBUG_SCAN_PHYSICAL
	bool ScanPhysicalDisk();
#endif

#ifdef DEBUG_SCAN_UFD	
	void initialCheck();
	void initialList();
	void EnableAllEditbox();
	 void EnableAllCheckbox();
	void setCheckBox(int i, bool chk);
	int getCopySize();		
	bool ScanUFD();		
	UINT getDriveInformation();
	static UINT threadRead(LPVOID pParam);
#endif
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRead();
	afx_msg void OnBnClickedWrite();	
	afx_msg void OnBnClickedCopyCompare();
	
public:
	CListCtrl m_List;
	CFlashDevice m_cFlashDrive[TOTAL_DRIVE_NUMBER];
	DriveInform m_DriveInformTbl[TOTAL_DRIVE_NUMBER];
	afx_msg void OnBnClickedAddPattern();	
	afx_msg void OnBnClickedRemove();		
	afx_msg void OnBnClickedClearAll();		
	afx_msg void OnBnClickedPause();		
	CButton m_pauseStart;		
#ifdef DEBUG_UPDATE_REPORT	
	 void updateReport();	
#endif
	 afx_msg void OnTimer(UINT_PTR nIDEvent);	

private:
	CButton m_errorStop;	
	
#ifdef DEBUG_SELECT_FILE
	CButton m_selectFile;
	CComboBox m_selectPattern;
	
public:
	afx_msg void OnSelchangeCombo2();		
	afx_msg void OnBnClickedSelectFile();	
	
#endif	
#ifdef DEBUG_DUMP_LOG							
	CEdit m_errorLog;							
#endif
	void showOK(int DriveIndex);				
	void hideOK();
	CStatic m_bitmapD;
	CStatic m_bitmapE;
	CStatic m_bitmapF;
	CStatic m_bitmapG;
	CStatic m_bitmapH;
	CStatic m_bitmapI;
	CStatic m_bitmapJ;
	CStatic m_bitmapK;
	CStatic m_bitmapL;
	CStatic m_bitmapM;
	CStatic m_bitmapN;
	CStatic m_bitmapO;
	CStatic m_bitmapP;
	CStatic m_bitmapQ;
	CStatic m_bitmapR;
	CStatic m_bitmapS;
	CStatic m_bitmapT;
	CStatic m_bitmapU;
	CStatic m_bitmapV;
	CStatic m_bitmapW;
	CStatic m_bitmapX;
	CStatic m_bitmapY;
	CStatic m_bitmapZ;
	afx_msg void OnBnClickedSaveIni();	
	afx_msg void OnBnClickedLoadIni();	
	afx_msg void OnCbnSelchangeIdeCommand();	
	virtual void OnOK();
#if(DEBUG_TAB_CONTROL == TRUE)	
	/* Tab control */
	CTabCtrl m_Tab;
	Page1 m_Page1;
	Page2 m_Page2;
	afx_msg void OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult);
#endif
};

extern CString searchDriveLabel(int Index);
