// $Id: IFlavourTaggingTool.h,v 1.2 2002-09-03 08:21:55 odie Exp $
#ifndef FLAVOURTAGGING_IFLAVOURTAGGINGTOOL_H 
#define FLAVOURTAGGING_IFLAVOURTAGGINGTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/Particle.h"
#include "Event/Vertex.h"
#include "Event/FlavourTag.h"

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IFlavourTaggingTool("IFlavourTaggingTool", 1, 0);

/** @class IFlavourTaggingTool IFlavourTaggingTool.h Tagging/IFlavourTaggingTool.h
 *  
 *
 *  @author Olivier Dormond
 *  @date   14/03/2002
 */
class IFlavourTaggingTool : virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IFlavourTaggingTool; }

  /** tagThisB( theB, theEvent, thePrimVtx, theTag )
   *  
   *  Return a Tag object (theTag) filled with the tagging decision.
   *
   *  The decision is made by looking in theEvent for particles that
   *  do not comes from theB but which should come from its compagnion.
   *
   *  thePrimVtx should be the primary vertex theB is pointing to.
   */
  virtual void tagThisB( const Particle &theB,
                         const Particles &theEvent,
                         const Vertex &thePrimVtx,
                         FlavourTag &theTag ) = 0;
  virtual void tagThisB( const Particle &theB,
                         const ParticleVector &theEvent,
                         const Vertex &thePrimVtx,
                         FlavourTag &theTag ) = 0;

  /** tagFromList( theB, theCandidates, thePrimVtx, theTag )
   *  
   *  Return a Tag object (theTag) filled with the tagging decision.
   *
   *  The decision is made by looking in theCandidates for particles that
   *  might come the compagnion of theB.
   *
   *  thePrimVtx should be the primary vertex theB is pointing to.
   */
  virtual void tagFromList( const Particle &theB,
                            const ParticleVector &theCandidates,
                            const Vertex &thePrimVtx,
                            FlavourTag &theTag ) = 0;

  /** tagExcludingFromList( theB, theEvent, theExcluded, thePrimVtx, theTag )
   *  
   *  Return a Tag object (theTag) filled with the tagging decision.
   *
   *  The decision is made by looking in theEvent for particles that
   *  are not in theExcluded and might come the compagnion of theB.
   *
   *  thePrimVtx should be the primary vertex theB is pointing to.
   */
  virtual void tagExcludingFromList( const Particle &theB,
                                     const Particles &theEvent,
                                     const ParticleVector &theExcluded,
                                     const Vertex &thePrimVtx,
                                     FlavourTag &theTag ) = 0;
  virtual void tagExcludingFromList( const Particle &theB,
                                     const ParticleVector &theEvent,
                                     const ParticleVector &theExcluded,
                                     const Vertex &thePrimVtx,
                                     FlavourTag &theTag ) = 0;
};
#endif // TAGGING_IFLAVOURTAGGINGTOOL_H
