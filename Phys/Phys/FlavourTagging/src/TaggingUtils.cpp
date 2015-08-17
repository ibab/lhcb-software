// Include files 
#include <limits>
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/IParticleDescendants.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/IVertexFit.h"


#include "TaggingHelpers.h"
#include "TaggingUtils.h"

#include "LoKi/ParticleProperties.h" // NB: added by VB  2015-01-21

//--------------------------------------------------------------------
// Implementation file for class : TaggingUtils
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb;
using namespace Gaudi::Units;

#include "LoKi/ParticleCuts.h" 
#include "LoKi/ParticleContextCuts.h" 

using namespace LoKi::Cuts;
using namespace LoKi::Types;
using namespace LoKi::Particles;

using CharmTaggerSpace::CharmMode;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggingUtils )

//====================================================================
  TaggingUtils::TaggingUtils( const std::string& type,
                              const std::string& name,
                              const IInterface* parent ) :
    GaudiTool ( type, name, parent ),
    m_dva(nullptr),
    m_personality("Reco14"),
    m_PVSelCriterion("PVbyIPs"),
    m_algNamePVReFitter("LoKi::PVReFitter:PUBLIC"),
    m_algNameLifetimeFitter("LoKi::LifetimeFitter:PUBLIC"),
    m_algNameVertexFitter("LoKi::VertexFitter:PUBLIC"),
    m_algNameDistanceCalculator("LoKi::DistanceCalculator:PUBLIC"),
    m_algNameParticleDescendants("ParticleDescendants"),
    m_PVReFitter(nullptr),
    m_LifetimeFitter(nullptr),
    m_VertexFitter(nullptr),
    m_DistanceCalculator(nullptr),
    m_ParticleDescendants(nullptr)
{
  declareInterface<ITaggingUtils>(this);
 
  declareProperty( "Personality"    , m_personality    = "Reco14" );  
  declareProperty( "PVSelCriterion" , m_PVSelCriterion = "PVbyIPs");

  declareProperty( "PVReFitter"         , m_algNamePVReFitter          =  m_algNamePVReFitter        );
  declareProperty( "LifetimeFitter"     , m_algNameLifetimeFitter      =  m_algNameLifetimeFitter    );
  declareProperty( "VertexFitter"       , m_algNameVertexFitter        =  m_algNameVertexFitter      );
  declareProperty( "DistanceCalculator" , m_algNameDistanceCalculator  =  m_algNameDistanceCalculator);
  declareProperty( "ParticleDescendants", m_algNameParticleDescendants = m_algNameParticleDescendants);

 }

TaggingUtils::~TaggingUtils() {}

//=====================================================================
StatusCode TaggingUtils::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this );
  if ( m_dva == nullptr ) {
    fatal() << "Coudn't get parent DVAlgorithm" << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_PVReFitter = tool<IPVReFitter>( m_algNamePVReFitter, this );  
  if ( m_PVReFitter == nullptr ) {
    fatal() << "Unable to retrieve IPVRefitter \'" << m_algNamePVReFitter << "\'" << endmsg;
    return StatusCode::FAILURE;
  }

  m_LifetimeFitter = tool<ILifetimeFitter>( m_algNameLifetimeFitter, this );
  if ( m_LifetimeFitter == nullptr ) {
    fatal() << "Unable to retrieve ILifetimFitter \'" << m_algNameLifetimeFitter << "\'" << endmsg;
    return StatusCode::FAILURE;
  }

  m_VertexFitter = tool<IVertexFit>( m_algNameVertexFitter, this );
  if ( m_VertexFitter == nullptr ) {
    fatal() << "Unable to retrieve IVertexFit \'" << m_algNameVertexFitter << "\'" << endmsg;
    return StatusCode::FAILURE;
  }

  m_DistanceCalculator = tool<IDistanceCalculator>( m_algNameDistanceCalculator, this );//m_dva->distanceCalculator();
  if( m_DistanceCalculator == nullptr ) {
    fatal() << "Unable to retrieve the IDistanceCalculator tool " 
            << m_algNameDistanceCalculator << endmsg;
    return StatusCode::FAILURE;
  }
  
  m_ParticleDescendants = tool<IParticleDescendants> ( m_algNameParticleDescendants, this );
  if( ! m_ParticleDescendants ) {
    fatal() << "Unable to retrieve ParticleDescendants tool " 
            << m_algNameParticleDescendants
            << endmsg;
    return StatusCode::FAILURE;
  }

  // register multiple personalities
  m_countTracks.registerPersonality("Reco12",
	  this, &TaggingUtils::countTracksReco12);
  m_countTracks.registerPersonality("Reco14",
	  this, &TaggingUtils::countTracksReco14);
  m_isinTree.registerPersonality("Reco12",
	  this, &TaggingUtils::isinTreeReco12);
  m_isinTree.registerPersonality("Reco14",
	  this, &TaggingUtils::isinTreeReco14);

  // select personality
  // (this throws an exception if the personality chosen by the user is not
  // known)
  try {
      m_countTracks.setPersonality(m_personality);
      m_isinTree.setPersonality(m_personality);
  } catch (const std::exception& e) {
      error() << "Caught exception while setting personality: " << e.what() <<
	  endmsg;
      return StatusCode::FAILURE;
  };

  // Particle IDs
  lambda_pid = LoKi::Particles::pidFromName("Lambda0").abspid();
  pi_pid = LoKi::Particles::pidFromName("pi+").abspid();
  pi0_pid = LoKi::Particles::pidFromName("pi0").abspid();
  k_pid = LoKi::Particles::pidFromName("K+").abspid();
  ks_pid = LoKi::Particles::pidFromName("KS0").abspid();
  p_pid = LoKi::Particles::pidFromName("p+").abspid();
  e_pid = LoKi::Particles::pidFromName("e+").abspid();
  mu_pid = LoKi::Particles::pidFromName("mu+").abspid();
  d0_pid = LoKi::Particles::pidFromName("D0").abspid();
  d_pid = LoKi::Particles::pidFromName("D+").abspid();
  lambdaC_pid = LoKi::Particles::pidFromName("Lambda_c+").abspid();


  return sc;
}
//==========================================================================
StatusCode TaggingUtils::calcDOCAmin( const Particle* axp,
                                      const Particle* p1,
                                      const Particle* p2,
                                      double& doca, double& docaerr) 
{
  double doca1(0), doca2(0), err1(0), err2(0);
  const StatusCode sc1 = m_DistanceCalculator->distance (axp, p1, doca1, err1);
  const StatusCode sc2 = m_DistanceCalculator->distance (axp, p2, doca2, err2);

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
  StatusCode sc2 = m_DistanceCalculator->distance (axp, v, ipC, ipChi2);
  Gaudi::XYZVector ipV;
  StatusCode sc = m_DistanceCalculator->distance (axp, v, ipV);
  if ( msgLevel(MSG::DEBUG) ) debug()<<"ipS: "<<ipC<<", ipV.R: "<<ipV.R()<<endmsg;
  if(sc2 && ipChi2!=0) 
  {
    if (sc) zsign = ipV.z()>0? 1:-1;
    ip=ipC*zsign;          // IP with sign
    iperr=ipC/std::sqrt(ipChi2);
  }
  if ( msgLevel(MSG::DEBUG) ) 
    debug()<<"IP: "<<ipC<<", "<<ip<<", (sign = "<<zsign<<" )"<<endmsg;
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
    sc = m_DistanceCalculator->distance (axp, *iv, ipC, ipChi2);
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
int TaggingUtils::countTracks( const LHCb::Particle::ConstVector& vtags )
{ return m_countTracks(vtags); }
//=========================================================================
int TaggingUtils::countTracksReco12( const LHCb::Particle::ConstVector& vtags ) {

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
//=========================================================================
int TaggingUtils::countTracksReco14( const LHCb::Particle::ConstVector& vtags ) {

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
bool TaggingUtils::isinTree(const LHCb::Particle* axp,
                            const LHCb::Particle::ConstVector& sons,
                            double& dist_phi)
{
    return m_isinTree(axp, sons, *(&dist_phi));
}
//============================================================================
bool TaggingUtils::isinTreeReco12(const LHCb::Particle* axp, 
			    const LHCb::Particle::ConstVector& sons, 
                            double& dist_phi){
  double p_axp  = axp->p();
  double pt_axp = axp->pt();
  double phi_axp= axp->momentum().phi();
  dist_phi=1000.;

  for( Particle::ConstVector::const_iterator ip = sons.begin(); 
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
                  << " proto_axp,ip="<<axp->proto()<<" "<<(*ip)->proto()<<endmsg;
      return true;
    }
  }
  return false;
}
//============================================================================
bool TaggingUtils::isinTreeReco14(const LHCb::Particle* axp,
                            const LHCb::Particle::ConstVector& sons,
                            double& dist_phi)
{

  dist_phi = std::numeric_limits<double>::max();

  for ( Particle::ConstVector::const_iterator ip = sons.begin();
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
                  << " proto_axp,ip="<<axp->proto()<<" "<<(*ip)->proto() << endmsg;
      return true;
    }
  }
  return false;
}

//=============================================================================
LHCb::Particle::ConstVector TaggingUtils::purgeCands(const LHCb::Particle::Range& cands, 
                                                     const LHCb::Particle& BS) {
  // remove any charm cand that has descendents in common with the signal B
  LHCb::Particle::ConstVector purgedCands;

  Particle::ConstVector signalDaus = m_ParticleDescendants->descendants(&BS);

  Particle::ConstVector::const_iterator icand;
  for( icand = cands.begin(); icand != cands.end(); ++icand ) {  

    const Particle* cand = *icand;
    Particle::ConstVector candDaus = m_ParticleDescendants->descendants(cand);
    bool isUsedForSignal = false;

    Particle::ConstVector::const_iterator icandDau;
    for( icandDau = candDaus.begin(); icandDau != candDaus.end(); ++icandDau ) {  

      const LHCb::ProtoParticle* proto_candDau = (*icandDau)->proto();

      Particle::ConstVector::const_iterator isignalDau;
      for( isignalDau = signalDaus.begin(); isignalDau != signalDaus.end(); ++isignalDau ) {
        if (proto_candDau == (*isignalDau)->proto()) isUsedForSignal = true;
        if (isUsedForSignal) break;
      }

      if (isUsedForSignal) break;
    }
    
    if (!isUsedForSignal) purgedCands.push_back(cand);

  }
    
  return purgedCands;

}

//=============================================================================
CharmMode TaggingUtils::getCharmDecayMode(const LHCb::Particle* cand, int candType)
{

  CharmMode mode = CharmMode::None;

  const SmartRefVector<Particle>& daus = cand->daughters();
  int numDaus = daus.size();
  
  switch (candType) {
    
  case 0: // full reco, exclusive
    
    if (cand->particleID().abspid() == d0_pid) {

      switch (numDaus) {
      
      case 2:
        if (daus[0]->particleID().abspid() == k_pid and
            daus[1]->particleID().abspid() == pi_pid)
          mode = CharmMode::Dz2kpi;
        break;
      
      case 3:
        // if (daus[0]->particleID().abspid() == ks_pid and
        //     daus[1]->particleID().abspid() == pi_pid and
        //     daus[2]->particleID().abspid() == pi_pid)
        //   mode = "D0_Kspipi";
        if (daus[0]->particleID().abspid() == k_pid and
            daus[1]->particleID().abspid() == pi_pid and 
            daus[2]->particleID().abspid() == pi0_pid)
          mode = CharmMode::Dz2kpipiz;
        break;
      
      case 4:
        if (daus[0]->particleID().abspid() == k_pid and
            daus[1]->particleID().abspid() == pi_pid and
            daus[2]->particleID().abspid() == pi_pid and
            daus[3]->particleID().abspid() == pi_pid)
          mode = CharmMode::Dz2kpipipi;
        break;

      default:
        fatal() << "Invalid daus size: " << numDaus << " for candtype: " << candType << endmsg;

      }
      
    } else if (cand->particleID().abspid() == d_pid) {

      switch (numDaus) {
      
      // case 2:
      //   if (daus[0]->particleID().abspid() == ks_pid and
      //       daus[1]->particleID().abspid() == pi_pid)
      //     mode = "Dp_Kspi";
      //   break;
      
      case 3:
        if (daus[0]->particleID().abspid() == k_pid and
            daus[1]->particleID().abspid() == pi_pid and
            daus[2]->particleID().abspid() == pi_pid)
          mode = CharmMode::Dp2kpipi;
        break;
      
      default:
        fatal() << "Invalid daus size: " << numDaus << " for candtype: " << candType << endmsg;

      }

    } else {
      fatal() << "Invalid charm type: " << cand->particleID().abspid() << " for candtype: " << candType << endmsg;
    }
    
    break;

  case 1: // part reco, inclusive

    if (cand->particleID().abspid() == d0_pid) {
      
      switch (numDaus) {
        
      case 2:
        if (daus[0]->particleID().abspid() == k_pid and
            daus[1]->particleID().abspid() == pi_pid)
          mode = CharmMode::Dz2kpiX;
        if (daus[0]->particleID().abspid() == k_pid and
            daus[1]->particleID().abspid() == e_pid)
          mode = CharmMode::Dz2keX;
        if (daus[0]->particleID().abspid() == k_pid and
            daus[1]->particleID().abspid() == mu_pid)
          mode = CharmMode::Dz2kmuX;
        break;
        
      default:
        fatal() << "Invalid daus size: " << numDaus << " for candtype: " << candType << endmsg;

      }
      
    } else if (cand->particleID().abspid() == d_pid) {

      switch (numDaus) {
      
      // case 2:
      //   if (daus[0]->particleID().abspid() == k_pid and
      //       daus[1]->particleID().abspid() == pi_pid)
      //     mode = "Dp_KpiX";
      //   if (daus[0]->particleID().abspid() == k_pid and
      //       daus[1]->particleID().abspid() == e_pid)
      //     mode = "Dp_KeX";
      //   if (daus[0]->particleID().abspid() == k_pid and
      //       daus[1]->particleID().abspid() == mu_pid)
      //     mode = "Dp_KmuX";
      //   break;
      
      default:
        fatal() << "Invalid daus size: " << numDaus << " for candtype: " << candType << endmsg;
        
      }

    } else {
      fatal() << "Invalid charm type: " << cand->particleID().abspid() << " for candtype: " << candType << endmsg;
    }

    break;
    
  case 2: // dstar reco

    switch (numDaus) {
      
    // case 3:
    //   if (daus[0]->particleID().abspid() == ks_pid and
    //       daus[1]->particleID().abspid() == pi_pid and
    //       daus[2]->particleID().abspid() == pi_pid)
    //     mode = "Dstar_D0_Kspipi";
    //   break;

    default:
      fatal() << "Invalid daus size: " << numDaus << " candtype: " << candType << endmsg;

    }
    break;

  case 3: // lambda reco

    if (cand->particleID().abspid() == lambdaC_pid) {

      switch (numDaus) {
      
      // case 2:
      //   if (daus[0]->particleID().abspid() == p_pid and
      //       daus[1]->particleID().abspid() == ks_pid)
      //     mode = "LambdaC_pKs";
      //   if (daus[0]->particleID().abspid() == lambda_pid and
      //       daus[1]->particleID().abspid() == pi_pid)
      //     mode = "LambdaC_LambdaPi";
      //   break;
      
      case 3:
        if (daus[0]->particleID().abspid() == p_pid and
            daus[1]->particleID().abspid() == k_pid and
            daus[2]->particleID().abspid() == pi_pid)
          mode = CharmMode::LambdaC2pkpi;
        break;
        
      default:
        fatal() << "Invalid daus size: " << numDaus << " for candtype: " << candType << endmsg;
        
      }

    } else {
      fatal() << "Invalid charm type: " << cand->particleID().abspid() << " for candtype: " << candType << endmsg;
    }
    break;
      
  default:
    fatal() << "Invalid candtype: " << candType << endmsg;

  }
  
  return mode;
  
}


//====================================================================
StatusCode TaggingUtils::finalize() { return GaudiTool::finalize(); }

