// $Id: FlatZSmearVertex.h,v 1.2 2005-12-12 16:06:20 robbep Exp $
#ifndef GENERATORS_FLATZSMEARVERTEX_H 
#define GENERATORS_FLATZSMEARVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 

#include "Generators/IVertexSmearingTool.h"

/** @class FlatZSmearVertex FlatZSmearVertex.h "FlatZSmearVertex.h"
 *  
 *  Tool to smear vertex with flat smearing along the z-axis and Gaussian
 *  smearing for the other axis (as in BeamSpotSmearVertex). Concrete
 *  implementation of a IVertexSmearingTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class FlatZSmearVertex : public GaudiTool, virtual public IVertexSmearingTool {
public:
  /// Standard constructor
  FlatZSmearVertex( const std::string& type, 
                    const std::string& name,
                    const IInterface* parent);
  
  virtual ~FlatZSmearVertex( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize( ) ;
  
  /** Implements IVertexSmearingTool::smearVertex.
   *  Does the same than BeamSpotSmearVertex::smearVertex for the x and y
   *  direction but generates flat distribution for the z-coordinate of
   *  the primary vertex.
   */
  virtual StatusCode smearVertex( HepMCEvent * theEvent ) ;
  
 private:
  /// Width of the smearing along the x-axis (set by job options).
  double m_sigmaX ;

  /// Width of the smearing along the y-axis (set by job options).
  double m_sigmaY ;

  /// Number of sigma above which to cut for x-axis smearing (set by options)
  double m_xcut   ;

  /// Number of sigma above which to cut for y-axis smearing (set by options)
  double m_ycut   ;

  /// Minimum value for the z coordinate of the vertex (set by options)
  double m_zmin   ;
  
  /// Maximum value for the z coordinate of the vertex (set by options)
  double m_zmax   ;

  Rndm::Numbers m_gaussDist ; ///< Gaussian random number generator

  Rndm::Numbers m_flatDist ; ///< Flat random number generator
};
#endif // GENERATORS_FLATZSMEARVERTEX_H
