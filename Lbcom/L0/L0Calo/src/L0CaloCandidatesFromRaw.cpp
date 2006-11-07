// $Id: L0CaloCandidatesFromRaw.cpp,v 1.9 2006-11-07 10:25:41 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// local
#include "L0CaloCandidatesFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0CaloCandidatesFromRaw
//
// 2003-12-15 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( L0CaloCandidatesFromRaw );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0CaloCandidatesFromRaw::L0CaloCandidatesFromRaw( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "Extension",      m_extension = "" );
}
//=============================================================================
// Destructor
//=============================================================================
L0CaloCandidatesFromRaw::~L0CaloCandidatesFromRaw() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_convertTool = tool<L0CaloCandidatesFromRawBank>( "L0CaloCandidatesFromRawBank" );

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::execute() {

  debug() << "==> Execute" << endmsg;
  
  std::string name     = LHCb::L0CaloCandidateLocation::Default + m_extension;
  std::string nameFull = LHCb::L0CaloCandidateLocation::Full + m_extension;
  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );

  const std::vector<LHCb::RawBank*>& banks = rawEvt->banks( LHCb::RawBank::L0Calo );

  // convert the banks to two arrays of ints.
  std::vector<std::vector<unsigned int> > data;
  for ( std::vector<LHCb::RawBank*>::const_iterator itBnk = banks.begin(); banks.end() != itBnk; ++itBnk ) {
    unsigned int* body = (*itBnk)->data();
    std::vector<unsigned int> temp;
    for ( int k = 0; (*itBnk)->size()/4 > k; ++k ) {
      temp.push_back( *body++ );
    }
    data.push_back( temp );
  }
  m_convertTool->convertRawBankToTES( data, nameFull, name );
  
  if ( "" != m_extension ) {
    //== Compare 
    LHCb::L0CaloCandidates* ref     = get<LHCb::L0CaloCandidates>( LHCb::L0CaloCandidateLocation::Full );
    LHCb::L0CaloCandidates* outFull = get<LHCb::L0CaloCandidates>( nameFull );
    LHCb::L0CaloCandidates::const_iterator itOld = ref->begin();
    LHCb::L0CaloCandidates::const_iterator itNew = outFull->begin();
    while ( ref->end() != itOld && outFull->end() != itNew ) {
      if ( (*itOld)->type()     != (*itNew)->type()   ||
           //(*itOld)->id()       != (*itNew)->id()     ||
           (*itOld)->etCode()   != (*itNew)->etCode() ||
           (*itOld)->et()       != (*itNew)->et()     ||
           (*itOld)->posTol()   != (*itNew)->posTol() ||
           (*itOld)->position() != (*itNew)->position() ) {
        info() << "Error : old " << (*itOld) << endreq
               << "        new " << (*itNew) << endreq;
      } else {
        debug() << "Entry OK " << itNew-outFull->begin() 
                << " type " << (*itNew)->type() << endreq;
      }
      itOld++;
      itNew++;
    }
  }
  
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0CaloCandidatesFromRaw::finalize() {
  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
