// $Id: PackCollision.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef PACKCOLLISION_H 
#define PACKCOLLISION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackCollision PackCollision.h
 *  Pack the Collision objects.
 *
 *  @author Olivier Callot
 *  @date   2005-03-18
 */
class PackCollision : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackCollision( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackCollision( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // PACKCOLLISION_H
