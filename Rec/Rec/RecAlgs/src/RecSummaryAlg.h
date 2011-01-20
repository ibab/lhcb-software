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

  /// TES location to save the summary object
  std::string m_summaryLoc;

  /// Location in the TES to load the recosntructed tracks from
  std::string m_trackLoc;

  /// Location in the TES to load the reconstructed PVs from
  std::string m_pvLoc;

};

#endif // RECSUMMARYALG_H
