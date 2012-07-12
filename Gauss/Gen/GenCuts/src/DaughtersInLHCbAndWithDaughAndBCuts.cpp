// $Id: DaughtersInLHCbAndWithDaughAndBCuts.cpp,v 1.7 2008-07-09 14:33:47 robbep Exp $
// Include files 

// local
#include "DaughtersInLHCbAndWithDaughAndBCuts.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/SystemOfUnits.h"

// from Kernel
#include "Kernel/ParticleID.h"
#include "GaudiKernel/Vector4DTypes.h"

// from HepMC
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

// from Generators
#include "GenEvent/HepMCUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaughtersInLHCbAndWithDaughAndBCuts
//
// 2005-03-02 : Alex Shires
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( DaughtersInLHCbAndWithDaughAndBCuts );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaughtersInLHCbAndWithDaughAndBCuts::DaughtersInLHCbAndWithDaughAndBCuts( const std::string& type,
                                                                          const std::string& name,
                                                                          const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
  declareInterface< IGenCutTool >( this ) ;
  declareProperty( "ChargedThetaMin" , m_chargedThetaMin = 10 * Gaudi::Units::mrad ) ;
  declareProperty( "ChargedThetaMax" , m_chargedThetaMax = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMin" , m_neutralThetaMin = 5 * Gaudi::Units::mrad ) ;
  declareProperty( "NeutralThetaMax" , m_neutralThetaMax = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "LongLivedThetaMin" , m_llThetaMin = 0 * Gaudi::Units::mrad ) ;
  declareProperty( "LongLivedThetaMax" , m_llThetaMax = 400 * Gaudi::Units::mrad ) ;
  declareProperty( "MinMuonP" ,        m_minMuonP = 0 * Gaudi::Units::MeV ) ;
  declareProperty( "MinTrackP" ,       m_minTrackP = 0 * Gaudi::Units::MeV ) ;
  declareProperty( "MinLongLivedP" ,        m_minLongLivedP = 0 * Gaudi::Units::MeV ) ;
  declareProperty( "MinLongLivedDaughP" ,        m_minLongLivedDaughP = 0 * Gaudi::Units::MeV ) ;
  declareProperty( "MinSumP" ,        m_minSumP = 0* Gaudi::Units::MeV ) ;
  declareProperty( "MinBP" ,        m_minBP = 0 * Gaudi::Units::MeV ) ;
  declareProperty( "MinMuonPT" ,        m_minMuonPT = 0 * Gaudi::Units::MeV ) ;
  declareProperty( "MinTrackPT" ,       m_minTrackPT = 0 * Gaudi::Units::MeV ) ;
  declareProperty( "MinLongLivedPT" ,      m_minLongLivedPT = 0 * Gaudi::Units::MeV ) ;
  declareProperty( "MinLongLivedDaughPT" ,      m_minLongLivedDaughPT = 0 * Gaudi::Units::MeV ) ;
  declareProperty( "MinBPT" ,        m_minBPT = 0 * Gaudi::Units::MeV ) ;
  declareProperty( "MinSumPT" ,        m_minSumPT = 0* Gaudi::Units::MeV ) ;
  declareProperty( "MinBFD" ,        m_minBFD = 0 * Gaudi::Units::mm ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
DaughtersInLHCbAndWithDaughAndBCuts::~DaughtersInLHCbAndWithDaughAndBCuts( ) { ; }

//=============================================================================
// AndWithMinP function
//=============================================================================
bool DaughtersInLHCbAndWithDaughAndBCuts::applyCut( ParticleVector & theParticleVector ,
							const HepMC::GenEvent * /* theEvent */ ,
							const LHCb::GenCollision * /* theHardInfo */ )
  const {
  ParticleVector::iterator it ;
  
  for ( it = theParticleVector.begin() ; it != theParticleVector.end() ; ) {    
    if ( ! passCuts( *it ) ) {
      it = theParticleVector.erase( it ) ;
    } else ++it ;
  }

  
  return ( ! theParticleVector.empty() ) ;
}

//=============================================================================
// Functions to test if all daughters are in AndWithMinP
//=============================================================================
bool DaughtersInLHCbAndWithDaughAndBCuts::passCuts( const HepMC::GenParticle * theSignal ) const {
  HepMC::GenVertex * EV = theSignal -> end_vertex() ;
  if ( 0 == EV ) return true ;

double fakeSum(0.);
  //B signal cuts
  bool pass = flightCut(theSignal, m_minBFD );
  if (!pass) return false;
  pass = momentumCut(theSignal, m_minBP,fakeSum );
  if (!pass) return false;
  pass = transverseMomentumCut(theSignal, m_minBPT,fakeSum );
  if (!pass) return false;
  
  typedef std::vector< HepMC::GenParticle * > Particles ;
  Particles stables ;
  Particles unstables;
  HepMC::GenVertex::particle_iterator iter ;

  for ( iter = EV -> particles_begin( HepMC::descendants ) ; 
        iter != EV -> particles_end( HepMC::descendants ) ; ++iter ) {
    if ( 0 == (*iter) -> end_vertex() ) stables.push_back( *iter ) ;
    else unstables.push_back( *iter ) ;
  }  

  if ( stables.empty() )
    Exception( "Signal has no stable daughters !" ) ;
  
  double angle( 0. ) ;
  double sumP( 0. );
  double sumPt( 0. );
  double firstpz = stables.front() -> momentum().pz() ;
  
  debug() << "New event" << endmsg ;

  //KS and Lambda cuts
  for ( Particles::const_iterator it = unstables.begin() ; it != unstables.end() ;
        ++it ) {
    
    // look for KS or Lambda
    if ( ( 310 == abs( (*it) -> pdg_id() ) ) || (3122 == abs( (*it) -> pdg_id() ) ) ) continue;
    
    angle = (*it) -> momentum().theta() ;
    
    // cut on the KS/Lambda decay angle 
    if ( fabs( sin( angle ) ) > fabs( sin( m_llThetaMax ) ) )
      return false ;
    if ( fabs( sin( angle ) ) < fabs( sin( m_llThetaMin ) ) )
      return false ;
    
    // momentum cut
    pass = momentumCut((*it), m_minLongLivedP, sumP );
    if (!pass) return false;
      
    //transerve momentum cut
    pass = transverseMomentumCut((*it), m_minLongLivedPT, sumPt );
    if (!pass) return false;
    
  }
  
  for ( Particles::const_iterator it = stables.begin() ; it != stables.end() ;
        ++it ) {

    debug() << "Check particle " << (*it) -> pdg_id() << " with angle " 
            << (*it) -> momentum().theta() / Gaudi::Units::mrad 
            << " mrad." << endmsg ;
   
    // Remove neutrinos
    if ( ( 12 == abs( (*it) -> pdg_id() ) ) || 
         ( 14 == abs( (*it) -> pdg_id() ) ) || 
         ( 16 == abs( (*it) -> pdg_id() ) ) ) continue ;
 
    // Specific use of daughters of Lambda and KS:
    HepMC::GenParticle * theParent ;
    theParent = 
      *( (*it) -> production_vertex() -> particles_in_const_begin() ) ;
    if ( 3122 == abs( theParent -> pdg_id() ) || 310 == theParent -> pdg_id() ) {

    // momentum cut
    bool pass = momentumCut((*it), m_minLongLivedDaughP, fakeSum );
    if (!pass) return false;
  
    //transerve momentum cut
    pass = transverseMomentumCut((*it), m_minLongLivedDaughPT,fakeSum );
    if (!pass) return false;

    continue;
   }


    // Consider only gammas from pi0 and eta
    if ( 22 == (*it) -> pdg_id() ) {
      if ( ( 111 != theParent -> pdg_id() ) &&
           ( 221 != theParent -> pdg_id() ) ) continue ;
    }

    // All particles in same direction
    if ( 0 > ( firstpz * ( (*it) -> momentum().pz() ) ) ) return false ;

    // check angle
    angle = (*it) -> momentum().theta() ;
    LHCb::ParticleID pid( (*it) -> pdg_id() ) ;
    if ( 0 == pid.threeCharge() ) {
      if ( fabs( sin( angle ) ) > fabs( sin( m_neutralThetaMax ) ) ) 
        return false ;
      if ( fabs( sin( angle ) ) < fabs( sin( m_neutralThetaMin ) ) ) 
        return false ;
    } 
    else {
      if ( fabs( sin( angle ) ) > fabs( sin( m_chargedThetaMax ) ) ) 
        return false ;
      if ( fabs( sin( angle ) ) < fabs( sin( m_chargedThetaMin ) ) ) 
        return false ;
    }

    //check momentum
    bool pass = true;
    if ( 13 == abs( (*it) -> pdg_id() ) )  pass = momentumCut((*it), m_minMuonP, sumP );
    else   pass = momentumCut((*it), m_minTrackP, sumP );
    if (!pass) return false;

    //check transverse momentum
    if ( 13 == abs( (*it) -> pdg_id() ) )  pass = transverseMomentumCut((*it), m_minMuonPT, sumPt );
    else   pass = transverseMomentumCut((*it), m_minTrackPT, sumPt );
    if (!pass) return false;

  }

debug() << sumP << " vs " << m_minSumP << " " << sumPt << " " << m_minSumPT << endmsg;

  if (sumP < m_minSumP) return false;
  if (sumPt < m_minSumPT) return false;
  
  debug() << "Event passed !" << endmsg ;
  
  return true ;
}

bool DaughtersInLHCbAndWithDaughAndBCuts::transverseMomentumCut( const HepMC::GenParticle *p,
                                                                 const double pTmin,
                                                                 double& sumPt ) const
{
  bool pass(true);
  double px, py, pz, pt;

  pz = p->momentum().pz();

  if (pz < 0) pass = false;
  else {
    px = p->momentum().px();
    py = p->momentum().py();

    pt = sqrt( px*px + py*py );

    if ( pt < pTmin ) pass = false ;
    sumPt += pt;
  }

  return pass;
}


bool DaughtersInLHCbAndWithDaughAndBCuts::momentumCut( const HepMC::GenParticle *p, 
                                                       const double pmin,
                                                       double& sumP ) const 
{
  bool pass(true);  
  double px, py, pz, pp;
   
   pz = p->momentum().pz();
   
   if (pz < 0) pass = false;
   else {
       px = p->momentum().px();
       py = p->momentum().py();
   
       pp = sqrt( px*px + py*py + pz*pz );

       if (abs(p->pdg_id())==511)
         debug() << "momentum " << pp << " " << pmin << endmsg;

       if ( pp < pmin ) pass = false ;
       sumP += pp;
   }
      
   return pass;
}

bool DaughtersInLHCbAndWithDaughAndBCuts::flightCut( const HepMC::GenParticle *p,
								const double fdmin ) const
{
  bool pass(true);
  double px, py, pz;
  double dx, dy, dz;

  if (p->production_vertex()==0 || p->end_vertex()==0)
    return false;

  px = p->production_vertex()->point3d().x();
  py = p->production_vertex()->point3d().y();
  pz = p->production_vertex()->point3d().z();

  dx = p->end_vertex()->point3d().x();
  dy = p->end_vertex()->point3d().y();
  dz = p->end_vertex()->point3d().z();
  
  double FD = sqrt(pow(dx-px,2)+pow(dy-py,2)+pow(dz-pz,2));
  if (FD<fdmin)
    pass = false;

  return pass;
}
