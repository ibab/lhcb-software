// $Id: UnpackMCVertex.h,v 1.1 2006-02-20 08:21:41 cattanem Exp $
#ifndef UNPACKMCVERTEX_H 
#define UNPACKMCVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class UnpackMCVertex UnpackMCVertex.h
 *  Unpack the MCVertices
 *
 *  @author Olivier Callot
 *  @date   2005-03-18
 */
class UnpackMCVertex : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackMCVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackMCVertex( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // UNPACKMCVERTEX_H
