// $Id: FlatZSmearVertex.h,v 1.1 2005-10-03 10:21:46 robbep Exp $
#ifndef GENERATORS_FLATZSMEARVERTEX_H 
#define GENERATORS_FLATZSMEARVERTEX_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/RndmGenerators.h" 

#include "Generators/IVertexSmearingTool.h"

/** @class FlatZSmearVertex FlatZSmearVertex.h 
 *  
 *  Utility tool to keep events with particles only in LHCb acceptance
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

  virtual StatusCode initialize( ) ;
  
  virtual StatusCode smearVertex( HepMCEvent * theEvent ) ;
  
protected:
  
private:
  double m_sigmaX , m_sigmaY ;
  double m_xcut   , m_ycut   ;
  double m_zmin   , m_zmax   ;
  Rndm::Numbers m_gaussDist , m_flatDist ;
  
};
#endif // GENERATORS_FLATZSMEARVERTEX_H
