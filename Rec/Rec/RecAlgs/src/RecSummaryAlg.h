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

  /// Check if an object exists
  template <class TYPE>
  bool safeExist  ( const std::string& location ) 
  {
    DataObject * obj = NULL ;
    const StatusCode sc = evtSvc()->findObject( location , obj ) ;
    return ( sc.isSuccess() && 
             NULL != obj    && 
             NULL != dynamic_cast<TYPE*>(obj) ) ;
  }

  /// generic templated method to extract the number of entries in a given location.
  /// usage int n = number<LHCb::Particles>('/Event/Phys/MyParts/Particles')
  template<class CLASS> 
  inline int number( const std::string& location)
  {
    return ( safeExist<CLASS>(location) ? (get<CLASS>(location))->size() : -1 ) ;
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
