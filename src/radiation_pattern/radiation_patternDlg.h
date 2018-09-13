
// radiation_patternDlg.h : header file
//

#pragma once

#include <util/common/gui/SimulationDialog.h>

// CRadiationPatternDlg dialog
class CRadiationPatternDlg : public CSimulationDialog
{
// Construction
public:
    CRadiationPatternDlg(CWnd* pParent = NULL);    // standard constructor

// Dialog Data
    enum { IDD = IDD_RADIATION_PATTERN_DIALOG };

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()
};
