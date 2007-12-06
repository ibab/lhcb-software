// $Id: FilterMuonSegments.h,v 1.1 2007-12-06 16:12:15 hernando Exp $
#ifndef FILTERMUONSEGMENTS_H 
#define FILTERMUONSEGMENTS_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

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
