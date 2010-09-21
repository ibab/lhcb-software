#ifndef STRIPPINGTCK_H 
#define STRIPPINGTCK_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class StrippingTCK StrippingTCK.h
 *  
 * A simple algorithm that sets the TCK field in the stripping DecReports 
 * structure
 * 
 *  @author Anton Poluektov
 *  @date   2010-09-20
 */
class StrippingTCK : public GaudiAlgorithm {
public: 
  /// Standard constructor
  StrippingTCK( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~StrippingTCK( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  std::string m_hdrLocation;
  unsigned int m_tck;

};
#endif // StrippingTCK_H
