// $Id: $
#ifndef STRndmEffSelector_H
#define STRndmEffSelector_H 1

// ST tool base class
#include "Kernel/STToolBase.h"
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IRndmGen.h"

// LHCbKernel
#include "Kernel/ISTChannelIDSelector.h"

/** @class STRndmEffSelector STRndmEffSelector.h
 *
 *  Tool for selecting clusters at random using the measured eff
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

class STRndmEffSelector: public ST::ToolBase, 
                         virtual public ISTChannelIDSelector {
 public: 
   
  /// constructer
  STRndmEffSelector( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  ~STRndmEffSelector() override = default;

  /** intialize */
  StatusCode initialize() override;

  /**  @param  cluster pointer to calo cluster object to be selected 
  *  @return true if cluster is selected
  */
  bool select     ( const LHCb::STChannelID& id ) const override;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to calo cluster object to be selected 
   *  @return true if cluster is selected
   */
  bool operator() ( const LHCb::STChannelID& id ) const  override;


  ///   default  constructor  is  private 
  STRndmEffSelector() = delete;
  ///   copy     constructor  is  private 
  STRndmEffSelector (const STRndmEffSelector& ) = delete;
  ///   assignement operator  is  private 
  STRndmEffSelector& operator=(const STRndmEffSelector& ) = delete;

 private:
  // smart interface to generator
  SmartIF<IRndmGen> m_uniformDist; 

 
};

#endif // STRndmEffSelector_H
