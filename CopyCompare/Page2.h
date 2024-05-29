#pragma once

// Page2 dialog

class Page2 : public CDialog
{
	DECLARE_DYNAMIC(Page2)

public:
	Page2(CWnd* pParent = NULL);   // standard constructor
	virtual ~Page2();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()
};
