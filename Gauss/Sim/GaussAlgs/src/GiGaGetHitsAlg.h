// $Id: GiGaGetHitsAlg.h,v 1.1.1.1 2004-02-20 19:43:29 ibelyaev Exp $
#ifndef GETGIGAHITSALG_H 
#define GETGIGAHITSALG_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

/** @class GiGaGetHitsAlg GiGaGetHitsAlg.h
 *  
 *
 *  @author Witold Pokorski
 *  @date   2002-08-13
 */

class GiGaGetHitsAlg : public Algorithm {

public:

  /// Standard constructor
  GiGaGetHitsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GiGaGetHitsAlg( ); ///< Destructor

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
  std::string    m_richsegments  ;
  std::string    m_richtracks    ;
  
  typedef std::vector<std::string> Addresses ;
  Addresses      m_caloHits    ;
  
};
#endif // GETGIGAHITSALG_H
