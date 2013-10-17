// $Id: $
#ifndef TOPOVERTEXANALYSERTUPLETOOL_H 
#define TOPOVERTEXANALYSERTUPLETOOL_H 1

// Include files
// from Gaudi
#include "IncTopoVert/ITopoVertexAnalyserTupleTool.h"

#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"



/** @class TopoVertexAnalyserTupleTool TopoVertexAnalyserTupleTool.h
 *  This class analyses the vertex found by the TopoVertexTool.
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2013-01-31
 */
class TopoVertexAnalyserTupleTool : public GaudiTool, virtual public ITopoVertexAnalyserTupleTool 
{

public: 

  /// Standard constructor
  TopoVertexAnalyserTupleTool( const std::string& type, 
                               const std::string& name,
                               const IInterface* parent);

  virtual ~TopoVertexAnalyserTupleTool( ); ///< Destructor

public:

  StatusCode analyseVertices(Tuples::Tuple* tuple, const std::string& tesLocation );

};

#endif // TOPOVERTEXANALYSERTUPLETOOL_H
