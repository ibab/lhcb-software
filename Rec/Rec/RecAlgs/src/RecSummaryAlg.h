#ifndef RECSUMMARYALG_H
#define RECSUMMARYALG_H 1

// STL
#include <string>
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event Model
#include "Event/Track.h"
#include "Event/RecSummary.h"
#include "Event/RecVertex.h"
#include "Event/STCluster.h"
#include "Event/STLiteCluster.h"
#include "Event/VeloCluster.h"
#include "Event/VeloLiteCluster.h"
#include "Event/VPCluster.h"
#include "Event/FTLiteCluster.h"
#include "Event/CaloDigit.h"
#include "Event/MuonCoord.h"

// tool interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "OTDAQ/IOTRawBankDecoder.h"
#include "L0Interfaces/IL0DUFromRawTool.h"
#include "Kernel/ICountContainedObjects.h"

// boost
#include "boost/optional.hpp"
#include <boost/numeric/conversion/cast.hpp>

/** @class RecSummaryAlg RecSummaryAlg.h
 *
 *  Fill the LHCb::RecSummary class with summary information from the event
 *  reconstruction.
 *
 *  @author Chris Jones
 *  @date   2011-01-19
 */
class RecSummaryAlg : public GaudiAlgorithm
{

public:

  /// Standard constructor
  RecSummaryAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RecSummaryAlg( ); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution
  virtual StatusCode initialize(); ///<  Algorithm initialization

private:

  /// Adds the number of objects at the given TES location to the summary object
   template<class CLASS, class FALLBACK = CLASS>
  inline void addSizeSummary( LHCb::RecSummary * summary,
                              const LHCb::RecSummary::DataTypes id,
                              const std::string& location ) const
  {
     boost::optional<int> size;
     {
        const CLASS * data = getIfExists<CLASS>(location);
        if (data) size = boost::numeric_cast<int>(data->size());
     }
     if (!size && !std::is_same<CLASS, FALLBACK>::value) {
        const FALLBACK * data = getIfExists<FALLBACK>(location);
        if (data) size = boost::numeric_cast<int>(data->size());
     }
     if (size) {
        summary->addInfo( id, *size );
     } else {
        summary->addInfo( id, 0 );
        Warning( "No data at '" + location + "'", StatusCode::FAILURE, 0 ).ignore();
     }
  }

  /// Access on demand the RICH decoding tool
  inline Rich::DAQ::IRawBufferToSmartIDsTool * richTool()
  {
    if ( !m_richTool )
    {
      m_richTool =
        tool<Rich::DAQ::IRawBufferToSmartIDsTool>
        ("Rich::DAQ::RawBufferToSmartIDsTool","RichSmartIDDecoder");
    }
    return m_richTool;
  }

  /// Access on demand the OT decoder
  const IOTRawBankDecoder* otTool()
  {
    if ( !m_otTool )
    {
      m_otTool = tool<IOTRawBankDecoder>("OTRawBankDecoder");
    }
    return m_otTool;
  }

  /// Access on-demand the Velo track counter
  inline const ICountContainedObjects* countVeloTracks()
  {
    if ( !m_countVeloTracks )
    {
      m_countVeloTracks = tool<ICountContainedObjects>("CountVeloTracks");
    }
    return m_countVeloTracks;
  }

private:

  /// List of sub-detectors to add
  std::vector<std::string> m_dets;

  /// List of known-detectors to add
  std::vector<std::string> m_knownDets;

  /// TES location to save the summary object
  std::string m_summaryLoc;

  /// Location in the TES to load the recosntructed tracks from
  std::string m_trackLoc;

  /// Are split track locations used in the HLT
  bool m_split;

  /// Location in the TES of split long tracks in the HLT
  std::string m_trackLongLoc;

  /// Location in the TES of split downstream tracks in the HLT
  std::string m_trackDownLoc;

  /// Location in the TES to load the reconstructed PVs from
  std::string m_pvLoc;

  /// TES location of Velo clusters
  std::string m_veloLoc;

  /// TES location of VP clusters
  std::string m_vpLoc;

  /// TES location of IT clusters
  std::string m_itLoc;

  /// TES location of TT clusters
  std::string m_ttLoc;

  /// TES location of UT clusters
  std::string m_utLoc;

  /// TES location of FT clusters
  std::string m_ftLoc;

  /// TES location of CaloDigits
  std::string m_spdLoc;

  /// TES location of Muon Coords
  std::string m_muonCoordsLoc;

  /// TES location of Muon Tracks
  std::string m_muonTracksLoc;

  /// Pointer to RICH DAQ decoding tool
  Rich::DAQ::IRawBufferToSmartIDsTool* m_richTool;

  /// OT tool
  const IOTRawBankDecoder* m_otTool;

  /// CountVeloTracks tool
  const ICountContainedObjects* m_countVeloTracks;

};

#endif // RECSUMMARYALG_H
