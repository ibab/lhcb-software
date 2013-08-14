// $Id: $
#ifndef VERTEXFUNCTIONTOOLNEWRESOLVER_H 
#define VERTEXFUNCTIONTOOLNEWRESOLVER_H 1

// Include files
#include "VertexFunctionTool.h"

/** @class VertexFunctionToolNewResolver VertexFunctionToolNewResolver.h
 *   This tool implements IVertexFunctionTool. See the interface class for a description
 *   of the purpose of these classes.
 *
 *  @author Julien Cogan and Mathieu Perrin-Terrin
 *  @date   2012-10-15
 */


class VertexFunctionToolNewResolver : public VertexFunctionTool{
public: 
  /// Standard constructor
  VertexFunctionToolNewResolver( const std::string& type, 
                                 const std::string& name,
                                 const IInterface* parent);
  //VertexFunctionToolNewResolver( ); 

  
  virtual ~VertexFunctionToolNewResolver( ); ///< Destructor

  bool   areResolved(LHCb::RecVertex & V1,LHCb::RecVertex & V2);


protected:

private:

};



#endif // VERTEXFUNCTIONTOOLNEWRESOLVER_H
