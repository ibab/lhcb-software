// $Id: STSelectChannelIDByStatus.h,v 1.1 2009-03-14 09:16:35 mneedham Exp $
#ifndef STSelectChannelIDByStatus_H
#define STSelectChannelIDByStatus_H 1

#include <string>
#include <vector>

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTChannelIDSelector.h"

#include "STDet/DeSTSector.h"

/** @class STSelectChannelIDByStatus STSelectChannelIDByStatus.h
 *
 *  Tool for selecting clusters using the conditions
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

class STSelectChannelIDByStatus: public ST::ToolBase, 
                         virtual public ISTChannelIDSelector {

 public: 
   
  /// constructer
  STSelectChannelIDByStatus( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectChannelIDByStatus();

  /// intialize
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

  std::vector<std::string> m_statusNames;
  std::vector<DeSTSector::Status> m_statusList;

  ///   default  constructor  is  private 
  STSelectChannelIDByStatus();

  ///   copy     constructor  is  private 
  STSelectChannelIDByStatus(const STSelectChannelIDByStatus& );

  ///   assignement operator  is  private 
  STSelectChannelIDByStatus& operator= (const STSelectChannelIDByStatus& );  
 
};

#endif // STSelectChannelIDByStatus_H
