// $Id: DeCaloTiming.h,v 1.1 2007-08-22 19:05:13 odescham Exp $
#ifndef COMPONENT_DECALOTIMING_H 
#define COMPONENT_DECALOTIMING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/IRndmGenSvc.h" 
#include "CaloDet/DeCalorimeter.h"



/** @class DeCaloTiming DeCaloTiming.h component/DeCaloTiming.h
 *  
 *
 *  @author Olivier DESCHAMPS
 *  @date   2007-08-22
 */
class DeCaloTiming : public GaudiTupleAlg {
public: 
  /// Standard constructor
  DeCaloTiming( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DeCaloTiming( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization



protected:
  IRndmGenSvc* rndmSvc() const  { return m_rndmSvc ; }
private:
  double delta(long id ){
     std::stringstream sid("");
     sid << id ;
     return (m_deltas.find( sid.str() ) != m_deltas.end() ) ? m_deltas[sid.str()] : m_deltas["Default"];
  }

  

  std::string m_detectorName;
  std::string m_method;
  std::vector<double> m_params;
  std::map<std::string , double > m_deltas;
  std::string m_key;
  DeCalorimeter* m_calo;
  mutable IRndmGenSvc*   m_rndmSvc;        ///< random number service 
};
#endif // COMPONENT_DECALOTIMING_H
