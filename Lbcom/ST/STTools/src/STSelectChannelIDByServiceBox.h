// $Id: STSelectChannelIDByServiceBox.h,v 1.1 2009-03-14 09:16:35 mneedham Exp $
#ifndef STSelectChannelIDByServiceBox_H
#define STSelectChannelIDByServiceBox_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTChannelIDSelector.h"

/** @class STSelectChannelIDByServiceBox STSelectChannelIDByServiceBox.h
 *
 *  Tool for selecting clusters using a list of service boxes
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

namespace LHCb{
  class STChannelID;
}

class STSelectChannelIDByServiceBox: public ST::ToolBase, 
                         virtual public ISTChannelIDSelector {

 public: 
   
  /// constructer
  STSelectChannelIDByServiceBox( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectChannelIDByServiceBox();

  /// initialize
  virtual StatusCode initialize();

  /**  @param  cluster pointer to ST cluster object to be selected 
  *  @return true if cluster is selected
  */
  virtual bool select     ( const LHCb::STChannelID& id ) const ;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to ST cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const LHCb::STChannelID& id ) const  ;

 private:

  ///   default  constructor  is  private 
  STSelectChannelIDByServiceBox();
  ///   copy     constructor  is  private 
  STSelectChannelIDByServiceBox (const STSelectChannelIDByServiceBox& );
  ///   assignement operator  is  private 
  STSelectChannelIDByServiceBox& operator= (const STSelectChannelIDByServiceBox& );  

  std::vector<std::string> m_serviceBoxes;
  std::vector<unsigned int> m_sectors;

};

#endif // STSelectChannelIDByServiceBox_H
