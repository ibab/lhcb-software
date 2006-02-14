// $Id: PrepareVeloRawBuffer.h,v 1.3 2006-02-14 18:20:59 krinnert Exp $
#ifndef PREPAREVELORAWBUFFER_H 
#define PREPAREVELORAWBUFFER_H 1

#include <string>

#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PrepareVeloRawBuffer PrepareVeloRawBuffer.h
 *  Prepare the cluster container from 1MHz raw buffer.
 *  Completely rewritten for the new 1MHz raw buffer.
 *
 *  @author David Jones
 *  @author Kurt Rinnert
 *  @author Olivier Callot
 *  @date   2003-04-11
 *  2004-04-13 : modified and moved to Velo area Chris Parkes
 *  2004-11-04 : modified to use new GaudiAlgorithm by David Hutchcroft
 */
class PrepareVeloRawBuffer : public GaudiAlgorithm {
public:
  /// Standard constructor
  PrepareVeloRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrepareVeloRawBuffer( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize();    ///< Algorithm finalisation

private:

};
#endif // PREPAREVELORAWBUFFER_H
