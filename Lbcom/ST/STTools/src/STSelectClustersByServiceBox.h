// $Id: STSelectClustersByServiceBox.h,v 1.1 2009-02-10 09:18:38 mneedham Exp $
#ifndef STSelectClustersByServiceBox_H
#define STSelectClustersByServiceBox_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTClusterSelector.h"

/** @class STSelectClustersByServiceBox STSelectClustersByServiceBox.h
 *
 *  Tool for selecting clusters using a list of service boxes
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

namespace LHCb{
  class STChannelID;
}

class STSelectClustersByServiceBox: public ST::ToolBase, 
                         virtual public ISTClusterSelector {

 public: 
   
  /// constructer
  STSelectClustersByServiceBox( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectClustersByServiceBox();

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
  STSelectClustersByServiceBox();
  ///   copy     constructor  is  private 
  STSelectClustersByServiceBox (const STSelectClustersByServiceBox& );
  ///   assignement operator  is  private 
  STSelectClustersByServiceBox& operator= (const STSelectClustersByServiceBox& );  

  std::vector<std::string> m_serviceBoxes;
  std::vector<unsigned int> m_sectors;

};

#endif // STSelectClustersByServiceBox_H
