#ifndef STSelectChannelIDByTruth_H
#define STSelectChannelIDByTruth_H 1

#include <vector>

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTChannelIDSelector.h"

/** @class STSelectChannelIDByTruth STSelectChannelIDByTruth.h
 *
 *  Tool for selecting clusters using a list of service boxes
 *
 *  @author M.Needham
 *  @date   3/2/2009
 */

#include "Linker/LinkedTo.h"

#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

namespace LHCb{
  class MCParticle;
  class STChannelID;
  class STCluster;
}

class IMCParticleSelector;

class STSelectChannelIDByTruth: public ST::ToolBase, 
                         virtual public ISTChannelIDSelector,  
                         virtual public IIncidentListener {

 public: 
   
  /// constructer
  STSelectChannelIDByTruth( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectChannelIDByTruth();

  /// initialize
  virtual StatusCode initialize();

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
  STSelectChannelIDByTruth();
  ///   copy     constructor  is  private 
  STSelectChannelIDByTruth (const STSelectChannelIDByTruth& );
  ///   assignement operator  is  private 
  STSelectChannelIDByTruth& operator= (const STSelectChannelIDByTruth& );  

  enum Types {Spillover = 0 , Real = 1 };

  typedef LinkedTo<LHCb::MCParticle,LHCb::STCluster> STLinks;
  mutable STLinks m_links; 

  mutable bool m_configured;
  std::string m_clusterLocation;
  std::vector<int> m_types;
  IMCParticleSelector* m_selector;

};

#endif // STSelectChannelIDByTruth_H
