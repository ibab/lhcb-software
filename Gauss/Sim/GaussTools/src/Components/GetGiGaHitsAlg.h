// $Id: GetGiGaHitsAlg.h,v 1.2 2003-06-17 15:54:36 witoldp Exp $
#ifndef GETGIGAHITSALG_H 
#define GETGIGAHITSALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"


/** @class GetGiGaHitsAlg GetGiGaHitsAlg.h
 *  
 *
 *  @author Witold Pokorski
 *  @date   2002-08-13
 */
class GetGiGaHitsAlg : public Algorithm {
public:
  /// Standard constructor
  GetGiGaHitsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GetGiGaHitsAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  std::string    m_othits      ;
  std::string    m_ithits      ;
  std::string    m_velohits    ;
  std::string    m_puvelohits  ;
  std::string    m_muonhits    ;
  std::string    m_richhits    ;
  std::string    m_richop      ;
  
  typedef std::vector<std::string> Addresses ;
  Addresses      m_caloHits    ;
  
};
#endif // GETGIGAHITSALG_H
