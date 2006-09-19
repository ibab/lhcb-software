// $Id: RelatedPV.h,v 1.2 2006-09-19 13:12:56 pkoppenb Exp $
#ifndef RELATEDPV_H 
#define RELATEDPV_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IRelatedPV.h"            // Interface
/** @class RelatedPV RelatedPV.h
 *  
 *  Returns the related PV for a Particle.
 *  Actually tried to access the desktop of the 
 *  parent algorithm.
 *
 *  @author Patrick Koppenburg
 *  @date   2006-09-12
 */
class IGeomDispCalculator;
class IContextTool ;
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

  /// Build relation table and store it in desktop
  StatusCode buildRelations(const LHCb::Particle* p) ;

protected:

private:
  IPhysDesktop* desktop(){return m_desktop;}; ///< return pointer to PhysDesktop of (grand-)parent DVAlgorithm

  StatusCode getTools();

private:

  IPhysDesktop* m_desktop; ///< pointer to PhysDesktop of (grand-)parent DVAlgorithm
  IContextTool* m_context ; ///< On offline tool
  const IGeomDispCalculator* m_geom; ///< pointer to Geom tool. Take same as DVAlgo.

  bool m_closestZ ; ///< Take closest PV in Z
  bool m_closest ; ///< Take closest PV
  bool m_smallestIP ; ///< Take the one with smallest IP
  bool m_significance ; ///< Cut on significance, not hard numbers
  
};
#endif // RELATEDPV_H
