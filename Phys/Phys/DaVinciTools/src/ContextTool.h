// $Id: ContextTool.h,v 1.4 2008-08-04 16:31:29 pkoppenb Exp $
#ifndef CONTEXTTOOL_H 
#define CONTEXTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IContextTool.h"            // Interface
/** @class ContextTool ContextTool.h
 *
 *  Tool the returns context-dependent stuff. Provides
 *  primary vertices, the geom tool and the vertex fitter
 *  to be used. 
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-06-08
 */
class IOnOffline ;
class ContextTool : public GaudiTool, virtual public IContextTool {
public: 
  /// Standard constructor
  ContextTool( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~ContextTool( ); ///< Destructor

  StatusCode initialize();

  /// Get primary vertices
  LHCb::RecVertex::ConstVector primaryVertices() const  ;
  
  /// Get obsolete geom tool - load on demand only.
  const IGeomDispCalculator* geomTool()const  ;

  /// Get distance tool
  const IDistanceCalculator* distanceTool()const{return m_dist;} ;

  /// Get default vertex fitter
  const IVertexFit* vertexFitter()const {return m_fitter;}  ;

  /// Get physDesktop if available
  IPhysDesktop* desktop()const {return m_desktop;}  ;

private :
  /// get tools
  StatusCode getTools() ;
  /// get tools
  const DVAlgorithm* getDVAlgorithm() const ;

private:

  IPhysDesktop* m_desktop; ///< pointer to dektop
  IDistanceCalculator* m_dist ;    ///< pointer to distance tool
  IVertexFit* m_fitter; ///< pointer to fitter
  IOnOffline* m_onOffline ; ///< on offline tool provides locations if needed
  
  
};
#endif // CONTEXTTOOL_H
