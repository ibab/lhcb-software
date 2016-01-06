// $Id: PrintMCDecayTreeTool.h,v 1.3 2009-01-08 09:44:37 cattanem Exp $
#ifndef PRINTMCDECAYTREETOOL_H 
#define PRINTMCDECAYTREETOOL_H 1

// Include files
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from LHCb
#include "MCInterfaces/IPrintMCDecayTreeTool.h"

// Forward declarations
namespace LHCb {
  class IParticlePropertySvc;
}

class MsgStream;

/** @class PrintMCDecayTreeTool PrintMCDecayTreeTool.h
 *  This is an implementation of the decay tree printing tool,
 *  defined in interface IPrintMCDecayTreeTool
 *
 *  @author Olivier Dormond
 *  @date   29/03/2001
 */
class PrintMCDecayTreeTool : public GaudiTool,
                           virtual public IPrintMCDecayTreeTool {
public:  
  /// Standard Constructor
  PrintMCDecayTreeTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );

  /// Destructor 
  virtual ~PrintMCDecayTreeTool( ){}; ///< Destructor

  StatusCode initialize( void );

  virtual void printTree( const LHCb::MCParticle* mother, 
                          int maxDepth = -1);


  /// Print all the MC particles leading to this one.
  virtual void printAncestor( const LHCb::MCParticle *child );

  virtual void printAsTree( const LHCb::MCParticle::ConstVector& event );

  virtual void printAsTree( const LHCb::MCParticles &event );

  virtual void printAsList( const LHCb::MCParticle::ConstVector &event );

  virtual void printAsList( const LHCb::MCParticles &event );

private:

  enum InfoKeys { Name, PID, E, M, P, Pt, Px, Py, Pz, Vx, Vy, Vz,
                  theta, phi, eta, flags, fromSignal, idcl, };

  void printHeader( MsgStream &log );

  void printInfo( const std::string& prefix, 
                  const LHCb::MCParticle* part,
                  MsgStream& log );
  void printDecayTree( const LHCb::MCParticle *mother,
                       const std::string &prefix, 
                       int depth, 
                       MsgStream &log );

  LHCb::IParticlePropertySvc* m_ppSvc; ///< Pointer to particle property service
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
#endif // PRINTMDECAYTREETOOL_H
