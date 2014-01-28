// $Id: $
#ifndef ITOPOVERTEXANALYSERTUPLETOOL_H 
#define ITOPOVERTEXANALYSERTUPLETOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"



/** @class ITopoVertexAnalyserTupleTool ITopoVertexAnalyserTupleTool.h
 *  This class is an interface for the tool analysing the output of 
 *  the IncTopoVertAlg
 *
 *  @author Mathieu Perrin-Terrin
 *  @date   2013-08-09
 */
class GAUDI_API ITopoVertexAnalyserTupleTool : virtual public IAlgTool 
{

public: 

  DeclareInterfaceID(ITopoVertexAnalyserTupleTool, 2, 0);

  ///analyse the vertices at the given tesLocation
  virtual StatusCode analyseVertices( Tuples::Tuple* tuple, 
                                      const std::string& tesLocation ) = 0;

};
#endif // ITOPOVERTEXANALYSERTUPLETOOL_H
