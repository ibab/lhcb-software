// $Id: STSelectChannelIDByBoard.h,v 1.1 2009-03-14 09:16:35 mneedham Exp $
#ifndef STSelectChannelIDByBoard_H
#define STSelectChannelIDByBoard_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTChannelIDSelector.h"

/** @class STSelectChannelIDByBoard STSelectChannelIDByBoard.h
 *
 *  Tool for selecting clusters using a list of service boxes
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

namespace LHCb{
  class STChannelID;
}

class STSelectChannelIDByBoard: public ST::ToolBase, 
                         virtual public ISTChannelIDSelector {

 public: 
   
  /// constructer
  STSelectChannelIDByBoard( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectChannelIDByBoard();

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
  STSelectChannelIDByBoard();
  ///   copy     constructor  is  private 
  STSelectChannelIDByBoard (const STSelectChannelIDByBoard& );
  ///   assignement operator  is  private 
  STSelectChannelIDByBoard& operator= (const STSelectChannelIDByBoard& );  

  std::vector<unsigned int> m_boards;
  std::vector<unsigned int> m_sectors;

};

#endif // STSelectChannelIDByBoard_H
