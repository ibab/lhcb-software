// $Id: SingleParticleTaggingTool.h,v 1.4 2002-09-10 07:44:35 odie Exp $
#ifndef SINGLEPARTICLETAGGINGTOOL_H 
#define SINGLEPARTICLETAGGINGTOOL_H 1

// Include files
#include <functional>

#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTupleItems.h"
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

protected:
  NTuple::Item<long>   m_n_run, m_n_event;
  NTuple::Item<long>   m_n_cands, m_n_vtxs, m_i_selected, m_tag;
  NTuple::Array<float> m_px, m_py, m_pz, m_vx, m_vy, m_vz, m_id, m_cl;
  NTuple::Matrix<float> m_ip, m_iperr;

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
  INTupleSvc *m_NTupleSvc;
  std::string m_NTupleName;
  bool m_DoNTuple;
  std::string m_MonitorLocation;
  bool m_Monitor;
};
#endif // SINGLEPARTICLETAGGINGTOOL_H
