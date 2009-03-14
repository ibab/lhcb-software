// $Id: STSelectByCharge.h,v 1.1 2009-03-14 09:16:35 mneedham Exp $
#ifndef STSelectByCharge_H
#define STSelectByCharge_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTClusterSelector.h"

/** @class STSelectByCharge STSelectByCharge.h
 *
 *  Tool for selecting clusters by charge
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

namespace LHCb{
  class STChannelID;
}

class STSelectByCharge: public ST::ToolBase, 
                         virtual public ISTClusterSelector {

 public: 
   
  /// constructer
  STSelectByCharge( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectByCharge();


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
  STSelectByCharge();
  ///   copy     constructor  is  private 
  STSelectByCharge (const STSelectByCharge& );
  ///   assignement operator  is  private 
  STSelectByCharge& operator= (const STSelectByCharge& );  

  double m_minCharge;
  double m_maxCharge;


};

#endif // STSelectByCharge_H
