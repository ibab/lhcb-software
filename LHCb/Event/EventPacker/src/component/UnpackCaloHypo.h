// $Id: UnpackCaloHypo.h,v 1.2 2009-11-07 12:20:39 jonrob Exp $
#ifndef UNPACKCALOHYPO_H 
#define UNPACKCALOHYPO_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/PackedCaloHypo.h"

/** @class UnpackCaloHypo UnpackCaloHypo.h
 *  
 *  Unpack CaloHypos
 *
 *  @author Olivier Callot
 *  @date   2008-11-14
 */
class UnpackCaloHypo : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  UnpackCaloHypo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackCaloHypo(); ///< Destructor

  virtual StatusCode execute(); ///< Algorithm execution

private:

  std::string m_inputName;
  std::string m_outputName;
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing

};

#endif // UNPACKCALOHYPO_H
