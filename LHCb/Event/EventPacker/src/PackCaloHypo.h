// $Id: PackCaloHypo.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
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

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // PACKCALOHYPO_H
