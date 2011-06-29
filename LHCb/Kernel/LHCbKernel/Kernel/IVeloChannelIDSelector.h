// $Id: $
#ifndef IVELOCHANNELIDSELECTOR_H 
#define IVELOCHANNELIDSELECTOR_H 1

// Include files
// -------------
// From STL
#include <string>

// From Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb {
  class VeloChannelID;
};


static const InterfaceID IID_IVeloChannelIDSelector ( "IVeloChannelIDSelector", 1, 0 );

/** @class IVeloChannelIDSelector IVeloChannelIDSelector.h Kernel/IVeloChannelIDSelector.h
 *  
 *  Abstract interface for Velo channelID selection tools
 *  which allow a fast and efficient selection/preselection of Velo channelIDs
 *  (simply copied from the ST implementation)
 *
 *  @author Eduardo Rodrigues
 *  @date   2011-06-28
 */
class IVeloChannelIDSelector : virtual public IAlgTool,
                               public std::unary_function<const LHCb::VeloChannelID,bool>
 {
 public: 
   
   // Return the interface ID
   static const InterfaceID& interfaceID() { return IID_IVeloChannelIDSelector; }
   
   /** "select"/"preselect" method 
    *  @param  channelID pointer to object to be selected 
    *  @return true if channelID is selected
    */
   virtual bool select( const LHCb::VeloChannelID& cluster ) const = 0 ;
   
   /** "select"/"preselect" method (functor interface)
    *  @param  channelID pointer to object to be selected 
    *  @return true if channelID is selected
    */
   virtual bool operator() ( const LHCb::VeloChannelID& cluster ) const = 0 ;
   
 protected:
   
 private:
   
};
#endif // IVELOCHANNELIDSELECTOR_H
