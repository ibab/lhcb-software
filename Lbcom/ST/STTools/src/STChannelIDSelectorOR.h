// $Id: STChannelIDSelectorOR.h,v 1.1 2009-03-14 09:16:34 mneedham Exp $
#ifndef STTTOOLS_STCHANNELIDSELECTOR_H 
#define STTTOOLS_STCHANNELIDSELECTOR_H 1
// Include files
// from STL
#include <string>
// from GaudiAlg
#include "GaudiAlg/GaudiTool.h"
// From CaloInterfaces
#include "Kernel/ISTChannelIDSelector.h"

/** @class STChannelIDSelectorOR STChannelIDSelectorOR.h
 *   
 *  Helper concrete tool for selection of STChannelID objects 
 *  This selector selects the cluster if 
 *  at least one  of its daughter selector select it!
 *
 *  @author M Needham
 *  @date   03/02/2009
 */
class STChannelIDSelectorOR : 
  public virtual ISTChannelIDSelector ,
  public          GaudiTool 
{
public:
  /// container of types&names 
  typedef std::vector<std::string> Names     ;
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
  STChannelIDSelectorOR
  ( const std::string& type, 
    const std::string& name,
    const IInterface* parent);
  
  /** "select"/"preselect" method 
   *  @see ISTChannelIDSelector
   *  @param  cluster pointer to ST cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool select     
  ( const LHCb::STChannelID& id ) const  ;

  /** "select"/"preselect" method (functor interface)
   *  @see ISTChannelIDSelector
   *  @param  cluster pointer to ST cluster object to be selected 
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
  
  /// destructor (virtual OR protected)
  virtual ~STChannelIDSelectorOR() ; 
  
private:

  ///   default  constructor  is  private 
  STChannelIDSelectorOR();
  ///   copy     constructor  is  private 
  STChannelIDSelectorOR
  (const STChannelIDSelectorOR& );
  ///   assignement operator  is  private 
  STChannelIDSelectorOR& operator=
  (const STChannelIDSelectorOR& );  
  
private:
  
  Names     m_selectorsTypeNames ;
  Selectors m_selectors          ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // STTTOOLS_STCLUSTERSELECTOROR_H
// ============================================================================
