// $Id: BeamSpotSmearVertex.h,v 1.1 2005-10-03 10:21:04 robbep Exp $
#ifndef GENERATORS_BEAMSPOTSMEARVERTEX_H 
#define GENERATORS_BEAMSPOTSMEARVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h"

#include "Generators/IVertexSmearingTool.h"

/** @class BeamSpotSmearVertex BeamSpotSmearVertex.h 
 *  
 *  Utility tool to keep events with particles only in LHCb acceptance
 * 
 *  @author Patrick Robbe
 *  @date   2005-08-24
 */
class BeamSpotSmearVertex : public GaudiTool, 
                            virtual public IVertexSmearingTool {
public:
  /// Standard constructor
  BeamSpotSmearVertex( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);
  
  virtual ~BeamSpotSmearVertex( ); ///< Destructor

  virtual StatusCode initialize( ) ;

  virtual StatusCode smearVertex( HepMCEvent * theEvent ) ;

protected:

private:
  double m_sigmaX , m_sigmaY , m_sigmaZ ;
  double m_xcut   , m_ycut   , m_zcut   ;
  Rndm::Numbers m_gaussDist ;
  
};
#endif // GENERATORS_BEAMSPOTSMEARVERTEX_H
