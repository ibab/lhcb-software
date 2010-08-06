// $Id: $
#ifndef STSelectChannelIDOnTrack_H
#define STSelectChannelIDOnTrack_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTChannelIDSelector.h"

/** @class STSelectSpilloverCluster STSelectSpilloverCluster.h
 *
 *  Tool for selecting clusters that are not spillover
 *  Requires you have the previous spill, ie TAE or upgrade !
 *
 *  @author M.Needham
 *  @date   28/3/2009
 */

#include "GaudiKernel/IIncidentListener.h"


namespace LHCb{
  class STChannelID;
}

class STSelectChannelIDOnTrack: public ST::ToolBase, 
                                virtual public ISTChannelIDSelector,
                                virtual public IIncidentListener  {

 public: 
   
  /// constructer
  STSelectChannelIDOnTrack( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

  /// destructer
  virtual ~STSelectChannelIDOnTrack();


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

  /** Implement the handle method for the Incident service.
  *  This is used to inform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  void handle( const Incident& incident );

 private:

  ///   default  constructor  is  private 
  STSelectChannelIDOnTrack();
  ///   copy     constructor  is  private 
  STSelectChannelIDOnTrack (const STSelectChannelIDOnTrack& );
  ///   assignement operator  is  private 
  STSelectChannelIDOnTrack& operator= (const STSelectChannelIDOnTrack& );  

  /// initEvent
  void initEvent() const;

  std::string m_trackLocation;
  mutable std::vector<LHCb::STChannelID> m_ids;
  mutable bool m_configured;

};

#endif // STSelectByCharge_H
