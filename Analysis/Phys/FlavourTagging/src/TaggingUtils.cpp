// Include files 
#include "TaggingUtils.h"
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IPVReFitter.h"

#include "dphi.h"

//--------------------------------------------------------------------
// Implementation file for class : TaggingUtils
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggingUtils );

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
TaggingUtils::~TaggingUtils() {}; 

//=====================================================================
StatusCode TaggingUtils::initialize() { 
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

  return StatusCode::SUCCESS; 
}

//==========================================================================                                                                                 
StatusCode TaggingUtils::calcDOCAmin( const Particle* axp,
				      const Particle* p1,
				      const Particle* p2,
				      double& doca, double& docaerr) {
  double doca1, doca2, err1, err2;
  StatusCode sc1 = m_Dist->distance (axp, p1, doca1, err1);
  StatusCode sc2 = m_Dist->distance (axp, p2, doca2, err2);

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
  debug()<<"ipS: "<<ipC<<", ipV.R: "<<ipV.R()<<endreq;
  if(sc2 && ipChi2!=0) {
    if (sc) zsign = ipV.z()>0? 1:-1;
    ip=ipC*zsign;          // IP with sign
    iperr=ipC/sqrt(ipChi2);
  }
  debug()<<"IP: "<<ipC<<", "<<ip<<", (sign = "<<zsign<<" )"<<endreq;
  return sc2;
}

//=========================================================================
StatusCode TaggingUtils::calcIP( const Particle* axp,
                                 const RecVertex::ConstVector& PileUpVtx,
                                 double& ip, double& ipe) {
  double ipmin = 100000.0;
  double ipminerr = 0.0;
  StatusCode sc, lastsc=1;

  RecVertex::ConstVector::const_iterator iv;
  for(iv = PileUpVtx.begin(); iv != PileUpVtx.end(); iv++){
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
int TaggingUtils::countTracks( Particle::ConstVector& vtags ) {

  int nr = 0;

  //nr = vtags.size();

  Particle::ConstVector::const_iterator ipart, jpart;
  for( ipart = vtags.begin(); ipart != vtags.end(); ipart++ ) {
    bool duplic=false;
    for( jpart = ipart+1; jpart != vtags.end(); jpart++ ) {
      if((*jpart)->proto()==(*ipart)->proto()) { 
        duplic=true; 
        break; 
      }
    }
    if(!duplic) ++nr;
  }

  return nr;
}
//============================================================================
bool TaggingUtils::isinTree(const Particle* axp, 
			    Particle::ConstVector& sons, 
                            double& dist_phi){
  double p_axp  = axp->p();
  double pt_axp = axp->pt();
  double phi_axp= axp->momentum().phi();
  dist_phi=1000.;

  for( Particle::ConstVector::iterator ip = sons.begin(); 
       ip != sons.end(); ip++ ){

    double dphi = fabs(TaggingHelpers::dphi(phi_axp,(*ip)->momentum().phi()));
    dist_phi= std::min(dist_phi, dphi);
    
    if( (    fabs(p_axp -(*ip)->p()) < 0.1 
             && fabs(pt_axp-(*ip)->pt())< 0.01 
             && dphi < 0.1 )
        || axp->proto()==(*ip)->proto() ) {
      if (msgLevel(MSG::VERBOSE)) 
        verbose() << " isinTree part: " << axp->particleID().pid() 
                  << " with p="<<p_axp/Gaudi::Units::GeV 
                  << " pt="<<pt_axp/Gaudi::Units::GeV 
                  << " proto_axp,ip="<<axp->proto()<<" "<<(*ip)->proto()<<endreq;
      return true;
    }
  }
  return false;
}

//====================================================================
StatusCode TaggingUtils::finalize() { return GaudiTool::finalize(); }

