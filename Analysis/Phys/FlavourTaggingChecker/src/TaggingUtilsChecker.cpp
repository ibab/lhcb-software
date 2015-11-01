// Include files 
#include "TaggingUtilsChecker.h"
#include "TaggingHelpers.h"
//--------------------------------------------------------------------
// Implementation file for class : TaggingUtilsChecker
//
// Author: Marco Musy
//--------------------------------------------------------------------

using namespace LHCb;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TaggingUtilsChecker )

//====================================================================
TaggingUtilsChecker::TaggingUtilsChecker( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent ) :
  GaudiTool ( type, name, parent ), 
  m_debug(0),
  m_Dist(0)
{

  declareInterface<ITaggingUtilsChecker>(this);

}
TaggingUtilsChecker::~TaggingUtilsChecker() {}

//=====================================================================
StatusCode TaggingUtilsChecker::initialize() { 

  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", 
                             StatusCode::FAILURE);  

  m_Dist = m_dva->distanceCalculator ();
  if( !m_Dist ){
    Error("Unable to retrieve the IDistanceCalculator tool");
    return StatusCode::FAILURE;
  }
  m_debug = tool<IPrintMCDecayTreeTool> ( "PrintMCDecayTreeTool", this );
  if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endmsg;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS; 
}

//==========================================================================
StatusCode TaggingUtilsChecker::calcDOCAmin( const Particle* axp,
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
/*
StatusCode TaggingUtilsChecker::calcIP( const Particle* axp, 
                                        const VertexBase* v, 
                                        double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;

  double ipC=0, ipChi2=0;
  StatusCode sc2 = m_Dist->distance (axp, v, ipC, ipChi2);
  if(sc2 && ipChi2!=0) {
     ip=ipC;
     iperr=ipC/sqrt(ipChi2);
  }

  return sc2;
}
*/
//==========================================================================                          
StatusCode TaggingUtilsChecker::calcIP( const Particle* axp,
                                        const VertexBase* v,
                                        double& ip, double& iperr) {
  ip   =-100.0;
  iperr= 0.0;
  int zsign = 0;
  
  double ipC=0, ipChi2=0;
  StatusCode sc2 = m_Dist->distance (axp, v, ipC, ipChi2);
  Gaudi::XYZVector ipV;
  StatusCode sc = m_Dist->distance (axp, v, ipV);
  debug()<<"ipS: "<<ipC<<", ipV.R: "<<ipV.R()<<endmsg;
  if(sc2 && ipChi2!=0) {
    if (sc) zsign = ipV.z()>0? 1:-1;
    //ip=ipC;
    ip=ipC*zsign;
    iperr=ipC/sqrt(ipChi2);
  }
  debug()<<"IP: "<<ipC<<", "<<ip<<", (sign = "<<zsign<<" )"<<endmsg;
  return sc2;
}

//=========================================================================
StatusCode TaggingUtilsChecker::calcIP( const Particle* axp,
                                        const RecVertex::ConstVector& PileUpVtx,
                                        double& ip, double& ipe) {
  double ipmin = 100000.0;
  double ipminerr = 0.0;
  StatusCode lastsc=1;

  RecVertex::ConstVector::const_iterator iv;
  for(iv = PileUpVtx.begin(); iv != PileUpVtx.end(); ++iv){
    double ipx=0, ipex=0;
    double ipC=0, ipChi2=0;
    StatusCode sc = m_Dist->distance (axp, *iv, ipC, ipChi2);
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
StatusCode TaggingUtilsChecker::calcIPPU( const Particle* axp,
                                          const Particle* AXBS,
                                          const RecVertex::ConstVector& PileUpVtx,
                                          const double lastipmin,
                                          double& ippu, double& ippuchi2, double& ipmean, 
                                          double& xpos, double& ypos, double& zpos,
                                          double& xerrpos, double& yerrpos, double& zerrpos,
                                          int& tracks, double& ipBS, double& ipChi2BS) {
  double ipmin = 100000.0;
  double ipchi2min = 100000.0;
  double ipsum = 0;
  double vtxcount = 0;
  StatusCode lastsc=1;
  int ntracks=0;
  xpos=ypos=zpos=xerrpos=yerrpos=zerrpos=100000.0;
  
  RecVertex::ConstVector::const_iterator iv;
  for(iv = PileUpVtx.begin(); iv != PileUpVtx.end(); ++iv){
    double ipC=0, ipChi2=0;
    StatusCode sc = m_Dist->distance (axp, *iv, ipC, ipChi2);
    if( sc ) {
      ++vtxcount;
      ipsum+=ipC;
      if( (ipC < ipmin) && (ipC > lastipmin) ) {
        ipmin = ipC;
        ipchi2min = ipChi2;
        xpos = (*iv)->position().x()/mm;
        xerrpos = (*iv)->covMatrix().At(0,0);
        ypos = (*iv)->position().y()/mm;
        yerrpos = (*iv)->covMatrix().At(1,1);
        zpos = (*iv)->position().z()/mm;
        zerrpos = (*iv)->covMatrix().At(2,2);
        ntracks = (*iv)->tracks().size(); 
        double ipp=0, ipchi2p=0;
        StatusCode sc2 = m_Dist->distance (AXBS, *iv, ipp, ipchi2p);
        if (sc2) {
          ipBS=ipp;
          ipChi2BS=ipchi2p;
        }
      } 
    } else lastsc = sc;
  }
  ipmean = ipsum/vtxcount;
  ippu  = ipmin;
  ippuchi2 = ipchi2min;
  tracks = ntracks;
  return lastsc;
}
//=========================================================================
int TaggingUtilsChecker::countTracks( Particle::ConstVector& vtags ) {
  
  int nr = 0;

  typedef Particle::ConstVector::const_iterator Iterator;
  using TaggingHelpers::SameTrackStatus;
  using TaggingHelpers::isSameTrack;
  for (Iterator ipart = vtags.begin(); vtags.end() != ipart; ++ipart ) {
    bool duplic=false;
    for( Iterator jpart = vtags.begin(); ipart != jpart; ++jpart ) {
      SameTrackStatus isSame = isSameTrack(**ipart, **jpart);
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
bool TaggingUtilsChecker::isinTree(const Particle* axp, 
			    Particle::ConstVector& sons, 
                            double& dist_phi){
  dist_phi = std::numeric_limits<double>::max();

  for (Particle::ConstVector::iterator ip = sons.begin(); ip != sons.end(); ++ip) {
    using TaggingHelpers::SameTrackStatus;
    using TaggingHelpers::isSameTrack;
    using TaggingHelpers::toString;
    using TaggingHelpers::dphi;
    
    const double deltaphi =
      fabs(dphi(axp->momentum().phi(), (*ip)->momentum().phi()));
    if (dist_phi > deltaphi) dist_phi = deltaphi;
    SameTrackStatus isSame = isSameTrack(*axp, **ip);
    if (isSame) {
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
//============================================================================
const Particle* TaggingUtilsChecker::motherof( const Particle* axp,
                                               const Particle::ConstVector& sons ) {

  for( Particle::ConstVector::const_iterator ip = sons.begin(); 
       ip != sons.end(); ++ip ){
    const Particle::ConstVector daught = (*ip)->daughtersVector();
    for( Particle::ConstVector::const_iterator ip2 = daught.begin(); 
         ip2 != daught.end(); ++ip2 ){
      if((*ip2)!=(*ip)) if((*ip2) == axp ) return (*ip);
    }
  }
  return 0;
}
//==========================================================================
const MCParticle* TaggingUtilsChecker::originof( const MCParticle* product ) {
  const MCParticle* mother = product->mother();
  if ( (!mother) || product->particleID().hasBottom() ) return product; 
  else return originof( mother );
}
//=============================================================================
//return a vector containing all daughters of signal 
Particle::ConstVector TaggingUtilsChecker::FindDaughters( const Particle* axp ) {

  Particle::ConstVector apv(0), apv2, aplist(0);
  apv.push_back(axp);

  do {
    apv2.clear();
    for( Particle::ConstVector::const_iterator ip=apv.begin(); 
         ip!=apv.end(); ++ip ) {
      if( (*ip)->endVertex() ) {
        Particle::ConstVector tmp= (*ip)->endVertex()->outgoingParticlesVector();
        for( Particle::ConstVector::const_iterator itmp=tmp.begin(); 
             itmp!=tmp.end(); ++itmp) {
          apv2.push_back(*itmp);
          aplist.push_back(*itmp);
          debug() << " ID daughter= "<< (*itmp)->particleID().pid() 
                  << " P=" << (*itmp)->p()/Gaudi::Units::GeV  
                  << " Pt=" << (*itmp)->pt()/Gaudi::Units::GeV  << endmsg;
        }
      }
    }
    apv = apv2;
  } while ( apv2.size() );
   	    
  return aplist;
}

//=============================================================================
/* OLD CODE
MCParticle::ConstVector 
TaggingUtilsChecker::prodsBstarstar( const MCParticle* B0 ) {

  HepMC::GenParticle* HEPB0 = HEPassociated(B0);
  HepMC::GenParticle* Bstar2=0;
  MCParticle::ConstVector ExcitedProdsBstar2(0);

  if(HEPB0){
    for ( HepMC::GenVertex::particle_iterator  
            iances = HEPB0->production_vertex()->
            particles_begin(HepMC::ancestors);
          iances != HEPB0->production_vertex()->
            particles_end(HepMC::ancestors); ++iances ) {
    
      int genid = abs((*iances)->pdg_id());
      if (genid == 535 || genid == 525 || genid == 515 || 
          genid == 10511 || genid == 10521 || genid == 10531 || 
          genid == 10513 || genid == 10523 || genid == 10533 || 
          genid == 20513 || genid == 20523 || genid == 20533 
          ){
        //B** found
        Bstar2 = (*iances);
        break;
      }
    }
  }
  // return all descendants of B**
  if ( Bstar2 ) {
    for ( HepMC::GenVertex::particle_iterator idescend 
            = Bstar2->end_vertex()->
            particles_begin(HepMC::descendants);
          idescend != Bstar2->end_vertex()->
            particles_end(HepMC::descendants); ++idescend ) {

      MCParticle* mcdescend = associatedofHEP(*idescend);
      if(mcdescend) {
        ExcitedProdsBstar2.push_back(mcdescend);
        // 	debug() <<"Bstarstar descendant "<<(*idescend)->pdg_id()
        // 		<<" correps MCPart P="
        // 		<<mcdescend->momentum().P()/Gaudi::Units::GeV 
        // 		<<endmsg;
      }
    }
  }
  
  return ExcitedProdsBstar2;
}
*/
//=============================================================================
/* OLD CODE
MCParticle::ConstVector 
TaggingUtilsChecker::prodsBstar( const MCParticle* B0 ) {

  HepMC::GenParticle* HEPB0 = HEPassociated(B0);
  HepMC::GenParticle* Bstar1=0;
  MCParticle::ConstVector ExcitedProdsBstar1(0);

  if(HEPB0){
    for ( HepMC::GenVertex::particle_iterator iances 
            = HEPB0->production_vertex()->
            particles_begin(HepMC::ancestors);
          iances != HEPB0->production_vertex()->
            particles_end(HepMC::ancestors); ++iances ) {
    
      int genid = abs((*iances)->pdg_id());
      if (genid == 533 || genid == 523 || genid == 513){
        //B* found
        Bstar1 = (*iances);
        break;
      }
    }
  }
  if ( Bstar1 ) {
    for ( HepMC::GenVertex::particle_iterator idescend 
            = Bstar1->end_vertex()->
            particles_begin(HepMC::descendants);
          idescend != Bstar1->end_vertex()->
            particles_end(HepMC::descendants); ++idescend ) {

      MCParticle* mcdescend = associatedofHEP(*idescend);
      if(mcdescend) ExcitedProdsBstar1.push_back(mcdescend);
      //       if(mcdescend) debug() <<"Bstar descendant "<<(*idescend)->pdg_id()
      // 			    <<" correps MCPart P="
      // 			    <<mcdescend->momentum().P()/Gaudi::Units::GeV
      // 			    <<endmsg;
    }
  }

  return ExcitedProdsBstar1;
}
*/
//=============================================================================
/* OLD CODE
MCParticle::ConstVector 
TaggingUtilsChecker::prodsBstring( const MCParticle* B0 ) {

  HepMC::GenParticle* HEPB0 = HEPassociated(B0);
  HepMC::GenParticle* Bstring=0;
  MCParticle::ConstVector BstringProds(0);

  if(HEPB0){
    for ( HepMC::GenVertex::particle_iterator iances 
            = HEPB0->production_vertex()->
            particles_begin(HepMC::ancestors);
          iances != HEPB0->production_vertex()->
            particles_end(HepMC::ancestors); ++iances ) {
    
      int genid = abs((*iances)->pdg_id());
      if (genid == 91 || genid == 92 ){
        Bstring = (*iances);
        break;
      }
    }
  }
  // return all descendants of B string
  if ( Bstring ) {
    for ( HepMC::GenVertex::particle_iterator idescend
            = Bstring->end_vertex()->
            particles_begin(HepMC::descendants);
          idescend != Bstring->end_vertex()->
            particles_end(HepMC::descendants); ++idescend ) {

      MCParticle* mcdescend = associatedofHEP(*idescend);
      if(mcdescend) BstringProds.push_back(mcdescend);
    }
  }
  
  return BstringProds;
}
*/
//=============================================================================
/* OLD CODE
HepMC::GenParticle* TaggingUtilsChecker::HEPassociated(const MCParticle* mcp) {

  LHCb::HepMCEvents* hepVect =
    get<LHCb::HepMCEvents>( HepMCEventLocation::Default );

  int mid = mcp->particleID().pid();
  double mothmom = mcp->momentum().P();
  double moththeta = mcp->momentum().Theta();

  for( std::vector<LHCb::HepMCEvent*>::iterator q=hepVect->begin();
       q!=hepVect->end(); ++q ) {
    for ( HepMC::GenEvent::particle_iterator
            p  = (*q)->pGenEvt()->particles_begin();
          p != (*q)->pGenEvt()->particles_end();   ++p ) {
      if( mid == (*p)->pdg_id() ) {
        if( fabs(mothmom - (*p)->momentum().rho()) < 1.0 ){
          if( fabs(moththeta -(*p)->momentum().theta())< 0.0001 ){
            return (*p);
          }
        }
      }
    }
  }
  return 0;
}
*/
//============================================================================
/* OLD CODE
MCParticle* TaggingUtilsChecker::associatedofHEP(HepMC::GenParticle* hepmcp) {

  MCParticles* mcpart = get<MCParticles> (MCParticleLocation::Default);

  int mid = hepmcp->pdg_id();
  double mothmom = hepmcp->momentum().rho();
  double moththeta = hepmcp->momentum().theta();
  MCParticles::const_iterator imc;
  for ( imc = mcpart->begin(); imc != mcpart->end(); ++imc ) {
    if( mid == (*imc)->particleID().pid() ) {
      if( fabs(mothmom - (*imc)->momentum().P())< 1.0){
        if( fabs(moththeta -(*imc)->momentum().Theta())< 0.0001){
          return (*imc);
        }
      }
    }
  }
  return 0;
}
*/
//=============================================================================
/* OLD CODE
int TaggingUtilsChecker::comes_from_excitedB(const MCParticle* BS,
                                             const MCParticle* mcp ) {
  MCParticle::ConstVector::iterator iexc;
  int origin=0;

  MCParticle::ConstVector BstringProds= prodsBstring(BS);
  for( iexc=BstringProds.begin();
       iexc!=BstringProds.end(); ++iexc) {
    if( mcp == (*iexc) ) { origin=3; break; }
  }
  MCParticle::ConstVector ExcitedProdsBstar1= prodsBstar(BS);
  for( iexc=ExcitedProdsBstar1.begin(); 
       iexc!=ExcitedProdsBstar1.end(); ++iexc) {
    if( mcp == (*iexc) ) { origin=1; break; }
  }
  MCParticle::ConstVector ExcitedProdsBstar2= prodsBstarstar(BS);
  for( iexc=ExcitedProdsBstar2.begin(); 
       iexc!=ExcitedProdsBstar2.end(); ++iexc) {
    if( mcp == (*iexc) ) { origin=2; break; }
  }

  return origin;
}

*/
//=============================================================================
int TaggingUtilsChecker::comes_from_excitedB(const MCParticle* BS,
                                             const MCParticle* mcp ) {
  MCParticle::ConstVector::iterator iexc;
  int origin=0;

  MCParticle *ancestorBS = const_cast<LHCb::MCParticle*>( BS ); 
  MCParticle *ancestorP  = const_cast<LHCb::MCParticle*>( mcp );
  MCParticle *originP    = const_cast<LHCb::MCParticle*>( mcp );

  while( ancestorP->mother() ) {
    originP = ancestorP;    
    ancestorP = const_cast<LHCb::MCParticle*>(ancestorP->mother());
  }

  while( ancestorBS->mother() ) {
    ancestorBS = const_cast<LHCb::MCParticle*>(ancestorBS->mother());
  }
    
  if(ancestorP->particleID().hasBottom() ){    
    if(ancestorP == ancestorBS) {  // come from the same b-quark
      origin = 3; 
      if (originP->particleID().hasBottom()) origin = 2;

      int genid = abs(originP->particleID().pid());
      if (genid == 533 || genid == 523 || genid == 513) origin=1;  //std::cout<<" Tagging Particle is coming from a B*"<<genid<<std::endl;
      /*
      if (genid == 535 || genid == 525 || genid == 515 || 
          genid == 10511 || genid == 10521 || genid == 10531 || 
          genid == 10513 || genid == 10523 || genid == 10533 || 
          genid == 20513 || genid == 20523 || genid == 20533 
          ) oriding = 2; //std::cout<<" Tagging Particle is coming from a B**"<<genid<<std::endl;
      */

    }else{
      origin = -1;
      // Tagging Particle and Signal B have the DIFFERENT ancestors
    }
  } else {    
    origin = -abs(ancestorP->particleID().pid());
  }
  if (ancestorP == mcp ) origin = 0;  

  debug()<<" TaggingUtilsChecker::comes_from_excitedB has found xFlag="<< origin<< endmsg;
  
  return origin;
}
//=============================================================================
StatusCode TaggingUtilsChecker::finalize() { return StatusCode::SUCCESS; }

