// Include files
#include "GaudiKernel/IMagneticFieldSvc.h"

// Units
#include "GaudiKernel/SystemOfUnits.h"

// Mathlib
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"

// local
#include "BdlTool.h"
#include "LutForBdlFunction.h"

// Instantiation of a static factory class used by clients to create
// instances of this tool

DECLARE_TOOL_FACTORY( BdlTool )

// Standard Constructor
BdlTool::BdlTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent)
  : GaudiTool( type, name, parent ),
    m_BdlTrack(0.0), m_zHalfBdlTrack(0.0), m_magFieldSvc(0),
    m_lutBdl(0), m_lutZHalfBdl(0)
{
  declareInterface<IBdlTool>(this);

  declareProperty( "zCenterTT", m_zCenterTT = 247.*Gaudi::Units::cm );

}
//=========================================================================
//
//=========================================================================
StatusCode BdlTool::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;

  // retrieve pointer to magnetic field service
  m_magFieldSvc = svc<IMagneticFieldSvc>( "MagneticFieldSvc", true );

  info() << "Start generation of L1 LUT tables" << endmsg;
  // prepare table with Bdl integrations
  int nVarLut = 3;
  // Bdl integral depends on 3 track parameters
  //  slopeY     - y slope of the track
  //  zOrigin    - z of track intersection with z axis (in YZ projection)
  //  zVeloEnd   - z of the track at which slopeY is given
  //
  //                     slopeY zOrigin                 zVeloEnd
  int nBinVar[3]      = {   30,   10,                    10    };
  double  leftVar[3]  = { -0.3, -250.*Gaudi::Units::mm,   0.*Gaudi::Units::mm};
  double  rightVar[3] = {  0.3,  250.*Gaudi::Units::mm, 800.*Gaudi::Units::mm};
  m_lutBdl      = new LutForBdlFunction(nVarLut, nBinVar, leftVar, rightVar);
  m_lutZHalfBdl = new LutForBdlFunction(nVarLut, nBinVar, leftVar, rightVar);

  double lutVar[3];
  m_lutBdl->resetIndexVector();
  m_lutZHalfBdl->resetIndexVector();
  int iover = 0;
  while(!iover) {
    m_lutBdl->getVariableVector(lutVar);
    double slopeY   = lutVar[0];
    double zOrigin  = lutVar[1];
    double zEndVelo = lutVar[2];
    f_bdl(slopeY, zOrigin, zEndVelo, m_zCenterTT);
    m_lutBdl->fillTable(m_BdlTrack);
    m_lutZHalfBdl->fillTable(m_zHalfBdlTrack);
    iover = m_lutBdl->incrementIndexVector();
    iover = m_lutZHalfBdl->incrementIndexVector();
  }

  info() << "Generation of L1 LUT tables finished" << endmsg;

  return StatusCode::SUCCESS;
}
//=========================================================================
//  Destructor
//=========================================================================
BdlTool::~BdlTool() {}

//=========================================================================
//  Finalization
//=========================================================================
StatusCode BdlTool::finalize() {
  delete m_lutBdl;
  delete m_lutZHalfBdl;
  return GaudiTool::finalize();
}
//****************************************************************************
double BdlTool::bdlIntegral(double ySlopeVelo, double zOrigin, double zVelo) {

    double var[3];
    var[0] = ySlopeVelo;
    var[1] = zOrigin;
    var[2] = zVelo;
    return  m_lutBdl->getInterpolatedValueFromTable(var);
}
//****************************************************************************
double BdlTool::zBdlMiddle(double ySlopeVelo, double zOrigin, double zVelo) {

    double var[3];
    var[0] = ySlopeVelo;
    var[1] = zOrigin;
    var[2] = zVelo;
    return m_lutZHalfBdl->getInterpolatedValueFromTable(var);
}


//****************************************************************************
void BdlTool::f_bdl( double slopeY, double zOrigin,
                     double zStart, double zStop){

    m_BdlTrack=0.0;
    m_zHalfBdlTrack=0.0;

    if(zStart>zStop) return;

    double Bdl=0.0;
    double zHalfBdl=0.0;

   // vectors to calculate z of half Bdl
    m_bdlTmp.clear();
    m_zTmp.clear();

    // prepare m_zBdlHalf;
    Gaudi::XYZPoint  aPoint(0.,0.,0.);
    Gaudi::XYZVector bField;

    int np    = 500;
    double dz = (zStop - zStart)/np;
    double dy = dz*slopeY;

    aPoint.SetX( 0.0 );

    double z = zStart+dz/2.;
    double y = slopeY*(zStart-zOrigin);

    double bdl = 0.;

    while( z<zStop ) {

      aPoint.SetY( y );
      aPoint.SetZ( z );

      m_magFieldSvc->fieldVector( aPoint, bField );
      bdl += dy*bField.z() - dz*bField.y();
      if(z>100.*Gaudi::Units::cm) {
        m_bdlTmp.push_back(bdl);
        m_zTmp.push_back(z);
      }
      z += dz;
      y += dy;
    }

    Bdl=bdl;
    double bdlhalf = fabs(Bdl)/2.;

    for(unsigned int i=5; i<m_bdlTmp.size()-5; i++) {
      if(fabs(m_bdlTmp[i])>bdlhalf) {
        double zrat = (Bdl/2.-m_bdlTmp[i-1])/(m_bdlTmp[i]-m_bdlTmp[i-1]);
        zHalfBdl = m_zTmp[i-1]+dz*zrat;
        break;
      }
    }

    m_BdlTrack = Bdl;
    m_zHalfBdlTrack = zHalfBdl;

 }

