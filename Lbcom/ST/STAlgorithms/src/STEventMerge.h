// $Id: STEventMerge.h,v 1.2 2008-10-11 10:42:23 mneedham Exp $
#ifndef STEVENTMERGE_H
#define STEVENTMERGE_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/STCluster.h"


/** @class STEventMerge STEventMerge.h
 *
 *  Class for merging events
 *
 *  @author M.Needham
 *  @date   16/06/2008
 */

namespace LHCb{
  class STChannelID;
}

class STEventMerge :public GaudiAlgorithm {

public:
  
  // Constructors and destructor
  STEventMerge( const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~STEventMerge();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:


  void overlappingClusters(const LHCb::STClusters* inCont, 
                           const LHCb::STCluster* testClus,
                           std::vector<LHCb::STCluster*>& outCont) const;


  unsigned int spillDifference(const LHCb::STCluster* clus1, 
                               const LHCb::STCluster* clus2 ) const;
  
  LHCb::STCluster* selectBestCluster(const std::vector<LHCb::STCluster*>& clusters, const LHCb::STCluster* testClus) const;

  std::vector<std::string> m_spillsVector;
  std::vector<std::string> m_spillPath;
  std::string m_inputLocation;
  std::string m_clusterLocation;
  std::string m_detType;

  class Less_by_Channel : public std::binary_function<LHCb::STLiteCluster,LHCb::STLiteCluster ,bool>{
  public:

    /** compare the channel of one object with the
     *  channel of another object
     *  @param obj1   first  object
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    //
    inline bool operator() ( LHCb::STLiteCluster obj1 , LHCb::STLiteCluster obj2 ) const
    {
      return obj1.channelID() < obj2.channelID() ;
    }
  };

   
  class Equal_Channel : public std::binary_function<LHCb::STLiteCluster,LHCb::STLiteCluster ,bool>{
  public:

    /** compare the channel of one object with the
     *  channel of another object
     *  @param obj1   first  object
     *  @param obj2   second object
     *  @return  result of the comparision
     */
    //
    inline bool operator() ( LHCb::STLiteCluster obj1 , LHCb::STLiteCluster obj2 ) const
    {
      return obj1.channelID() == obj2.channelID() ;
    }
  };



};

#endif // STEventMerge
