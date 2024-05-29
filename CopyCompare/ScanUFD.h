// ScanUFD.h
//
// Copyright(c) Jaron Cheng.
// All right reserved
//
// Verbatim
// Determines whether a disk drive is a removable, fixed, CD-ROM, RAM disk, or network drive.
#ifndef SCAN_UFD_H
#define SCAN_UFD_H
#include "Switch.h"

#ifdef DEBUG_SCAN_UFD	// 131122CTJ08 CTJ13120311

// To list drive labels
CString driveLabelTbl[TOTAL_DRIVE_NUMBER] =	// CTJ14011611
{
	_T("A://"),		_T("B://"),		_T("C://"),		_T("D://"),	_T("E://"),		_T("F://"),		_T("G://"),
	_T("H://"),	_T("I://"),		_T("J://"),		_T("K://"),		_T("L://"),		_T("M://"),	_T("N://"),	
	_T("O://"),	_T("P://"),		_T("Q://"),	_T("R://"),		_T("S://"),		_T("T://"),		_T("U://"),	
	_T("V://"),		_T("W://"),	_T("X://"),		_T("Y://"),		_T("Z://"),
};

// To declare function
#ifdef DEBUG_SCAN_UFD
//bool ScanUFD(DriveInform *m_DriveInformTbl);	// CTJ13120314
//bool ScanUFD();								// CTJ13120314
#endif

#endif

	













#endif