
// radiation_pattern.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"        // main symbols


// CRadiationPatternApp:
// See radiation_pattern.cpp for the implementation of this class
//

class CRadiationPatternApp : public CWinApp
{
public:
    CRadiationPatternApp();

// Overrides
public:
    virtual BOOL InitInstance();

// Implementation

    DECLARE_MESSAGE_MAP()
};

extern CRadiationPatternApp theApp;