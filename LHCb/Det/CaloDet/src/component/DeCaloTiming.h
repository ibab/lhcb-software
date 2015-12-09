// $Id: DeCaloTiming.h,v 1.1 2007-08-22 19:05:13 odescham Exp $
#ifndef COMPONENT_DECALOTIMING_H 
#define COMPONENT_DECALOTIMING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
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

  ~DeCaloTiming( ) override = default; ///< Destructor

  StatusCode initialize() override;    ///< Algorithm initialization



private:
  double delta(long id ){
     auto i = m_deltas.find(std::to_string(id) ) ;
     if ( i == m_deltas.end() ) i = m_deltas.find( "Default" );
     return i->second;
  }

  std::string m_detectorName;
  std::string m_method;
  std::array<double,2> m_params;
  std::map<std::string , double > m_deltas;
  std::string m_key;
  DeCalorimeter* m_calo = nullptr;
};
#endif // COMPONENT_DECALOTIMING_H
