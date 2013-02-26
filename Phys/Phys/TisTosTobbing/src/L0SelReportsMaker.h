// $Id: L0SelReportsMaker.h,v 1.1 2010-06-23 22:50:27 tskwarni Exp $
#ifndef L0SELREPORTSMAKER_H
#define L0SELREPORTSMAKER_H 1

// Include files
#include <string>
// from Gaudi
#include "GaudiKernel/VectorMap.h"
#include "Event/HltObjectSummary.h"
#include "GaudiAlg/GaudiAlgorithm.h"


namespace LHCb
{
  class L0MuonCandidate;
  class L0CaloCandidate;
}


/** @class L0SelReportsMaker L0SelReportsMaker.h
 *
 *  @author Tomasz Skwarnicki
 *  @date   2010-06-23
 *
 *  Algorithm to translate L0DO report into HltSelResults and associated HltObjectSummaries
 *  (this algorithm was derived from HltSelReportsMaker)
 *
 */
class L0SelReportsMaker : public GaudiAlgorithm 
{

private:

  enum GlobalSelectionIDs { kL0GlobalID=3 };

public:

  /// Standard constructor
  L0SelReportsMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0SelReportsMaker( ); ///< Destructor

  virtual StatusCode execute();  ///< Algorithm execution

private:

  /// for producing numerical info to be saved on the object
  LHCb::HltObjectSummary::Info infoToSave( const LHCb::HltObjectSummary* hos ) const;

  /// store L0MuonCandidate in HltObjectSummary store
  const LHCb::HltObjectSummary* store_(const LHCb::L0MuonCandidate* object);

  /// store L0MuonCandidate in HltObjectSummary store
  const LHCb::HltObjectSummary* store_(const LHCb::L0CaloCandidate* object);

  template <typename T> const LHCb::HltObjectSummary* store(const ContainedObject* obj)
  {
    const T* t = dynamic_cast<const T*>(obj);
    return ( t ? store_(t) : NULL );
  }

private:

  // ----------------------- data members

  /// location of input L0 DU Report
  StringProperty m_inputL0DUReportLocation;

  /// location of output Hlt Summary
  StringProperty m_outputHltSelReportsLocation;

  /// HltObjectSummary container
  LHCb::HltObjectSummary::Container* m_objectSummaries;

  /// L0 conditions resulting is selections
  std::vector< std::string > m_selectionConditions;

};

#endif // L0SELREPORTSMAKER_H
