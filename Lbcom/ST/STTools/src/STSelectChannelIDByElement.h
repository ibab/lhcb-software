// $Id: STSelectChannelIDByElement.h,v 1.1 2009-03-14 09:16:35 mneedham Exp $
#ifndef STSelectChannelIDByElement_H
#define STSelectChannelIDByElement_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTChannelIDSelector.h"

/** @class STSelectChannelIDByElement STSelectChannelIDByElement.h
 *
 *  Tool for selecting clusters using the conditions
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

class DeSTBaseElement;

class STSelectChannelIDByElement: public ST::ToolBase, 
                         virtual public ISTChannelIDSelector {

 public: 
   
  /// constructer
  STSelectChannelIDByElement( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectChannelIDByElement();


  /// intialize 
  StatusCode initialize();

  /**  @param  cluster pointer to ST cluster object to be selected 
  *  @return true if cluster is selected
  */
  virtual bool select     ( const LHCb::STChannelID& id ) const ;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to ST cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const LHCb::STChannelID& id ) const  ;


private:

  ///   default  constructor  is  private 
  STSelectChannelIDByElement();

  ///   copy     constructor  is  private 
  STSelectChannelIDByElement(const STSelectChannelIDByElement& );

  ///   assignement operator  is  private 
  STSelectChannelIDByElement& operator= (const STSelectChannelIDByElement& );  
 
  std::vector<DeSTBaseElement*> m_detElements;

  std::vector<std::string> m_elementNames;

};

#endif // STSelectChannelIDByElement_H
