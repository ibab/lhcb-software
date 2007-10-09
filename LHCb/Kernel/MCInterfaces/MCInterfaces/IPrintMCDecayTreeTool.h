// $Id: IPrintMCDecayTreeTool.h,v 1.2 2007-10-09 16:28:54 jpalac Exp $
#ifndef MCINTERFACES_IMCPRINTDECAYTREETOOL_H
#define MCINTERFACES_IMCPRINTDECAYTREETOOL_H 1

// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/MCParticle.h"

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IPrintMCDecayTreeTool("IPrintMCDecayTreeTool", 1, 0);

/** @class IPrintMCDecayTreeTool IPrintMCDecayTreeTool.h DaVinciMCTools/IPrintMCDecayTreeTool.h
 *  
 *
 *  Interface for the printing of decay trees of MCParticles.
 *  Taken from the original IDebugTool from Olivier Dormond.
 *  
 *  @todo figure out exactly what each method is supposed to do 
 *        and clarify the doxygen!!!!
 *  @author Juan Palacios juancho@nikhef.nl
 *  @date   09/10/2007
 */
class IPrintMCDecayTreeTool : virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IPrintMCDecayTreeTool; }

  /// Print decay tree for a given MC particle up to a depth maxDepth.
  /// Default value of -1 is what exactly???
  virtual void printTree( const LHCb::MCParticle* mother, 
                          int maxDepth = -1 ) = 0;

  /// Print all the MC particles leading to this one.
  virtual void printAncestor( const LHCb::MCParticle* child ) = 0;

  /// Print all the MCParticles in a vector as a tree.
  /// Top level particles are those without parent.
  virtual void printAsTree( const LHCb::MCParticle::ConstVector& event ) = 0;
  /// Print all the MCParticles in a keyed container as a tree.
  /// Top level particles are those without parent.
  virtual void printAsTree( const LHCb::MCParticles& event ) = 0;

  /// Print all the MCParticles in a vector as a flat list.
  /// Top level particles are those without parent.
  virtual void printAsList( const LHCb::MCParticle::ConstVector& event ) = 0;

  /// Print all the MCParticles in a keyed container as a flat list.
  /// Top level particles are those without parent.
  virtual void printAsList( const LHCb::MCParticles& event ) = 0;

};
#endif // MCINTERFACES_IPRINTDECAYTREETOOL_H
