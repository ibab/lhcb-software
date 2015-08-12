// Include files 

// local
#include "FitTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FitTool
//
// 2006-12-08 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FitTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FitTool::FitTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_fit2(2), m_fit3(3), m_fit4(4)
{
  declareInterface<FitTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
FitTool::~FitTool() {} 

//=========================================================================
//  Fit a simple line in the specified projection
//=========================================================================
bool FitTool::fitLine ( const std::vector<Gaudi::XYZPoint>& hits, int mode,
                        double z0, double& a, double& b )
{
  m_fit2.clear();
  for (const auto& p: hits)
      m_fit2.accumulate((mode ? p.y() : p.x()), 1., p.z() - z0);
  if (!m_fit2.solve()) return false;
  a = m_fit2[0], b = m_fit2[1];
  return true;
}
//=========================================================================
//  Fit a parabola in the specified projection
//=========================================================================
bool FitTool::fitParabola ( const std::vector<Gaudi::XYZPoint>& hits, int mode,
                            double z0, double& a, double& b, double& c )
{
  m_fit3.clear();
  for (const auto& p: hits)
      m_fit3.accumulate((mode ? p.y() : p.x()), 1., 1e-3 * (p.z() - z0));
  if (!m_fit3.solve()) return false;
  a = m_fit3[0], b = 1e-3 * m_fit3[1], c = 1e-6 * m_fit3[2];
  return true;
}
//=========================================================================
//  Fit a Cubic in the specified projection
//=========================================================================
bool FitTool::fitCubic ( const std::vector<Gaudi::XYZPoint>& hits, int mode,
                         double z0, double& a, double& b, double& c, double& d )
{
  m_fit4.clear();
  for (const auto& p: hits)
      m_fit4.accumulate((mode ? p.y() : p.x()), 1., 1e-3 * (p.z() - z0));
  if (!m_fit4.solve()) return false;
  a = m_fit4[0], b = 1e-3 * m_fit4[1], c = 1e-6 * m_fit4[2], d = 1e-9 * m_fit4[3];
  return true;
}

//=============================================================================
