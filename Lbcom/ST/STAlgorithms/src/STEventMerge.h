// $Id: STEventMerge.h,v 1.1 2008-09-20 12:56:35 mneedham Exp $
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
  
  std::vector<std::string> m_spillsVector;
  std::vector<std::string> m_spillPath;
  std::string m_inputLocation;
  std::string m_clusterLocation;

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
