// $Id: DebugTool.h,v 1.9 2006-10-11 14:19:19 jpalac Exp $
#ifndef DEBUGTOOL_H 
#define DEBUGTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "Kernel/IDebugTool.h"


// Forward declarations
class IParticlePropertySvc;
class MsgStream;

/** @class DebugTool DebugTool.h
 *  This is an implementation for the Debugging Tool.
 *
 *  @author Olivier Dormond
 *  @date   29/03/2001
 */
class DebugTool : public GaudiTool,
                  virtual public IDebugTool {
public:  
  /// Standard Constructor
  DebugTool( const std::string& type,
             const std::string& name,
             const IInterface* parent );

  /// Destructor 
  virtual ~DebugTool( ){}; ///< Destructor

  StatusCode initialize( void );

  /// Implementation of IDebugTool interface.
  /// In the following function a maxDepth is provided with a default value
  /// of -1. That way the tool "PrintDepth" property is used.
  /// Print decay tree for a given MC particle
  void printTree( const LHCb::MCParticle* mother, int maxDepth );

  /// Print decay tree for a given particle
  void printTree( const LHCb::Particle* mother, int maxDepth );

  /// Print side by side decay tree for a given MC particle and it's
  /// reconstructed items
  void printTree(const LHCb::MCParticle* mother, 
                 Particle2MCLinker* assoc, int maxDepth );

  /// Print side by side decay tree for a given particle and it's
  /// associated MC particle.
  void printTree( const LHCb::Particle* mother, 
                  Particle2MCLinker* assoc, int maxDepth );

  /// Print all the MC particles leading to this one.
  void printAncestor( const LHCb::MCParticle *child );

  /// Print all the (MC)Particles in event as a tree.
  /// Top level particles are those without parent.
  virtual void printEventAsTree( const LHCb::MCParticle::ConstVector &event );
  /// Also print the reconstructed info if available.
  virtual void printEventAsTree( const LHCb::MCParticle::ConstVector &event,
                                 Particle2MCLinker* assoc );
  /// Same from a keyed container.
  virtual void printEventAsTree( const LHCb::MCParticles &event );
  virtual void printEventAsTree( const LHCb::MCParticles &event,
                                 Particle2MCLinker* assoc );

  /// Print all the (MC)Particles in the event as a flat list.
  virtual void printEventAsList( const LHCb::Particle::ConstVector &event );
  virtual void printEventAsList( const LHCb::MCParticle::ConstVector &event );
  /// Also print the truth/reconstructed info if available.
  virtual void printEventAsList( const LHCb::Particle::ConstVector &event,
                                 Particle2MCLinker* assoc );
  virtual void printEventAsList( const LHCb::MCParticle::ConstVector &event,
                                 Particle2MCLinker* assoc );
  /// Same from a keyed container.
  virtual void printEventAsList( const LHCb::Particles &event );
  virtual void printEventAsList( const LHCb::MCParticles &event );
  virtual void printEventAsList( const LHCb::Particles &event,
                                 Particle2MCLinker* assoc );
  virtual void printEventAsList( const LHCb::MCParticles &event,
                                 Particle2MCLinker* assoc );

private:
  enum InfoKeys { Name, E, M, P, Pt, Px, Py, Pz, Vx, Vy, Vz,
                  theta, phi, eta, idcl };

  void printHeader( MsgStream &log, bool mcfirst, bool associated );
  void printInfo( const std::string &prefix, const LHCb::MCParticle *part,
                  Particle2MCLinker* assoc, MsgStream &log );
  void printInfo( const std::string &prefix, const LHCb::Particle *part,
                  Particle2MCLinker* assoc, MsgStream &log );
  void printDecayTree( const LHCb::MCParticle *mother, Particle2MCLinker* assoc,
                       const std::string &prefix, int depth, MsgStream &log );
  void printDecayTree( const LHCb::Particle *mother, Particle2MCLinker* assoc,
                       const std::string &prefix, int depth, MsgStream &log );
  
  
  IParticlePropertySvc* m_ppSvc; ///< Reference to particle property service
  int m_depth;         ///< Depth of printing for tree
  int m_treeWidth;     ///< width of the tree drawing
  int m_fWidth;        ///< width of the data fields
  int m_fPrecision;    ///< precision of the data fields
  std::string m_arrow; ///< arrow drawing
  std::string m_informationsDeprecated; ///< For backward compatibility. Will be removed
  std::string m_information; ///< The specification of the values to print
  std::vector<InfoKeys> m_keys; ///< The list of information to print
  double m_energyUnit ; /// Unit for energies, momenta and masses
  double m_lengthUnit ; /// Unit for distances
  std::string m_energyUnitName; ///< Unit for energies, momenta and masses
  std::string m_lengthUnitName; ///< Unit for distances
   
};
#endif // DEBUGTOOL_H
