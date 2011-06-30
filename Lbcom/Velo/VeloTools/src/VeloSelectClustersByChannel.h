// $Id: $
#ifndef VELOSELECTCLUSTERSBYCHANNEL_H 
#define VELOSELECTCLUSTERSBYCHANNEL_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Kernel/LHCbKernel
#include "Kernel/IVeloClusterSelector.h"            // Interface

// Forward declarations
class IVeloChannelIDSelector;
namespace LHCb {
  class VeloChannelID;
}


/** @class VeloSelectClustersByChannel VeloSelectClustersByChannel.h
 *  
 *  Tool for selecting clusters using a list of service boxes
 *  (copied from STSelectClustersByChannel)
 *
 *  @author Eduardo Rodrigues
 *  @date   2011-06-28
 */

class VeloSelectClustersByChannel : public GaudiTool,
                                    virtual public IVeloClusterSelector
{
public: 
  /// Standard constructor
  VeloSelectClustersByChannel( const std::string& type, 
                               const std::string& name,
                               const IInterface* parent);
  
  virtual ~VeloSelectClustersByChannel( ); ///< Destructor
 
  /// Initialize
  virtual StatusCode initialize();
  
  /** "select"/"preselect" method 
   *  @param  cluster pointer to object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select( const LHCb::VeloCluster* cluster ) const ;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const LHCb::VeloCluster* cluster ) const ;
  
protected:

private:
  /// Default constructor is private 
  VeloSelectClustersByChannel();
  /// Copy constructor is private 
  VeloSelectClustersByChannel( const VeloSelectClustersByChannel& );
  /// Assignement operator is private 
  VeloSelectClustersByChannel& operator=( const VeloSelectClustersByChannel& );
  
private:  
  IVeloChannelIDSelector* m_selector;
  
  std::string m_selectorType;
  std::string m_selectorName;
  
};
#endif // VELOSELECTCLUSTERSBYCHANNEL_H
