// $Id: STRndmClusterSelector.h,v 1.1 2009-02-10 09:18:38 mneedham Exp $
#ifndef STRndmClusterSelector_H
#define STRndmClusterSelector_H 1

// ST tool base class
#include "Kernel/STToolBase.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

// LHCbKernel
#include "Kernel/ISTClusterSelector.h"

/** @class STRndmClusterSelector STRndmClusterSelector.h
 *
 *  Tool for selecting clusters at random
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

class STRndmClusterSelector: public ST::ToolBase, 
                         virtual public ISTClusterSelector {

 public: 
   
  /// constructer
  STRndmClusterSelector( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STRndmClusterSelector();

  /** intialize */
  StatusCode initialize();

  /**  @param  cluster pointer to calo cluster object to be selected 
  *  @return true if cluster is selected
  */
  virtual bool select     ( const LHCb::STCluster* cluster ) const ;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const LHCb::STCluster* cluster ) const  ;

 private:

  ///   default  constructor  is  private 
  STRndmClusterSelector();

  ///   copy     constructor  is  private 
  STRndmClusterSelector (const STRndmClusterSelector& );

  ///   assignement operator  is  private 
  STRndmClusterSelector& operator=(const STRndmClusterSelector& );  

  // smart interface to generator
  SmartIF<IRndmGen> m_uniformDist; 

  double m_fractionToAccept; 
 
};

#endif // STRndmClusterSelector_H
