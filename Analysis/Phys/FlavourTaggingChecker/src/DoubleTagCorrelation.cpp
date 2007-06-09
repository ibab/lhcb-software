// $Id: DoubleTagCorrelation.cpp,v 1.4 2007-06-09 12:47:20 musy Exp $
#include "DoubleTagCorrelation.h"

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
  : DVAlgorithm ( name , pSvcLocator ), m_debug(0), m_forcedBtool(0)
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

  m_forcedBtool = tool<IForcedBDecayTool> ( "ForcedBDecayTool", this );
  if( ! m_forcedBtool ) {
    fatal() << "Unable to retrieve ForcedBDecayTool tool "<< endreq;
    return StatusCode::FAILURE;
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
  const MCParticle* B0 =  m_forcedBtool->forcedB();
  MCParticle::Vector B0daughters(0);
  int truetag = B0->particleID().pid()>0 ? 1 : -1;
  //-------------------

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
    
    ++ntags;
  }

  setFilterPassed( true );
  return StatusCode::SUCCESS;
};
//============================================================================
MCParticle* DoubleTagCorrelation::associatedofHEP(HepMC::GenParticle* hepmcp) {

  MCParticles* mcpart = get<MCParticles> ( MCParticleLocation::Default );

  int mid = hepmcp->pdg_id();
  double mothmom = hepmcp->momentum().vect().mag();
  double moththeta = hepmcp->momentum().vect().theta();
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

