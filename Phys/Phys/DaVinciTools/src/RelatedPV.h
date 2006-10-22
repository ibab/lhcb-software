// $Id: RelatedPV.h,v 1.3 2006-10-22 13:19:49 pkoppenb Exp $
#ifndef RELATEDPV_H 
#define RELATEDPV_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IRelatedPV.h"            // Interface
/** @class RelatedPV RelatedPV.h
 *  
 *  Returns the related PV for a Particle.
 *  Actually tries to access the desktop of the 
 *  parent algorithm. So this tool is essentially an interface to
 *  IPhysDesktop method relatedPV().
 *
 *  @author Patrick Koppenburg
 *  @date   2006-09-12
 */
class RelatedPV : public GaudiTool, virtual public IRelatedPV {
public: 
  /// Standard constructor
  RelatedPV( const std::string& type, 
             const std::string& name,
             const IInterface* parent);
  
  virtual ~RelatedPV( ); ///< Destructor
  
  /// initialize
  StatusCode initialize();
  
  /// Returns the best related PV 
  const LHCb::RecVertex* bestPV(const LHCb::Particle* p);
  
  /// Returns all related PVs ordered by weight and above a given weight 
  StatusCode allPVs(const LHCb::Particle* p, LHCb::RecVertex::ConstVector&, double minweight=0) ;

protected:

private:
  IPhysDesktop* desktop(){return m_desktop;}; ///< return pointer to PhysDesktop of (grand-)parent DVAlgorithm

  StatusCode getTools();

private:

  IContextTool* m_context ; ///< Context tool
  IPhysDesktop* m_desktop; ///< pointer to PhysDesktop of (grand-)parent DVAlgorithm
  
};
#endif // RELATEDPV_H
