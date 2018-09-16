
// radiation_patternDlg.cpp : implementation file
//

#include "stdafx.h"
#include "radiation_pattern.h"
#include "radiation_patternDlg.h"
#include "afxdialogex.h"

#include "LayoutDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CRadiationPatternDlg dialog



CRadiationPatternDlg::CRadiationPatternDlg(CWnd* pParent /*=NULL*/)
    : CSimulationDialog(CRadiationPatternDlg::IDD, pParent)
    , m_data(model::make_model_data())
    , m_bFlat(FALSE)
    , m_bFlatInZDir(FALSE)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRadiationPatternDlg::DoDataExchange(CDataExchange* pDX)
{
    CSimulationDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_DIAGRAM, m_plot);
    DDX_Text(pDX, IDC_EDIT1, m_data.params->R);
    DDX_Text(pDX, IDC_EDIT2, m_data.params->lambda);
    DDX_Text(pDX, IDC_EDIT4, m_data.params->dx);
    DDX_Text(pDX, IDC_EDIT3, m_data.params->ncells);
    DDX_Check(pDX, IDC_CHECK1, m_bFlat);
    DDX_Check(pDX, IDC_CHECK2, m_bFlatInZDir);
}

BEGIN_MESSAGE_MAP(CRadiationPatternDlg, CSimulationDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON2, &CRadiationPatternDlg::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON1, &CRadiationPatternDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CRadiationPatternDlg message handlers

BOOL CRadiationPatternDlg::OnInitDialog()
{
    CSimulationDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    // TODO: Add extra initialization here

    omp_set_num_threads(omp_get_num_procs() / 2);

    model::update_board(m_data);

    m_data.board[3][3].second = true;
    m_data.board[7][7].second = true;
    m_data.board[3][7].second = true;
    m_data.board[7][3].second = true;

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CRadiationPatternDlg::OnPaint()
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
        CSimulationDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CRadiationPatternDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CRadiationPatternDlg::OnBnClickedButton2()
{
    UpdateData(TRUE);

    m_data.params->A = 15;

    model::update_data(m_data, m_bFlat ? (m_bFlatInZDir ? model::flat_z : model::flat) : model::diagram);

    m_plot.points = m_data.grid;
    m_plot.values = m_data.grid_data;

    double maxd = 0, maxz = 0;
    #pragma omp parallel for
    for (int i = 0; i < m_plot.values.size(); ++i)
    for (int j = 0; j < m_plot.values.size(); ++j)
    {
        maxd = std::fmax(maxd, math::sqnorm(
            math::v3 <> (m_plot.points[i][j].x, m_plot.points[i][j].y, m_plot.values[i][j])));
        maxz = std::fmax(maxz, m_plot.values[i][j]);
    }

    maxd = std::sqrt(maxd);
    
    #pragma omp parallel for
    for (int i = 0; i < m_plot.values.size(); ++i)
    for (int j = 0; j < m_plot.values.size(); ++j)
    {
        m_plot.points[i][j].x /= maxd;
        m_plot.points[i][j].y /= maxd;
        m_plot.values[i][j] /= m_bFlat ? maxz : maxd;
    }

    m_plot.RedrawWindow();
}


void CRadiationPatternDlg::OnBnClickedButton1()
{
    UpdateData(TRUE);

    model::update_board(m_data);

    std::vector < std::vector < bool > > grid;
    grid.resize(m_data.board.size());
    for (size_t i = 0; i < m_data.board.size(); ++i)
    {
        grid[i].resize(m_data.board.size());
        for (size_t j = 0; j < m_data.board.size(); ++j)
        {
            grid[i][j] = m_data.board[i][j].second;
        }
    }

    CLayoutDialog d(this, std::move(grid));
    if (d.DoModal() == IDOK)
    {
        grid = std::move(d.m_grid);
        for (size_t i = 0; i < m_data.board.size(); ++i)
        for (size_t j = 0; j < m_data.board.size(); ++j)
            m_data.board[i][j].second = grid[i][j];
    }
}
