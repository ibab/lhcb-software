// Include files 

// local
#include "PrFitTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrFitTool
//
// 2006-12-08 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PrFitTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrFitTool::PrFitTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<PrFitTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
PrFitTool::~PrFitTool() {}

//=========================================================================
//  Fit a simple line in the specified projection
//=========================================================================
void PrFitTool::fitLine ( std::vector<Gaudi::XYZPoint>& hits, int mode,
                          double z0, double& a, double& b ) {
  PrFitPolinomial line(1);
  for ( std::vector<Gaudi::XYZPoint>::const_iterator itP = hits.begin();
        hits.end() > itP; itP++ ) {
    double dz = (*itP).z() - z0;
    double x =  (*itP).x();
    if ( 0 != mode ) x = (*itP).y();
    line.fill( x, dz );
  }
  line.solve();
  a = line.param( 0 );
  b = line.param( 1 );
}
//=========================================================================
//  Fit a parabola in the specified projection
//=========================================================================
void PrFitTool::fitParabola ( std::vector<Gaudi::XYZPoint>& hits, int mode,
                              double z0, double& a, double& b, double& c ) {

  PrFitPolinomial parab( 2 );

  for ( std::vector<Gaudi::XYZPoint>::const_iterator itP = hits.begin();
        hits.end() > itP; itP++ ) {
    double dz = ( (*itP).z() - z0 ) * 1.e-3;
    double X;
    if ( 0 == mode ) { X = (*itP).x(); } else { X = (*itP).y(); }
    parab.fill( X, dz );
  }
  parab.solve();
  a = parab.param(0);
  b = parab.param(1) * 1.e-3;
  c = parab.param(2) * 1.e-6;
}
//=========================================================================
//  Fit a Cubic in the specified projection
//=========================================================================
void PrFitTool::fitCubic ( std::vector<Gaudi::XYZPoint>& hits, int mode,
                           double z0, double& a, double& b, double& c, double& d ){

  PrFitPolinomial cubic( 3 );
  for ( std::vector<Gaudi::XYZPoint>::const_iterator itP = hits.begin();
        hits.end() > itP; itP++ ) {
    double dz = ( (*itP).z() - z0 ) * 1.e-3;
    double X;
    if ( 0 == mode ) { X = (*itP).x(); } else { X = (*itP).y(); }
    cubic.fill( X, dz );
  }
  cubic.solve();

  a = cubic.param(0);
  b = cubic.param(1) * 1.e-3;
  c = cubic.param(2) * 1.e-6;
  d = cubic.param(3) * 1.e-9;
}

//=============================================================================
