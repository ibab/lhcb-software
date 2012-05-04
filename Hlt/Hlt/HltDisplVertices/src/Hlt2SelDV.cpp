// Include files 

// from Gaudi
//#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h"

//Use ODIN
#include "Event/ODIN.h"

//get the Header of the event
#include "Event/RecHeader.h"

// local
#include "Hlt2SelDV.h"
#include "LoKi/BasicFunctors.h"
#include "LoKi/OdinCuts.h"

//calorimeter
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"


using namespace Gaudi::Units ;
using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2SelDV
//
// 2010-01-22 : Neal Gauvin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Hlt2SelDV );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2SelDV::Hlt2SelDV( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_PreyID(0)
{
  declareProperty("Prey", m_Prey = "~chi_10" );
  declareProperty("MinNBCands", m_NbCands = 1 );
  declareProperty("MinNBCandsExclusive", m_NbCandsExclusive = false );
  declareProperty("PreyMinMass", m_PreyMinMass = 6.3*GeV );
  //Unlimited
  declareProperty("PreyMaxMass", m_PreyMaxMass = 14.*TeV );
  declareProperty("PreyMaxSumpt", m_PreyMaxSumPt = 14.*TeV );
  declareProperty("PreyMinSumpt", m_SumPt = 0.*GeV );
  declareProperty("PreyMinAngle", m_PreyMinAngle = 0. );
  declareProperty("PreyMinFD", m_PreyMinFD = 0. );
  declareProperty("PreyMinHighestMass", m_PreyMinHighMass = 0*GeV );
  declareProperty("AllOutDet", m_allOutDet = true );

  declareProperty("MinRecpt", m_MinRecpt = 0*GeV );
  declareProperty("RMin", m_RMin = 0.3*mm );//0.06 in K
  declareProperty("RMax", m_RMax = 10.*m );
  declareProperty("MaxChi2OvNDoF", m_MaxChi2OvNDoF = 1000. );
  declareProperty("NbTracks", m_nTracks = 1 );//~ nb B meson max # of tracks 5
  declareProperty("RemVtxFromDet", m_RemVtxFromDet = 0  );
  declareProperty("SigmaZ", m_SigmaZ = 1000. );
  declareProperty("SigmaR", m_SigmaR = 1000. );
  declareProperty("MinZ", m_MinZ = -10.*m );
  declareProperty("MaxZ", m_MaxZ = 100*m );
  declareProperty("FractionEFrom1Track",   m_fracE = 0.85 );
  declareProperty("FractionTrackUpstreamV",   m_fracT = 0.49 );
  declareProperty("PVnbtrks", m_PVnbtrks = 5 ); //corr. to 'tight' PV reco
  declareProperty("UseVelo", m_useVelo = true );
  //declareProperty("PhaseSpacePS", m_phaseSpacePS = false ); //phase space prescaler


}
//=============================================================================
// Destructor
//=============================================================================
Hlt2SelDV::~Hlt2SelDV() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt2SelDV::initialize() {
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;
  const ParticleProperty* Prey = ppSvc()->find( m_Prey );
  if ( !Prey ) { //
    err() << "Cannot find particle property for " << m_Prey << endmsg ;
    return StatusCode::FAILURE;
  }
  m_PreyID = Prey->particleID();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Hlt2SelDV::execute() {
  setFilterPassed(false);
  //------------------The Code---------------------------  
  Particle::Range preys = particles();
  if( preys.size() < m_NbCands ){
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Insufficent number of particles in TES !" << endmsg;
    return StatusCode::SUCCESS;
  }

  vector<int>  nboftracks;
  vector<double> chindof, px, py, pz, e, x, y, z, errx, erry, errz, sumpts,  indets,masses,minvd,rs,massCorrs,massCorrsIP,thetas;
  const RecVertex::Range PVs = this->primaryVertices();
  //double tmp = 1000;
  std::vector<const RecVertex*> SelectedPVs; 
  for ( RecVertex::Range::const_iterator i = PVs.begin(); 
        i != PVs.end() ; ++i ){
    const RecVertex* pv = *i;
    //Apply some cuts
    if( abs(pv->position().x()>1.5*mm) || abs(pv->position().y()>1.5*mm))
      continue;
    double z = pv->position().z();
    if( abs(z) > 150*mm ) continue;
    if( !HasBackAndForwardTracks( pv ) ) continue;
    SelectedPVs.push_back(pv);
  }
 
  if( msgLevel( MSG::DEBUG ) )
    debug()<<"--------Reconstructed Displ. Vertices --------------"<< endmsg;
  //Particle::ConstVector Cands;
  int nbCands(0);
  double highestMass(0.);
  bool oneOutMat = false;

  Particle::Range::const_iterator iend = preys.end();
  for( Particle::Range::const_iterator is = preys.begin(); 
       is < iend; ++is ){
    const Particle * p = (*is);
    
    //Get rid of non-reconstructed particles, i.e. with no daughters.
    if( p->isBasicParticle() ){ 
      debug()<<"Basic particle !" << endmsg; 
      continue;
    }
    double mass = p->measuredMass();
    int nbtrks = p->endVertex()->outgoingParticles().size();
    double chi = p->endVertex()->chi2PerDoF();
    double fracT = p->info(55,-1000.) ; 
    double fracE = p->info(56,-1000.) ; 
    const Gaudi::XYZPoint & pos = p->endVertex()->position();
    double rho = p->info(52,-1000.);
    bool indet = false;
    if (p->info(51,-1000.)>0.5) indet = true;
    Gaudi::LorentzVector mom = p->momentum();
    double sumpt = GetSumPt(p);
    const Gaudi::SymMatrix3x3 & err = p->endVertex()->covMatrix();
    double errr = sqrt( err(0,0) + err(1,1) );
    
    //Is the particle close to the detector material ?
    if( m_allOutDet && m_RemVtxFromDet!=0 &&  indet ) continue; 
 
    if( mass < m_PreyMinMass || 
        nbtrks < m_nTracks || rho <  m_RMin || rho > m_RMax || 
        sumpt < m_SumPt || chi > m_MaxChi2OvNDoF || 
        pos.z() < m_MinZ || pos.z() > m_MaxZ ||
        errr > m_SigmaR || sqrt(err(2,2)) > m_SigmaZ || ( mass > m_PreyMaxMass && sumpt > m_PreyMaxSumPt)
        || fracT>m_fracT || fracE > m_fracE ){ 
      if( msgLevel( MSG::DEBUG ) )
        debug()<<"Particle do not pass the cuts"<< endmsg; 
      continue; 
    }
    if(mass > highestMass )highestMass = mass;

    double theta = 0.;
    double mindist = 1000000000.;
    double minip = 1000000000.;
    for (std::vector<const RecVertex*>::const_iterator ipv = SelectedPVs.begin(); ipv != SelectedPVs.end(); ipv++){
      //Check that we are forward
      if ((p->endVertex()->position().z()-(*ipv)->position().z())<0.)continue;
      // Get BestIP...
      Gaudi::XYZPoint vertex = (*ipv)->position();
      Gaudi::XYZVector direction (mom.Px(),mom.Py(),mom.Pz()); 
      Gaudi::XYZVector d = direction.unit();
      Gaudi::XYZVector ipV = d.Cross((pos-vertex).Cross(d));
      double ip = (ipV.z() < 0) ? -ipV.R() : ipV.R();
      if (ip< minip ){
        theta = fabs(atan(sqrt(pow(pos.x()-(*ipv)->position().x(),2)+
                               pow(pos.y()-(*ipv)->position().y(),2))/(pos.z()-(*ipv)->position().z())));
        minip = ip;
      }
      if((p->endVertex()->position()-(*ipv)->position()).R()<mindist){
        mindist=(p->endVertex()->position()-(*ipv)->position()).R();
      }
    }
    
    if( theta< m_PreyMinAngle || mindist < m_PreyMinFD ){
      if( msgLevel( MSG::DEBUG ) )
        debug()<<"Particle do not pass the min angle/ FD cut"<< endmsg; 
      continue; 
    }
    if( !m_allOutDet && !indet) oneOutMat=true;
    Particle clone = Particle( *p );
    clone.addInfo(80,mindist ); 
    clone.addInfo(81, mass ); 
    clone.addInfo(82, sumpt ); 
    clone.addInfo(51,p->info(51,-1000.) ); 
    clone.addInfo(52,p->info(52,-1000.) ); 
    clone.addInfo(55,p->info(55,-1000.) ); 
    clone.addInfo(56,p->info(56,-1000.) ); 
    clone.setParticleID( m_PreyID );
    this->markTree( &clone );
    nbCands++;
  }//  <--- end of Prey loop
  if( (unsigned int)nbCands < m_NbCands || 
      (m_NbCandsExclusive && (unsigned int)nbCands!=m_NbCands )|| 
      (m_NbCands>1 && ( highestMass < m_PreyMinHighMass || (!m_allOutDet && !oneOutMat ))))
  {
    if( msgLevel( MSG::DEBUG ) )
      debug() << "Insufficent number of candidates !"<< endmsg;
    return StatusCode::SUCCESS;
  }
  setFilterPassed(true);

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Hlt2SelDV::finalize() {
  return DVAlgorithm::finalize();
}



//=============================================================================
//  Loop on the daughter track to see if there is at least one backward track
//  and one forward tracks
//=============================================================================
bool Hlt2SelDV::HasBackAndForwardTracks( const RecVertex* RV ){
  SmartRefVector< Track >::const_iterator i = RV->tracks().begin();
  SmartRefVector< Track >::const_iterator iend = RV->tracks().end();
  bool back = false;
  bool forw = false;
  for( ; i != iend; ++i ){
    if ( (*i)->checkFlag( Track::Backward ) ){ back = true;}
    else { forw = true;}
    if( back && forw ) return true;
  }
  return false;
}


//=============================================================================
// Compute the sum pT of a bunch of track (daughters of a Particle)
//=============================================================================
double Hlt2SelDV::GetSumPt( const Particle * p ){

  double sumpt = 0;
  SmartRefVector<Particle>::const_iterator iend = p->daughters().end();
  for( SmartRefVector<Particle>::const_iterator i = 
	 p->daughters().begin(); i != iend; ++i ){
    if(!m_useVelo &&  std::abs(i->target()->pt()-400.)<1e-8 )continue;
    sumpt += i->target()->pt();
  }
  return sumpt;
}


// //============================================================================
// // Prescale as function of mass and r
// //============================================================================
// bool Hlt2SelDV::parametricPrescaler( double mass , double r , int cand ){
//   // the function
//   double acceptFrac = 1. ;
//   std::ostringstream seed;
//   seed << "DisplVertPrescale" << cand ;
//   // Prescale or not?
//   const LoKi::Cuts::ODIN_PRESCALE scale  = LoKi::Cuts::ODIN_PRESCALE ( acceptFrac , seed.str() ) ;
//   const LHCb::ODIN* odin = get<LHCb::ODIN*>( LHCb::ODINLocation::Default ) ;
//   return scale ( odin ) ;
// }

