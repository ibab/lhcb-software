// $Id: PrintDecayTreeTool.h,v 1.3 2008-04-09 20:24:28 ibelyaev Exp $
#ifndef PRINTDECAYTREETOOL_H
#define PRINTDECAYTREETOOL_H 1

#include <set>
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "Kernel/IPrintDecayTreeTool.h"
#include "Kernel/IPrintDecay.h"

#include "boost/format.hpp"
#include "boost/lexical_cast.hpp"

// Forward declarations
namespace LHCb
{
  class IParticlePropertySvc;
}

class MsgStream;

/** @class PrintDecayTreeTool PrintDecayTreeTool.h
 *
 *  This is an implementation of the  IPrintDecayTreeTool.
 *  It is based on Olivier Dormond's DebugTool.
 *
 *  In the following function a maxDepth is provided with a default value
 *  of -1. That way the tool "PrintDepth" property is used.
 *  @todo write documentation stating what the options do.
 *
 *  @author Olivier Dormond
 *  @author Juan Palacios juancho@nikhef.nl
 *  @date   10/10/2007
 */
class PrintDecayTreeTool
  : public GaudiTool
  , virtual public IPrintDecay
  , virtual public IPrintDecayTreeTool
{
public:
  /// Standard Constructor
  PrintDecayTreeTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Destructor
  virtual ~PrintDecayTreeTool( ){}; ///< Destructor

  StatusCode initialize( void );

  /**
   *   Print decay tree for a given particle.
   *   Here, a maxDepth is provided with a default value
   *   of -1. The default uses the value set by the
   *   "PrintDepth" property.
   */
  virtual void printTree( const LHCb::Particle* mother,
                          int maxDepth = -1 );

  virtual void printTree(const LHCb::MCParticle* mother,
                         Particle2MCLinker* assoc,
                         int maxDepth =-1 );

  virtual void printTree( const LHCb::Particle* mother,
                          Particle2MCLinker* assoc,
                          int maxDepth =-1 );

  virtual void printAsTree( const LHCb::MCParticle::ConstVector& particles,
                            Particle2MCLinker* assoc );

  virtual void printAsTree( const LHCb::MCParticles& particles,
                            Particle2MCLinker* assoc );

  virtual void printAsList( const LHCb::Particle::ConstVector& particles );

  virtual void printAsList( const LHCb::Particle::ConstVector& particles,
                            Particle2MCLinker* assoc );

  virtual void printAsList( const LHCb::MCParticle::ConstVector& particles,
                            Particle2MCLinker* assoc );

  virtual void printAsList( const LHCb::Particles& particles );

  virtual void printAsList( const LHCb::Particles& particles,
                            Particle2MCLinker* assoc );

  virtual void printAsList( const LHCb::MCParticles& particles,
                            Particle2MCLinker* assoc );

private:

  enum InfoKeys { Name, E, M, P, Pt, Px, Py, Pz, Vx, Vy, Vz,
                  theta, phi, eta, idcl, chi2, PK, PPK };

private:

  void printHeader( MsgStream& log,
                    bool mcfirst,
                    bool associated );

  void printInfo( const std::string& prefix,
                  const LHCb::MCParticle *part,
                  Particle2MCLinker* assoc,
                  MsgStream& log );

  void printInfo( const std::string& prefix,
                  const LHCb::Particle* part,
                  Particle2MCLinker* assoc,
                  MsgStream &log );

  void printDecayTree( const LHCb::MCParticle* mother,
                       Particle2MCLinker* assoc,
                       const std::string& prefix,
                       int depth,
                       MsgStream& log );

  void printDecayTree( const LHCb::Particle* mother,
                       Particle2MCLinker* assoc,
                       const std::string& prefix,
                       int depth,
                       MsgStream &log );

  void printUsedContainers( MsgStream &log );

  /// Get TES location for an object
  template<class TYPE>
  std::string tesLocation( const TYPE * obj ) const
  {
    return ( obj && obj->parent() && obj->parent()->registry() ?
             obj->parent()->registry()->identifier() : "NotInTES" );
  }

  unsigned int tesCode( const std::string& loc )
  {
    if ( m_tesLocs.find(loc) == m_tesLocs.end() )
    {
      m_tesLocs[loc] = m_lastTESCode++;
    }
    return m_tesLocs[loc];
  }

private:

  LHCb::IParticlePropertySvc* m_ppSvc; ///< Reference to particle property service
  int m_depth;         ///< Depth of printing for tree
  int m_treeWidth;     ///< width of the tree drawing
  int m_fWidth;        ///< width of the data fields
  int m_fPrecision;    ///< precision of the data fields
  std::string m_arrow; ///< arrow drawing
  std::string m_information; ///< The specification of the values to print
  std::vector<InfoKeys> m_keys; ///< The list of information to print
  double m_energyUnit ; /// Unit for energies, momenta and masses
  double m_lengthUnit ; /// Unit for distances
  std::string m_energyUnitName; ///< Unit for energies, momenta and masses
  std::string m_lengthUnitName; ///< Unit for distances

  std::map<std::string,unsigned int> m_tesLocs;
  unsigned int m_lastTESCode;
  std::set<std::string> m_usedTesLocs;

};

#endif // DEBUGTOOL_H
