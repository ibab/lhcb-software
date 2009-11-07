// $Id: PackCaloHypo.h,v 1.2 2009-11-07 12:20:39 jonrob Exp $
#ifndef PACKCALOHYPO_H 
#define PACKCALOHYPO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/PackedCaloHypo.h"

/** @class PackCaloHypo PackCaloHypo.h
 *  Pack a CaloHypo container
 *
 *  @author Olivier Callot
 *  @date   2008-11-12
 */
class PackCaloHypo : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackCaloHypo( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackCaloHypo( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  std::string m_inputName;
  std::string m_outputName; 
  bool m_alwaysOutput;      ///< Flag to turn on the creation of output, even when input is missing

};
#endif // PACKCALOHYPO_H
