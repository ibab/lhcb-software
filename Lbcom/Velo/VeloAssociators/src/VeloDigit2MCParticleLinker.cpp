// Include files 

// local
#include "VeloDigit2MCParticleLinker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloDigit2MCParticleLinker
//
// 2006-03-05 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloDigit2MCParticleLinker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloDigit2MCParticleLinker::VeloDigit2MCParticleLinker( const std::string& name,
                                                        ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_asctName ( LHCb::VeloDigitLocation::Default ),
    m_inputDigits ( LHCb::VeloDigitLocation::Default ),
    m_inputParts ( LHCb::MCParticleLocation::Default )
{ }
//=============================================================================
// Destructor
//=============================================================================
VeloDigit2MCParticleLinker::~VeloDigit2MCParticleLinker() {}
//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloDigit2MCParticleLinker::execute() {

  bool isDebug = msgLevel(MSG::DEBUG);

  if(isDebug) debug() << "==> Execute" << endmsg;
  // take the input data
  // VeloDigits
  LHCb::VeloDigits* digits;
  if(!exist<LHCb::VeloDigits>(m_inputDigits)){
    error()<< "There is no digits container at: " << m_inputDigits <<endmsg;
    return ( StatusCode::FAILURE );
  }else{
    digits=get<LHCb::VeloDigits>(m_inputDigits);
  }
  // MCParticles
  if(!exist<LHCb::MCParticles>(m_inputParts)){
    error()<< "There is no parts container at: " << m_inputParts <<endmsg;
    return ( StatusCode::FAILURE );
  }else{
    // Load the target container, not sure if this is needed though
    LHCb::MCParticles* parts=get<LHCb::MCParticles>(m_inputParts);
    if(isDebug) debug()<< "size of parts: " << parts->size() <<endmsg;
  }
  // ==> create linker table VeloDigits -> MCParticles
  LinkerWithKey<LHCb::MCParticle, LHCb::VeloDigit> 
               myLink(evtSvc(), msgSvc(), asctName());
  // take the VeloDigits -> MCHit linker table  
  LHCb::VeloDigits::iterator digIt;
  // map with particles and weights
  std::map<const LHCb::MCParticle*, double> relations;
  // get the table with digit->hit associations
  typedef LinkerTool<LHCb::VeloDigit, LHCb::MCHit> asctTool;
  typedef asctTool::DirectType Table;
  typedef Table::Range Range;
  typedef Table::iterator iterator;
  //
  asctTool associator(evtSvc(), LHCb::VeloDigitLocation::Default+"2MCHits");
  const Table* table=associator.direct();
  if(!table){
    error()<< "Empty table with associations" <<endmsg;
    return ( StatusCode::FAILURE );
  }
  //
  for(digIt=digits->begin(); digIt!=digits->end(); digIt++){
    //
    Range range1=table->relations(*digIt);
    iterator asctIt;
    // fill the relations map, loop over hits related to given digit
    for(asctIt=range1.begin(); asctIt!=range1.end(); asctIt++){
      const LHCb::MCHit* myHit=asctIt->to();
      const LHCb::MCParticle* myPart=myHit->mcParticle();
      if(isDebug) debug()<< "energy: " << myPart->momentum().e() <<endmsg;
      
      double charge=myHit->energy();
      relations[myPart]+=charge;
    }
    // fill the link table
    if(relations.size()!=0){
      if(isDebug) debug()<< " ==> Make links " <<endmsg;
      //
      std::map<const LHCb::MCParticle*, double>::const_iterator relIt;
      for(relIt=relations.begin(); relIt!=relations.end(); relIt++){
        SmartRef<LHCb::MCParticle> myPart=(*relIt).first;
        double weight=(*relIt).second;
        myLink.link(*digIt, myPart, weight);
      }
    }else{
      if(isDebug) debug()<< " ==> No associations made! " <<endmsg;
    }
    relations.clear();
  }
  //
  return StatusCode::SUCCESS;
}
//=============================================================================
