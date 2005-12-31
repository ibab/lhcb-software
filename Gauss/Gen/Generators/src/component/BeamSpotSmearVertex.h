// $Id: BeamSpotSmearVertex.h,v 1.4 2005-12-31 17:31:24 robbep Exp $
#ifndef GENERATORS_BEAMSPOTSMEARVERTEX_H 
#define GENERATORS_BEAMSPOTSMEARVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IVertexSmearingTool.h"

/** @class BeamSpotSmearVertex BeamSpotSmearVertex.h "BeamSpotSmearVertex.h"
 *  
 *  VertexSmearingTool to smear vertex according to beam spot parameters.
 *  Concrete implementation of IVertexSmearingTool.
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class BeamSpotSmearVertex : public GaudiTool, 
                            virtual public IVertexSmearingTool {
public:
  /// Standard constructor
  BeamSpotSmearVertex( const std::string& type, const std::string& name,
                       const IInterface* parent);
  
  virtual ~BeamSpotSmearVertex( ); ///< Destructor

  /// Initialize function
  virtual StatusCode initialize( ) ;

  /** Implementation of IVertexSmearingTool::smearVertex.
   *  Gaussian smearing of spatial position of primary event truncated
   *  at a given number of sigma. 
   */
  virtual StatusCode smearVertex( LHCb::HepMCEvent * theEvent ) ;
  
 private:
  /// Width of the smearing along the x-axis (set by job options).
  double m_sigmaX ;
  
  /// Width of the smearing along the y-axis (set by job options).
  double m_sigmaY ;

  /// Width of the smearing along the z-axis (set by job options).
  double m_sigmaZ ;

  /// Number of sigma above which to cut for x-axis smearing (set by options)
  double m_xcut   ;

  /// Number of sigma above which to cut for y-axis smearing (set by options)
  double m_ycut   ;

  /// Number of sigma above which to cut for z-axis smearing (set by options)
  double m_zcut   ;

  Rndm::Numbers m_gaussDist ; ///< Gaussian random number generator
  
};
#endif // GENERATORS_BEAMSPOTSMEARVERTEX_H
