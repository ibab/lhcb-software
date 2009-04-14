// $Id: STSelectSpilloverCluster.h,v 1.1 2009-04-14 13:08:46 mneedham Exp $
#ifndef STSelectSpilloverCluster_H
#define STSelectSpilloverCluster_H 1

// ST tool base class
#include "Kernel/STToolBase.h"

// LHCbKernel
#include "Kernel/ISTClusterSelector.h"

/** @class STSelectSpilloverCluster STSelectSpilloverCluster.h
 *
 *  Tool for selecting clusters that are not spillover
 *  Requires you have the previous spill, ie TAE or upgrade !
 *
 *  @author M.Needham
 *  @date   28/3/2009
 */

#include "GaudiKernel/IIncidentListener.h"

#include "Event/STCluster.h"

namespace LHCb{
  class STChannelID;
}

class STSelectSpilloverCluster: public ST::ToolBase, 
                                virtual public ISTClusterSelector,
                                virtual public IIncidentListener  {

 public: 
   
  /// constructer
  STSelectSpilloverCluster( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

  /// destructer
  virtual ~STSelectSpilloverCluster();


  /// intialize
  StatusCode initialize();

  /**  @param  cluster pointer to ST cluster object to be selected 
  *  @return true if cluster is selected
  */
  virtual bool select     ( const LHCb::STCluster* cluster ) const ;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  cluster pointer to ST cluster object to be selected 
   *  @return true if cluster is selected
   */
  virtual bool operator() ( const LHCb::STCluster* cluster ) const  ;

  /** Implement the handle method for the Incident service.
  *  This is used to inform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  void handle( const Incident& incident );

 private:

  ///   default  constructor  is  private 
  STSelectSpilloverCluster();
  ///   copy     constructor  is  private 
  STSelectSpilloverCluster (const STSelectSpilloverCluster& );
  ///   assignement operator  is  private 
  STSelectSpilloverCluster& operator= (const STSelectSpilloverCluster& );  

  bool chargeSelection(const LHCb::STCluster* spillClus, 
		       const LHCb::STCluster* cluster) const;

  std::string m_spilloverLocation;
  mutable LHCb::STClusters* m_spilloverData; 
  double m_spilloverFraction;
  mutable bool m_configured;
  unsigned int m_nBits;
  double m_maxCharge;

};

#endif // STSelectByCharge_H
