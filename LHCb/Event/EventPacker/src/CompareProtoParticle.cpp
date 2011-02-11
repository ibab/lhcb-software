// $Id: CompareProtoParticle.cpp,v 1.2 2009-07-09 09:44:16 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/ProtoParticle.h"

// local
#include "CompareProtoParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareProtoParticle
//
// 2008-11-14 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CompareProtoParticle )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareProtoParticle::CompareProtoParticle( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::ProtoParticleLocation::Charged );
  declareProperty( "TestName"  , m_testName   = LHCb::ProtoParticleLocation::Charged+"Test" );
}
//=============================================================================
// Destructor
//=============================================================================
CompareProtoParticle::~CompareProtoParticle() {} 

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareProtoParticle::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  LHCb::ProtoParticles* old  = get<LHCb::ProtoParticles>( m_inputName  );
  LHCb::ProtoParticles* test = get<LHCb::ProtoParticles>( m_testName  );

  if ( old->size() != test->size() ) {
    err() << "Old ProtoParticle size " << old->size()
          << " differs form Test " << test->size()
          << endmsg;
    return StatusCode::FAILURE;
  }  
  LHCb::ProtoParticles::const_iterator itOld  = old->begin();
  LHCb::ProtoParticles::const_iterator itTest = test->begin();

  while ( old->end() != itOld ) {
    LHCb::ProtoParticle* oPart = (*itOld++);
    LHCb::ProtoParticle* tPart = (*itTest++);
    if ( oPart->key() != tPart->key() ) {
      warning() << "Wrong key : old " <<  oPart->key() << " test " << tPart->key() << endmsg;
    }

    bool isOK = true;

    const LHCb::Track* oTrack = oPart->track();   // resolve to a pointer...
    const LHCb::Track* tTrack = tPart->track();
    if ( oTrack != tTrack ) isOK = false;
    
    const LHCb::RichPID* oRichPID = oPart->richPID();   // resolve to a pointer...
    const LHCb::RichPID* tRichPID = tPart->richPID();
    if ( oRichPID != tRichPID ) isOK = false;
    
    const LHCb::MuonPID* oMuonPID = oPart->muonPID();   // resolve to a pointer...
    const LHCb::MuonPID* tMuonPID = tPart->muonPID();
    if ( oMuonPID != tMuonPID ) isOK = false;

    unsigned int kk;    
    std::vector<const LHCb::CaloHypo*> oHypo, tHypo;
    SmartRefVector<LHCb::CaloHypo>::const_iterator itH;
    for ( itH = oPart->calo().begin(); oPart->calo().end() != itH; ++itH ) {
      const LHCb::CaloHypo* z = *itH;
      oHypo.push_back( z );
    }
    for ( itH = tPart->calo().begin(); tPart->calo().end() != itH; ++itH ) {
      const LHCb::CaloHypo* z = *itH;
      tHypo.push_back( z );
    }
    if ( oHypo.size() != tHypo.size() ) isOK = false;
    for ( kk = 0; oHypo.size() > kk ; ++kk ) {
      if ( oHypo[kk] != tHypo[kk] ) isOK = false;
    }   
 
    LHCb::ProtoParticle::ExtraInfo oExtra = oPart->extraInfo();
    LHCb::ProtoParticle::ExtraInfo tExtra = tPart->extraInfo();
    if ( oExtra.size() != tExtra.size() ) isOK = false;
    LHCb::ProtoParticle::ExtraInfo::const_iterator oIt = oExtra.begin();
    LHCb::ProtoParticle::ExtraInfo::const_iterator tIt = tExtra.begin();
    for ( kk = 0; tExtra.size() > kk; ++kk, ++oIt, ++tIt ) {
      if ( (*oIt).first != (*tIt).first ) isOK = false;
      if ( 1.e-7 < fabs( ((*oIt).second - (*oIt).second ) / (*oIt).second ) ) isOK = false;
    }
    

    if ( !isOK || MSG::DEBUG >= msgLevel() ) {
      info() << "===== ProtoParticle key " << oPart->key() << endmsg;
      info() << format( "Old   track %8x  richPID %8X  muonPID%8x  nCaloHypo%4d nExtra%4d",
                        oTrack, oRichPID, oMuonPID, oHypo.size(), oExtra.size() )
             << endmsg;
      info() << format( "Test  track %8x  richPID %8X  muonPID%8x  nCaloHypo%4d nExtra%4d",
                        tTrack, tRichPID, tMuonPID, tHypo.size(), tExtra.size() )
             << endmsg;
      for ( kk = 0 ; oHypo.size() != kk ; ++kk ) {
        info() << format( "   old CaloHypo %8x   new %8x", oHypo[kk], tHypo[kk] )  << endmsg;
      }
      oIt = oExtra.begin();
      tIt = tExtra.begin();
      for ( kk = 0 ; oExtra.size() != kk ; ++kk, ++oIt, ++tIt ) {
        info() << format( "   old Extra %5d %12.4f     new %5d %12.4f", 
                          (*oIt).first, (*oIt).second, (*tIt).first, (*tIt).second ) 
               << endmsg;
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
