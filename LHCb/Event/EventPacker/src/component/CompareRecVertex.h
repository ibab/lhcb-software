// $Id: CompareRecVertex.h,v 1.1.1.1 2008-11-18 17:12:59 ocallot Exp $
#ifndef COMPARERECVERTEX_H 
#define COMPARERECVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class CompareRecVertex CompareRecVertex.h
 *  Compare two containers of RecVertices
 *
 *  @author Olivier Callot
 *  @date   2008-11-17
 */
class CompareRecVertex : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  CompareRecVertex( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CompareRecVertex(); ///< Destructor

  virtual StatusCode execute();  ///< Algorithm execution

private:

  std::string m_inputName;
  std::string m_testName;

};

#endif // COMPARERECVERTEX_H
