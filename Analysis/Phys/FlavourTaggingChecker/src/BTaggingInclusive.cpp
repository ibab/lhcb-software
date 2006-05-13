// $Id: BTaggingInclusive.cpp,v 1.1 2006-05-13 16:08:49 musy Exp $

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
 

// Declaration of the Algorithm Factory

static const  AlgFactory<BTaggingInclusive>          s_factory ;
const        IAlgFactory& BTaggingInclusiveFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BTaggingInclusive::BTaggingInclusive( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
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

  m_pAsctLinks = tool<Particle2MCLinksAsct::IAsct>( "Particle2MCLinksAsct", "TagMonitor", this );
  if(0 == m_pAsctLinks) {
    fatal() << "Unable to retrieve Link Associator tool"<<endreq;
    return StatusCode::FAILURE;
  }

  m_debug = tool<IDebugTool> ( "DebugTool", this );
  if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endreq;
    return StatusCode::FAILURE;
  }

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

  MCParticle* B0 = 0;
  GenMCLinks* sigL = 0;
  if( exist<GenMCLinks> (GenMCLinkLocation::Default) ) {
    sigL = get<GenMCLinks> (GenMCLinkLocation::Default);
  } else {
    err() << "Unable to Retrieve GenMCLinks" << endreq;
    return StatusCode::FAILURE; 
  }
  if(sigL->size() > 1) err()<< "sigL->size() > 1 !?!" <<endreq;
  B0 = (*(sigL->begin()))->signal();
  if(!B0) {
    err() << "No signal B in GenMCLinks" << endreq;
    return StatusCode::SUCCESS; 
  }
  
  //-------------------

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
    debug() << "Tags dont exist" << endreq;
    return StatusCode::SUCCESS;
  }
  debug()<< tags->size()  <<" tags found in "<< m_tagslocation   << endreq;
  debug()<< ostags->size()<<" tags found in "<< m_ostagslocation << endreq;
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
           << endreq;
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
         << endreq;
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
         << endreq;
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
         << endreq;
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
         << endreq;
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
    double eftag = (rtag+wtag)/nsele;           // tagging efficiency
    double epsil = eftag*pow(1-2*omtag,2);      // effective efficiency
    if(rtag<wtag) epsil= -epsil;

    rtt      += rtag;
    wtt      += wtag;
    ef_tot   += eftag;
    effe_tot += epsil;

    //errors on efficiency and omega
     
    double eftag_err= sqrt((rtag*utag + utag*wtag)/nsele)/nsele;
    double omtag_err= sqrt( rtag*wtag /(rtag+wtag) ) / (rtag+wtag);

    epsilerr = sqrt((pow(rtag - wtag,2)*
                     (-(pow(rtag - wtag,2)*(rtag +wtag))+nsele
                      *(pow(rtag,2) +14*rtag*wtag+ pow(wtag,2))))
                    /(pow(rtag+wtag+utag,3)*pow(rtag + wtag,3)));
    epsilerrtot = sqrt(pow(epsilerrtot,2)+pow(epsilerr,2));

  
    info()<< tagname <<    " "
	  <<setprecision(2)<<std::setw(8)
	  << epsil*100 << "+-" << epsilerr*100
	  <<" "<<std::setw(8)<< eftag*100 << "+-" <<eftag_err*100
	  <<" "<<std::setprecision(1)<<std::setw(8)
	  << omtag*100 << "+-" <<omtag_err*100
	  <<" "<<std::setw(7)<< (int) rtag
	  <<" "<<std::setw(7)<< (int) wtag
	  << endreq;
  }
  info()<<"========================================================="<<endreq;
  info()<<"==========END OF INCLUSIVE BTAGGING PERFORMANCE ======="<<endmsg;

  return  StatusCode::SUCCESS;
}

//=============================================================================
