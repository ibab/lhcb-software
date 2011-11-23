// $Id: Hlt2StatisticsBase.h,v 1.4 2009-10-16 11:29:32 graven Exp $
#ifndef HLT2STATISTICSBASE_H 
#define HLT2STATISTICSBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/IAlgorithmCorrelations.h"            // Interface
/** @class Hlt2StatisticsBase Hlt2StatisticsBase.h
 *  
 *  Common base class for Hlt2 algorithms making stats
 *
 *  @author Patrick Koppenburg
 *  @date   2008-06-23
 */
typedef std::vector<std::string> strings ;
class Hlt2StatisticsBase : public GaudiAlgorithm {
public: 
  /// Standard constructor
  Hlt2StatisticsBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Hlt2StatisticsBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  IAlgorithmCorrelations* m_algoCorr ;  ///< Correlation tool

  strings getSelections() const ;

private:
  std::string m_hlt2SelectionID ; ///< majore key in ANNSvc
  

};
#endif // HLT2STATISTICSBASE_H
