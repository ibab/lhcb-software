// $Id: MCReconstructible.h,v 1.1 2007-01-12 14:16:41 cattanem Exp $
#ifndef MCRECONSTRUCTIBLE_H 
#define MCRECONSTRUCTIBLE_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Interface
#include "Kernel/IMCReconstructible.h"            

/** @class MCReconstructible MCReconstructible.h
 *  
 *
 *  @author Christian Jacoby
 *  @date   2004-04-28
 */
class MCReconstructible : public GaudiTool, virtual public IMCReconstructible {
public: 
  /// Standard constructor
  MCReconstructible( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~MCReconstructible( ); ///< Destructor
 
  /// Initialize
  StatusCode initialize();
  
  /// Finalize
  StatusCode finalize();

  /// Get the reconstruction category for the given MCParticle
  virtual IMCReconstructible::RecblCategory reconstructible( const LHCb::MCParticle* mcPart );

  /// Is the MCParticle in the detector acceptance?
  virtual bool accepted( const LHCb::MCParticle* part );
  
protected:

private:
  
  /// Acceptance parameters (neutrals)
  double m_zECAL, m_xECALInn, m_yECALInn, m_xECALOut, m_yECALOut;
  /// Threshold for Et gammas reconstructibility
  double m_lowEt;    
  

};
#endif // MCRECONSTRUCTIBLE_H
