// $Id: DoubleTagCorrelation.cpp,v 1.2 2006-10-24 10:21:07 jpalac Exp $
#include "DoubleTagCorrelation.h"
//#include "Kernel/StringUtils.h"

//--------------------------------------------------------------------------
// Implementation file for class : DoubleTagCorrelation
//
// 2006-01-28 : Chris Barnes
//--------------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of Factory
DECLARE_ALGORITHM_FACTORY( DoubleTagCorrelation );

//==========================================================================
DoubleTagCorrelation::DoubleTagCorrelation( const std::string& name,
					    ISvcLocator* pSvcLocator )
  : DVAlgorithm ( name , pSvcLocator ) 
  , m_setDecay(false)
  , m_evtTypeSvc(NULL)
  , m_mcFinder(NULL)
{  
  declareProperty("TagsLocation",m_tagslocation = "/Event/Phys/Tags" );
  declareProperty("OSTagsLocation",m_ostagslocation = "/Event/Phys/OSTags" );

  matrix = new int*[2];
  for(int i=0;i<2;++i) matrix[i] = new int[2];    
}

//==========================================================================
DoubleTagCorrelation::~DoubleTagCorrelation() {}; 

//==========================================================================
StatusCode DoubleTagCorrelation::initialize() {
  
  m_debug = tool<IDebugTool> ( "DebugTool", this );
  if( ! m_debug ) {
    fatal() << "Unable to retrieve Debug tool "<< endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve the EvtTypeSvc here so that it is always done at initialization
  m_evtTypeSvc = svc<IEvtTypeSvc>( "EvtTypeSvc", true );  
  // Check that EvtType code has been set with appropriate value
  // if it will not be read from data
  if( !m_fromData ) {
    if(  m_evtCode == 0 ) {
      fatal() << "With EvtCodeFromData = false you MUST set EvtCode"<< endmsg;
      return StatusCode::FAILURE;
    }
    // Set the decay descriptor to pass to the MCDecayFinder if using evtCode
    if( (setDecayToFind( m_evtCode )).isFailure() ) {
      fatal() << " 'setDecayToFind' failed in 'initialize' "<< endmsg;
      return StatusCode::FAILURE;
    }
  }

  for(int i=0;i<2;++i) for(int j=0;j<2;++j) matrix[i][j] = 0;  

  return StatusCode::SUCCESS;
};

//==========================================================================
// Main execution
//==========================================================================
StatusCode DoubleTagCorrelation::execute() {
 
  setFilterPassed( false );

  //choose the forced B
  MCParticle* B0 = 0;
  MCParticle::Vector B0daughters(0);

  if( m_fromData && !m_setDecay ) {
    LHCb::GenHeader* header = 
      get<LHCb::GenHeader>( evtSvc(), LHCb::GenHeaderLocation::Default );
    if( setDecayToFind( header->evType()) ) {
      fatal() << " 'setDecayToFind' failed in 'execute' "<< endmsg;
      return StatusCode::FAILURE;
    }
  }
  if( m_mcFinder ) if( m_mcFinder->hasDecay() ){
    m_mcFinder->decayMembers( B0, B0daughters );
    debug()<<" Analysing decay: "<<m_mcFinder->decay()<< endmsg;
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
//=============================================================================
StatusCode DoubleTagCorrelation::setDecayToFind( const int evtCode ) {
 
  // Check if code exist
  if( !(m_evtTypeSvc->typeExists( evtCode )) ) {
    fatal() << "EvtCode " << evtCode << "is not known by the EvtTypeSvc"
            << endmsg;
    return StatusCode::FAILURE;
  }
   
  // Retrieve tool and set decay descriptor
  m_mcFinder = tool<IMCDecayFinder>( "MCDecayFinder", this );
  std::string sdecay = m_evtTypeSvc->decayDescriptor( evtCode );
  if( (m_mcFinder->setDecay( sdecay )).isFailure() ) {
    fatal() << "Unable to set decay for EvtCode " << evtCode << endmsg;
    return StatusCode::FAILURE;
  }
   
  m_setDecay = true;
  m_evtCode  = evtCode;   // in case called when reading data
     
  return StatusCode::SUCCESS;
}

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

