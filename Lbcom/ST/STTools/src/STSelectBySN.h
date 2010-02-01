// $Id: STSelectBySN.h,v 1.1 2010-02-01 07:53:57 mneedham Exp $
#ifndef STSelectBySN_H
#define STSelectBySN_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTClusterSelector.h"

/** @class STSelectBySN STSelectBySN.h
 *
 *  Tool for selecting clusters by charge
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

namespace LHCb{
  class STChannelID;
}

class STSelectBySN: public ST::ToolBase, 
                         virtual public ISTClusterSelector {

 public: 
   
  /// constructer
  STSelectBySN( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectBySN();

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
  STSelectBySN();
  ///   copy     constructor  is  private 
  STSelectBySN (const STSelectBySN& );
  ///   assignement operator  is  private 
  STSelectBySN& operator= (const STSelectBySN& );  

  double m_minSN;
  double m_maxSN;


};

#endif // STSelectBySN_H
