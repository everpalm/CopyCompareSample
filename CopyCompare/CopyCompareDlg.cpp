// CopyCompareDlg.cpp
//
// Copyright(c) 2014 Jaron Cheng.
// All right reserved
//
// Description
// Implementation file
//
#include "stdafx.h"
#include "CopyCompare.h"
#include "CopyCompareDlg.h"
#include "afxdialogex.h"
#include <Mmsystem.h>	
#include "Switch.h"
#if(DEBUG_TAB_CONTROL == TRUE)
#include "Page1.h"
#include "Page2.h"
#endif
#ifdef DEBUG_DEVICE_IO
#include <ntddscsi.h>
#include <winioctl.h>
#include <stdlib.h>
#include "ScanUFD.h"
#include "DLL_CopyCore.h"
#include "GetCapacity.h"
#endif

#ifdef DEBUG_JUDGE_OS
#include "JudgeOS.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

double g_dbProcessTime[TOTAL_DRIVE_NUMBER];

static PhysicalInformation PhysicalDrive[TOTAL_DRIVE_NUMBER] = { 0 };
#if(DEBUG_CHECK_TIMER_FORMAT == TRUE)	// CTJ15012716
static CompareTime g_StopTimer;
#endif	
HBITMAP g_hBmp;	

// CAboutDlg dialog used for App About
//ULONGLONG g_dwErrorLogOffset;

// CTJ14062417 Move to top
char *CStringToChar(CString str)
{
	char *charString = new char[str.GetLength() + 1];
	wcstombs_s(NULL, charString, str.GetLength() + 1, str, _TRUNCATE);
	return charString;
}

int searchDriveIndex(char *chDriveName)
{
	CString szDriveName = (CString)chDriveName;		// \\.\D:=>D://
	szDriveName.Replace(L"\\\\.\\",L"");
	szDriveName = szDriveName + _T("\\");
	for(int nIndex = 0; nIndex < TOTAL_DRIVE_NUMBER; nIndex++)
	{
		if(driveLabelTbl[nIndex] == szDriveName)
		{
			#ifdef DUMP_SEARCH_DRIVE_INDEX
			TRACE(_T("[%s]searchDriveIndex = %d\n"), szDriveName, nIndex);
			#endif
			return nIndex;
		}
	}
	return 0;
}

CString searchStringLength(CString ItemName, CString TotalString, int *nStart)
{
	int nStringOffset = TotalString.Find(ItemName, *nStart);
	if(nStringOffset == -1)
	{
		return _T("");
	}
	int nItemSize = ItemName.GetLength() + 1;
	#ifdef DUMP_SEARCH_ITEM
	TRACE(_T("searchStringLength=>sizeof (%s) = %d\n"),ItemName, nItemSize);
	#endif
	int nReturnOffset = TotalString.Find(_T("\r\n"), nStringOffset);
	*nStart = nReturnOffset;
	return TotalString.Mid(nStringOffset + nItemSize , nReturnOffset - nItemSize - nStringOffset);
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CCopyCompareDlg dialog




CCopyCompareDlg::CCopyCompareDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CCopyCompareDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCopyCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_List);
	DDX_Control(pDX, IDC_BUTTON7, m_pauseStart);
#ifdef DEBUG_SELECT_FILE
	DDX_Control(pDX, IDC_BUTTON8, m_selectFile);
#endif
	DDX_Control(pDX, IDC_COMBO2, m_selectPattern);
#ifdef DEBUG_DUMP_LOG
	DDX_Control(pDX, IDC_ERROR_LOG, m_errorLog);
#endif	
	DDX_Control(pDX, IDB_BITMAP1, m_bitmapD);
	DDX_Control(pDX, IDB_BITMAP2, m_bitmapE);
	DDX_Control(pDX, IDB_BITMAP3, m_bitmapF);
	DDX_Control(pDX, IDB_BITMAP4, m_bitmapG);
	DDX_Control(pDX, IDB_BITMAP5, m_bitmapH);
	DDX_Control(pDX, IDB_BITMAP6, m_bitmapI);
	DDX_Control(pDX, IDB_BITMAP7, m_bitmapJ);
	DDX_Control(pDX, IDB_BITMAP8, m_bitmapK);
	DDX_Control(pDX, IDB_BITMAP9, m_bitmapL);
	DDX_Control(pDX, IDB_BITMAP10, m_bitmapM);
	DDX_Control(pDX, IDB_BITMAP11, m_bitmapN);
	DDX_Control(pDX, IDB_BITMAP12, m_bitmapO);
	DDX_Control(pDX, IDB_BITMAP13, m_bitmapP);
	DDX_Control(pDX, IDB_BITMAP14, m_bitmapQ);
	DDX_Control(pDX, IDB_BITMAP15, m_bitmapR);
	DDX_Control(pDX, IDB_BITMAP16, m_bitmapS);
	DDX_Control(pDX, IDB_BITMAP17, m_bitmapT);
	DDX_Control(pDX, IDB_BITMAP18, m_bitmapU);
	DDX_Control(pDX, IDB_BITMAP19, m_bitmapV);
	DDX_Control(pDX, IDB_BITMAP20, m_bitmapW);
	DDX_Control(pDX, IDB_BITMAP21, m_bitmapX);
	DDX_Control(pDX, IDB_BITMAP22, m_bitmapY);
	DDX_Control(pDX, IDB_BITMAP23, m_bitmapZ);
#if(DEBUG_TAB_CONTROL == TRUE)
	DDX_Control(pDX, IDC_TAB1, m_Tab);
#endif	
}

BEGIN_MESSAGE_MAP(CCopyCompareDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_TIMER()	
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO1, &CCopyCompareDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CCopyCompareDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_BUTTON4, &CCopyCompareDlg::OnBnClickedLoadIni)
	ON_BN_CLICKED(IDC_BUTTON2, &CCopyCompareDlg::OnBnClickedSaveIni)
	ON_BN_CLICKED(IDC_BUTTON1, &CCopyCompareDlg::OnBnClickedCopyCompare)
	ON_BN_CLICKED(IDC_BUTTON3, &CCopyCompareDlg::OnBnClickedAddPattern)
	ON_BN_CLICKED(IDC_BUTTON5, &CCopyCompareDlg::OnBnClickedRemove)	
	ON_BN_CLICKED(IDC_BUTTON6, &CCopyCompareDlg::OnBnClickedClearAll)
	ON_BN_CLICKED(IDC_BUTTON7, &CCopyCompareDlg::OnBnClickedPause)	
#ifdef DEBUG_SELECT_FILE
	ON_CBN_SELCHANGE(IDC_COMBO2, &CCopyCompareDlg::OnSelchangeCombo2)
	ON_BN_CLICKED(IDC_BUTTON8, &CCopyCompareDlg::OnBnClickedSelectFile)	
#endif	
	ON_BN_CLICKED(IDC_SAVE_INI, &CCopyCompareDlg::OnBnClickedSaveIni)
	ON_BN_CLICKED(IDC_LOAD_INI, &CCopyCompareDlg::OnBnClickedLoadIni)	
	ON_CBN_SELCHANGE(IDC_IDE_COMMAND, &CCopyCompareDlg::OnCbnSelchangeIdeCommand)	
#if(DEBUG_TAB_CONTROL == TRUE)	
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CCopyCompareDlg::OnTcnSelchangeTabMain)
#endif	
END_MESSAGE_MAP()

// CTJ14062417 Preload a default ini file
// CTJ14062418 Warning if open Default.ini fails
// CTJ14062509 Change function and file name
// CTJ14062510 Correct missing file name
#ifdef DEBUG_PRELOAD_INI
void CCopyCompareDlg::screenDisk()
{	
	CString szFileName = _T("DeleteDisk.ini");
	CFile MySetting;
	CFileException fileException;
	CString szTempString;
	unsigned int nFileLength;
	
	if (MySetting.Open(szFileName, CFile::modeRead| CFile::shareDenyNone, &fileException))
	{
		nFileLength = (unsigned int)MySetting.GetLength();
		MySetting.SeekToBegin();
		MySetting.Read(szTempString.GetBuffer(nFileLength), nFileLength);
		szTempString.ReleaseBuffer(nFileLength);	
		MySetting.Close();

#ifdef DEBUG_DELETE_DISK
			int nDeleteDisk, nStart = 0;
			CString szDeleteDisk, szDummyString;
			szDummyString = szTempString;
			do
			{
				szDeleteDisk = searchStringLength(_T("disk"), szDummyString, &nStart);
				szDeleteDisk = szDeleteDisk + _T(":");
				char *chDeleteDisk = CStringToChar(szDeleteDisk);
				nDeleteDisk = searchDriveIndex(chDeleteDisk);

#ifndef DEBUG_VENDOR_MODE
				if(nDeleteDisk)
				{
					this->setCheckBox(nDeleteDisk, FALSE);
					
					/* Disable checkbox */
					int nCheckBoxNumber = m_cFlashDrive[nDeleteDisk].getCheckBoxNumber();
					(CButton*)GetDlgItem(nCheckBoxNumber)-> EnableWindow( false );
					
					/* Disable editbox */
					int nEditBoxNumber = m_cFlashDrive[nDeleteDisk].getEditBoxNumber();
					(CButton*)GetDlgItem(nEditBoxNumber)-> EnableWindow( false );
					(CButton*)GetDlgItem(nEditBoxNumber+1)-> EnableWindow( false );

					/* Clear editbox */
					CListBox* pList = (CListBox*)GetDlgItem(nEditBoxNumber);
					pList->SetWindowText(_T(""));
					pList = (CListBox*)GetDlgItem(nEditBoxNumber+1);
					pList->SetWindowText(_T(""));		
				}
#endif

				#ifdef DUMP_LOAD_INI
				TRACE(_T("nDeleteDisk = %d\n"), nDeleteDisk);
				#endif
			}
			while(szDeleteDisk!=_T(":"));
#endif
	}
	else
	{
		CString str;
		str.Format(_T("DeleteDisk.ini is missing\n"));
		AfxMessageBox(str);
	}
}
#endif
// CCopyCompareDlg message handlers

BOOL CCopyCompareDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	SetWindowPos(&CWnd::wndBottom, 0, 0, 1024,650,	
	 //SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	 SWP_NOMOVE);
#if(DEBUG_TAB_CONTROL == TRUE)
	/* Set up tab control */
	m_Tab.InsertItem(0, _T("Options"));
	m_Tab.InsertItem(1, _T("Timer"));

	m_Page1.Create(IDD_DIALOG1, GetDlgItem(IDC_TAB1));
	m_Page2.Create(IDD_DIALOG2, GetDlgItem(IDC_TAB1));

	CRect rs;
	m_Tab.GetClientRect(&rs);

	rs.top+=20;
	rs.bottom-=3;
	rs.left+=2;
	rs.right-=2;

	m_Page1.MoveWindow(&rs);
	m_Page2.MoveWindow(&rs);

	m_Page1.ShowWindow(true);
	m_Page2.ShowWindow(false);

	m_Tab.SetCurSel(0);
#endif
	/* Initial mode */
	for(int i = START_DRIVE_A; i < TOTAL_DRIVE_NUMBER; i++)	
	{
		g_DriveControl[i].Mode = MODE_INITIAL;	
	}

#ifdef DEBUG_JUDGE_OS
	int nOSVersion = JudgeOS();
	#ifdef DUMP_JUDGE_OS
	TRACE(_T("nOSVersion = %d\n"), nOSVersion);
	#endif
#endif	

	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(1);
	this->OnBnClickedRadio1();

	((CComboBox*)GetDlgItem(IDC_COMBO2))->ResetContent(); 
	
	CString szCOMBO2("Random");
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
	szCOMBO2 = _T("0 - 255");	
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
#ifdef DEBUG_SELECT_FILE	
	szCOMBO2 = _T("Custom");		
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
#endif
	
#ifdef DEBUG_ASENDING_PATTERN
	for(int i = 0; i <256; i++)
	{
		szCOMBO2.Format(_T("%d"),i);	
		szCOMBO2 = _T("0x")+szCOMBO2;	
		((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
	}
#else	
	szCOMBO2 = _T("0x00");		
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
	szCOMBO2 = _T("0x33");		
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
	szCOMBO2 = _T("0x55");		
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
	szCOMBO2 = _T("0x66");		
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
	szCOMBO2 = _T("0x99");		
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
	szCOMBO2 = _T("0xAA");		
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
	szCOMBO2 = _T("0xCC");		
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
	szCOMBO2 = _T("0xFF");		
	((CComboBox*)GetDlgItem(IDC_COMBO2))->AddString(szCOMBO2);
	((CComboBox*)GetDlgItem(IDC_COMBO2))->SetCurSel(0);
#endif

	CString szLoop;
	((CComboBox*)GetDlgItem(IDC_COMBO3))->ResetContent(); 
	for(int i = 1; i <1001; i++)
	{
		szLoop.Format(_T("%d"),i);
		szLoop = _T("Loop: ")+szLoop;
		((CComboBox*)GetDlgItem(IDC_COMBO3))->AddString(szLoop);
	}
	((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(0);

#ifdef DEBUG_READ_MODE
	CString szReadMode("Read each write");
	((CComboBox*)GetDlgItem(IDC_READ_MODE))->ResetContent(); 
	((CComboBox*)GetDlgItem(IDC_READ_MODE))->AddString(szReadMode);
	szReadMode = _T("Read after write complete");
	((CComboBox*)GetDlgItem(IDC_READ_MODE))->AddString(szReadMode);
	((CComboBox*)GetDlgItem(IDC_READ_MODE))->SetCurSel(0);
#endif

#ifdef DEBUG_ACCESS_MODE
	CString szAccessMode("Sequential");
	((CComboBox*)GetDlgItem(IDC_ACCESS_MODE))->ResetContent(); 
	((CComboBox*)GetDlgItem(IDC_ACCESS_MODE))->AddString(szAccessMode);
	szAccessMode = _T("Randomize");
	((CComboBox*)GetDlgItem(IDC_ACCESS_MODE))->AddString(szAccessMode);
	((CComboBox*)GetDlgItem(IDC_ACCESS_MODE))->SetCurSel(0);
#endif

#ifdef DEBUG_FORMAT_DISK
	((CComboBox*)GetDlgItem(IDC_DISK_FORMAT))->ResetContent(); 
	CString szDiskFormat = _T("FAT32");
	((CComboBox*)GetDlgItem(IDC_DISK_FORMAT))->AddString(szDiskFormat);
	szDiskFormat = _T("NTFS");
	((CComboBox*)GetDlgItem(IDC_DISK_FORMAT))->AddString(szDiskFormat);
	((CComboBox*)GetDlgItem(IDC_DISK_FORMAT))->SetCurSel(0);
#endif	
	m_List.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_List.InsertColumn(0, _T("Index"));		
	m_List.InsertColumn(1, _T("Pattern"));
	m_List.InsertColumn(2, _T("Pattern Content"));
	m_List.SetColumnWidth(0,50);
	m_List.SetColumnWidth(1,60);
	m_List.SetColumnWidth(2,120); 
	m_List.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	
#ifdef DEBUG_SELECT_FILE
	(CButton*)GetDlgItem(IDC_BUTTON8)->EnableWindow( false );
	(CEdit*)GetDlgItem(IDC_EDITSelectFile)->EnableWindow(false);
#endif
	(CButton*)GetDlgItem(IDC_BUTTON7)->EnableWindow( false );	// Prevent drive enter stop mode in the beginning

	/* Tentative process control */
	CProgressCtrl *pProgCtrl=(CProgressCtrl*)GetDlgItem(IDC_PROGRESS_D);
   	pProgCtrl->SetRange(0,100);
	
	// CTJ14062417
	// CTJ14062509
#ifdef DEBUG_PRELOAD_INI
	this->screenDisk();
#endif

	return TRUE;  
}

void CCopyCompareDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCopyCompareDlg::OnPaint()
{
	
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();

		CDialog::UpdateWindow();
		
		for(int i = START_DRIVE_A; i < TOTAL_DRIVE_NUMBER; i++)
		{
			if(m_cFlashDrive[i].getStatus())
			{
				if(g_DriveControl[i].Mode== MODE_STOP)
				{
			 		//TBD
				}
			}
		}
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCopyCompareDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#ifdef DEBUG_SCAN_PHYSICAL
BOOL GetDriveGeometryEx(LPWSTR wszPath, DISK_GEOMETRY_EX *pdg)
{
	  HANDLE hDevice = INVALID_HANDLE_VALUE;  // handle to the drive to be examined 
	  BOOL bResult   = FALSE;                 // results flag
	  DWORD junk     = 0;                     // discard results

	  hDevice = CreateFileW(wszPath,          // drive to open
	                        0,                // no access to the drive
	                        FILE_SHARE_READ | // share mode
	                        FILE_SHARE_WRITE, 
	                        NULL,             // default security attributes
	                        OPEN_EXISTING,    // disposition
	                        0,                // file attributes
	                        NULL);            // do not copy file attributes

	  if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
	  {
	    	return (FALSE);
	  }

	  bResult = DeviceIoControl(hDevice,                       // device to be queried
	                            IOCTL_DISK_GET_DRIVE_GEOMETRY_EX, // operation to perform
	                            NULL, 0,                       // no input buffer
	                            pdg, sizeof(*pdg),            // output buffer
	                            &junk,                         // # bytes returned
	                            (LPOVERLAPPED) NULL);          // synchronous I/O

	  CloseHandle(hDevice);
	  return (bResult);
}

BOOL GetPartitionInfoEx(LPWSTR wszPath, PARTITION_INFORMATION_EX *pdg)
{
	  HANDLE hDevice = INVALID_HANDLE_VALUE;  // handle to the drive to be examined 
	  BOOL bResult   = FALSE;                 // results flag
	  DWORD junk     = 0;                     // discard results

	  hDevice = CreateFileW(wszPath,          // drive to open
	                        0,                // no access to the drive
	                        FILE_SHARE_READ | // share mode
	                        FILE_SHARE_WRITE, 
	                        NULL,             // default security attributes
	                        OPEN_EXISTING,    // disposition
	                        0,                // file attributes
	                        NULL);            // do not copy file attributes

	  if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
	  {
	    	return (FALSE);
	  }

	  bResult = DeviceIoControl(hDevice,                       // device to be queried
	                            IOCTL_DISK_GET_PARTITION_INFO_EX, // operation to perform
	                            NULL, 0,                       // no input buffer
	                            pdg, sizeof(*pdg),            // output buffer
	                            &junk,                         // # bytes returned
	                            (LPOVERLAPPED) NULL);          // synchronous I/O

	  CloseHandle(hDevice);
	  return (bResult);
}

BOOL GetVolumeDiskExtents(LPWSTR wszPath, VOLUME_DISK_EXTENTS *de)
{
	  HANDLE hDevice = INVALID_HANDLE_VALUE;  // handle to the drive to be examined 
	  BOOL bResult   = FALSE;                 // results flag
	  DWORD junk     = 0;                     // discard results

	  hDevice = CreateFileW(wszPath,          // drive to open
	                        0,                // no access to the drive
	                        FILE_SHARE_READ | // share mode
	                        FILE_SHARE_WRITE, 
	                        NULL,             // default security attributes
	                        OPEN_EXISTING,    // disposition
	                        0,                // file attributes
	                        NULL);            // do not copy file attributes

	  if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
	  {
	    	return (FALSE);
	  }

	  bResult = DeviceIoControl(hDevice,                       // device to be queried
	                            IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS, // operation to perform
	                            NULL, 0,                       // no input buffer
	                            de, sizeof(*de),            // output buffer
	                            &junk,                         // # bytes returned
	                            (LPOVERLAPPED) NULL);          // synchronous I/O

	  CloseHandle(hDevice);
	  return (bResult);
}

bool  CCopyCompareDlg::ScanPhysicalDisk()
{
	DISK_GEOMETRY_EX dg = { 0 };
	VOLUME_DISK_EXTENTS pdg = { 0 };
	PARTITION_INFORMATION_EX pie;
	VOLUME_DISK_EXTENTS  de = { 0 };
	unsigned int nDriveCount = 0;
	
	for(int j = START_DRIVE_A; j < TOTAL_DRIVE_NUMBER ; j++)
	{
		PhysicalInformation Buf = {L""};
		WCHAR  wcVolumeName[MAX_PATH] = L"";
		CString szTempName = L"";
		DWORD cchBufferLength = MAX_PATH;
		
		szTempName.Format(_T("%s"), driveLabelTbl[j]);
		szTempName.Replace(L"//", L"");
		char *chLabel = CStringToChar(szTempName);
		wcVolumeName[0] = chLabel[0];
		wcVolumeName[1] = L':';
		wcVolumeName[2] = L'\\';
		bool fFlag = false;
		PhysicalInformation *pName = PhysicalDrive;
		CString szPhysicalDrive;
		
		if(::GetVolumeNameForVolumeMountPoint((LPCTSTR )wcVolumeName, (LPTSTR)Buf.GUID, cchBufferLength))
		{
#ifdef DUMP_SCAN_PHYSICAL		
			TRACE(_T("%s GUID=%s\n"), szTempName, Buf.GUID);
#endif
			/* Set drive label */
			CString szPartitionName = L"";;
			szTempName.Replace(L"\\",L"");
			szPartitionName = _T("\\\\.\\") + szTempName;

			/* Prepare input string of drive label for GetPartitionInfoEx */
#ifdef DUMP_SCAN_PHYSICAL
			TRACE(_T("ScanPhysicalDisk=>szTempName = %s\n"), szTempName);
#endif
			if(GetPartitionInfoEx((LPTSTR)(LPCWSTR)szPartitionName, &pie))
			{
#ifdef DUMP_SCAN_PHYSICAL			
				TRACE(_T("StartingOffset=%llu\n"), pie.StartingOffset);
				TRACE(_T("PartitionLength=%llu\n"), pie.PartitionLength);
				TRACE(_T("PartitionNumber=%u\n"), pie.PartitionNumber);
				TRACE(_T("Mbr.HiddenSectors=%u\n"), pie.Mbr.HiddenSectors);
#endif

#ifdef DUMP_PARTITION_INFO_EX
				TRACE(_T("PartitionStyle=%u\n"), pie.PartitionStyle);
				TRACE(_T("RewritePartition=%u\n"), pie.RewritePartition);
				TRACE(_T("Mbr.PartitionType=%u\n"), pie.Mbr.PartitionType);
				TRACE(_T("Mbr.BootIndicator=%u\n"), pie.Mbr.BootIndicator);
				TRACE(_T("Mbr.RecognizedPartition=%u\n"), pie.Mbr.RecognizedPartition);
				TRACE(_T("Gpt.PartitionType=%u\n"), pie.Gpt.PartitionType);
				TRACE(_T("Gpt.PartitionId=%s\n"), pie.Gpt.PartitionId);
				TRACE(_T("Gpt.Attributes=%lu\n"), pie.Gpt.Attributes);
				TRACE(_T("Gpt.Name=s%\n"), pie.Gpt.Name);
#endif
				m_cFlashDrive[j].setStartingOffset(pie.StartingOffset);
				m_cFlashDrive[j].setPartitionNumber(pie.PartitionNumber);
				m_cFlashDrive[j].setPartitionLength(pie.PartitionLength);
				m_cFlashDrive[j].setHiddenSectors(pie.Mbr.HiddenSectors);
			}
			
			/* To get physical disk number */
			if(GetVolumeDiskExtents((LPTSTR)(LPCWSTR)szPartitionName, &de))
			{
#ifdef DUMP_SCAN_PHYSICAL			
				TRACE(_T("Number of disk extents=%u\n"), de.NumberOfDiskExtents);
#endif
				DWORD dwExtentsNumber = de.NumberOfDiskExtents, dwCount;
				
				for(dwCount = 0; dwCount < dwExtentsNumber; dwCount++)
				{
					DWORD dwDiskNumber = de.Extents[dwCount].DiskNumber;
					m_cFlashDrive[j].setPhysicalNumber((UINT)dwDiskNumber);
#ifdef DUMP_SCAN_PHYSICAL					
					TRACE(_T("Physical disk number=%u\n"), dwDiskNumber);
#endif					
					pName += dwDiskNumber;
					pName->LastPartition= pie.PartitionNumber;
					pName->PhysicalNumber = dwDiskNumber;
					//memcpy(pName->GUID, Buf.GUID, sizeof(Buf));
					
					CString szCount;
					szCount.Format(_T("%d"), de.Extents[dwCount].DiskNumber);
					szPhysicalDrive = PHYSICAL_DRIVE+szCount;

					/* Get physical disk capacity */
					if(GetDriveGeometryEx((LPTSTR)(LPCWSTR)szPhysicalDrive, &dg))
					{
#ifdef DUMP_SCAN_PHYSICAL				
						TRACE(_T("%s\n"), szPhysicalDrive);
#endif
						m_cFlashDrive[j].setPhysicalName(szPhysicalDrive);
						
						__int64 DiskSize = 0;
						DiskSize = dg.DiskSize.QuadPart;
						m_cFlashDrive[j].setPhysicalEnd((DiskSize<SECTOR_SIZE )? 1: DiskSize/SECTOR_SIZE);
						pName->DiskSize= DiskSize;
#ifdef DUMP_SCAN_PHYSICAL									
						TRACE(_T("DiskSize=%llu Bytes\n"), DiskSize);
						TRACE(_T("%s Physical end = %llu\n"), szTempName, m_cFlashDrive[j].getPhysicalEnd());
#endif					
					}
				}
			}
		}
	}
	return true;
}
#endif

#ifdef DEBUG_SCAN_UFD
bool CCopyCompareDlg::ScanUFD()
{
	PARTITION_INFORMATION_EX pie;
	
	for(int i = START_DRIVE_A; i < TOTAL_DRIVE_NUMBER ; i++)
	{
		WCHAR  wcVolumeName[MAX_PATH] = L"";
		DWORD cchBufferLength = MAX_PATH;
		CString szTempName = L"";
		szTempName.Format(_T("%s"), driveLabelTbl[i]);
		szTempName.Replace(L"//", L"");
		char *chLabel = CStringToChar(szTempName);
		wcVolumeName[0] = chLabel[0];
		wcVolumeName[1] = L':';
		wcVolumeName[2] = L'\\';
		PhysicalInformation Buf = {L""};

		/* Get drive type */	// CTJ15020911
		int nDriveType = ::GetDriveType(driveLabelTbl[i]);
		m_cFlashDrive[i].setDriveType(nDriveType);
#ifdef DUMP_SCAN_UFD 
		 switch(m_cFlashDrive[i].getDriveType())
		 {
		 case DRIVE_NO_ROOT_DIR:
			 TRACE(_T("[%s]ScanUFD=>driveLabelTbl[%s ] = DRIVE_NO_ROOT_DIR \n"),m_cFlashDrive[i].getDriveLabel(), driveLabelTbl[i]);
			 break;
		 case DRIVE_REMOVABLE:
			 TRACE(_T("[%s]ScanUFD=>driveLabelTbl[%s ] = DRIVE_REMOVABLE \n"),m_cFlashDrive[i].getDriveLabel(), driveLabelTbl[i]);
			 break;
		 case DRIVE_FIXED:
			 TRACE(_T("[%s]ScanUFD=>driveLabelTbl[%s ] = DRIVE_FIXED \n"), m_cFlashDrive[i].getDriveLabel(), driveLabelTbl[i]);
			 break;
		 case DRIVE_REMOTE:
			 TRACE(_T("[%s]ScanUFD=>driveLabelTbl[%s ] = DRIVE_REMOTE \n"), m_cFlashDrive[i].getDriveLabel(), driveLabelTbl[i]);
			 break;
		 case DRIVE_CDROM:
			TRACE(_T("[%s]ScanUFD=>driveLabelTbl[%s ] = DRIVE_CDROM \n"), m_cFlashDrive[i].getDriveLabel(), driveLabelTbl[i]);
			 break;
		 case DRIVE_RAMDISK:
			TRACE(_T("[%s]ScanUFD=>driveLabelTbl[%s ] = DRIVE_RAMDISK \n"), m_cFlashDrive[i].getDriveLabel(), driveLabelTbl[i]);
			break;
		 case DRIVE_UNKNOWN:
		 default:
			 TRACE(_T("[%s]ScanUFD=>driveLabelTbl[%s ] = DRIVE_UNKNOWN \n"), m_cFlashDrive[i].getDriveLabel(), driveLabelTbl[i]);
			 break;
		 }
#endif
		if((nDriveType==DRIVE_REMOVABLE)||(nDriveType==DRIVE_FIXED))
		{

#ifdef DUMP_LAST_PARTITION
			TRACE(_T("ScanUFD=>%s \n"), szTempName);
#endif	
			/* Set drive label */
			CString szPartitionName = L"";;
			szTempName.Replace(L"\\",L"");
			szPartitionName = _T("\\\\.\\") + szTempName;
			if(GetPartitionInfoEx((LPTSTR)(LPCWSTR)szPartitionName, &pie))
			{
#ifdef DUMP_LAST_PARTITION					
				TRACE(_T("ScanUFD=>%s partitionNumber=%u\n"), szPartitionName, pie.PartitionNumber);
#endif		
			__int64 i64StartAddress, i64Offset;
			DWORD dwPhysicalNumber = m_cFlashDrive[i].getPhysicalNumber();
			if(pie.PartitionNumber == PhysicalDrive[dwPhysicalNumber].LastPartition)
			{
#ifdef DUMP_LAST_PARTITION						
				TRACE(_T("ScanUFD=>PhysicalDrive%d's last parition=%d\n"), dwPhysicalNumber, pie.PartitionNumber );
#endif
				m_cFlashDrive[i].setLastPartition(true);
				__int64 i64PhysicalEnd = PhysicalDrive[dwPhysicalNumber].DiskSize / SECTOR_SIZE;
				m_cFlashDrive[i].setEndAddress(i64PhysicalEnd);
				i64Offset = m_cFlashDrive[i].getStartingOffset().QuadPart;
				if(i64Offset)
				{
					i64StartAddress = i64Offset / SECTOR_SIZE - m_cFlashDrive[i].getHiddenSectors();
					m_cFlashDrive[i].setStartAddress( i64StartAddress);
				}
				else
				{
					m_cFlashDrive[i].setStartAddress( START_CAPACITY);
				}
			}
			else
			{
				m_cFlashDrive[i].setLastPartition(false);
				__int64 i64Endaddress = m_cFlashDrive[i].getPartitionLength().QuadPart;
				i64Offset = m_cFlashDrive[i].getStartingOffset().QuadPart;
				i64Endaddress +=i64Offset;
				m_cFlashDrive[i].setEndAddress(i64Endaddress);
				__int64 i64StartAddress = i64Offset  / SECTOR_SIZE - m_cFlashDrive[i].getHiddenSectors();
				m_cFlashDrive[i].setStartAddress( i64StartAddress);
			}
		}
					
		/* Set drive label */
		driveLabelTbl[i].Replace(L"//",L"\\");		// Replace CString
		char *pTempLabel = CStringToChar(driveLabelTbl[i]);
		m_cFlashDrive[i].setDriveLabel(driveLabelTbl[i]);
#ifdef DUMP_SCAN_UFD
		TRACE(_T("ScanUFD=>m_cFlashDrive[%d].getDriveLabel() = %s\n"), i, m_cFlashDrive[i].getDriveLabel());
#endif

		CString szTempString = m_cFlashDrive[i].getDriveLabel();
		szTempString.Replace(L"\\",L"");
		szTempString = _T("\\\\.\\")+szTempString;
		char *pTempString = CStringToChar(szTempString);
		m_cFlashDrive[i].setDriveName(pTempString);

		  /* Set start address */
		  //m_cFlashDrive[i].setStartAddress( START_CAPACITY);

		  /* Set end address */
		  // CTJ14062410 Last sector equals total sector number minus one
		  // CTJ14062415 Set last sector by device type
		  // CTJ14062417 Argument should be pTempLabel
		  // CTJ14062711 To get disk format within getDriveTotalSector function
		//  __int64 int64EndSector;
		 __int64 int64LogicalEnd = getDriveTotalSector(pTempString);
		 //m_cFlashDrive[i].setEndAddress(int64EndSector);
		 m_cFlashDrive[i].setLogicalEnd(int64LogicalEnd);
	
		  /* Set editbox number */
		 m_cFlashDrive[i].setEditBoxNumber(START_EDITBOX+2*i);	// D:\ start address->IDC_EDIT1 = 1034 
#ifdef DUMP_SCAN_UFD	
		TRACE(_T("[%s]ScanUFD=>EditBoxNumber = %d\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getEditBoxNumber());
#endif

		/* Set checkbox number */
		m_cFlashDrive[i].setCheckBoxNumber(CHECKBOX_OFFSET+i);	// D:\ -> IDC_CHECK_D = 1002, IDC_CHECK_E = 1003, ..., etc
#ifdef DUMP_SCAN_UFD
		TRACE(_T("[%s]ScanUFD=>CheckBoxNumber = %d\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getCheckBoxNumber());
#endif			
		}
	}
	return true;
}

void CCopyCompareDlg::initialCheck()
{
	// TODO: Add your control notification handler code here
	
	for(int i = IDC_CHECK2; i < IDC_CHECK24; i++)		// IDC_CHECK1 is empty
	{
		((CButton*)GetDlgItem(i))->SetCheck(0); 
	}

	for(int i = START_DRIVE_A; i < TOTAL_DRIVE_NUMBER ; i++)
	{
		m_DriveInformTbl[i].fEnable = false;
	}
}

void CCopyCompareDlg::initialList()
{
	for(int i = IDC_EDIT1; i < (IDC_EDIT46 + 1); i++)
	{
		CListBox* pList = (CListBox*)GetDlgItem(i);
		pList->SetWindowText(_T(""));
	}
	
}

 void CCopyCompareDlg::EnableAllCheckbox()
{
	// TODO: Add your control notification handler code here
	
	for(int i = IDC_CHECK2; i < IDC_CHECK24; i++)
	{
		(CButton*)GetDlgItem(i)-> EnableWindow(TRUE);
	}
}

void CCopyCompareDlg::EnableAllEditbox()
{
	for(int i = IDC_EDIT1; i < (IDC_EDIT46 + 1); i++)
	{
		(CButton*)GetDlgItem(i)-> EnableWindow(TRUE);
	}
	
}

void CCopyCompareDlg::setCheckBox(int i, bool chk)
{
	m_DriveInformTbl[i].fEnable = TRUE;	
	i += CHECKBOX_OFFSET;	// IDC_CHECK2 = 1002

	((CButton*)GetDlgItem(i))->SetCheck(chk); 

#ifdef DEBUG_VENDOR_MODE
	((CButton*)GetDlgItem(IDC_CHECK_E))->SetCheck(!chk); 
#endif			
}
#endif

void CCopyCompareDlg::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
#ifdef DEBUG_SCAN_UFD
	CString szEditStartSector[TOTAL_DRIVE_NUMBER], szEditEndSector[TOTAL_DRIVE_NUMBER];

	/* Enable and initail checkbox and editbox */
	EnableAllCheckbox();
	EnableAllEditbox();
	initialCheck();	
	initialList();
	
#ifdef DEBUG_JUDGE_OS	
	int nOSIndex = JudgeOS();
#endif
	ScanPhysicalDisk();

	if(ScanUFD())
	{	
		if(((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck())
		{	// Radio1 pressed
			((CComboBox*)GetDlgItem(IDC_COMBO1))->ResetContent(); 
#ifdef DEBUG_VENDOR_MODE
			CString szSpeed("Speed (R/W size): 1K");		// SetCurSel(0)
			((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
			szSpeed = _T("Speed (R/W size): 2K");
			((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
			szSpeed = _T("Speed (R/W size): 64K");
			((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
			szSpeed = _T("Speed (R/W size): 1024K");
			((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
			((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(VENDOR_SPEED_1024K);	
#else
			CString szPrefix("Speed (R/W size): ");
			CString szSpeed, szTotal;
			for(int nSpeedCount = 1; nSpeedCount < NOMINAL_SPEED_NUMBER; nSpeedCount++)
			{
				szSpeed.Format(_T("%dk"), nSpeedCount);
				szTotal = szPrefix + szSpeed;
				((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szTotal);
			}
			szSpeed = _T("Speed (R/W size): 1024K");
			((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
			((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(REMOVABLE_SPEED_1024K);	
#endif			
				
		
#ifdef DEBUG_ATA_COMMAND
			((CComboBox*)GetDlgItem(IDC_IDE_COMMAND))->ResetContent(); 
			CString szCommandSet("SCSI Command");		
			((CComboBox*)GetDlgItem(IDC_IDE_COMMAND))->AddString(szCommandSet);
			((CComboBox*)GetDlgItem(IDC_IDE_COMMAND))->SetCurSel(0);
#endif
		
			for(int i = START_DRIVE_A; i < TOTAL_DRIVE_NUMBER ; i++)	
			{
				if(m_cFlashDrive[i].getDriveType() == DRIVE_REMOVABLE)
				{
					setCheckBox(i, TRUE);	
					szEditStartSector[i].Format(_T("%llu"), m_cFlashDrive[i].getStartAddress());
					szEditEndSector[i].Format(_T("%llu"), m_cFlashDrive[i].getEndAddress());
					if(m_cFlashDrive[i].getEditBoxNumber()>=IDC_EDIT1)
					{
					CListBox* pList1 = (CListBox*)GetDlgItem(m_cFlashDrive[i].getEditBoxNumber());
					CListBox* pList2 = (CListBox*)GetDlgItem(m_cFlashDrive[i].getEditBoxNumber()+1);
					pList1->SetWindowTextW(szEditStartSector[i]);	
					pList2->SetWindowTextW(szEditEndSector[i]);	
					pList1+=2;
					pList2+=2;
					}
				}
			}	
		}	
	}	
#endif
	// CTJ14062417
	// CTJ14062509
#ifdef DEBUG_PRELOAD_INI
	this->screenDisk();
#endif

}

void CCopyCompareDlg::OnBnClickedRadio2()
{
#ifdef DEBUG_SCAN_UFD
	// TODO: Add your control notification handler code here
	CString szEditStartSector[TOTAL_DRIVE_NUMBER], szEditEndSector[TOTAL_DRIVE_NUMBER];

	EnableAllCheckbox();
	EnableAllEditbox();
	initialCheck();
	initialList();

#ifndef DEBUG_VENDOR_MODE
	 CString str;
	str.Format(_T("You selected FIXED DISK,\nIt may overwrite your important data. "));
	AfxMessageBox(str);
#endif

	if(((CButton*)GetDlgItem(IDC_RADIO2))->GetCheck())
	{	// Radio2 pressed
		((CComboBox*)GetDlgItem(IDC_COMBO1))->ResetContent(); 
#ifdef DEBUG_VENDOR_MODE
		CString szSpeed("Speed (R/W size): 1K");		// SetCurSel(0)
		((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
		szSpeed = _T("Speed (R/W size): 2K");
		((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
		szSpeed = _T("Speed (R/W size): 64K");
		((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
		szSpeed = _T("Speed (R/W size): 1024K");
		((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
		((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(VENDOR_SPEED_1024K);	
#else
		CString szPrefix("Speed (R/W size): ");
		CString szSpeed, szTotal;
		for(int nSpeedCount = 1; nSpeedCount < NOMINAL_SPEED_NUMBER; nSpeedCount++)
		{
			szSpeed.Format(_T("%dk"), nSpeedCount);
			szTotal = szPrefix + szSpeed;
			((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szTotal);
		}

			szSpeed = _T("Speed (R/W size): 128K");
			((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
			szSpeed = _T("Speed (R/W size): 1024K");
			((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
			((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(FIX_SPEED_1024K);

#endif
		

#ifdef DEBUG_ATA_COMMAND
		((CComboBox*)GetDlgItem(IDC_IDE_COMMAND))->ResetContent(); 
		CString szCommandSet("SCSI Command");	
		((CComboBox*)GetDlgItem(IDC_IDE_COMMAND))->AddString(szCommandSet);
		szCommandSet = _T("ATA Command");	
		((CComboBox*)GetDlgItem(IDC_IDE_COMMAND))->AddString(szCommandSet);
		((CComboBox*)GetDlgItem(IDC_IDE_COMMAND))->SetCurSel(0);
#endif	
		ScanPhysicalDisk();

		if(ScanUFD())
		{
			for(int i = START_DRIVE_C; i < TOTAL_DRIVE_NUMBER ; i++)	// CTJ15012719
			{
				if(m_cFlashDrive[i].getDriveType() == DRIVE_FIXED)
				{
					setCheckBox(i, TRUE);
					/* Get start and end address from member variable */
					szEditStartSector[i].Format(_T("%llu"), m_cFlashDrive[i].getStartAddress());
					szEditEndSector[i].Format(_T("%llu"), m_cFlashDrive[i].getEndAddress());

					/* Display start and end address */
					if(m_cFlashDrive[i].getEditBoxNumber() >=IDC_EDIT1)
					{
					CListBox* pList1 = (CListBox*)GetDlgItem(m_cFlashDrive[i].getEditBoxNumber());
					CListBox* pList2 = (CListBox*)GetDlgItem(m_cFlashDrive[i].getEditBoxNumber()+1);
					pList1->SetWindowTextW(szEditStartSector[i]);	
					pList2->SetWindowTextW(szEditEndSector[i]);	
					pList1+=2;
					pList2+=2;
					}
				}	
			}	
		}	
	}	
#endif
	// CTJ14062417
	// CTJ14062509
#ifdef DEBUG_PRELOAD_INI
	this->screenDisk();
#endif
}


int CCopyCompareDlg::getCopySize()
{

	CString szComboBox1;
	((CComboBox*)GetDlgItem(IDC_COMBO1))->GetWindowText(szComboBox1);
	
	#ifdef DUMP_GET_COPY_SIZE
	TRACE(_T("szComboBox1 = %s\n"), szComboBox1);
	#endif

#ifdef DEBUG_VENDOR_MODE
	if(szComboBox1 == "Speed (R/W size): 1K")
	{
		#ifdef DUMP_GET_COPY_SIZE
		TRACE(_T("Speed (R/W size): 1K\n"));
		#endif
		return  COPY_SIZE_1K;	
	}
	else if(szComboBox1 == "Speed (R/W size): 2K")
	{
		#ifdef DUMP_GET_COPY_SIZE
		TRACE(_T("Speed (R/W size): 2K\n"));
		#endif
		return COPY_SIZE_2K;	
	}
	else if(szComboBox1 == "Speed (R/W size): 64K")
	{
		#ifdef DUMP_GET_COPY_SIZE
		TRACE(_T("Speed (R/W size): 64K\n"));
		#endif
		return COPY_SIZE_64K;	
	}
	else if(szComboBox1 == "Speed (R/W size): 128K")	
	{	
		#ifdef DUMP_GET_COPY_SIZE
		TRACE(_T("Speed (R/W size): 128K\n"));
		#endif
		return COPY_SIZE_128K;	
	}
	else
	{	
		#ifdef DUMP_GET_COPY_SIZE	
		TRACE(_T("Speed (R/W size): 1024K\n"));
		#endif
		return COPY_SIZE_1024K;
	}
#else
	CString szTempSpeed = szComboBox1.Mid(18, 4);
	szTempSpeed.Remove('k');
	return  _tstoi(szTempSpeed) * 1024;
#endif	
}

void CCopyCompareDlg::OnBnClickedCopyCompare()
{
	// TODO: Add your control notification handler code here
#ifdef DEBUG_DEVICE_IO
	CString szComboBox1, szEdit1, szEdit2, szTempString;
	CWinThread **myThread = new CWinThread* [TOTAL_DRIVE_NUMBER];
	DriveInform myDriveInform[TOTAL_DRIVE_NUMBER];

#ifdef DEBUG_TIMER
	g_myTimer.nSecond = 0;
	g_myTimer.nMinute= 0;
	g_myTimer.nHour= 0;
	g_myTimer.nDay= 0;
#endif
	
	g_pauseStart = false;

	hideOK();

	/* Get drive information */
	if(this->getDriveInformation())
	{
		return;
	}
#if(DEBUG_CHECK_TIMER_FORMAT == TRUE)	// CTJ15012716
	CString szTimerDay, szTimerHour, szTimerMinute, szTimerSecond;		
	CListBox* pTimerDay = (CListBox*)m_Page2.GetDlgItem(IDC_DAY);
	CListBox* pTimerHour = (CListBox*)m_Page2.GetDlgItem(IDC_HOUR);
	CListBox* pTimerMinute = (CListBox*)m_Page2.GetDlgItem(IDC_MINUTE);
	CListBox* pTimerSecond = (CListBox*)m_Page2.GetDlgItem(IDC_SECOND);
	pTimerDay->GetWindowTextW(szTimerDay);
	pTimerHour->GetWindowTextW(szTimerHour);
	pTimerMinute->GetWindowTextW(szTimerMinute);
	pTimerSecond->GetWindowTextW(szTimerSecond);

	wchar_t *end = NULL;
	g_StopTimer.nDay = _tcstol(szTimerDay, &end, 10);
	g_StopTimer.nHour = _tcstol(szTimerHour, &end, 10);
	g_StopTimer.nMinute = _tcstol(szTimerMinute, &end, 10);
	g_StopTimer.nSecond = _tcstol(szTimerSecond, &end, 10);
	
	/* Prevent wrong timer format */		
	if((g_StopTimer.nDay < 0)||(g_StopTimer.nHour < 0)||(g_StopTimer.nMinute < 0)||(g_StopTimer.nSecond < 0)||
		(g_StopTimer.nHour > 24)||(g_StopTimer.nHour > 24)||(g_StopTimer.nMinute > 60)||(g_StopTimer.nSecond > 60))
	{
		CString szErrorMessage;
		szErrorMessage.Format(_T("Wrong Timer Format!\n"));
		AfxMessageBox(szErrorMessage);
		return;
	}
#endif
	/* Check every drives and execute copy compare */
	for(int i = START_DRIVE_C; i < TOTAL_DRIVE_NUMBER; i++)		// D:��IDC_CHECK2��1002	
	{
		if(m_cFlashDrive[i].getStatus())
		{	
			// Initialize 
			g_DriveControl[i].Mode = MODE_INITIAL;
			g_DriveControl[i].Option.BYTE = 0;		
			g_DriveControl[i].Status.BIT.Processing = CLR;
#ifdef DEBUG_FORMAT_DISK
			g_DriveControl[i].Status.BIT.Formating = CLR;
#endif

#ifdef DEBUG_FORCE_STOP			
			g_DriveControl[i].Status.BIT.ForceStop = CLR;	
#endif			
			g_DriveControl[i].Status.BYTE = 0x0;	
#ifdef DEBUG_SHOW_PERCENTAGE
			g_nPercentage[i] = 0;
#endif			
			g_dwBufferStart[i] = 0;
			g_dwBufferEnd[i] = 0;
			g_nRetryCount[i] = 0;
#ifdef DEBUG_DISDINGUISH_SPEED		
			g_dbProcessLength[i] = g_dbWriteLength[i] = g_dbReadLength[i] = 0;
			g_dbWriteTime[i] = g_dbReadTime[i] = 0;
#endif			
			
			m_cFlashDrive[i].setBufferStartAddress(0);
			m_cFlashDrive[i].setBufferEndAddress(0);
			m_cFlashDrive[i].setLoopCount(1);
			m_cFlashDrive[i].setPercentage(0);

			if(m_cFlashDrive[i].getStatus())
			{
				g_DriveControl[i].Option.BIT.Enable= SET;
			}
			else
			{
				g_DriveControl[i].Option.BIT.Enable = CLR;
			}
			
			if(m_cFlashDrive[i].getDriveType() == DRIVE_REMOVABLE)
			{
				g_DriveControl[i].Option2.BIT.Removable = SET;
			}
			else
			{
				g_DriveControl[i].Option2.BIT.Removable = CLR;
			}
			
#ifdef DEBUG_ACCESS_MODE	
			CString szAccessMode;
			((CComboBox*)GetDlgItem(IDC_ACCESS_MODE))->GetWindowText(szAccessMode);
			if(szAccessMode== "Sequential")
			{
				g_DriveControl[i].Option.BIT.Sequential= SET;
			}
			else
			{
				g_DriveControl[i].Option.BIT.Sequential= CLR;
			}
#endif


#ifdef DEBUG_READ_MODE	
			CString szReadMode;
			((CComboBox*)GetDlgItem(IDC_READ_MODE))->GetWindowText(szReadMode);
			if(szReadMode== "Read each write")
			{
				g_DriveControl[i].Option.BIT.ReadEachWrite = SET;
			}
			else
			{
				g_DriveControl[i].Option.BIT.ReadEachWrite = CLR;
			}
#endif			
			
#ifdef DEBUG_ATA_COMMAND	
			CString szDriveCommand;
			((CComboBox*)GetDlgItem(IDC_IDE_COMMAND))->GetWindowText(szDriveCommand);
			if(szDriveCommand== "ATA Command")
			{	// ATA command
				g_DriveControl[i].Option.BIT.ATACommand = SET;
			}
			else
			{	// SCSI command
				g_DriveControl[i].Option.BIT.ATACommand = CLR;
			}
#endif			
			
#if(DEBUG_TAB_CONTROL == TRUE)	// CTJ15012715
			if(((CButton*)m_Page1.GetDlgItem(IDC_BEEP))->GetCheck())
			{
				g_DriveControl[i].Option.BIT.Beep = SET;
			}
			else
			{
				g_DriveControl[i].Option.BIT.Beep = CLR;
			}

			if(((CButton*)m_Page1.GetDlgItem(IDC_HIDE_WARNING))->GetCheck())
			{
				g_DriveControl[i].Option.BIT.HideWarning = SET;
			}
			else
			{
				g_DriveControl[i].Option.BIT.HideWarning = CLR;
			}

			if(((CButton*)m_Page1.GetDlgItem(IDC_ERROR_STOP))->GetCheck())
			{
				g_DriveControl[i].Option.BIT.ErrorStop = SET;
			}
			else
			{
				g_DriveControl[i].Option.BIT.ErrorStop = CLR;
			}

			if(((CButton*)m_Page1.GetDlgItem(IDC_CHECK_FORMAT))->GetCheck())
			{
				g_DriveControl[i].Option.BIT.FormatAfterward= SET;
			}
			else
			{
				g_DriveControl[i].Option.BIT.FormatAfterward = CLR;
			}
#endif
			int nCheckBoxNumber = m_cFlashDrive[i].getCheckBoxNumber();
			(CButton*)GetDlgItem(nCheckBoxNumber)-> EnableWindow( false );

			int nEditBoxNumber = m_cFlashDrive[i].getEditBoxNumber();
			(CButton*)GetDlgItem(nEditBoxNumber)-> EnableWindow( false );
			(CButton*)GetDlgItem(nEditBoxNumber+1)-> EnableWindow( false );

			(CButton*)GetDlgItem(IDC_RADIO1)-> EnableWindow( false );
			(CButton*)GetDlgItem(IDC_RADIO2)-> EnableWindow( false );	

			(CButton*)GetDlgItem(IDC_COMBO1)-> EnableWindow( false );	
			(CButton*)GetDlgItem(IDC_COMBO2)-> EnableWindow( false );	
			(CButton*)GetDlgItem(IDC_COMBO3)-> EnableWindow( false );
			(CButton*)GetDlgItem(IDC_IDE_COMMAND)-> EnableWindow( false );	
			(CButton*)GetDlgItem(IDC_READ_MODE)-> EnableWindow( false );	
			(CButton*)GetDlgItem(IDC_DISK_FORMAT)-> EnableWindow( false );	
			(CButton*)GetDlgItem(IDC_ACCESS_MODE)-> EnableWindow( false );	

			(CButton*)GetDlgItem(IDC_BUTTON1)-> EnableWindow( false );	
			(CButton*)GetDlgItem(IDC_BUTTON3)-> EnableWindow( false );	
			(CButton*)GetDlgItem(IDC_BUTTON5)-> EnableWindow( false );	
			(CButton*)GetDlgItem(IDC_BUTTON6)-> EnableWindow( false );	
#if(DEBUG_TAB_CONTROL == TRUE)
			m_Page1.GetDlgItem(IDC_ERROR_STOP)-> EnableWindow( false );	
			m_Page1.GetDlgItem(IDC_BEEP)-> EnableWindow( false );			
			m_Page1.GetDlgItem(IDC_HIDE_WARNING)-> EnableWindow( false );	
			m_Page1.GetDlgItem(IDC_CHECK_FORMAT)-> EnableWindow( false );	

			/* CTJ15012716 Disable timer window */
			m_Page2.GetDlgItem(IDC_DAY)-> EnableWindow( false );
			m_Page2.GetDlgItem(IDC_HOUR)-> EnableWindow( false );	
			m_Page2.GetDlgItem(IDC_MINUTE)-> EnableWindow( false );	
			m_Page2.GetDlgItem(IDC_SECOND)-> EnableWindow( false );	
#endif			
			(CButton*)GetDlgItem(IDC_SAVE_INI)-> EnableWindow( false );		
			(CButton*)GetDlgItem(IDC_LOAD_INI)-> EnableWindow( false );		
			
			(CButton*)GetDlgItem(IDC_BUTTON7)->EnableWindow( true );
#ifdef DEBUG_DUMP_LOG
			m_errorLog.SetSel(0, -1);
			m_errorLog.Clear();
#endif
			myDriveInform[i].nMode = m_cFlashDrive[i].getMode();
			myDriveInform[i].i64StartSector = m_cFlashDrive[i].getStartAddress();
			myDriveInform[i].i64EndSector = m_cFlashDrive[i].getEndAddress();
			myDriveInform[i].i64TotalSector = m_cFlashDrive[i].getTotalSector();	
			myDriveInform[i].chDriveName = m_cFlashDrive[i].getDriveName();
			myDriveInform[i].nCopySize = m_cFlashDrive[i].getCopySize();
			myDriveInform[i].nPatternCount = m_cFlashDrive[i].getPatternCount();
			myDriveInform[i].nLoopNumber = m_cFlashDrive[i].getLoopNumber();
			myDriveInform[i].nPercentage = m_cFlashDrive[i].getPercentage();
			myDriveInform[i].nDriveIndex = m_cFlashDrive[i].getDriveIndex();
			myDriveInform[i].szDriveLabel = m_cFlashDrive[i].getDriveLabel();	
			myDriveInform[i].nDriveType = m_cFlashDrive[i].getDriveType();		
			myDriveInform[i].szPhysicalName = m_cFlashDrive[i].getPhysicalName();
			myDriveInform[i].dwHiddenSectors = m_cFlashDrive[i].getHiddenSectors();
			myDriveInform[i].llStartingOffset = m_cFlashDrive[i].getStartingOffset();
			myDriveInform[i].i64PhysicalEnd = m_cFlashDrive[i].getPhysicalEnd();
			myDriveInform[i].i64LogicalEnd = m_cFlashDrive[i].getLogicalEnd();
			myDriveInform[i].nPartitionNumber = m_cFlashDrive[i].getPartitionNumber();
			myDriveInform[i].fLastPartition = m_cFlashDrive[i].checkLastPartition();
			myDriveInform[i].llPartitionLength= m_cFlashDrive[i].getPartitionLength();			
			
			if(myDriveInform[i].nPatternCount)
			{
				g_DriveControl[i].Option2.BIT.ListControl= SET;	
				for(int j = 0; j < myDriveInform[i].nPatternCount; j++)
				{
					myDriveInform[i].myPatternData[j].nPatternMode = m_cFlashDrive[i].getPatternMode(j);
					myDriveInform[i].myPatternData[j].nPattern = m_cFlashDrive[i].getPattern(j);			
					myDriveInform[i].myPatternData[j].szCustomFile = m_cFlashDrive[i].getCustomFile(j);
				}
			}
			else
			{
				g_DriveControl[i].Option2.BIT.ListControl= CLR;	
				myDriveInform[i].myPatternData[0].nPatternMode = m_cFlashDrive[i].getPatternMode(0);
				myDriveInform[i].myPatternData[0].nPattern = m_cFlashDrive[i].getPattern(0);		
				myDriveInform[i].myPatternData[0].szCustomFile = m_cFlashDrive[i].getCustomFile(0);
			}

			this->SetTimer(1,1000,0);	
			myThread[i] = ::AfxBeginThread(deviceMain, &myDriveInform[i]);	
			
			::DeleteObject(g_hBmp);	
			::Sleep(1000);	
		}	
	}	

#endif	
}

void CCopyCompareDlg::OnBnClickedAddPattern()
{
	// TODO: Add your control notification handler code here
#ifdef DEBUG_PATTERN	
	CString szComboBox2;
	((CComboBox*)GetDlgItem(IDC_COMBO2))->GetWindowText(szComboBox2);
	#ifdef DUMP_ON_BN_CLICKED_ADD_PATTERN
	if(szComboBox2 == "0 - 255")
	{
		TRACE(_T("Pattern = %s\n"),szComboBox2);
	}
	else if(szComboBox2 == "Random")
	{
		TRACE(_T("Pattern = %s\n"),szComboBox2);
	}
	else if(szComboBox2 == "Custom")
	{
		TRACE(_T("Select File = %s\n"),m_DriveInformTbl[0].szCustomFile);
	}
	else
	{
		szComboBox2.Replace(L"0x",L"");
		int nComboBox2 = _tstoi(szComboBox2);
		TRACE(_T("Pattern = %x\n"),nComboBox2);
	}
	#endif
	
	int nItem;
	CString szIndex;
	int nIndex	= m_List.GetItemCount();
	szIndex.Format(_T("%d"),nIndex);

	if(szComboBox2 == "Custom")
	{
		if(m_DriveInformTbl[0].szCustomFile == "")
		{
			CString str;
			str.Format(_T("Please Select Pattern File\n"));
			AfxMessageBox(str);
			return;
		}
	}

	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nIndex = m_List.GetNextSelectedItem(pos);
#ifdef DUMP_ON_BN_CLICKED_ADD_PATTERN
		TRACE(_T("nIndex = %d\n"), nIndex);
#endif
		CString szIndex;
		szIndex.Format(_T("%d"), nIndex+1);
		nItem = m_List.InsertItem(nIndex+1, szIndex);
		m_List.SetItemText(nItem, 1, szComboBox2);
		if(szComboBox2 == "Custom")
		{
			m_List.SetItemText(nItem, 2, m_DriveInformTbl[0].szCustomFile);
		}
		nItem++;
		for(nItem; nItem < m_List.GetItemCount(); nItem++)
		{
			szIndex.Format(_T("%d"), nItem);
			m_List.SetItemText(nItem, 0, szIndex);
		}
	}
	else
	{
		nItem = m_List.InsertItem(m_List.GetItemCount(), szIndex);
		m_List.SetItemText(nItem, 1, szComboBox2);
		if(szComboBox2 == "Custom")
		{
			m_List.SetItemText(nItem, 2, m_DriveInformTbl[0].szCustomFile);
		}
	}
	
#endif	
}

void CCopyCompareDlg::OnBnClickedRemove()
{
#ifdef DEBUG_PATTERN	

	POSITION pos = m_List.GetFirstSelectedItemPosition();
	if(pos)
	{
		int nIndex = m_List.GetNextSelectedItem(pos);
		#ifdef DUMP_ON_BN_CLICKED_REMOVE		
		TRACE(_T("nIndex = %d\n"), nIndex);		
		#endif
		m_List.DeleteItem(nIndex);
		for(nIndex; nIndex < m_List.GetItemCount(); nIndex++)
		{
			CString szIndex;
			szIndex.Format(_T("%d"), nIndex);
			m_List.SetItemText(nIndex, 0, szIndex);
		}
	}
	else
	{
		int nPatternCount = m_List.GetItemCount();	
		#ifdef DUMP_ON_BN_CLICKED_REMOVE		
		TRACE(_T("nPatternCount = %d\n"), nPatternCount);	
		#endif
		m_List.DeleteItem(nPatternCount - 1);	
	}
#endif
}

void CCopyCompareDlg::OnBnClickedClearAll()
{
#ifdef DEBUG_PATTERN	
	// TODO: Add your control notification handler code here
	m_List.DeleteAllItems();
	SetDlgItemText(IDC_EDITSelectFile, _T(""));
	
#endif
}


 UINT CCopyCompareDlg::getDriveInformation()	
 {

	int nCheckState, nPattern, nPatternIndex = 0;
	CString szComboBox1, szStartAddress, szEndAddress, szTempString, szErrorMessage;

	for(int i = START_DRIVE_C; i < TOTAL_DRIVE_NUMBER; i++)
	{
		/* Set drive check enable */
		nCheckState = ((CButton*)GetDlgItem(CHECKBOX_OFFSET+i))->GetCheck();
		if(nCheckState)
		{
			/* Set drive index */
			m_cFlashDrive[i].setDriveIndex(i);
			#ifdef DUMP_GET_DRIVE_INFORMATION	
			TRACE(_T("[%s]getDriveInformation=>getDriveIndex = %d\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getDriveIndex());	
			#endif
		
			/* Set status from nCheckState */
			 m_cFlashDrive[i].setEnable();
			#ifdef DUMP_GET_DRIVE_INFORMATION	
			TRACE(_T("[%s]getDriveInformation=>fEnable = %d\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getStatus());	
			#endif
			
			/* Set start address from editbox1 */
			CListBox* pStartAdress = (CListBox*)GetDlgItem(IDC_RADIO2+i*2);
			pStartAdress->GetWindowTextW(szStartAddress);

			/* Prevent empty start address */
			if(szStartAddress == "")	
			{
				if(!g_DriveControl[i].Option.BIT.HideWarning)
				{
					szErrorMessage.Format(_T("Please enter start address\n"));
					AfxMessageBox(szErrorMessage);
				}
#if(DEBUG_ERROR_CODE == TRUE)
				g_ErrorCode[i].PARSE.SYNOPSIS.Symptom = ERROR_START_ADDRESS;
#endif
				return ERROR_START_ADDRESS;
			}

			/* Set start address */
			unsigned long lStartSector = _tcstoul(szStartAddress, 0, 10);				// Conver CString to unsigned long
			m_cFlashDrive[i].setStartAddress(lStartSector);
			#ifdef DUMP_GET_DRIVE_INFORMATION
			TRACE(_T("[%s]getDriveInformation=> StartAddress = %xh\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getStartAddress());	
			#endif
			
			/* Get end address from editbox2 */
			CListBox* pEndAddress = (CListBox*)GetDlgItem(IDC_BUTTON1+i*2);
			pEndAddress->GetWindowTextW(szEndAddress);

			/* Prevent empty end address */
			if(szEndAddress == "")	
			{
				if(!g_DriveControl[i].Option.BIT.HideWarning)
				{
					szErrorMessage.Format(_T("Please enter end address\n"));
					AfxMessageBox(szErrorMessage);
				}
#if(DEBUG_ERROR_CODE == TRUE)			
				g_ErrorCode[i].PARSE.SYNOPSIS.Symptom = ERROR_END_ADDRESS;
#endif
				return ERROR_END_ADDRESS;
			}

			/* Set end address */
			unsigned long lEndSector = _tcstoul(szEndAddress, 0, 10);				// Conver CString to unsigned long
			m_cFlashDrive[i].setEndAddress(lEndSector);
			#ifdef DUMP_GET_DRIVE_INFORMATION
			TRACE(_T("[%s]getDriveInformation=> EndAddress = %xh\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getEndAddress());	
			#endif

			#ifdef DUMP_GET_DRIVE_INFORMATION
			TRACE(_T("[%s]getDriveInformation=> Total sectors = %xh\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getTotalSector());	
			#endif				
			
			/* Set loop number form combobox 3 */
			CString szComboBox3;
			((CComboBox*)GetDlgItem(IDC_COMBO3))->GetWindowText(szComboBox3);
			szComboBox3.Replace(L"Loop: ",L"");
			int nLoopNumber = _tstoi(szComboBox3);		// convert CString to int
			m_cFlashDrive[i].setLoopNumber(nLoopNumber);
			#ifdef DUMP_GET_DRIVE_INFORMATION
			TRACE(_T("[%s]getDriveInformation=>LoopNumber = %d\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getLoopNumber());	
			#endif
			
			/* Set copy size from getCopySize function */
			int nCopySize = this->getCopySize();	
			m_cFlashDrive[i].setCopySize(nCopySize);	
			#ifdef DUMP_GET_DRIVE_INFORMATION
			TRACE(_T("[%s]getDriveInformation=>CopySize = %d byte\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getCopySize());
			#endif	
			
			/* Set write pattern from list control 2 */
			int nPatternCount = m_List.GetItemCount();
			m_cFlashDrive[i].setPatternCount(nPatternCount);
			#ifdef DUMP_GET_DRIVE_INFORMATION
			TRACE(_T("[%s]getDriveInformation=>PatternCount = %d\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getPatternCount());	
			#endif

			/* CTJ14022410 Set Format after copy compare */
			CString szDiskFormat;
			((CComboBox*)GetDlgItem(IDC_DISK_FORMAT))->GetWindowText(szDiskFormat);
			m_cFlashDrive[i].setDriveFormat(szDiskFormat);
			#ifdef DUMP_GET_DRIVE_INFORMATION
			TRACE(_T("[%s]getDriveInformation=>DriveFormat = %s\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getDriveFormat());
			#endif

			if(nPatternCount)	
			{	// List control
				if(nPatternCount > PATTERN_BUFFER_SIZE)
				{
					// CTJ14052712 Add error code
					if(!g_DriveControl[i].Option.BIT.HideWarning)
					{
						szErrorMessage.Format(_T("Pattern buffer full!\n"));
						AfxMessageBox(szErrorMessage);						
					}
#if(DEBUG_ERROR_CODE == TRUE)				
					g_ErrorCode[i].PARSE.SYNOPSIS.Symptom = ERROR_BUFFER_FULL;
#endif
					return ERROR_BUFFER_FULL;
				}
				
				for(nPatternIndex = 0; nPatternIndex < nPatternCount; nPatternIndex++)
				{
					CString szWritePattern = m_List.GetItemText ( nPatternIndex , 1 );
					CString szCustomFile = m_List.GetItemText ( nPatternIndex , 2 );	
					
					if(szWritePattern == "0 - 255")
					{
						m_cFlashDrive[i].setPatternMode(nPatternIndex, PATTERN_ASCEND);
					}
					else if(szWritePattern == "Random")	
					{
						m_cFlashDrive[i].setPatternMode(nPatternIndex, PATTERN_RANDOM);
					}
					else if(szWritePattern == "Custom")	
					{
						m_cFlashDrive[i].setPatternMode(nPatternIndex, PATTERN_CUSTOM);
						m_cFlashDrive[i].setCustomFile(nPatternIndex, szCustomFile);	
					}
					else
					{
						m_cFlashDrive[i].setPatternMode(nPatternIndex, PATTERN_FIX);
					}

					/* Set write pattern for each drives */
					CString szTempPattern = szWritePattern.Mid(2, 2);
					wchar_t *end = NULL;
					nPattern = _tcstol(szTempPattern, &end, 16);	// Convert hex cstring to integer in decimal by "_tcstol" function
					m_cFlashDrive[i].setPattern(nPatternIndex, nPattern);
				}
			}
			else
			{	// Not list control
				/* Set write pattern from combox2 */
				CString szComboBox2;
				((CComboBox*)GetDlgItem(IDC_COMBO2))->GetWindowText(szComboBox2);
				
				if(szComboBox2 == "0 - 255")
				{
					m_cFlashDrive[i].setPatternMode(nPatternIndex, PATTERN_ASCEND);
					#ifdef DUMP_GET_DRIVE_INFORMATION
					TRACE(_T("[%s]getDriveInformation=>PatternMode = PATTERN_ASCEND\n"), m_cFlashDrive[i].getDriveLabel());
					#endif
				}
				else if(szComboBox2 == "Random")
				{
					m_cFlashDrive[i].setPatternMode(nPatternIndex, PATTERN_RANDOM);
					#ifdef DUMP_GET_DRIVE_INFORMATION
					TRACE(_T("[%s]getDriveInformation=>PatternMode = PATTERN_RANDOM\n"), m_cFlashDrive[i].getDriveLabel());	
					#endif
				}
				else if(szComboBox2 == "Custom")
				{
					CString szCustomFile;

					CListBox* pCustomFile = (CListBox*)GetDlgItem(IDC_EDITSelectFile);	// D: IDC_EDIT1 1034
					pCustomFile->GetWindowTextW(szCustomFile);
					if(szCustomFile == "")	
					{
						if(!g_DriveControl[i].Option.BIT.HideWarning)
						{
							szErrorMessage.Format(_T("Please Select Pattern File\n"));	
							AfxMessageBox(szErrorMessage);							
						}
#if(DEBUG_ERROR_CODE == TRUE)					
						g_ErrorCode[i].PARSE.SYNOPSIS.Symptom = ERROR_PATTERN;
#endif
						return ERROR_PATTERN;
					}

					CFile MyFile;
					CFileException fileException;
					if(!MyFile.Open(szCustomFile,CFile::modeRead))	// Take 1st drive
					{
						if(!g_DriveControl[i].Option.BIT.HideWarning)
						{
							szErrorMessage.Format(_T("Can't open file!\n"));	
							AfxMessageBox(szErrorMessage);						
						}
#if(DEBUG_ERROR_CODE == TRUE)					
						g_ErrorCode[i].PARSE.SYNOPSIS.Symptom= ERROR_OPEN_FILE;
#endif
						#ifdef DUMP_GET_DRIVE_INFORMATION
						TRACE( _T("[%s]getDriveInformation=>Can't open file %s, error = %u\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[0].getCustomFile(nPatternIndex), fileException.m_cause );	
						#endif
						return ERROR_OPEN_FILE;
					}

					/* Store pattern information in member variables */
					m_cFlashDrive[i].setPatternMode(nPatternIndex, PATTERN_CUSTOM);
					m_cFlashDrive[i].setCustomFile(nPatternIndex, szCustomFile);
#ifdef DUMP_GET_DRIVE_INFORMATION
					TRACE(_T("[%s]getDriveInformation=>CustomFile = \n"),m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getCustomFile(nPatternIndex));	
#endif
				}
				else
				{
					m_cFlashDrive[i].setPatternMode(nPatternIndex, PATTERN_FIX);
#ifdef DUMP_GET_DRIVE_INFORMATION 
					TRACE(_T("[%s]getDriveInformation=>PatternMode = PATTERN_FIX\n"), m_cFlashDrive[i].getDriveLabel());
					TRACE(_T("[%s]getDriveInformation=>Pattern = %d\n"), m_cFlashDrive[i].getDriveLabel(), m_cFlashDrive[i].getPattern(nPatternIndex));	
#endif
				}	
								
				/* Set write pattern of each drives */
				CString szTempPattern = szComboBox2.Mid(2, 2);
				wchar_t *end = NULL;
				nPattern = _tcstol(szTempPattern, &end, 16);	// Convert hex cstring to integer in decimal by "_tcstol" function
				m_cFlashDrive[i].setPattern(nPatternIndex, nPattern);
			}
		}
		else
		{
			m_cFlashDrive[i].setDisable();	
		}
	}
	return ERROR_OK_END;
 }

 void CCopyCompareDlg::OnBnClickedPause()
 {
	 // TODO: Add your control notification handler code here
	CRect rect;
	 if(!g_pauseStart)
	 {
	 	m_pauseStart.EnableWindow( true );
		 g_pauseStart = true;
		
		 for(int i = START_DRIVE_C; i < TOTAL_DRIVE_NUMBER; i++)
		 {
#ifdef DEBUG_FORCE_STOP
			if(g_DriveControl[i].Status.BIT.Processing | g_DriveControl[i].Status.BIT.Looping| g_DriveControl[i].Status.BIT.Patterning)
			{
				 g_DriveControl[i].Status.BIT.ForceStop = SET;	
			}
#endif			 
			 g_DriveControl[i].Mode= MODE_STOP;	
			(CButton*)GetDlgItem(CHECKBOX_OFFSET+i)->EnableWindow( true );
			
		 }
#ifdef DUMP_CLICKED_PAUSE
	 	TRACE(_T("OnBnClickedPause=>Pause!\n"));
#endif		
	 }
	 this->updateReport();
 }

void CCopyCompareDlg::showOK(int DriveIndex)
{
	if(g_DriveControl[DriveIndex].Status.BIT.ErrorOccurred)
	{	// NG
		g_hBmp = ::LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_BITMAP2));
	}
	else
	{	// OK
		g_hBmp = ::LoadBitmap(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDB_BITMAP1));
	}
		
	switch(DriveIndex)
	{
		case 3:
			m_bitmapD.SetBitmap(g_hBmp);
			break;
		case 4:
			m_bitmapE.SetBitmap(g_hBmp);
			break;
		case 5:
			m_bitmapF.SetBitmap(g_hBmp);
			break;
		case 6:
			m_bitmapG.SetBitmap(g_hBmp);
			break;
		case 7:
			m_bitmapH.SetBitmap(g_hBmp);
			break;
		case 8:
			m_bitmapI.SetBitmap(g_hBmp);
			break;
		case 9:
			m_bitmapJ.SetBitmap(g_hBmp);
			break;
		case 10:
			m_bitmapK.SetBitmap(g_hBmp);
			break;
		case 11:
			m_bitmapL.SetBitmap(g_hBmp);
			break;
		case 12:
			m_bitmapM.SetBitmap(g_hBmp);
			break;
		case 13:
			m_bitmapN.SetBitmap(g_hBmp);
			break;
		case 14:
			m_bitmapO.SetBitmap(g_hBmp);
			break;
		case 15:
			m_bitmapP.SetBitmap(g_hBmp);
			break;
		case 16:
			m_bitmapQ.SetBitmap(g_hBmp);
			break;
		case 17:
			m_bitmapR.SetBitmap(g_hBmp);
			break;
		case 18:
			m_bitmapS.SetBitmap(g_hBmp);
			break;
		case 19:
			m_bitmapT.SetBitmap(g_hBmp);
			break;
		case 20:
			m_bitmapU.SetBitmap(g_hBmp);
			break;
		case 21:
			m_bitmapV.SetBitmap(g_hBmp);
			break;
		case 22:
			m_bitmapW.SetBitmap(g_hBmp);
			break;
		case 23:
			m_bitmapX.SetBitmap(g_hBmp);
			break;
		case 24:
			m_bitmapY.SetBitmap(g_hBmp);
			break;
		case 25:
			m_bitmapZ.SetBitmap(g_hBmp);
			break;
		default:
			break;
	}						
}

void CCopyCompareDlg::hideOK()
{
	m_bitmapD.SetBitmap(NULL);
	m_bitmapE.SetBitmap(NULL);
	m_bitmapF.SetBitmap(NULL);
	m_bitmapG.SetBitmap(NULL);
	m_bitmapH.SetBitmap(NULL);
	m_bitmapI.SetBitmap(NULL);
	m_bitmapJ.SetBitmap(NULL);
	m_bitmapK.SetBitmap(NULL);
	m_bitmapL.SetBitmap(NULL);
	m_bitmapM.SetBitmap(NULL);
	m_bitmapN.SetBitmap(NULL);
	m_bitmapO.SetBitmap(NULL);
	m_bitmapP.SetBitmap(NULL);
	m_bitmapQ.SetBitmap(NULL);
	m_bitmapR.SetBitmap(NULL);
	m_bitmapS.SetBitmap(NULL);
	m_bitmapT.SetBitmap(NULL);
	m_bitmapU.SetBitmap(NULL);
	m_bitmapV.SetBitmap(NULL);
	m_bitmapW.SetBitmap(NULL);
	m_bitmapX.SetBitmap(NULL);
	m_bitmapY.SetBitmap(NULL);
	m_bitmapZ.SetBitmap(NULL);
}

#ifdef DEBUG_UPDATE_REPORT
 void CCopyCompareDlg::updateReport()
{
	CString szTempLabel, szLoopNumber, szMode, szAddressTitle, szTimeSecond, szTransferRate;
#ifdef DEBUG_DISDINGUISH_SPEED
	CString szReadWriteSpeed, szSection, szErrorAddress, szErrorCode;
	double dbReadSpeed, dbWriteSpeed;
#endif	
	UINT nProcessing = 0;

#ifdef DEBUG_SHOW_PERCENTAGE
	CString szPercentage;
#endif

#ifdef DEBUG_DUMP_LOG
	int nLoopCount[TOTAL_DRIVE_NUMBER] ;
	for(int i = START_DRIVE_A; i < TOTAL_DRIVE_NUMBER; i++)
	{
		nLoopCount[i] = 1;
	}
#endif

#ifdef DEBUG_TIMER
		CString szDay, szHour, szMinute, szSecond;
		if(g_myTimer.nDay)
		{
			szDay.Format(_T("Through Time: %d Day, %02d:%02d:%02d"), 
						g_myTimer.nDay, 
						g_myTimer.nHour,
						g_myTimer.nMinute,
						g_myTimer.nSecond);
		}
		else
		{
			szDay.Format(_T("Through Time: %02d:%02d:%02d"),
						g_myTimer.nHour,
						g_myTimer.nMinute,
						g_myTimer.nSecond);
		}
		SetDlgItemText(IDC_TIME, szDay);
#endif		

		/* Scan all drives */
		for(int i = START_DRIVE_C; i < TOTAL_DRIVE_NUMBER; i++)
		{	
			if(g_DriveControl[i].Status.BIT.Processing)
			{	// Processing 
				g_dbProcessTime[i] = g_myTimer.nSecond + g_myTimer.nMinute * SECONDS_PER_MINUTE + g_myTimer.nHour * SECONDS_PER_HOUR + g_myTimer.nDay * SECONDS_PER_DAY;
			}
		
#ifdef DUMP_UPDATE_REPORT
			if(m_cFlashDrive[i].getStatus())
			{
				TRACE(_T("[%s]updateReport=>Enable\n"), m_cFlashDrive[i].getDriveLabel());	
			}
			else
			{
				TRACE(_T("[%s]updateReport=>Disable\n"), m_cFlashDrive[i].getDriveLabel());
			}
#endif
			
			if(m_cFlashDrive[i].getStatus())	
			{	// Enabled drives
				nProcessing = (g_DriveControl[i].Status.BIT.Processing |nProcessing);
				
				switch(g_DriveControl[i].Mode)
				{
					case MODE_INITIAL:
						szMode = "[Initialize] ";
						break;
					
					case MODE_SEARCH:
						szMode = "[Search] ";
						break;
						
					case MODE_SET_DATA:	
						szMode = "[Set Data]";
						break;
				
					case MODE_READ:
						szMode = "[Read] ";
						break;
				
					case MODE_WRITE:
						szMode = "[Write] ";
						break;

					case MODE_COMPARE:
						szMode = "[Compare] ";
						break;
					
					case MODE_FORMAT:
						szMode = "[Format] ";
						break;

					case MODE_PAUSE:
						szMode = "[Pause]";
						break;
						
					case MODE_STOP:
						szMode = "[Stop] ";

					default:
						int nCheckboxNumber = m_cFlashDrive[i].getCheckBoxNumber();
						(CButton*)GetDlgItem(nCheckboxNumber)-> EnableWindow( true );
					
						int nEditboxNumber = m_cFlashDrive[i].getEditBoxNumber();
						(CButton*)GetDlgItem(nEditboxNumber)-> EnableWindow( true );
						(CButton*)GetDlgItem(nEditboxNumber+1)-> EnableWindow( true );
#ifdef DUMP_UPDATE_REPORT
						TRACE(_T("[%s]updateReport=>EditBox = %d\n"), m_cFlashDrive[i].getDriveLabel(), nEditboxNumber);
#endif

#if (DEBUG_PRODUCTION_LINE == TRUE)
						if(g_DriveControl[i].Status.BIT.ForceStop == SET)
						{
							g_DriveControl[i].Status.BIT.ErrorOccurred = SET;
						}
						showOK(i);
#elif (DEBUG_TEST_TEAM == TRUE)
						showOK(i);
#else
						if(g_DriveControl[i].Status.BIT.ForceStop == CLR)
						{
							showOK(i);
						}
#endif						
						szLoopNumber.Format(_T("Loop %d - "),g_nLoopCount[i]);
#ifdef DEBUG_SHOW_PERCENTAGE		
						szPercentage.Format(_T("%d%%"), g_nPercentage[i]);
#endif						
						szErrorAddress.Format(_T(" at %d"), g_lStartSector[i]);
						
#if(DEBUG_ERROR_CODE == TRUE)	// CTJ15020216 Return error code
						szErrorCode.Format(_T("%02X-%02X-%02X"), 							
										g_ErrorCode[i].PARSE.SYNOPSIS.Mode, 
										g_ErrorCode[i].PARSE.SYNOPSIS.Status, 
										g_ErrorCode[i].PARSE.SYNOPSIS.Symptom);
#endif
				}
				
#ifdef DEBUG_DISDINGUISH_SPEED
					dbReadSpeed = g_dbReadLength[i]/ g_dbReadTime[i]/SECTORS_PER_MBYTE*1000;
					dbWriteSpeed = g_dbWriteLength[i]/ g_dbWriteTime[i]/SECTORS_PER_MBYTE*1000;
					if((g_DriveControl[i].Status.BIT.Reading == SET)||(g_DriveControl[i].Option.BIT.ReadEachWrite))
					{
						szReadWriteSpeed.Format(_T(" write %.2f MB/s  read %.2f MB/s"), dbWriteSpeed, dbReadSpeed);
					}
					else
					{
						szReadWriteSpeed.Format(_T(" write %.2f MB/s"), dbWriteSpeed);
					}
#endif				

#ifdef DUMP_UPDATE_REPORT
					TRACE(_T("[%s]updateReport=>%s\n"), m_cFlashDrive[i].getDriveLabel(), szReadWriteSpeed);
					TRACE(_T("[%s]updateReport=>szTransferRate = %s\n"), m_cFlashDrive[i].getDriveLabel(), szTransferRate);
#endif					
				/* Spell display strings */
				szLoopNumber.Format(_T("Loop %d - "),g_nLoopCount[i]);
				
				if(m_cFlashDrive[i].getPatternCount())
				{
					switch(m_cFlashDrive[i].getPatternMode(g_nSectionCount[i]))
					{
						case PATTERN_ASCEND:
							szSection.Format(_T(" Pattern(0-255) "));
							break;
							
						case PATTERN_FIX:
							szSection.Format(_T(" Pattern(0x%02x) "), m_cFlashDrive[i].getPattern(g_nSectionCount[i]));
							break;
							
						case PATTERN_CUSTOM:
							szSection.Format(_T(" Pattern(Custom) "));
							break;
							
						case PATTERN_RANDOM:
							szSection.Format(_T(" Pattern(Random) "));
							break;
							
						default:
							szSection.Format(_T(" Pattern(Unknown) "));
							break;
					}
				}
				
#ifdef DEBUG_SHOW_PERCENTAGE
				if(g_DriveControl[i].Status.BIT.Reading)
				{	// Reading
					szPercentage.Format(_T("%d%%"), g_nPercentageRead[i]);
				}
				else
				{	// Writing
					//TRACE(_T("g_nPercentageWrite[%d] = %d\n"), i, g_nPercentageWrite[i]);
					szPercentage.Format(_T("%d%%"), g_nPercentageWrite[i]);
				}
				
				szTempLabel =  driveLabelTbl[i] + szMode + szLoopNumber + szSection + szPercentage;
#else				
				szTempLabel =  driveLabelTbl[i] + szMode + szLoopNumber;
#endif
				if(g_DriveControl[i].Status.BIT.Formating)
				{
					szTempLabel =  driveLabelTbl[i] + szMode;	
				}

#ifdef DEBUG_TRANSFER_RATE	
				if(!g_DriveControl[i].Status.BIT.Processing)
				{	// Show transfer rate when copy compare stops
#ifdef DEBUG_DISDINGUISH_SPEED		
					if(g_DriveControl[i].Status.BIT.ErrorOccurred)
					{
#if(DEBUG_ERROR_CODE == TRUE)
						szTempLabel =  driveLabelTbl[i]  + szMode + szLoopNumber + szSection + szErrorCode + szErrorAddress;
#else
						szTempLabel =  driveLabelTbl[i]  + szMode + szLoopNumber + szSection + szErrorAddress;
#endif							
					}
					else
					{
						szTempLabel = driveLabelTbl[i] + szMode + szLoopNumber + szPercentage + szReadWriteSpeed;
					}
#else
					szTempLabel = szTempLabel + szTransferRate;				
#endif				
				}
#endif
				szTempLabel.Replace(L"//",L"\\");

#ifdef DEBUG_SHOW_PERCENTAGE
				CProgressCtrl *pProgCtrl=(CProgressCtrl*)GetDlgItem(1096+i);	// IDC_PROGRESS_D = 1099 

					if(g_DriveControl[i].Status.BIT.Reading)
					{
						int nPrePos=pProgCtrl->SetPos(g_nPercentageRead[i]);
					}
					else
					{
						int nPrePos=pProgCtrl->SetPos(g_nPercentageWrite[i]);
					}
				SetDlgItemText(IDC_PROGRESS_X+i, szTempLabel);	//IDC_STATIC_D = 1122, 1122 -3 =1119=>IDC_PROGRESS_X 	
#endif
				
#ifdef DEBUG_DUMP_LOG
				/* Show error log: select H-scroll and V- scroll in properties */
				CFile MyFile;
				CString szErrorLog, szLogFile = _T("DumpLog.dat");
				ULONGLONG dwFileLength, dwReadLength;
				
				if(!MyFile.Open(szLogFile, CFile::modeRead))
				{
					#ifdef DUMP_UPDATE_REPORT
					TRACE(_T("Unable to open file\n"));
					#endif
					return;
				}
				dwFileLength = MyFile.GetLength();
				dwReadLength = dwFileLength - g_dwErrorLogOffset;
				#ifdef DUMP_UPDATE_REPORT
				TRACE(_T("updateReport=>dwReadLength = %ld\n"), dwReadLength);
				#endif
				if(dwReadLength)
				{
					/* Read ErrorLog.dat */					
					unsigned char *pLogBuffer = (UCHAR *)malloc((int)dwReadLength);
					memset(pLogBuffer, 0, (int)dwReadLength);
					MyFile.Seek( g_dwErrorLogOffset, CFile::begin);
					MyFile.Read((LPTSTR)pLogBuffer, (int)dwReadLength);
					g_dwErrorLogOffset = MyFile.GetPosition();

					/* Deal with CString */
					szErrorLog.Format(_T("%s"), (LPTSTR)pLogBuffer);	// pchar to cstring
					int nStringOffset = 0;
					int nStringStart = 0;
					int nStringLength = szErrorLog.GetLength();
					while(nStringOffset != -1)
					{
						CString szTempErrorLog;
						nStringOffset = szErrorLog.Find(_T("\n"), nStringStart);
						
						if(nStringOffset != -1)
						{
							szTempErrorLog = szErrorLog.Mid(nStringStart, nStringOffset - nStringStart);
							nStringStart = nStringOffset + 1;
							szTempErrorLog +=  _T("\n");	// No need to return line while "\n" not found

							/* Display in list control box */
							int nLength = m_errorLog.GetWindowTextLength();
							m_errorLog.SetSel(nLength, nLength);
							m_errorLog.ReplaceSel(szTempErrorLog);
						}
					}
		
					/* Release memory */
					free(pLogBuffer);	
					pLogBuffer = nullptr;
				}
				MyFile.Close();
#endif			
			}
			else
			{
				/* Do nothing */
			}
		}
		
		/* Enable all bottons */
		if(!nProcessing)
		{	// All drives are stopped=>kill timer and unlock all buttons
			(CButton*)GetDlgItem(IDC_BUTTON1)-> EnableWindow( true );
			(CButton*)GetDlgItem(IDC_BUTTON3)-> EnableWindow( true );
			(CButton*)GetDlgItem(IDC_BUTTON5)-> EnableWindow( true );
			(CButton*)GetDlgItem(IDC_BUTTON6)-> EnableWindow( true );
			(CButton*)GetDlgItem(IDC_RADIO1)-> EnableWindow( true );
			(CButton*)GetDlgItem(IDC_RADIO2)-> EnableWindow( true );	
			(CButton*)GetDlgItem(IDC_COMBO1)-> EnableWindow( true );	
			(CButton*)GetDlgItem(IDC_COMBO2)-> EnableWindow( true );	
			(CButton*)GetDlgItem(IDC_COMBO3)-> EnableWindow( true );
			(CButton*)GetDlgItem(IDC_IDE_COMMAND)-> EnableWindow( true );
#if(DEBUG_TAB_CONTROL == TRUE)
			m_Page1.GetDlgItem(IDC_ERROR_STOP)-> EnableWindow( true );
			m_Page1.GetDlgItem(IDC_BEEP)-> EnableWindow( true );
			m_Page1.GetDlgItem(IDC_HIDE_WARNING)-> EnableWindow( true );
			m_Page1.GetDlgItem(IDC_CHECK_FORMAT)-> EnableWindow( true );

			/* CTJ15012716 Enable timer window */
			m_Page2.GetDlgItem(IDC_DAY)-> EnableWindow( true );
			m_Page2.GetDlgItem(IDC_HOUR)-> EnableWindow( true );	
			m_Page2.GetDlgItem(IDC_MINUTE)-> EnableWindow( true );	
			m_Page2.GetDlgItem(IDC_SECOND)-> EnableWindow( true );	
			
			(CButton*)GetDlgItem(IDC_READ_MODE)-> EnableWindow( true );	
			(CButton*)GetDlgItem(IDC_ACCESS_MODE)-> EnableWindow( true );
			(CButton*)GetDlgItem(IDC_DISK_FORMAT)-> EnableWindow( true );
			(CButton*)GetDlgItem(IDC_SAVE_INI)-> EnableWindow( true );
			(CButton*)GetDlgItem(IDC_LOAD_INI)-> EnableWindow( true );
			this->KillTimer(1);
			
			// Disable stop button after process stops
			(CButton*)GetDlgItem(IDC_BUTTON7)->EnableWindow( false );

			if(((CButton*)m_Page1.GetDlgItem(IDC_BEEP))->GetCheck())	// CTJ15012715
			{
				int nBeepcount = 2;
				while(nBeepcount--)		
				{
					Beep(2000, 100);
				}
			}
#endif
		}
		return;
}
#endif

 void CCopyCompareDlg::OnTimer(UINT_PTR nIDEvent)
 {
	 // TODO: Add your message handler code here and/or call default

	 //CDialogEx::OnTimer(nIDEvent);
	 this->updateReport();

#ifdef DEBUG_TIMER
	 g_myTimer.nSecond++;
	if(g_myTimer.nSecond>=SECONDS_PER_MINUTE)
	{
		g_myTimer.nSecond= 0;
		g_myTimer.nMinute++;
		if(g_myTimer.nMinute >= MINUTES_PER_HOUR)
		{
			g_myTimer.nMinute = 0;
			g_myTimer.nHour++;
			if(g_myTimer.nHour >= HOURS_PER_DAY)
			{
				g_myTimer.nHour= 0;
				g_myTimer.nDay++;
			}
		}
	}

	/* CTJ15012717 Check timer stop */
	if((g_myTimer.nDay == g_StopTimer.nDay)&&
		(g_myTimer.nHour == g_StopTimer.nHour)&&
		(g_myTimer.nMinute == g_StopTimer.nMinute)&&
		(g_myTimer.nSecond == g_StopTimer.nSecond))
	{
		this->OnBnClickedPause();
	}
#endif
 }

#ifdef DEBUG_SELECT_FILE	
void CCopyCompareDlg::OnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
	UINT nSelect = m_selectPattern.GetCurSel();
	switch(nSelect)
	{
	case COMBOBOX2_CUSTOM:
		(CButton*)GetDlgItem(IDC_BUTTON8)-> EnableWindow( true );
		(CEdit*)GetDlgItem(IDC_EDITSelectFile)->EnableWindow(true);	
		break;
	default:
		(CButton*)GetDlgItem(IDC_BUTTON8)-> EnableWindow( false );
		(CEdit*)GetDlgItem(IDC_EDITSelectFile)->EnableWindow(false);	
		break;
	}
}

void CCopyCompareDlg::OnBnClickedSelectFile()
{
	CString szFilter = _T("*.* | *.* ||");	
	CFileDialog fd(true, _T(""), _T("*.*"), OFN_HIDEREADONLY, szFilter, this);
	
	if(fd.DoModal() == IDOK)
	{
		for(int i = START_DRIVE_A; i < TOTAL_DRIVE_NUMBER; i++)
		{
			m_DriveInformTbl[i].szCustomFile = fd.GetPathName();
#ifdef DUMP_SELECT_FILE		
			TRACE(_T("m_DriveInformTbl[i].szCustomFile = %s\n"), m_DriveInformTbl[i].szCustomFile);	
#endif
		}
		SetDlgItemText(IDC_EDITSelectFile, m_DriveInformTbl[0].szCustomFile);
	}
	else
	{
		// TBD
	}
}
#endif

void CCopyCompareDlg::OnBnClickedSaveIni()
{
	CString szFilter = _T("*.* |*.* ||") ;
	CString szFileName;
	CFileDialog fd(true, _T(""), _T("*.ini"), OFN_HIDEREADONLY, szFilter, this);	
	if(fd.DoModal() == IDOK)
	{
		szFileName = fd.GetPathName();
		#ifdef DUMP_SAVE_INI
		TRACE(_T("szFileName = %s\n"), szFileName);
		#endif
	}
	else
	{
		// TBD
	}
	
	CString szDriveType;
	if(((CButton*)GetDlgItem(IDC_RADIO1))->GetCheck())
	{
		szDriveType.Format(_T("DriveType=Removable\r\n"));
	}
	else
	{
		szDriveType.Format(_T("DriveType=Fix\r\n"));
	}

	int nCopySize = ((CComboBox*)GetDlgItem(IDC_COMBO1))->GetCurSel();
	CString szCopySize;
	szCopySize.Format(_T("CopySize=%d\r\n"), nCopySize);

	CString szLoopNumber;
	int nLoopNumber= ((CComboBox*)GetDlgItem(IDC_COMBO3))->GetCurSel();
	szLoopNumber.Format(_T("LoopNumber=%d\r\n"), nLoopNumber);

	/* Get pattern count */
	int nPatternCount = m_List.GetItemCount();
	CString szPatternCount;
	szPatternCount.Format(_T("PatternCount=%d\r\n"), nPatternCount);

	/* Get Format after copy compare */
	CString szDiskFormat;
	((CComboBox*)GetDlgItem(IDC_DISK_FORMAT))->GetWindowText(szDiskFormat);
	szDiskFormat = _T("DiskFormat=") + szDiskFormat + _T("\r\n");
	
#ifdef DEBUG_ACCESS_MODE
		CString szAccessMode;
		((CComboBox*)GetDlgItem(IDC_ACCESS_MODE))->GetWindowText(szAccessMode);
		szAccessMode = _T("AccessMode=") + szAccessMode + _T("\r\n");
#endif

#ifdef DEBUG_READ_MODE
		CString szReadMode;
		((CComboBox*)GetDlgItem(IDC_READ_MODE))->GetWindowText(szReadMode);
		szReadMode = _T("ReadMode=") + szReadMode + _T("\r\n");	
#endif			
			
#ifdef DEBUG_ATA_COMMAND
		// Modify string format
		CString szDriveCommand;
		((CComboBox*)GetDlgItem(IDC_IDE_COMMAND))->GetWindowText(szDriveCommand);
		szDriveCommand = szDriveCommand.Mid(0, 4);	
		szDriveCommand.Remove(' ');					
		szDriveCommand = _T("DriveCommand=") + szDriveCommand + _T("\r\n");
#endif			
			
#if(DEBUG_TAB_CONTROL == TRUE)	
		CString szBeepWarning;
		if(((CButton*)m_Page1.GetDlgItem(IDC_BEEP))->GetCheck())
		{
			szBeepWarning.Format(_T("BeepWarning=true\r\n"));
		}
		else
		{
			szBeepWarning.Format(_T("BeepWarning=false\r\n"));
		}

		CString szHideWarning;
		if(((CButton*)m_Page1.GetDlgItem(IDC_HIDE_WARNING))->GetCheck())
		{
			szHideWarning.Format(_T("HideWarning=true\r\n"));
		}
		else
		{
			szHideWarning.Format(_T("HideWarning=false\r\n"));
		}

		CString szErrorStop;
		if(((CButton*)m_Page1.GetDlgItem(IDC_ERROR_STOP))->GetCheck())
		{
			szErrorStop.Format(_T("ErrorStop=true\r\n"));
		}
		else
		{
			szErrorStop.Format(_T("ErrorStop=false\r\n"));
		}

		CString szFormatAfterward;
		if(((CButton*)m_Page1.GetDlgItem(IDC_CHECK_FORMAT))->GetCheck())
		{
			szFormatAfterward.Format(_T("FormatAfterward=true\r\n"));
		}
		else
		{
			szFormatAfterward.Format(_T("FormatAfterward=false\r\n"));
		}
#endif
	CFile MySetting;
	CFileException fileException;
	CString szLoopCount;
	CString szStartAddress, szEndAddress, szDriveLabel, szTempStart, szTempEnd;	
#ifdef DEBUG_DELETE_DISK
	CString szTempString, szDummyString, szLabel[26];
	unsigned int nFileLength;
	int nStart=0;
	int nLabelNumber=0,	// CTJ14071401
		nLabelCount=0;
	
	if (MySetting.Open(szFileName, CFile::modeRead| CFile::shareDenyNone, &fileException))
	{
		nFileLength = (unsigned int)MySetting.GetLength();
		MySetting.SeekToBegin();
		MySetting.Read(szDummyString.GetBuffer(nFileLength), nFileLength);
		szDummyString.ReleaseBuffer(nFileLength);
		MySetting.Close();
		
		for(nLabelCount = 0; nLabelCount < TOTAL_DRIVE_NUMBER; nLabelCount++)
		{
			szLabel[nLabelCount] = searchStringLength(_T("disk"), szDummyString, &nStart);
#ifdef DUMP_SAVE_INI
			TRACE(_T("%s"), szLabel[nLabelCount]);
#endif
			if(szLabel[nLabelCount] == _T(""))
			{
				nLabelNumber = nLabelCount;
				break;
			}
		}
	}
#endif

	if (MySetting.Open(szFileName, CFile::modeCreate |CFile::modeWrite| CFile::shareDenyNone, &fileException))
	{
#ifdef DEBUG_DELETE_DISK
		// CTJ14071401
		if(nLabelNumber)
		{
			CString szDeleteDisk("[Delete_Disk]\r\n");
			MySetting.Write(szDeleteDisk, szDeleteDisk.GetLength()*2);
			for(nLabelCount = 0; nLabelCount < nLabelNumber; nLabelCount++)
			{
				CString szTest(_T("disk="));
				szTest = szTest + szLabel[nLabelCount] + _T("\r\n");
				MySetting.Write(szTest, szTest.GetLength()*2);
			}
		}
#endif
		//-------------Mode-----------------------------------------//
		CString szMode("[Mode]\r\n");
		MySetting.Write(szMode, szMode.GetLength()*2);

#ifdef DEBUG_VENDOR_MODE		
		for(int i = START_DRIVE_C; i < TOTAL_DRIVE_NUMBER; i++)
		{
			int nCheckState = ((CButton*)GetDlgItem(CHECKBOX_OFFSET+i))->GetCheck();
			if(nCheckState)
			{
				szDriveLabel = searchDriveLabel(i);	
		
				/* Set start address from editbox1 */
				CListBox* pStartAdress = (CListBox*)GetDlgItem(IDC_RADIO2+i*2);
				pStartAdress->GetWindowTextW(szTempStart);

				/* Get end address from editbox2 */
				CListBox* pEndAddress = (CListBox*)GetDlgItem(IDC_BUTTON1+i*2);
				pEndAddress->GetWindowTextW(szTempEnd);

				szStartAddress.Format(_T("[%s]StartAddress=%s\r\n"), szDriveLabel, szTempStart);
				szEndAddress.Format(_T("[%s]EndAddress=%s\r\n"), szDriveLabel, szTempEnd);

				MySetting.Write(szStartAddress, szStartAddress.GetLength()*2);
				MySetting.Write(szEndAddress, szEndAddress.GetLength()*2);
			}
		}
#endif	
		MySetting.Write(szDriveType, szDriveType.GetLength()*2);
		MySetting.Write(szAccessMode, szAccessMode.GetLength()*2);
		MySetting.Write(szReadMode, szReadMode.GetLength()*2);
		MySetting.Write(szDriveCommand, szDriveCommand.GetLength()*2);
		MySetting.Write(szDiskFormat, szDiskFormat.GetLength()*2);
		//-------------Option----------------------------------------//
		CString szOption("[Option]\r\n");
		MySetting.Write(szOption, szOption.GetLength()*2);
		MySetting.Write(szBeepWarning, szBeepWarning.GetLength()*2);		// Beep if read/write/compare error or copy compare complete
		MySetting.Write(szHideWarning, szHideWarning.GetLength()*2);		// Hide warning
		MySetting.Write(szErrorStop, szErrorStop.GetLength()*2);				// Error stop
		MySetting.Write(szFormatAfterward, szFormatAfterward.GetLength()*2);	// Format afterwards
		//-----------------------------------------------------------//
		MySetting.Write(szCopySize, szCopySize.GetLength()*2);			// Copy size
		MySetting.Write(szLoopNumber, szLoopNumber.GetLength()*2);	// Loop number
		MySetting.Write(szPatternCount, szPatternCount.GetLength()*2);	// Pattern count		
		CString szPattern("[Pattern]\r\n");
		MySetting.Write(szPattern, szPattern.GetLength()*2);

		/* Get write pattern */
		CString szCustomFile, szWritePattern, szPrefix;
		if(nPatternCount)	
		{	// List control
			for(int nPatternIndex = 0; nPatternIndex < nPatternCount; nPatternIndex++)
			{
				szWritePattern = m_List.GetItemText ( nPatternIndex, LIST_PATTERN);
				szPrefix.Format(_T("PTN%d="), nPatternIndex);
				szWritePattern = szPrefix + szWritePattern + _T("\r\n");
				MySetting.Write(szWritePattern, szWritePattern.GetLength()*2);			
			}
		}
		else
		{	// Not list control
			((CComboBox*)GetDlgItem(IDC_COMBO2))->GetWindowText(szWritePattern);
			szPrefix.Format(_T("PTN="));
			szWritePattern = szPrefix + szWritePattern + _T("\r\n");
			MySetting.Write(szWritePattern, szWritePattern.GetLength()*2);
			
			CListBox* pCustomFile = (CListBox*)GetDlgItem(IDC_EDITSelectFile);	// D: IDC_EDIT1 1034
			pCustomFile->GetWindowTextW(szCustomFile);
		}

		for(int nPatternIndex = 0; nPatternIndex < nPatternCount; nPatternIndex++)
		{	// List control
			CString szTempFile = m_List.GetItemText ( nPatternIndex , LIST_PATTERN_CONTENT );
																									
			if(!(szTempFile == ""))	
			{
				CString szCustomize("[Customize]\r\n");
				MySetting.Write(szCustomize, szCustomize.GetLength()*2);
			}
			nPatternIndex++;
		}		

		if(nPatternCount)	
		{	// List control
			int nCustomCount = 0;
			for(int nPatternIndex = 0; nPatternIndex < nPatternCount; nPatternIndex++)
			{
				CString szTempFile = m_List.GetItemText ( nPatternIndex , LIST_PATTERN_CONTENT);
				if(!(szTempFile == ""))	
				{
					szPrefix.Format(_T("CustomizeFile%d="), nCustomCount);
					szCustomFile = szPrefix + szTempFile + _T("\r\n");
					MySetting.Write(szCustomFile, szCustomFile.GetLength()*2);
					nCustomCount++;
				}
			}
		}
		else
		{	// Not list control
			CListBox* pCustomFile = (CListBox*)GetDlgItem(IDC_EDITSelectFile);	// D: IDC_EDIT1 1034
			pCustomFile->GetWindowTextW(szCustomFile);
		}

		MySetting.Close();
	}

}


IniTable VendorIniTbl[] =
{	// Item name			Description				DlgType			ID					Value
	{_T("DriveType"), 		_T("Removable"),			DLG_RADIO, 		IDC_RADIO1,		 	1 },
	{_T("DriveType"), 		_T("Fix")		,			DLG_RADIO, 		IDC_RADIO2,		 	1 },
	{_T("[D]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT1,		 	0 },
	{_T("[D]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT2,		 	1 },
	{_T("[E]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT3,		 	0 },
	{_T("[E]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT4,		 	1 },
	{_T("[F]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT5,		 	0 },
	{_T("[F]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT6,		 	1 },
	{_T("[G]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT7,		 	0 },
	{_T("[G]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT8,		 	1 },
	{_T("[H]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT9,		 	0 },
	{_T("[H]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT10,		 	1 },
	{_T("[I]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT11,		 	0 },
	{_T("[I]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT12,		 	1 },
	{_T("[J]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT13,		 	0 },
	{_T("[J]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT14,		 	1 },
	{_T("[K]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT15,		 	0 },
	{_T("[K]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT16,		 	1 },
	{_T("[L]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT17,		 	0 },
	{_T("[L]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT18,		 	1 },
	{_T("[M]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT19,		 	0 },
	{_T("[M]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT20,		 	1 },
	{_T("[N]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT21,		 	0 },
	{_T("[N]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT22,		 	1 },
	{_T("[O]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT23,		 	0 },
	{_T("[O]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT24,		 	1 },
	{_T("[P]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT25,		 	0 },
	{_T("[P]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT26,		 	1 },
	{_T("[Q]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT27,		 	0 },
	{_T("[Q]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT28,		 	1 },
	{_T("[R]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT29,		 	0 },
	{_T("[R]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT30,		 	1 },
	{_T("[S]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT31,		 	0 },
	{_T("[S]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT32,		 	1 },
	{_T("[T]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT33,		 	0 },
	{_T("[T]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT34,		 	1 },
	{_T("[U]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT35,		 	0 },
	{_T("[U]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT36,		 	1 },
	{_T("[V]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT37,		 	0 },
	{_T("[V]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT38,		 	1 },
	{_T("[W]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT39,		 	0 },
	{_T("[W]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT40,		 	1 },
	{_T("[X]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT41,		 	0 },
	{_T("[X]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT42,		 	1 },
	{_T("[Y]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT43,		 	0 },
	{_T("[Y]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT44,		 	1 },
	{_T("[Z]StartAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT45,		 	0 },
	{_T("[Z]EndAddress"), 	_T("FFFFFFFF"),				DLG_EDIT, 		IDC_EDIT46,		 	1 },
	{_T("AccessMode"), 		_T("Sequential"),			DLG_COMBOBOX, 	IDC_ACCESS_MODE,	 	0 },
	{_T("AccessMode"),		_T("Randomize"),			DLG_COMBOBOX, 	IDC_ACCESS_MODE,	 	1 },
	{_T("ReadMode"),		_T("Read each write"),		DLG_COMBOBOX, 	IDC_READ_MODE,		0 },
	{_T("ReadMode"),		_T("Read after write complete"),	DLG_COMBOBOX, 	IDC_READ_MODE,		1 },
	{_T("DriveCommand"),	_T("SCSI"),					DLG_COMBOBOX, 	IDC_IDE_COMMAND,	0 },
	{_T("DriveCommand"),	_T("ATA"),					DLG_COMBOBOX, 	IDC_IDE_COMMAND,	1 },
	{_T("DiskFormat"),		_T("FAT32"),				DLG_COMBOBOX, 	IDC_DISK_FORMAT,		0 },
	{_T("DiskFormat"),		_T("NTFS"),				DLG_COMBOBOX, 	IDC_DISK_FORMAT,		1 },
	{_T("BeepWarning"),		_T("true"),					DLG_CBUTTON, 	IDC_BEEP,				1 },
	{_T("BeepWarning"),		_T("false"),				DLG_CBUTTON,	IDC_BEEP,				0 },
	{_T("ErrorStop"),		_T("true"),					DLG_CBUTTON, 	IDC_ERROR_STOP,		1 },
	{_T("ErrorStop"),		_T("false"),				DLG_CBUTTON,	IDC_ERROR_STOP,		0 },
	{_T("HideWarning"),		_T("true"),					DLG_CBUTTON,	IDC_HIDE_WARNING,	1 },
	{_T("HideWarning"),		_T("false"),				DLG_CBUTTON,	IDC_HIDE_WARNING,	0 },
	{_T("FormatAfterward"),	_T("true"),					DLG_CBUTTON,	IDC_CHECK_FORMAT,	1 },
	{_T("FormatAfterward"),	_T("false"),				DLG_CBUTTON,	IDC_CHECK_FORMAT,	0 },
};

void CCopyCompareDlg::OnBnClickedLoadIni()
{
	CString szFilter = _T("*.* |*.* ||") ;
	CString szFileName;
	CFileDialog fd(true, _T(""), _T("*.ini"), OFN_HIDEREADONLY, szFilter, this);	

	this->initialCheck();
	this->initialList();
	
	if(fd.DoModal() == IDOK)
	{
		szFileName = fd.GetPathName();

		CFile MySetting;
		CFileException fileException;
		CString szTempString;
		unsigned int nFileLength;
	
		if (MySetting.Open(szFileName, CFile::modeRead| CFile::shareDenyNone, &fileException))
		{
			nFileLength = (unsigned int)MySetting.GetLength();
			MySetting.SeekToBegin();
			MySetting.Read(szTempString.GetBuffer(nFileLength), nFileLength);
			szTempString.ReleaseBuffer(nFileLength);	
			MySetting.Close();
 
			int nTableSize = sizeof(VendorIniTbl)/sizeof(*VendorIniTbl) ;
			#ifdef DUMP_LOAD_INI
			TRACE(_T("Size of ini table = %d\n"), nTableSize);
			#endif
			for(int i = 0; i < nTableSize; i++)
			{
				int nStart = 0;
				CString szDescription = searchStringLength(VendorIniTbl[i].ItemName, szTempString, &nStart);
				if(VendorIniTbl[i].Description == szDescription)
				{
					#ifdef DUMP_LOAD_INI
					TRACE(_T("%s = %s\n"), VendorIniTbl[i].ItemName, VendorIniTbl[i].Description);
					#endif
					if(VendorIniTbl[i].DlgType == DLG_COMBOBOX)
					{
						((CComboBox*)GetDlgItem(VendorIniTbl[i].ID))->SetCurSel(VendorIniTbl[i].Value);
					}
#if(DEBUG_TAB_CONTROL == TRUE)					
					else if((VendorIniTbl[i].ItemName == _T("BeepWarning"))||
						(VendorIniTbl[i].ItemName == _T("ErrorStop"))||
						(VendorIniTbl[i].ItemName == _T("HideWarning"))||
						(VendorIniTbl[i].ItemName == _T("FormatAfterward")))
					{
						((CButton*)m_Page1.GetDlgItem(VendorIniTbl[i].ID))->SetCheck(VendorIniTbl[i].Value);	
					}
#endif					
					else if(VendorIniTbl[i].DlgType == DLG_CBUTTON)
					{
						((CButton*)GetDlgItem(VendorIniTbl[i].ID))->SetCheck(VendorIniTbl[i].Value);	
					}
					else
					{	// Radio
						((CButton*)GetDlgItem(VendorIniTbl[i].ID))->SetCheck(VendorIniTbl[i].Value);	
						if(VendorIniTbl[i].ID == IDC_RADIO1)
						{
							this->OnBnClickedRadio1();
							((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(CLR);	
						}
						else
						{
							this->OnBnClickedRadio2();
							((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(CLR);	
						}
					}
				}
#ifdef DEBUG_VENDOR_MODE				
				else
				{	
					CListBox* pAddress;	
					if(VendorIniTbl[i].DlgType== DLG_EDIT)
					{
						pAddress = (CListBox*)GetDlgItem(VendorIniTbl[i].ID);
						pAddress->SetWindowTextW(szDescription);
					}
				}
#endif				
			}
#ifdef DEBUG_DELETE_DISK
			int nDeleteDisk, nStart = 0;
			CString szDeleteDisk, szDummyString;
			szDummyString = szTempString;
			do
			{
				szDeleteDisk = searchStringLength(_T("disk"), szDummyString, &nStart);
				szDeleteDisk = szDeleteDisk + _T(":");
				char *chDeleteDisk = CStringToChar(szDeleteDisk);
				nDeleteDisk = searchDriveIndex(chDeleteDisk);
				
#ifndef DEBUG_VENDOR_MODE
				if(nDeleteDisk)
				{
					this->setCheckBox(nDeleteDisk, FALSE);
					
					/* Disable checkbox */
					int nCheckBoxNumber = m_cFlashDrive[nDeleteDisk].getCheckBoxNumber();
					(CButton*)GetDlgItem(nCheckBoxNumber)-> EnableWindow( false );
					
					/* Disable editbox */
					int nEditBoxNumber = m_cFlashDrive[nDeleteDisk].getEditBoxNumber();
					(CButton*)GetDlgItem(nEditBoxNumber)-> EnableWindow( false );
					(CButton*)GetDlgItem(nEditBoxNumber+1)-> EnableWindow( false );

					/* Clear editbox */
					CListBox* pList = (CListBox*)GetDlgItem(nEditBoxNumber);
					pList->SetWindowText(_T(""));
					pList = (CListBox*)GetDlgItem(nEditBoxNumber+1);
					pList->SetWindowText(_T(""));		
				}
#endif
				#ifdef DUMP_LOAD_INI
				TRACE(_T("nDeleteDisk = %d\n"), nDeleteDisk);
				#endif
			}
			while(szDeleteDisk!=_T(":"));
#endif
			/* Set copy size */
			CString szCopySize = searchStringLength(_T("CopySize"), szTempString, &nStart);
			int nCopySize = _tstoi(szCopySize);
			((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(nCopySize);

			/* Set loop number */
			CString szLoopNumber= searchStringLength(_T("LoopNumber"), szTempString, &nStart);
			int nLoopNumber = _tstoi(szLoopNumber);
			((CComboBox*)GetDlgItem(IDC_COMBO3))->SetCurSel(nLoopNumber);

			/* Get write pattern count */
			CString szPatternCount = searchStringLength(_T("PatternCount"), szTempString, &nStart);	
			int nPatternCount = _tstoi(szPatternCount);
			#ifdef DUMP_LOAD_INI
			TRACE(_T("nPatternCount = %d\n"), nPatternCount);
			#endif

			/* Get path of customize file */
			CString szCustomPath = searchStringLength(_T("CustomizeFile0"), szTempString, &nStart);	

			/* CTJ14062310 Check customize file */
			CFile CustomizeFile;
			CFileException CustomizeException;
			if(szCustomPath !="")
			{
				if (CustomizeFile.Open(szCustomPath, CFile::modeRead| CFile::shareDenyNone, &CustomizeException))
				{
					CustomizeFile.Close();
				}
				else
				{
					CString str;
					str.Format(_T("Customized file is missing\n"));
					AfxMessageBox(str);
				}
			}
			
			/* Scan and set write pattern */
			m_List.DeleteAllItems();	
			for(int nIndex = 0; nIndex < nPatternCount; nIndex++)
			{
				CString szTarget, szIndex, szResult;
				int nStart = 0;
				szIndex.Format(_T("%d"),nIndex);
				szTarget.Format(_T("PTN%d"), nIndex);	
				szResult = searchStringLength(szTarget, szTempString, &nStart);
#ifdef DUMP_LOAD_INI
				TRACE(_T("szResult%d = %s\n"), nIndex, szResult);
#endif
				int nItem = m_List.InsertItem(m_List.GetItemCount() , szIndex);
				m_List.SetItemText(nItem, LIST_PATTERN, szResult);
				if(szResult == "Custom")
				{
					m_List.SetItemText(nItem, LIST_PATTERN_CONTENT, szCustomPath);
				}
			}
		}		
		else
		{
			// TBD
		}
	}
	else
	{
		// TBD
	}
}

void CCopyCompareDlg::OnCbnSelchangeIdeCommand()
{
	// TODO: Add your control notification handler code here
	if(((CComboBox*)GetDlgItem(IDC_IDE_COMMAND))->GetCurSel())
	{	// ATA command selected
		((CComboBox*)GetDlgItem(IDC_COMBO1))->DeleteString(FIX_SPEED_1024K);	
		((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(FIX_SPEED_128K);		
	}
	else
	{	// SCSI command selected
		CString szSpeed = _T("Speed (R/W size): 1024K");
		((CComboBox*)GetDlgItem(IDC_COMBO1))->AddString(szSpeed);
		((CComboBox*)GetDlgItem(IDC_COMBO1))->SetCurSel(FIX_SPEED_1024K);	
	}
}

// CTJ14070901
void CCopyCompareDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogEx::OnOK();
	//this->OnBnClickedLoadIni();
	this->OnBnClickedCopyCompare();	// CTJ15012718
}

#if(DEBUG_TAB_CONTROL == TRUE)
void CCopyCompareDlg::OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int CurSel = m_Tab.GetCurSel();
	switch(CurSel)
	{
	case 0:
		m_Page1.ShowWindow(true);
		m_Page2.ShowWindow(false);
		break;
	case 1:
		m_Page1.ShowWindow(false);
		m_Page2.ShowWindow(true);
		break;
	default:
		;
	}
	*pResult = 0;
}
#endif