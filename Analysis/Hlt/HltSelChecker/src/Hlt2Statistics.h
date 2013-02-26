// $Id: Hlt2Statistics.h,v 1.2 2009-01-26 17:30:02 pkoppenb Exp $
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
class IAlgorithmCorrelations ;
class Hlt2Statistics : public Hlt2StatisticsBase
{

public: 

  /// Standard constructor
  Hlt2Statistics( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Hlt2Statistics( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  StatusCode fillQuarks();            ///< Fill Quarks
  StatusCode fillHlt(std::string level = "Hlt2");               ///< Fil Hlt Decisions

private:

  strings m_cats ; ///< categories

};

#endif // HLT2STATISTICS_H
