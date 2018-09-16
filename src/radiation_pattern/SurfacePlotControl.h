#pragma once

#include <vector>

#include <util/common/geom/point.h>
#include <util/common/plot/shape.h>
#include <util/common/gui/OglControl.h>

// CSurfacePlotControl

class CSurfacePlotControl : public COglControl
{
    DECLARE_DYNAMIC(CSurfacePlotControl)

public:

    std::vector < std::vector < geom::point < double > > > points;
    std::vector < std::vector < double > > values;
    double color_factor;
    std::function < void () > custom_painter;

public:
    CSurfacePlotControl();
    virtual ~CSurfacePlotControl();
    virtual void OnDrawItemOGL();

protected:
    DECLARE_MESSAGE_MAP()
};
