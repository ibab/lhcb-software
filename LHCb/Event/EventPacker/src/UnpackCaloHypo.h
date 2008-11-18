// $Id: UnpackCaloHypo.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
#ifndef UNPACKCALOHYPO_H 
#define UNPACKCALOHYPO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class UnpackCaloHypo UnpackCaloHypo.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2008-11-14
 */
class UnpackCaloHypo : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackCaloHypo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackCaloHypo( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // UNPACKCALOHYPO_H
