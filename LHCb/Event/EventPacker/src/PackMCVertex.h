// $Id: PackMCVertex.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
#ifndef PACKMCVERTEX_H 
#define PACKMCVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class PackMCVertex PackMCVertex.h
 *  Pack the MCVertices
 *
 *  @author Olivier Callot
 *  @date   2005-03-18
 */
class PackMCVertex : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PackMCVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PackMCVertex( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:
  std::string m_inputName;
  std::string m_outputName;
};
#endif // PACKMCVERTEX_H
