// $Id: STSelectClustersByBoard.h,v 1.1 2009-02-10 09:18:38 mneedham Exp $
#ifndef STSelectClustersByBoard_H
#define STSelectClustersByBoard_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTClusterSelector.h"

/** @class STSelectClustersByBoard STSelectClustersByBoard.h
 *
 *  Tool for selecting clusters using a list of Tell1 boards
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

namespace LHCb{
  class STChannelID;
}

class STSelectClustersByBoard: public ST::ToolBase, 
                         virtual public ISTClusterSelector {

 public: 
   
  /// constructer
  STSelectClustersByBoard( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectClustersByBoard();

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
  STSelectClustersByBoard();
  ///   copy     constructor  is  private 
  STSelectClustersByBoard (const STSelectClustersByBoard& );
  ///   assignement operator  is  private 
  STSelectClustersByBoard& operator= (const STSelectClustersByBoard& );  

  std::vector<unsigned int> m_boards;

};

#endif // STSelectClustersByBoard_H
