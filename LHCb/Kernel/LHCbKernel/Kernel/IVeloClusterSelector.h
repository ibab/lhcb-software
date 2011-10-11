#ifndef IVELOCLUSTERSELECTOR_H 
#define IVELOCLUSTERSELECTOR_H 1

// Include files
// -------------
// From STL
#include <string>

// From Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb {
  class VeloCluster;
}


static const InterfaceID IID_IVeloClusterSelector ( "IVeloClusterSelector", 1, 0 );

/** @class IVeloClusterSelector IVeloClusterSelector.h Kernel/IVeloClusterSelector.h
 *  
 *  Abstract interface for Velo cluster selection tools
 *  which allow a fast and efficient selection/preselection of clusters
 *  (simply copied from the ST implementation)
 *
 *  @author Eduardo Rodrigues
 *  @date   2011-06-28
 *  ()
 */
class IVeloClusterSelector : virtual public IAlgTool,
                             public std::unary_function<const LHCb::VeloCluster*,bool>
{
public: 
  
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IVeloClusterSelector; }

  /** "select"/"preselect" method 
   *  @param  cluster pointer to object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select( const LHCb::VeloCluster* cluster ) const = 0 ;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const LHCb::VeloCluster* cluster ) const = 0 ;
  
protected:
  
private:
  
};
#endif // IVELOCLUSTERSELECTOR_H
