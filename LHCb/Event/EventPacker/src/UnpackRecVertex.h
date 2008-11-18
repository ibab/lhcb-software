// $Id: UnpackRecVertex.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
#ifndef UNPACKRECVERTEX_H 
#define UNPACKRECVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class UnpackRecVertex UnpackRecVertex.h
 *  Unpack a RecVertex
 *
 *  @author Olivier Callot
 *  @date   2008-11-17
 */
class UnpackRecVertex : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackRecVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackRecVertex( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // UNPACKRECVERTEX_H
