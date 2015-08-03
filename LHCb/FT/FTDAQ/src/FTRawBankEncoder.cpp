// $Id: $
// Include files 
#include "Event/FTCluster.h"
#include "Event/RawEvent.h"

// local
#include "FTRawBankEncoder.h"
#include "FTRawBankParams.h"

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
  //== These parameters should eventually come from the Detector Element
  m_nbBanks = FTRawBank::NbBanks;
  m_nbSipmPerTELL40 = FTRawBank::NbSiPMPerTELL40;

  declareProperty("OutputLocation", m_outputLocation = LHCb::RawEventLocation::Default, "RawBank output location");  

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

  //== create the vector of vectors of vectors with the proper size...
  std::vector<std::vector<unsigned int> > temp(m_nbSipmPerTELL40);
  m_sipmData.resize( m_nbBanks, temp );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode FTRawBankEncoder::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  LHCb::FTClusters* clusters = get<LHCb::FTClusters>(LHCb::FTClusterLocation::Default );
  LHCb::RawEvent* event = getOrCreate<LHCb::RawEvent,LHCb::RawEvent>( m_outputLocation );
  
  int codingVersion = 1;

  for ( unsigned int iBank = 0; m_sipmData.size() > iBank; ++iBank ) {
    for ( unsigned int iPm = 0; m_sipmData[iBank].size() > iPm; ++iPm ) {
      m_sipmData[iBank][iPm].clear();
    }
  }


  for ( LHCb::FTClusters::const_iterator itC = clusters->begin(); clusters->end() != itC; ++itC ) {
    LHCb::FTChannelID id = (*itC)->channelID();
    unsigned int bankNumber = id.quarter() + 4 * id.layer();   //== Temp, assumes 1 TELL40 per quarter. Should come from Det.Elem.
    if ( m_sipmData.size() <= bankNumber ) {
      info() << "*** Invalid bank number " << bankNumber << " channelID " << id << endmsg;
      return StatusCode::FAILURE;
    }
    unsigned int sipmNumber = id.sipmId() + 16 * QuarterModule(id.module());//== changes to be done here to include module + mat 
    if ( m_sipmData[bankNumber].size() <= sipmNumber ) {
      info() << "Invalid SiPM number " << sipmNumber << " in bank " << bankNumber << " channelID " << id << endmsg;
      return StatusCode::FAILURE;
    }
    
    if ( m_sipmData[bankNumber][sipmNumber].size() <= FTRawBank::nbClusMaximum ) {  // one extra word for sipm number + nbClus
      if ( 0 ==m_sipmData[bankNumber][sipmNumber].size() ) {
        m_sipmData[bankNumber][sipmNumber].push_back( sipmNumber << FTRawBank::sipmShift );
      }
      int frac = int( (*itC)->fraction() * (FTRawBank::fractionMaximum+1) );
      int cell = id.sipmCell();
      int sipmId = 0;
      int cSize = (*itC)->size() -1; //remove 1 to make sure to keep clusters with size 1,2,3 and 4 using 2 bits to encode the cluster size in the data format
      int charg = (*itC)->charge() / 16; // one MIP should be around 32 (6 bits ADC) -> coded as 2.
      if ( 0 > frac   || FTRawBank::fractionMaximum < frac  ) frac   = FTRawBank::fractionMaximum;
      if ( 0 > cell   || FTRawBank::cellMaximum     < cell  ) cell   = FTRawBank::cellMaximum;
      if ( 0 > sipmId || FTRawBank::sipmIdMaximum   < sipmId) sipmId = FTRawBank::sipmIdMaximum;
      if ( 0 > cSize  || FTRawBank::sizeMaximum     < cSize ) cSize  = FTRawBank::sizeMaximum;
      if ( 0 > charg  || FTRawBank::chargeMaximum   < charg ) charg  = FTRawBank::chargeMaximum;
      int coded = 
        (frac  << FTRawBank::fractionShift) + 
        (cell  << FTRawBank::cellShift) + 
        (sipmId<< FTRawBank::sipmIdShift) + 
        (cSize << FTRawBank::sizeShift) + 
        (charg << FTRawBank::chargeShift);
      if ( msgLevel( MSG::VERBOSE ) ) {
        verbose() << format( "Bank%3d sipm%4d cell %4d frac %6.4f sipmId %5d charge %5d size %3d code %4.4x",
                             bankNumber, sipmNumber, id.sipmCell(), (*itC)->fraction(), 
                             sipmId, (*itC)->charge(), (*itC)->size(), coded ) << endmsg;
      }
      m_sipmData[bankNumber][sipmNumber].push_back( coded );
      m_sipmData[bankNumber][sipmNumber][0] += 1;
    }
  }


  //== Now build the banks: We need to put the 16 bits content into 32 bits words.

  for ( unsigned int iBank = 0; m_sipmData.size() > iBank; ++iBank ) {
    if( msgLevel( MSG::VERBOSE ) ) verbose() << "*** Bank " << iBank << endmsg;
    std::vector<unsigned int> bank;
    int lowHigh = 0;
    int temp = 0;
    for ( unsigned int iPm = 0; m_sipmData[iBank].size() > iPm; ++iPm ) {
      for ( std::vector<unsigned int>::iterator itI = m_sipmData[iBank][iPm].begin(); 
            m_sipmData[iBank][iPm].end() != itI; ++itI ) {
        if ( 0 == lowHigh ) {
          temp = (*itI);
          lowHigh = 1;
        } else {
          temp += (*itI)<<16;
          lowHigh = 0;
          bank.push_back( temp );
          if ( msgLevel( MSG::VERBOSE ) ) {
            verbose() << format( "    at %5d data %8.8x", bank.size(), temp ) << endmsg;
          }
        }
      }
    }
    if ( 1 == lowHigh ) {
      bank.push_back( temp );
      if ( msgLevel( MSG::VERBOSE ) ) {
        verbose() << format( "    at %5d data %8.8x", bank.size(), temp ) << endmsg;
      }
    }
    event->addBank( iBank, LHCb::RawBank::FTCluster, codingVersion, bank );
  }


  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode FTRawBankEncoder::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  for ( unsigned int iBank = 0; m_sipmData.size() > iBank; ++iBank ) {
    for ( unsigned int iPm = 0; m_sipmData[iBank].size() > iPm; ++iPm ) {
      m_sipmData[iBank][iPm].clear();
    }
    m_sipmData[iBank].clear();
  }
  m_sipmData.clear();
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
unsigned int FTRawBankEncoder::QuarterModule(unsigned int module)
{
  unsigned int quarterModuleNber = 9;
  if(module < 5) quarterModuleNber = module;
  else 
  {
    quarterModuleNber = module - 4;
    if(module == 10) quarterModuleNber = 5;
    if(module == 11) quarterModuleNber = 0;
  }
  return quarterModuleNber;
}
