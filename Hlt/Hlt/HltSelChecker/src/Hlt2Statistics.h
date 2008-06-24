// $Id: Hlt2Statistics.h,v 1.1 2008-06-24 11:05:59 pkoppenb Exp $
#ifndef HLT2STATISTICS_H 
#define HLT2STATISTICS_H 1

// Include files
// from Gaudi
#include "Hlt2StatisticsBase.h"


/** @class Hlt2Statistics Hlt2Statistics.h
 *  
 *  Get b, c content, number of PVs, vs Hlt decision, etc
 *
 *  @author Patrick Koppenburg
 *  @date   2008-06-23
 */
class IHltSummaryTool ;
class IAlgorithmCorrelations ;
class Hlt2Statistics : public Hlt2StatisticsBase {
public: 
  /// Standard constructor
  Hlt2Statistics( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Hlt2Statistics( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  strings m_cats ; ///< categories

};
#endif // HLT2STATISTICS_H
