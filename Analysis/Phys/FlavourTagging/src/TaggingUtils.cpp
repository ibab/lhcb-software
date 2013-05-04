// Include files 
#include <limits>
#include "TaggingUtils.h"
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IPVReFitter.h"

#include "TaggingHelpers.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggingUtils
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggingUtils )

//====================================================================
  TaggingUtils::TaggingUtils( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ) :
    GaudiTool ( type, name, parent ),
    m_Dist(0),
    m_dva(0)
{
  declareProperty( "ChoosePVCriterium", m_ChoosePV = "PVbyIPs");
  declareInterface<ITaggingUtils>(this);
}

TaggingUtils::~TaggingUtils() {}

//=====================================================================
StatusCode TaggingUtils::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm",
                             StatusCode::FAILURE);

  m_Dist = m_dva->distanceCalculator();
  if( !m_Dist ) {
    Error("Unable to retrieve the IDistanceCalculator tool");
    return StatusCode::FAILURE;
  }
  m_pvReFitter = tool<IPVReFitter>("AdaptivePVReFitter", this );
  if(! m_pvReFitter) {
    fatal() << "Unable to retrieve AdaptivePVReFitter" << endreq;
    return StatusCode::FAILURE;
  }

  return sc;
}

//==========================================================================
StatusCode TaggingUtils::calcDOCAmin( const Particle* axp,
                                      const Particle* p1,
                                      const Particle* p2,
                                      double& doca, double& docaerr) 
{
  double doca1(0), doca2(0), err1(0), err2(0);
  const StatusCode sc1 = m_Dist->distance (axp, p1, doca1, err1);
  const StatusCode sc2 = m_Dist->distance (axp, p2, doca2, err2);

  doca = std::min(doca1, doca2);
  if(doca == doca1) docaerr=err1; else docaerr=err2;

  return (sc1 && sc2);
}

//==========================================================================
StatusCode TaggingUtils::calcIP( const Particle* axp,
                                 const VertexBase* v,
                                 double& ip, double& iperr)
{
  ip   =-100.0;
  iperr= 0.0;
  int zsign = 0;
  double ipC=0, ipChi2=0;
  StatusCode sc2 = m_Dist->distance (axp, v, ipC, ipChi2);
  Gaudi::XYZVector ipV;
  StatusCode sc = m_Dist->distance (axp, v, ipV);
  if ( msgLevel(MSG::DEBUG) ) debug()<<"ipS: "<<ipC<<", ipV.R: "<<ipV.R()<<endreq;
  if(sc2 && ipChi2!=0) 
  {
    if (sc) zsign = ipV.z()>0? 1:-1;
    ip=ipC*zsign;          // IP with sign
    iperr=ipC/std::sqrt(ipChi2);
  }
  if ( msgLevel(MSG::DEBUG) ) 
    debug()<<"IP: "<<ipC<<", "<<ip<<", (sign = "<<zsign<<" )"<<endreq;
  return sc2;
}

//=========================================================================
StatusCode TaggingUtils::calcIP( const Particle* axp,
                                 const RecVertex::ConstVector& PileUpVtx,
                                 double& ip, double& ipe) {
  double ipmin = std::numeric_limits<double>::max();
  double ipminerr = 0.0;
  StatusCode sc, lastsc=1;

  for ( RecVertex::ConstVector::const_iterator iv = PileUpVtx.begin(); 
        iv != PileUpVtx.end(); ++iv )
  {
    double ipx=0, ipex=0;
    double ipC=0, ipChi2=0;
    sc = m_Dist->distance (axp, *iv, ipC, ipChi2);
    if(ipChi2) { ipx=ipC; ipex=ipC/sqrt(ipChi2); }

    if( sc ) {
      if( ipx < ipmin ) {
        ipmin = ipx;
        ipminerr = ipex;
      }
    } else lastsc = sc;
  }
  ip  = ipmin;
  ipe = ipminerr;

  return lastsc;
}
//=========================================================================
int TaggingUtils::countTracks( const Particle::ConstVector& vtags ) {

  int nr = 0;

  typedef Particle::ConstVector::const_iterator Iterator;
  using TaggingHelpers::SameTrackStatus;
  using TaggingHelpers::isSameTrack;
  for (Iterator ipart = vtags.begin(); vtags.end() != ipart; ++ipart )
  {
    bool duplic=false;
    for( Iterator jpart = vtags.begin(); ipart != jpart; ++jpart ) 
    {
      const SameTrackStatus isSame = isSameTrack(**ipart, **jpart);
      if (isSame) {
        duplic=true;
        break;
      }
    }
    if (!duplic) ++nr;
  }

  return nr;
}
//============================================================================
bool TaggingUtils::isinTree(const Particle* axp,
                            Particle::ConstVector& sons,
                            double& dist_phi)
{

  dist_phi = std::numeric_limits<double>::max();

  for ( Particle::ConstVector::iterator ip = sons.begin();
        ip != sons.end(); ++ip )
  {
    using TaggingHelpers::SameTrackStatus;
    using TaggingHelpers::isSameTrack;
    using TaggingHelpers::toString;
    using TaggingHelpers::dphi;

    const double deltaphi =
      fabs(dphi(axp->momentum().phi(), (*ip)->momentum().phi()));
    if (dist_phi > deltaphi) dist_phi = deltaphi;
    const SameTrackStatus isSame = isSameTrack(*axp, **ip);
    if ( isSame )
    {
      if (msgLevel(MSG::VERBOSE))
        verbose() << " particle is: " << toString(isSame)
                  << " isinTree part: " << axp->particleID().pid()
                  << " with p="<< axp->p()/Gaudi::Units::GeV
                  << " pt="<< axp->pt()/Gaudi::Units::GeV
                  << " proto_axp,ip="<<axp->proto()<<" "<<(*ip)->proto() << endreq;
      return true;
    }
  }
  return false;
}

//====================================================================
