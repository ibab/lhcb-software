// $Id: FilterMuonSegments.h,v 1.4 2008-12-19 17:34:24 graven Exp $
#ifndef FILTERMUONSEGMENTS_H 
#define FILTERMUONSEGMENTS_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"

/** @class FilterMuonSegments FilterMuonSegments.h
 *  
 *
 *  @author Sandra Amato 
 *  @date   2007-11-29
 */
class FilterMuonSegments : public HltAlgorithm {
public: 
  /// Standard constructor
  FilterMuonSegments( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterMuonSegments( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  Hlt::SelectionContainer3<LHCb::Track,LHCb::Track,LHCb::Track> m_selections;
  
};
#endif // FILTERMUONSEGMENTS_H
