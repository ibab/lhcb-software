// $Id: RelatedPVFinder.h,v 1.4 2008-07-11 14:46:01 pkoppenb Exp $
#ifndef RELATEDPVFINDER_H 
#define RELATEDPVFINDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IRelatedPVFinder.h"            // Interface

class IDistanceCalculator;
class IContextTool ;

/** @class RelatedPVFinder RelatedPVFinder.h
 *  
 *  Returns the related PV for a Particle according to some criteria.
 *  Not to be used directly. Use the IPhysDesktop method relatedPV()
 *  that will call this tool.
 *
 *  @author Patrick Koppenburg
 *  @date   2006-10-22
 */
class RelatedPVFinder : public GaudiTool, virtual public IRelatedPVFinder {
public: 
  /// Standard constructor
  RelatedPVFinder( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~RelatedPVFinder( ); ///< Destructor

  StatusCode initialize() ; ///< Initialize

  /// Return all related PVs
  StatusCode relatedPVs(const LHCb::Particle* p, 
                       Particle2Vertex::Table* ) const ;

  virtual StatusCode setDefaults(std::string PVloc, std::string geomTool) ;

protected:
  
private:

  const IDistanceCalculator* m_dist; ///< pointer to Geom tool. Take same as DVAlgo.

  bool m_closestZ ; ///< Take closest PV in Z
  bool m_closest ; ///< Take closest PV
  bool m_smallestIP ; ///< Take the one with smallest IP
  bool m_chi2 ; ///< Cut on chi2 significance, not absolute numbers
  std::string m_pvLocation ; ///< Location of PV
  
};
#endif // RELATEDPVFINDER_H
