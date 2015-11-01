// $Id: BTaggingInclusive.cpp,v 1.7 2008-07-24 08:21:09 cattanem Exp $
// local
#include "BTaggingInclusive.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BTaggingInclusive
//
// 2006-05-11 : Onuora Awunor
//-----------------------------------------------------------------------------
//
// This algorithm is designed to compute the Inclusive Tagging performance
// for the taggers OS_Muon, OS_Electron, OS_Kaon, SS_Kaon & VtxCharge only.
// It retrieves from TES the tagging information dumped from running the
// DoubleTagging algorithm
//
 
using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of Factory
DECLARE_ALGORITHM_FACTORY( BTaggingInclusive )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BTaggingInclusive::BTaggingInclusive( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator ), m_forcedBtool(0)
{
  declareProperty("TagsLocation",m_tagslocation = "/Event/Phys/Tags" );
  declareProperty("OSTagsLocation",m_ostagslocation = "/Event/Phys/OSTags" );
}
//=============================================================================
// Destructor
//=============================================================================
BTaggingInclusive::~BTaggingInclusive() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode BTaggingInclusive::initialize() {

  m_debug = tool<IPrintMCDecayTreeTool> ( "PrintMCDecayTreeTool", this );
  if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endmsg;
    return StatusCode::FAILURE;
  }

  //m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  m_forcedBtool = tool<IBDecayTool> ( "BDecayTool", this );
  if( ! m_forcedBtool ) {
    fatal() << "Unable to retrieve BDecayTool tool "<< endmsg;
    return StatusCode::FAILURE;
  }

  nsele=0;
  for(int i=0; i<50; ++i) { nrt[i]=0; nwt[i]=0; }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode BTaggingInclusive::execute() {

  setFilterPassed( false );

  ///////////////////////////////////////
  //
  // Choose the forced B
  //
  ///////////////////////////////////////
  const MCParticle* B0 = m_forcedBtool->forcedB();
  MCParticle::Vector B0daughters(0);

  int truetag = B0->particleID().pid()>0 ? 1 : -1;
  int ix = 0;

  /////////////////////////////////////
  //
  //  Now look in Tag Location
  //
  //////////////////////////////////////
 
  FlavourTags* tags;
  FlavourTags* ostags;

  if( exist<FlavourTags>(m_tagslocation)  &&  
      exist<FlavourTags>(m_ostagslocation)){
    tags   = get<FlavourTags>( m_tagslocation );
    ostags = get<FlavourTags>( m_ostagslocation );
  } else {
    debug() << "Tags dont exist" << endmsg;
    return StatusCode::SUCCESS;
  }
  debug()<< tags->size()  <<" tags found in "<< m_tagslocation   << endmsg;
  debug()<< ostags->size()<<" tags found in "<< m_ostagslocation << endmsg;
  //
  int osmuondec = 0;
  int oselectrondec = 0;
  int oskaondec = 0;
  int sskaondec = 0;
  int vtxChargedec = 0;
    
  for(FlavourTags::const_iterator it=tags->begin(); it!=tags->end(); it++){
   
    std::vector<Tagger> taggers = (*it)->taggers();
    for(size_t i=0;i<taggers.size();++i){

      ////////////////////////////////////////
      //
      //   Retrieve Muon tagger decision
      //   NB: assigned fictious category ix=1
      ////////////////////////////////////////

      if(taggers[i].type() == Tagger::OS_Muon){
        ix = 1;
        osmuondec = taggers[i].decision();
      
        debug() << "OS Muon decision = " << osmuondec << endmsg;
      
	info() << "Muon Tagger Decision "
	       << std::setw(3) << truetag
	       << std::setw(3) << ix
	       << std::setw(3) << osmuondec
	       << endmsg;
      }

      ////////////////////////////////////////
      //
      //   Retrieve Electron tagger decision
      //   NB: assigned ficticious category ix=2
      ////////////////////////////////////////


      if(taggers[i].type() == Tagger::OS_Electron){
        ix = 2;
        oselectrondec = taggers[i].decision();

        debug() << "OS Electron decision = " << oselectrondec << endmsg;
      
	info() << "Electron Tagger Decision "
	       << std::setw(3) << truetag
	       << std::setw(3) << ix
	       << std::setw(3) << oselectrondec
	       << endmsg;
      }

      ////////////////////////////////////////
      //
      //   Retrieve OS Kaon tagger decision
      //   NB: assigned ficticious category ix=3
      ////////////////////////////////////////

      if(taggers[i].type() == Tagger::OS_Kaon){
        ix = 3;
        oskaondec = taggers[i].decision();

        debug() << "OS Kaon decision = " << oskaondec << endmsg;
      
	info() << "OS Kaon Decision "
	       << std::setw(3) << truetag
	       << std::setw(3) << ix
	       << std::setw(3) << oskaondec
	       << endmsg;
      }

      ////////////////////////////////////////
      //
      //   Retrieve SS Kaon tagger decision
      //   NB: assigned ficticious category ix=4
      ////////////////////////////////////////

      if(taggers[i].type() == Tagger::SS_Kaon){
        ix = 4;
        sskaondec = taggers[i].decision();

        debug() << "SS Kaon decision = " << sskaondec << endmsg;
      
	info() << "SS Kaon Tagger Decision "
	       << std::setw(3) << truetag
	       << std::setw(3) << ix
	       << std::setw(3) << sskaondec
	       << endmsg;
      }

      ////////////////////////////////////////
      //
      //   Retrieve VtxCharge tagger decision
      //   assigned ficticious category ix=5
      ////////////////////////////////////////

      if(taggers[i].type() == Tagger::VtxCharge){
        ix = 5;
        vtxChargedec = taggers[i].decision();

        debug() << "VtxCharge decision = " << vtxChargedec << endmsg;
      
	info() << "VtxCharge Tagger Decision "
	       << std::setw(3) << truetag
	       << std::setw(3) << ix
	       << std::setw(3) << vtxChargedec
	       << endmsg;
      }

      if (ix > 5) continue; 
      // m_debug->printTree( (*it)->taggedB() );
    }
    
  }
    
  //////////////////////////////////////////////////////////////
  //
  //  Count right-tags and wrong-tags, first compare to truetag
  //
  //////////////////////////////////////////////////////////////

  nsele++;

  if (osmuondec == truetag) {ix=1; nrt[ix]++;}
  if (osmuondec == -truetag) {ix=1; nwt[ix]++;}
 
  if (oselectrondec == truetag) {ix=2; nrt[ix]++;}
  if (oselectrondec == -truetag) {ix=2; nwt[ix]++;}

  if (oskaondec == truetag) {ix=3; nrt[ix]++;}
  if (oskaondec == -truetag) {ix=3; nwt[ix]++;}

  if (sskaondec == truetag) {ix=4; nrt[ix]++;}
  if (sskaondec == -truetag) {ix=4; nwt[ix]++;}

  if (vtxChargedec == truetag) {ix=5; nrt[ix]++;}
  if (vtxChargedec == -truetag) {ix=5; nwt[ix]++;}

  setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode BTaggingInclusive::finalize() {

  MsgStream req( msgSvc(), name() );

  ///////////////////////////////////////////////////////////
  //
  // Calculate effective efficiency for each Tagger with errors
  //
  ////////////////////////////////////////////////////////////
  double rtt=0;
  double wtt=0;
  double ef_tot=0;
  double effe_tot=0;
  double epsilerr, epsilerrtot=0; 

  info()<<"======================================================="<<endmsg;
  info()<< std::setw(40)<< "INCLUSIVE BTAGGING PERFORMANCE " <<endmsg;
  info()<<"======================================================="<<endmsg; 
  info()<< std::setw(10) <<"  Tagger           EFF        Etag          Wrong TF"
	<<" r       w "<<endmsg;

  for( int it=1; it < 6; it++ ) {
  
    using namespace std;
    string tagname; 
    if (it == 1) tagname = "Muon";
    if (it == 2) tagname = "Electron";
    if (it == 3) tagname = "OS Kaon";
    if (it == 4) tagname = "SS Kaon";
    if (it == 5) tagname = "VtxCharge"; 

    double rtag = nrt[it];               // right
    double wtag = nwt[it];               // wrong
    double utag = nsele-rtag-wtag;       // untaggeg

    if(rtag+wtag == 0) continue;         //empty category

    double omtag = wtag/(rtag+wtag);
    double eftag = (rtag+wtag)/nsele;                  // tagging efficiency
    double epsil = eftag*(1-2*omtag)*(1-2*omtag);      // effective efficiency
    //    if(rtag<wtag) epsil= -epsil;  // Who wrote this ????

    rtt      += rtag;
    wtt      += wtag;
    ef_tot   += eftag;
    effe_tot += epsil;

    //errors on efficiency and omega
    double eftag_err= sqrt((rtag*utag + utag*wtag)/nsele)/nsele;
    double omtag_err= sqrt( rtag*wtag /(rtag+wtag) ) / (rtag+wtag);
    double diff_tag = (rtag - wtag);
    double sum_tag  = (rtag + wtag);

    
    epsilerr = sqrt(( diff_tag*diff_tag*
                     (-(diff_tag*diff_tag*sum_tag)+nsele
                      *(rtag*rtag +14*rtag*wtag+ wtag*wtag)))
                    /(nsele*nsele*nsele*sum_tag*sum_tag*sum_tag));
    epsilerrtot = sqrt(epsilerrtot*epsilerrtot+epsilerr*epsilerr);


  
    info()<< tagname <<    " "
	  <<setprecision(2)<<std::setw(8)
	  << epsil*100 << "+-" << epsilerr*100
	  <<" "<<std::setw(8)<< eftag*100 << "+-" <<eftag_err*100
	  <<" "<<std::setprecision(1)<<std::setw(8)
	  << omtag*100 << "+-" <<omtag_err*100
	  <<" "<<std::setw(7)<< (int) rtag
	  <<" "<<std::setw(7)<< (int) wtag
	  << endmsg;
  }
  info()<<"========================================================="<<endmsg;
  info()<<"==========END OF INCLUSIVE BTAGGING PERFORMANCE ======="<<endmsg;

  return  StatusCode::SUCCESS;
}
//============================================================================
MCParticle* BTaggingInclusive::associatedofHEP(HepMC::GenParticle* hepmcp) {

  MCParticles* mcpart = get<MCParticles> ( MCParticleLocation::Default );

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

