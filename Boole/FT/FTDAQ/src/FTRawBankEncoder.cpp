// $Id$
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/FTCluster.h"
#include "Event/RawEvent.h"

// local
#include "FTRawBankEncoder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FTRawBankEncoder
//
// 2012-05-11 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( FTRawBankEncoder )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FTRawBankEncoder::FTRawBankEncoder( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
FTRawBankEncoder::~FTRawBankEncoder() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode FTRawBankEncoder::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTRawBankEncoder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::FTClusters* clusters = get<LHCb::FTClusters>(LHCb::FTClusterLocation::Default );
  LHCb::RawEvent* event = getOrCreate<LHCb::RawEvent,LHCb::RawEvent>(LHCb::RawEventLocation::Default);

  int m_nbBanks = 48;
  int m_nbSiPMPerTELL40 = 110;
  
  std::vector<std::vector<unsigned int> > bankContent(m_nbBanks);
  
  int codingVersion = 0;

  int prevBankNumber = -1;
  int prevSiPMNumber = -1;
  int sizeWord       = -1;
  int nbCluster      = 0;
  for ( LHCb::FTClusters::const_iterator itC = clusters->begin(); clusters->end() != itC; ++itC ) {
    LHCb::FTChannelID id = (*itC)->channelID();
    int bankNumber = id.quarter() + 4 * id.layer();   //== Temp, assumes 1 TELL40 per quarter. Should come from Det.Elem.
    int siPMNumber = id.sipmId();
    if ( bankNumber != prevBankNumber ) {
      if ( bankNumber >= m_nbBanks ) {
        info() << "*** Bank number invalid: " << bankNumber << " from cluster number " << itC - clusters->begin() 
               << " layer " << id.layer() << " quarter " << id.quarter() << endmsg;
        continue;
      }
      if ( sizeWord >= 0 ) {
        bankContent[prevBankNumber][sizeWord] += nbCluster;  // set the last number of clusters
        debug() << "Bank " << prevBankNumber << " SiPM " << prevSiPMNumber << " nbCluster " << nbCluster << endmsg;
      }
      while ( prevBankNumber < bankNumber ) {
        if ( prevBankNumber >= 0 ) {
          while( ++prevSiPMNumber < m_nbSiPMPerTELL40 ) {
            bankContent[prevBankNumber].push_back( prevSiPMNumber << 4 );
          }
        }
        ++prevBankNumber;
        prevSiPMNumber = -1;
      }
      prevBankNumber = bankNumber;
      prevSiPMNumber = -1;
      sizeWord = -1;
      nbCluster = 0;
    }
    while( prevSiPMNumber < siPMNumber ) {
      if ( sizeWord >= 0 ) {
        bankContent[prevBankNumber][sizeWord] += nbCluster;  // set the last number of clusters
        debug() << "Bank " << prevBankNumber << " SiPM " << prevSiPMNumber << " nbCluster " << nbCluster << endmsg;
      }
      prevSiPMNumber++;
      bankContent[bankNumber].push_back( prevSiPMNumber << 4 );
      sizeWord = bankContent[bankNumber].size() - 1;
      nbCluster = 0;
    }
    if ( nbCluster < 15 ) {
      int coded =  
        int( (*itC)->fraction() * 8 )
        + ( id.sipmCell() << 3 )
        //        + ( (*itC)->charge() << 10 )
        + ( (*itC)->size() << 10 );
      bankContent[bankNumber].push_back( coded );
      if ( msgLevel( MSG::VERBOSE ) ) {
        verbose() << format( "  cell %4d frac %6.4f charge %5d size %3d code %4.4x",
                             id.sipmCell(), (*itC)->fraction(), (*itC)->charge(), (*itC)->size(), coded ) << endmsg;
      }
      nbCluster++;
    }
  }
  if ( sizeWord >= 0 ) {
    bankContent[prevBankNumber][sizeWord] += nbCluster;  // set the last number of clusters
    debug() << "Bank " << prevBankNumber << " SiPM " << prevSiPMNumber << " nbCluster " << nbCluster << endmsg;
  }

  while ( prevBankNumber < m_nbBanks ) {
    if ( prevBankNumber >= 0 ) {
      while( ++prevSiPMNumber < m_nbSiPMPerTELL40 ) {
        bankContent[prevBankNumber].push_back( prevSiPMNumber << 4 );
      }
    }
    ++prevBankNumber;
    prevSiPMNumber = -1;
  }

  //== Now build the banks: We need to put the 16 bits content into 32 bits words.

  for ( std::vector<std::vector<unsigned int> >::iterator itV = bankContent.begin(); bankContent.end() != itV; ++itV ) {
    std::vector<unsigned int> bank;
    if( msgLevel( MSG::VERBOSE ) ) verbose() << "*** Bank " << itV - bankContent.begin() << endmsg;
    bank.reserve( ( (*itV).size()+1 )/2);
    for ( std::vector<unsigned int>::iterator itI = (*itV).begin(); (*itV).end() != itI; ++itI ) {
      int temp = (*itI);
      if ( itI+1 != (*itV).end() ) {
        ++itI;
        temp += (*itI)<<16;
      }
      bank.push_back( temp );
      if ( msgLevel( MSG::VERBOSE ) ) {
        verbose() << format( "    at %5d data %8.8x", bank.size(), temp ) << endmsg;
      }
    }
    event->addBank( itV-bankContent.begin(), LHCb::RawBank::FTCluster, codingVersion, bank );
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode FTRawBankEncoder::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
