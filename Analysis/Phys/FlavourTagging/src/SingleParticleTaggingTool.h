// $Id: SingleParticleTaggingTool.h,v 1.1.1.1 2002-05-23 23:25:50 gcorti Exp $
#ifndef SINGLEPARTICLETAGGINGTOOL_H 
#define SINGLEPARTICLETAGGINGTOOL_H 1

// Include files
#include <functional>

#include "GaudiKernel/AlgTool.h"
#include "Event/Particle.h"
#include "FlavourTagging/IFlavourTaggingTool.h"

// Forward declarations
class FlavourTag;
class IParticleFilter;
class IGeomDispCalculator;
class IDataProviderSvc;

/** @class SingleParticleTaggingTool SingleParticleTaggingTool.h
 *  
 *  Try to figure out the b/bbar flavour of the given particle.
 *
 *  @author Olivier Dormond
 *  @date   14/03/2002
 */
class SingleParticleTaggingTool : public AlgTool,
                                  virtual public IFlavourTaggingTool {
public:
  /// Standard constructor
  SingleParticleTaggingTool( const std::string &type,
                             const std::string &name,
                             const IInterface *parent ); 

  /// Standard destructor
  ~SingleParticleTaggingTool( ) {}

  StatusCode initialize();

  /// Implementation of the IFlavourTaggingTool Interface
  void tagThisB( const Particle &theB,
                 const Particles &theEvent,
                 const Vertex &thePrimVtx,
                 FlavourTag &theTag );
  void tagFromList( const Particle &theB,
                    const ParticleVector &theCandidates,
                    const Vertex &thePrimVtx,
                    FlavourTag &theTag );
  void tagExcludingFromList( const Particle &theB,
                             const Particles &theEvent,
                             const ParticleVector &theExcluded,
                             const Vertex &thePrimVtx,
                             FlavourTag &theTag );

private:
  void FlattenTree( Particle *part, ParticleVector &flat );

  /// Properties
  std::string m_FilterName;
  std::string m_GeomDispName; //< Name of the geometrical displacment tool
  std::vector<std::string> m_PrimVerticesLocations;
  bool m_InverseCharge; //< Does a + charge give a bbar tag or a b tag ?
  bool m_AllVertices;   //< Loop on all prim vertices instead of the given one.
  double m_ISMin;       //< Minimal impact parameter significance
  double m_ISMax;       //< Maximal impact parameter significance

  IParticleFilter *m_Filter;
  IGeomDispCalculator *m_GeomDisp;
  IDataProviderSvc *m_EventSvc;
};
#endif // SINGLEPARTICLETAGGINGTOOL_H
