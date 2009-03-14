// $Id: STRndmChannelIDSelector.h,v 1.1 2009-03-14 09:16:34 mneedham Exp $
#ifndef STRndmChannelIDSelector_H
#define STRndmChannelIDSelector_H 1

// ST tool base class
#include "Kernel/STToolBase.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

// LHCbKernel
#include "Kernel/ISTChannelIDSelector.h"

/** @class STRndmChannelIDSelector STRndmChannelIDSelector.h
 *
 *  Tool for selecting clusters at random
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

class STRndmChannelIDSelector: public ST::ToolBase, 
                         virtual public ISTChannelIDSelector {

 public: 
   
  /// constructer
  STRndmChannelIDSelector( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STRndmChannelIDSelector();

  /** intialize */
  StatusCode initialize();

  /**  @param  cluster pointer to calo cluster object to be selected 
  *  @return true if cluster is selected
  */
  virtual bool select     ( const LHCb::STChannelID& id ) const ;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const LHCb::STChannelID& id ) const  ;

 private:

  ///   default  constructor  is  private 
  STRndmChannelIDSelector();

  ///   copy     constructor  is  private 
  STRndmChannelIDSelector (const STRndmChannelIDSelector& );

  ///   assignement operator  is  private 
  STRndmChannelIDSelector& operator=(const STRndmChannelIDSelector& );  

  // smart interface to generator
  SmartIF<IRndmGen> m_uniformDist; 

  double m_fractionToReject; 
 
};

#endif // STRndmChannelIDSelector_H
