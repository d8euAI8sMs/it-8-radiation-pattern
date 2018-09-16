// LayoutDialog.cpp : implementation file
//

#include "stdafx.h"
#include "radiation_pattern.h"
#include "LayoutDialog.h"
#include "afxdialogex.h"


// CLayoutDialog dialog

IMPLEMENT_DYNAMIC(CLayoutDialog, CDialogEx)

CLayoutDialog::CLayoutDialog(CWnd* pParent, grid_t grid)
	: CDialogEx(CLayoutDialog::IDD, pParent)
    , m_grid(std::move(grid))
{
}

CLayoutDialog::~CLayoutDialog()
{
}

void CLayoutDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLayoutDialog, CDialogEx)
    ON_BN_CLICKED(IDOK, &CLayoutDialog::OnBnClickedOk)
END_MESSAGE_MAP()


// CLayoutDialog message handlers

BOOL CLayoutDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    m_checks.resize(m_grid.size());
    for (size_t i = 0; i < m_grid.size(); ++i)
    {
        m_checks[i].resize(m_grid.size());
        for (size_t j = 0; j < m_grid.size(); ++j)
        {
            m_checks[i][j] = std::make_unique < CButton > ();
            m_checks[i][j]->Create(TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_CHECKBOX | DT_CENTER,
                                   CRect(i * 20, j * 20, (i + 1) * 20, (j + 1) * 20), this, 1000);
            m_checks[i][j]->SetCheck(m_grid[i][j] ? BST_CHECKED : BST_UNCHECKED);
        }
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CLayoutDialog::OnBnClickedOk()
{
    for (size_t i = 0; i < m_grid.size(); ++i)
    for (size_t j = 0; j < m_grid.size(); ++j)
        m_grid[i][j] = m_checks[i][j]->GetCheck() == BST_CHECKED;
    CDialogEx::OnOK();
}
