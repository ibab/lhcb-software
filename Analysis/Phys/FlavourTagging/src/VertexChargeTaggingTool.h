// $Id: VertexChargeTaggingTool.h,v 1.1 2003-06-16 07:10:39 odie Exp $
#ifndef VERTEXCHARGETAGGINGTOOL_H 
#define VERTEXCHARGETAGGINGTOOL_H 1

// Include files
#include <functional>

#include "GaudiKernel/AlgTool.h"
#include "Event/Particle.h"
#include "FlavourTagging/IFlavourTaggingTool.h"

// Forward declarations
class FlavourTag;
class IParticleFilter;
class IGeomDispCalculator;
class IVertexFitter;
class IDataProviderSvc;

/** @class VertexChargeTaggingTool VertexChargeTaggingTool.h
 *  
 *  Try to figure out the b/bbar flavour of the given particle.
 *
 *  @author Olivier Dormond
 *  @date   14/03/2002
 */
class VertexChargeTaggingTool : public AlgTool,
                                  virtual public IFlavourTaggingTool {
public:
  /// Standard constructor
  VertexChargeTaggingTool( const std::string &type,
                             const std::string &name,
                             const IInterface *parent ); 

  /// Standard destructor
  ~VertexChargeTaggingTool( ) {}

  StatusCode initialize();

  /// Implementation of the IFlavourTaggingTool Interface
  void tagThisB( const Particle &theB,
                 const Particles &theEvent,
                 const Vertex &thePrimVtx,
                 FlavourTag &theTag );
  void tagThisB( const Particle &theB,
                 const ParticleVector &theEvent,
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
  void tagExcludingFromList( const Particle &theB,
                             const ParticleVector &theEvent,
                             const ParticleVector &theExcluded,
                             const Vertex &thePrimVtx,
                             FlavourTag &theTag );

private:
  void FlattenTree( Particle *part, ParticleVector &flat );

  /// Properties
  std::string m_GeomDispName; //< Name of the geometrical displacment tool
  std::string m_vtxFitterName; //< Name ot the vertex fitting tool
  std::vector<std::string> m_PrimVerticesLocations;
  bool m_InverseCharge; //< Does a + charge give a bbar tag or a b tag ?
  double m_minP;
  double m_minTheta;
  double m_minSIPPreCut;
  double m_maxSeedIPErr;
  double m_minSeedSIP;
  double m_maxSeedSIP;
  double m_maxSeedChi2NDoF;
  double m_minSeedPrimDist;
  double m_minKS0Mass;
  double m_maxKS0Mass;
  double m_minSeedLikelyhood;
  double m_maxIPErr;
  double m_minSIP;
  double m_maxSIP;
  double m_minLikelyhood;
  double m_maxChi2NDoF;
  double m_minPrimDist;
  double m_maxAddSIP;

  IVertexFitter *m_vtxFitter;
  IGeomDispCalculator *m_GeomDisp;
  IDataProviderSvc *m_EventSvc;
};
#endif // VERTEXCHARGETAGGINGTOOL_H
