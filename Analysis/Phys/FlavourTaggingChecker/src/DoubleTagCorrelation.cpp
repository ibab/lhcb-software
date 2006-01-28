// $Id: DoubleTagCorrelation.cpp,v 1.1 2006-01-28 22:34:20 musy Exp $
#include "DoubleTagCorrelation.h"

//--------------------------------------------------------------------------
// Implementation file for class : DoubleTagCorrelation
//
// 2006-01-28 : Chris Barnes
//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<DoubleTagCorrelation>          s_factory ;
const        IAlgFactory& DoubleTagCorrelationFactory = s_factory ; 

//==========================================================================
DoubleTagCorrelation::DoubleTagCorrelation( const std::string& name,ISvcLocator* pSvcLocator )
  : DVAlgorithm ( name , pSvcLocator ) {
  
  declareProperty("TagsLocation",m_tagslocation = "/Event/Phys/Tags" );
  declareProperty("OSTagsLocation",m_ostagslocation = "/Event/Phys/OSTags" );

  matrix = new int*[2];
  for(int i=0;i<2;++i) matrix[i] = new int[2];    
}

//==========================================================================
DoubleTagCorrelation::~DoubleTagCorrelation() {}; 

//==========================================================================
StatusCode DoubleTagCorrelation::initialize() {
  
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

  for(int i=0;i<2;++i) 
    for(int j=0;j<2;++j) matrix[i][j] = 0;  

  return StatusCode::SUCCESS;
};

//==========================================================================
// Main execution
//==========================================================================
StatusCode DoubleTagCorrelation::execute() {
 
  setFilterPassed( false );

  //choose the forced B
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

  //look in TagLocation
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

  int ntags = 0;
  for(FlavourTags::const_iterator it=tags->begin(); it!=tags->end(); it++){
    int sskaondec = 0;

    std::vector<Tagger> taggers = (*it)->taggers();
    for(size_t i=0;i<taggers.size();++i){
      if(taggers[i].type() == Tagger::SS_Kaon){
        sskaondec = taggers[i].decision();
        debug() << "SS Kaon decision = " << sskaondec << endmsg;
      }
    }
    
    if(sskaondec == 0) continue;
    
    //now find corresponding os tagdecison
    //iterate so that next time round we get the right one
    FlavourTags::const_iterator osit=(ostags->begin())++;
    
    int ostagdecision = (*osit)->decision();

    if(ostagdecision == 0) continue;

    int kaonright = sskaondec == truetag ? 1 : 0;
    int osright   = ostagdecision == truetag ? 1 : 0;

    if( kaonright && osright ) ++matrix[0][0];
    else if( !kaonright && !osright ) ++matrix[1][1];
    else if(  kaonright && !osright ) ++matrix[1][0];
    else if( !kaonright &&  osright ) ++matrix[0][1];
    else debug() << "Another decision?" << endmsg;
    
    //m_debug->printTree( (*it)->taggedB() );
    ++ntags;
  }

  setFilterPassed( true );
  return StatusCode::SUCCESS;
};

//==========================================================================
StatusCode DoubleTagCorrelation::finalize(){ 

  MsgStream req( msgSvc(), name() );

  info() << "******************TAGCORRELATION********************" << endmsg;  
  info() << "Matrix element [0][0] = OSR SSKR = " << matrix[0][0] << endmsg;
  info() << "Matrix element [0][1] = OSR SSKW = " << matrix[0][1] << endmsg;
  info() << "Matrix element [1][0] = OSW SSKR = " << matrix[1][0] << endmsg;
  info() << "Matrix element [1][1] = OSW SSKW = " << matrix[1][1] << endmsg;
  
  for(int i=0;i<2;++i) delete[] matrix[i];
  delete[] matrix;

  return StatusCode::SUCCESS; 
}
//==========================================================================

