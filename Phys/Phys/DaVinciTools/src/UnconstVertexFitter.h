// $Id: UnconstVertexFitter.h,v 1.2 2002-03-28 17:48:46 gcorti Exp $
#ifndef UNCONSTVERTEXFITTER_H
#define UNCONSTVERTEXFITTER_H 1

// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IVertexFitter.h"

// Forward declarations
class IParticleTransporter;

/** @class UnconstVertexFitter UnconstVertexFitter.h 
 *  Perform an unconstrained vertex fit.
 *  Based on the fortran axksvrtx.F and on its translation to 
 *  C++ by D. Hutchcroft
 *
 * @author S. Amato
*/

class UnconstVertexFitter : public AlgTool,
                            virtual public IVertexFitter {
  //  friend class LagrangeMassVertexFitter;
public:

  /// Standard Constructor
  UnconstVertexFitter( const std::string& type, const std::string& name, 
                       const IInterface* parent);
  
  /// Standard Destructor
  virtual ~UnconstVertexFitter() { }
  
  /// Actual operator function 
  StatusCode fitVertex( const ParticleVector& particleList,Vertex& myVertex ); 
  /// get the first estimative for the z position  
  double getZEstimate (const ParticleVector& particleList);
  

private:
  IParticleTransporter* m_pTransporter;  
  IToolSvc* m_pToolSvc;
  
  
};

#endif // UNCONSTVERTEXFITTER_H
