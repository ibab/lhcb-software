// $Id: UnpackTwoProngVertex.h,v 1.1 2009-01-26 09:45:51 ocallot Exp $
#ifndef UNPACKTWOPRONGVERTEX_H 
#define UNPACKTWOPRONGVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class UnpackTwoProngVertex UnpackTwoProngVertex.h
 *  Unoack the Two Prong vertex class
 *
 *  @author Olivier Callot
 *  @date   2009-01-21
 */
class UnpackTwoProngVertex : public GaudiAlgorithm {
public: 
  /// Standard constructor
  UnpackTwoProngVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~UnpackTwoProngVertex( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // UNPACKTWOPRONGVERTEX_H
