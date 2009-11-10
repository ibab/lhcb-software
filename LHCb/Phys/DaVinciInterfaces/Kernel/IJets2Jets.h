// $Id: IJets2Jets.h,v 1.1 2009-11-10 12:54:14 cocov Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ============================================================================
#ifndef DAVINCIKERNEL_IJETS2JETS_H 
#define DAVINCIKERNEL_IJETS2JETS_H 1

// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// forward decalrations 
// ============================================================================
#include "Event/Particle.h"
#include "Relations/RelationWeighted1D.h"
// ============================================================================
/** @class IJets2Jets IJets2Jets.h Kernel/IJets2Jets.h
 *
 *  The abstract interface for tools that match a set of jets to another
 *  It returns a relation table between both sets, links being weighted
 *  by the fraction of energy of the jet1 contained in jet2 at the level
 *  of their constituents.
 *
 *  @author Victor Coco Victor.Coco@cern.ch
 *  @date   2009-10-30
 */

class GAUDI_API IJets2Jets : public virtual IAlgTool 
{
public: 
  // ==========================================================================
  /// interface machinery 
  DeclareInterfaceID(IJets2Jets, 1, 0);
  // ==========================================================================
public:
  // ==========================================================================
  /// the actual type of  container of jets 
  typedef LHCb::Particle::Vector        Jets   ;  
  // ==========================================================================
  /// the actual type of relation table between jets
  typedef LHCb::RelationWeighted1D< LHCb::Particle,LHCb::Particle, double >  Table ;

  /// the main method: making relation weighted between two sets of jets
  virtual const Table makeRelation( const  Jets & jets1 , const  Jets & jets2) const = 0;
};
// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IJETMAKER_H
// ============================================================================

