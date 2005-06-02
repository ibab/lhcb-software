// $Id: CompareMCVertex.h,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
#ifndef COMPAREMCVERTEX_H 
#define COMPAREMCVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CompareMCVertex CompareMCVertex.h
 *  Compare two containers of MCVertices
 *
 *  @author Olivier Callot
 *  @date   2005-03-18
 */
class CompareMCVertex : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CompareMCVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompareMCVertex( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_inputName;
  std::string m_testName;
};
#endif // COMPAREMCVERTEX_H
