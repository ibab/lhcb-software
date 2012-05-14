// $Id: $
#ifndef CHECKCALOHYPOREF_H 
#define CHECKCALOHYPOREF_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CheckCaloHypoRef CheckCaloHypoRef.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2012-05-14
 */
class CheckCaloHypoRef : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CheckCaloHypoRef( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CheckCaloHypoRef( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::vector<std::string> m_inputs;
};
#endif // CHECKCALOHYPOREF_H
