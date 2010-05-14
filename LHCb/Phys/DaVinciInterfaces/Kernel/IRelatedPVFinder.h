// $Id: IRelatedPVFinder.h,v 1.7 2010-05-14 08:13:23 jpalac Exp $
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

/** @class IRelatedPVFinder IRelatedPVFinder.h Kernel/IRelatedPVFinder.h
 *  
 *  Fill a weighted relations table with the weighted relations between 
 *  an LHCb::Particle and a container of LHCb::RecVertices or LHCb::VertexBases.
 *  Replaces interface by Patrick Koppenburg, 2006-10-22.
 *
 *  @author Juan Palacios juan.palacios@nikhef.nl
 *
 *  @date   2008-09-29
 */
class GAUDI_API IRelatedPVFinder : virtual public IAlgTool {
public: 

  DeclareInterfaceID(IRelatedPVFinder, 3, 0);

  /**
   * Return a range of weighted relations between an LHCb::Particle and a
   * range of LHCb::RecVertex*
   * 
   * @param particle       The LHCb::Particle for which related PVs will be searched
   * @param PVs            Range of LHCb::RecVertex* to be related to particle
   *
   * @return               sorted weighted relations table relating particle and the vertices in PVs. 
   * Must return empty table if something goes wrong in association.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   */
  virtual const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                        const LHCb::RecVertex::Range& PVs) const =0;
  
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
  virtual const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
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
  virtual const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                       const LHCb::RecVertex::ConstVector& PVs) const =0;

  /**
   * Return a range of weighted relations between an LHCb::Particle and a
   * keyed container of LHCb::VerticeBases
   * 
   * @param particle       The LHCb::Particle for which related PVs will be searched
   * @param PVs            Container of LHCb::VerticeBases to be related to particle
   *
   * @return               sorted weighted relations table relating particle and the vertices in PVs. 
   * Must return empty table if something goes wrong in association.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   */
  virtual const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                       const LHCb::VertexBase::Container& PVs) const =0;
  /**
   * Return a range of weighted relations between an LHCb::Particle and a
   * vector of const LHCb::VertexBase pointers
   *
   * @param particle       The LHCb::Particle for which related PVs will be searched
   * @param PVs            std::vector of LHCb::VertexBase* to be related to particle
   *
   * @return               sorted weighted relations table relating particle and the vertices in PVs. 
   * Must return empty table if something goes wrong in association.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   */
  virtual const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                       const LHCb::VertexBase::ConstVector& PVs) const =0;

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
  virtual const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                       const std::string& PVLocation) const =0;


  /**
   *
   * New interfaces returning best related PV
   * @todo document.
   *
   */
  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                            const LHCb::RecVertex::Range& PVs) const =0;
  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                            const LHCb::RecVertex::Container& PVs) const =0;
  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                            const LHCb::RecVertex::ConstVector& PVs) const =0;
  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                            const LHCb::VertexBase::Container& PVs) const =0;
  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                            const LHCb::VertexBase::ConstVector& PVs) const =0;
  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                             const std::string& PVLocation) const =0;

};
#endif // KERNEL_IRELATEDPVFINDER_H
