// $Id: PackTwoProngVertex.h,v 1.1 2009-01-26 09:45:51 ocallot Exp $
#ifndef PACKTWOPRONGVERTEX_H 
#define PACKTWOPRONGVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackTwoProngVertex PackTwoProngVertex.h
 *  Pack a two prong vertex.
 *
 *  @author Olivier Callot
 *  @date   2009-01-21
 */
class PackTwoProngVertex : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackTwoProngVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackTwoProngVertex( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // PACKTWOPRONGVERTEX_H
