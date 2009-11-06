// $Id: STSelectLadders17.h,v 1.1 2009-11-06 12:29:29 mneedham Exp $
#ifndef STSelectLadders17_H
#define STSelectLadders17_H 1

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

class STSelectLadders17: public ST::ToolBase, 
                         virtual public ISTChannelIDSelector {

 public: 
   
  /// constructer
  STSelectLadders17( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectLadders17();


  /// intialize 
  //  StatusCode initialize();

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
  STSelectLadders17();

  ///   copy     constructor  is  private 
  STSelectLadders17(const STSelectLadders17& );

  ///   assignement operator  is  private 
  STSelectLadders17& operator= (const STSelectLadders17& );  
 
  //std::vector<DeSTBaseElement*> m_detElements;

  //std::vector<std::string> m_elementNames;

};

#endif // STSelectLadders17_H
