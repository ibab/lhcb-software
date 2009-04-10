// $Id: DeCaloCalib.h,v 1.3 2009-04-10 14:51:08 odescham Exp $
#ifndef COMPONENT_DECALOCALIB_H 
#define COMPONENT_DECALOCALIB_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/IRndmGenSvc.h" 
#include "CaloDet/DeCalorimeter.h"



/** @class DeCaloCalib DeCaloCalib.h component/DeCaloCalib.h
 *  
 *
 *  @author Olivier DESCHAMPS
 *  @date   2007-08-22
 */
class DeCaloCalib : public GaudiTupleAlg {
public: 
  /// Standard constructor
  DeCaloCalib( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DeCaloCalib( ); ///< Destructor

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
  void update();
  bool isDead(int channel);
  
  Rndm::Numbers m_shoot;

  std::string m_detectorName;
  std::string m_method;
  std::vector<double> m_params;
  std::map<std::string , double > m_deltas;
  std::string m_key;
  DeCalorimeter* m_calo;
  mutable IRndmGenSvc*   m_rndmSvc;        ///< random number service 
  bool m_update;
  bool m_ntup;
  std::vector<int> m_dead;
};
#endif // COMPONENT_DECALOCALIB_H
