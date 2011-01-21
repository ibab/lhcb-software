// $Id: $
#ifndef RECSUMMARYALG_H 
#define RECSUMMARYALG_H 1

// STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event Model
#include "Event/Track.h"
#include "Event/RecSummary.h"
#include "Event/RecVertex.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"

// tool interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"
#include "OTDAQ/IOTRawBankDecoder.h"
#include "L0Interfaces/IL0DUFromRawTool.h"

// LoKi
#include "LoKi/select.h"

/** @class RecSummaryAlg RecSummaryAlg.h
 *  
 *  Fill the LHCb::RecSUmmary class with summary information from the event
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

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  /// Adds the number of objectgs at the given TES location to the summary object
  template<class CLASS> 
  void addSummary( LHCb::RecSummary * summary,
                   const LHCb::RecSummary::DataTypes id,
                   const std::string& location ) const
  {
    if ( exist<CLASS>(location) )
    {
      summary->addInfo( id, (int)(get<CLASS>(location))->size() );
    }
    else
    {
      Warning( "No data at '" + location + "'" ).ignore();
    }
  }

private:

  /// TES location to save the summary object
  std::string m_summaryLoc;

  /// Location in the TES to load the recosntructed tracks from
  std::string m_trackLoc;

  /// Location in the TES to load the reconstructed PVs from
  std::string m_pvLoc;

  /// TES location of Velo clusters
  std::string m_veloLoc;

  /// TES location of IT clusters
  std::string m_itLoc;

  /// Pointer to RICH DAQ decoding tool
  Rich::DAQ::IRawBufferToSmartIDsTool* m_richTool;

  /// OT tool
  const IOTRawBankDecoder* m_otTool;

  /// L0 tool
  IL0DUFromRawTool* m_l0Tool;

};

#endif // RECSUMMARYALG_H
