// $Id: FilterMuonSegments.h,v 1.2 2008-01-22 09:58:06 hernando Exp $
#ifndef FILTERMUONSEGMENTS_H 
#define FILTERMUONSEGMENTS_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"

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
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
private:
  
};
#endif // FILTERMUONSEGMENTS_H
