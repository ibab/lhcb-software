// $Id: STChannelIDSelectorAND.h,v 1.1 2009-03-14 09:16:34 mneedham Exp $
#ifndef STCHANNELIDSELECTORAND_H 
#define STCHANNELIDSELECTORAND_H 1
// Include files
// from STL
#include <string>
// from GaudiAlg
#include "GaudiAlg/GaudiTool.h"
// From CaloInterfaces
#include "Kernel/ISTChannelIDSelector.h"

/** @class STChannelIDSelectorAND STChannelIDSelectorAND.h
 *   
 *  Helper concrete tool for selection of stcluster objects
 *  This selector selects the cluster if 
 *  all of its daughter selector select it!
 *
 *  @author M Needhams
 *  @date   03/02/2009
 */
class STChannelIDSelectorAND : 
  public virtual ISTChannelIDSelector ,
  public          GaudiTool 
{
public:
  /// container of types&names 
  typedef std::vector<std::string>           Names     ;
  /// container of selectors 
  typedef std::vector<ISTChannelIDSelector*> Selectors ;

public:

  /** Standard constructor
   *  @see GaudiTool
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @param type   tool type (?)
   *  @param name   tool name 
   *  @param parent tool parent   
   */
  STChannelIDSelectorAND
  ( const std::string& type, 
    const std::string& name,
    const IInterface* parent);
  
  /** "select"/"preselect" method 
   *  @see ISTChannelIDSelector
   *  @param  cluster pointer to st cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select     
  ( const LHCb::STChannelID& id ) const  ;

  /** "select"/"preselect" method (functor interface)
   *  @see ICaloSTChannelIDSelector
   *  @param  cluster pointer to st cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator () 
    ( const LHCb::STChannelID& id ) const  ;
  
  /** standard initialization of the tool 
   *  @see IAlgTool 
   *  @see AlgTool 
   *  @see GaudiTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ;
  
protected:
  
  /// destructor (virtual and protected)
  virtual ~STChannelIDSelectorAND() ; 
  
private:

  ///   default  constructor  is  private 
  STChannelIDSelectorAND();
  ///   copy     constructor  is  private 
  STChannelIDSelectorAND
  (const STChannelIDSelectorAND& );
  ///   assignement operator  is  private 
  STChannelIDSelectorAND& operator=
  (const STChannelIDSelectorAND& );  
  
private:
  
  Names     m_selectorsTypeNames ;
  Selectors m_selectors          ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // STTTOOLS_STCLUSTERSELECTORAND_H
// ============================================================================
