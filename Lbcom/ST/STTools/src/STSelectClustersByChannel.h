// $Id: STSelectClustersByChannel.h,v 1.1 2009-03-14 09:16:35 mneedham Exp $
#ifndef STSelectClustersByChannel_H
#define STSelectClustersByChannel_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTClusterSelector.h"

/** @class STSelectClustersByChannel STSelectClustersByChannel.h
 *
 *  Tool for selecting clusters using a list of service boxes
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

class ISTChannelIDSelector;

namespace LHCb{
  class STChannelID;
}

class STSelectClustersByChannel: public ST::ToolBase, 
                         virtual public ISTClusterSelector {

 public: 
   
  /// constructer
  STSelectClustersByChannel( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectClustersByChannel();

  /// initialize
  virtual StatusCode initialize();

  /**  @param  cluster pointer to ST cluster object to be selected 
  *  @return true if cluster is selected
  */
  virtual bool select     ( const LHCb::STCluster* cluster ) const ;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to ST cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const LHCb::STCluster* cluster ) const  ;

 private:

  ///   default  constructor  is  private 
  STSelectClustersByChannel();
  ///   copy     constructor  is  private 
  STSelectClustersByChannel (const STSelectClustersByChannel& );
  ///   assignement operator  is  private 
  STSelectClustersByChannel& operator= (const STSelectClustersByChannel& );  

  ISTChannelIDSelector* m_selector;
  std::string m_selectorType;
  std::string m_selectorName;
};

#endif // STSelectClustersByChannel_H
