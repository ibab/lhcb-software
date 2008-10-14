// $Id: RelatedPVFinder.h,v 1.6 2008-10-14 16:50:50 jpalac Exp $
#ifndef RELATEDPVFINDER_H 
#define RELATEDPVFINDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IRelatedPVFinder.h"            // Interface

class IDistanceCalculator;

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

  virtual const Particle2Vertex::Relations relatedPVs(const LHCb::Particle* particle,
                                                      const LHCb::RecVertex::Container& PVs,
                                                      const IDistanceCalculator* distanceCalculator) const;

  virtual const Particle2Vertex::Relations relatedPVs(const LHCb::Particle* particle,
                                                      const LHCb::RecVertex::ConstVector& PVs,
                                                      const IDistanceCalculator* distanceCalculator) const;
  

  virtual const Particle2Vertex::Relations relatedPVs(const LHCb::Particle* particle,
                                                      const std::string& PVLocation,
                                                      const IDistanceCalculator* distanceCalculator) const;
  
  
private:

  template <typename Iter> 
  const Particle2Vertex::Relations relatedPVs(const LHCb::Particle* particle,
                                              Iter begin,
                                              Iter end,
                                              const IDistanceCalculator* distanceCalculator) const;

private:

  bool m_closestZ ; ///< Take closest PV in Z
  bool m_closest ; ///< Take closest PV
  bool m_smallestIP ; ///< Take the one with smallest IP
  bool m_chi2 ; ///< Cut on chi2 significance, not absolute numbers
  
};
#endif // RELATEDPVFINDER_H
