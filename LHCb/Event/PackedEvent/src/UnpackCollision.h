// $Id: UnpackCollision.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef UNPACKCOLLISION_H 
#define UNPACKCOLLISION_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class UnpackCollision UnpackCollision.h
 *  Unpack the Collisions
 *
 *  @author Olivier Callot
 *  @date   2005-03-18
 */
class UnpackCollision : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackCollision( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackCollision( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // UNPACKCOLLISION_H
