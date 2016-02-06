// Include files
#include "boost/optional.hpp"
#include "Event/FTCluster.h"
#include "Event/RawEvent.h"

// local
#include "FTRawBankEncoder.h"
#include "FTRawBankParams.h"

//== These parameters should eventually come from the Detector Element
constexpr static int s_nbBanks = FTRawBank::NbBanks;
constexpr static int s_nbSipmPerTELL40 = FTRawBank::NbSiPMPerTELL40;
namespace {
unsigned int QuarterModule(unsigned int module)
{
  unsigned int quarterModuleNber = 9;
  if(module < 5) quarterModuleNber = module;
  else {
    quarterModuleNber = module - 4;
    if(module == 10) quarterModuleNber = 5;
    if(module == 11) quarterModuleNber = 0;
  }
  return quarterModuleNber;
}
}
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

  declareProperty("OutputLocation", m_outputLocation = LHCb::RawEventLocation::Default, "RawBank output location");

}

//=============================================================================
// Initialization
//=============================================================================
StatusCode FTRawBankEncoder::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  //== create the vector of vectors of vectors with the proper size...
  m_sipmData.resize( s_nbBanks, std::vector<std::vector<uint16_t> >{s_nbSipmPerTELL40});

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

  for (auto &b : m_sipmData ) for (auto &pm : b ) pm.clear();

  for ( const auto&  cluster : *clusters ) {
    LHCb::FTChannelID id = cluster->channelID();
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

    auto& data = m_sipmData[bankNumber][sipmNumber];
    if (data.size() > FTRawBank::nbClusMaximum ) continue;
    // one extra word for sipm number + nbClus
    if ( data.empty() ) data.push_back( sipmNumber << FTRawBank::sipmShift );
    auto frac =   std::min(uint16_t(cluster->fraction() * (FTRawBank::fractionMaximum+1)),
                           FTRawBank::fractionMaximum);
    auto cell =   std::min(uint16_t(id.sipmCell()),
                           FTRawBank::cellMaximum);
    auto sipmId = std::min(uint16_t(0),
                           FTRawBank::sipmIdMaximum);
    auto cSize =  std::min(uint16_t(cluster->size() -1),
                           FTRawBank::sizeMaximum); //remove 1 to make sure to keep clusters with size 1,2,3 and 4 using 2 bits to encode the cluster size in the data format
    auto charg =  std::min(uint16_t(cluster->charge() / 16),
                           FTRawBank::chargeMaximum); // one MIP should be around 32 (6 bits ADC) -> coded as 2.
    data.push_back(( frac  << FTRawBank::fractionShift ) |
                   ( cell  << FTRawBank::cellShift ) |
                   ( sipmId<< FTRawBank::sipmIdShift ) |
                   ( cSize << FTRawBank::sizeShift ) |
                   ( charg << FTRawBank::chargeShift ) );
    ++data[0];
    if ( msgLevel( MSG::VERBOSE ) ) {
      verbose() << format( "Bank%3d sipm%4d cell %4d frac %6.4f sipmId %5d charge %5d size %3d code %4.4x",
                           bankNumber, sipmNumber, id.sipmCell(), cluster->fraction(),
                           sipmId, cluster->charge(), cluster->size(), data.back() ) << endmsg;
    }

  }

  //== Now build the banks: We need to put the 16 bits content into 32 bits words.
  for ( unsigned int iBank = 0; m_sipmData.size() > iBank; ++iBank ) {
    if( msgLevel( MSG::VERBOSE ) ) verbose() << "*** Bank " << iBank << endmsg;
    auto words = std::accumulate( m_sipmData[iBank].begin(), m_sipmData[iBank].end(),
                                  0, [](int w, std::vector<uint16_t>& d) {
                                     return w  + d.size();
    });
    std::vector<unsigned int> bank; bank.reserve((words+1)/2);
    boost::optional<unsigned int> buf;
    for ( const auto& pm : m_sipmData[iBank] ) {
      for ( const auto& data : pm ) {
        if (!buf) {
            buf = data;
        } else {
            bank.emplace_back( *buf | ( static_cast<unsigned int>(data) << 16 ) );
            buf = boost::none;
        }
      }
    }
    if (buf) bank.emplace_back( *buf) ;
    if ( msgLevel( MSG::VERBOSE ) ) {
      size_t offset = 0;
      for( const auto& d : bank ) {
        verbose() << format( "    at %5d data %8.8x", offset++, d ) << endmsg;
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
  m_sipmData.clear();
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
