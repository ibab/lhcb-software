// $Id: IRelatedPVFinder.h,v 1.8 2008-10-21 10:12:45 jpalac Exp $
#ifndef KERNEL_IRELATEDPVFINDER_H 
#define KERNEL_IRELATEDPVFINDER_H 1

// Include files
// from STL
#include <string>

#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Kernel/Particle2Vertex.h"

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class IDistanceCalculator;

static const InterfaceID IID_IRelatedPVFinder ( "IRelatedPVFinder", 2, 0 );

/** @class IRelatedPVFinder IRelatedPVFinder.h Kernel/IRelatedPVFinder.h
 *  
 *  Fill a weighted relations table with the weighted relations between 
 *  an LHCb::Particle and a container of LHCb::RecVertices.
 *  Replaces interface by Patrick Koppenburg, 2006-10-22.
 *
 *  @author Juan Palacios juan.palacios@nikhef.nl
 *
 *  @date   2008-09-29
 */
class IRelatedPVFinder : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IRelatedPVFinder; }

  /**
   * Return a range of weighted relations between an LHCb::Particle and a
   * keyed container of LHCb::RecVertices
   * 
   * @param particle       The LHCb::Particle for which related PVs will be searched
   * @param PVs            Container of LHCb::RecVertices to be related to particle
   *
   * @return               sorted weighted relations table relating particle and the vertices in PVs. 
   * Must return empty table if something goes wrong in association.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   */
  virtual const Particle2Vertex::LightTable relatedPVs(const LHCb::Particle* particle,
                                                       const LHCb::RecVertex::Container& PVs) const =0;
  /**
   * Return a range of weighted relations between an LHCb::Particle and a
   * vector of const LHCb::RecVertex pointers
   *
   * @param particle       The LHCb::Particle for which related PVs will be searched
   * @param PVs            std::vector of LHCb::RecVertex* to be related to particle
   *
   * @return               sorted weighted relations table relating particle and the vertices in PVs. 
   * Must return empty table if something goes wrong in association.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   */
  virtual const Particle2Vertex::LightTable relatedPVs(const LHCb::Particle* particle,
                                                       const LHCb::RecVertex::ConstVector& PVs) const =0;
  /**
   * Return a range of weighted relations between an LHCb::Particle and a
   * keyed container of LHCb::RecVertices from a TES location
   *
   * @param particle       The LHCb::Particle for which related PVs will be searched
   * @param PVLocation     String giving the TES location of a keyed container 
   * of LHCb::RecVertices to be related to particle
   *
   * @return               Container of weighted relations between particle and the vertices in PVs. 
   *
   * Must return empty table if something goes wrong in association.
   *
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   */
  virtual const Particle2Vertex::LightTable relatedPVs(const LHCb::Particle* particle,
                                                       const std::string& PVLocation) const =0;

};
#endif // KERNEL_IRELATEDPVFINDER_H
