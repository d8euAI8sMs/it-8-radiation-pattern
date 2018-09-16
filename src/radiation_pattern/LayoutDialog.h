#pragma once

#include <vector>
#include <memory>

// CLayoutDialog dialog

class CLayoutDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CLayoutDialog)

public:

    using grid_t = std::vector < std::vector < bool > > ;

	CLayoutDialog(CWnd* pParent, grid_t grid);   // standard constructor
	virtual ~CLayoutDialog();

    grid_t m_grid;

// Dialog Data
	enum { IDD = IDD_LAYOUTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    std::vector < std::vector < std::unique_ptr < CButton > > > m_checks;

	DECLARE_MESSAGE_MAP()
public:
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedOk();
};
