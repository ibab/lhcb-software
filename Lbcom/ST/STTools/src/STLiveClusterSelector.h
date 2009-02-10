// $Id: STLiveClusterSelector.h,v 1.1 2009-02-10 09:18:38 mneedham Exp $
#ifndef STLiveClusterSelector_H
#define STLiveClusterSelector_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTClusterSelector.h"

/** @class STLiveClusterSelector STLiveClusterSelector.h
 *
 *  Tool for selecting clusters using the conditions
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

class STLiveClusterSelector: public ST::ToolBase, 
                         virtual public ISTClusterSelector {

 public: 
   
  /// constructer
  STLiveClusterSelector( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STLiveClusterSelector();

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
  STLiveClusterSelector();

  ///   copy     constructor  is  private 
  STLiveClusterSelector(const STLiveClusterSelector& );

  ///   assignement operator  is  private 
  STLiveClusterSelector& operator= (const STLiveClusterSelector& );  
 
};

#endif // STLiveClusterSelector_H
