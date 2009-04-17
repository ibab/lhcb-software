// $Id: CaloGetterTool.h,v 1.1 2009-04-17 11:43:51 odescham Exp $
#ifndef CALOGETTERTOOL_H 
#define CALOGETTERTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// from LHCb
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
#include "CaloDAQ/ICaloDataProvider.h"
#include "CaloInterfaces/ICaloGetterTool.h"            // Interface

/** @class CaloGetterTool CaloGetterTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2009-04-17
 */
class CaloGetterTool : public GaudiTool, virtual public ICaloGetterTool {
public: 
  /// Standard constructor
  CaloGetterTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~CaloGetterTool( ); ///< Destructor
  StatusCode initialize();

  // update
  void update();

  // setters
  void addToDigits  (std::string loc,bool clear = false){
    if(clear)m_digiLoc.clear();
    m_digiLoc.push_back( loc ) ;
  }
  void addToClusters(std::string loc,bool clear = false){
    if(clear)m_clusLoc.clear();
    m_clusLoc.push_back( loc ) ;
  }
  void addToHypos  (std::string loc,bool clear = false){
    if(clear)m_hypoLoc.clear();
    m_hypoLoc.push_back( loc ) ;
  }

  //getters
  LHCb::CaloDigits* digits(std::string loc){
    std::map<std::string,LHCb::CaloDigits*>::iterator it = m_digits.find( loc );
    return (it == m_digits.end()) ? NULL : (*it).second;
  }
  LHCb::CaloClusters* clusters(std::string loc){
    std::map<std::string,LHCb::CaloClusters*>::iterator it = m_clusters.find( loc );
    return (it == m_clusters.end()) ? NULL : (*it).second;
  }
  LHCb::CaloHypos* hypos(std::string loc){
    std::map<std::string,LHCb::CaloHypos*>::iterator it = m_hypos.find( loc );
    return (it == m_hypos.end()) ? NULL : (*it).second;
  }
  bool hasData(std::string det){
    std::map<std::string,bool>::iterator it = m_prov.find(det);
    return (it == m_prov.end()) ? false : (*it).second;
  }
  ICaloDataProvider* provider(std::string det){
    std::map<std::string,ICaloDataProvider*>::iterator it = m_provider.find(det);
    return (it == m_provider.end()) ? NULL : (*it).second;
  }  
  
protected:

private:
  std::map<std::string,LHCb::CaloDigits*>   m_digits;
  std::map<std::string,LHCb::CaloClusters*> m_clusters;
  std::map<std::string,LHCb::CaloHypos*>    m_hypos;
  std::map<std::string,ICaloDataProvider*> m_provider;
  std::map<std::string,bool> m_prov;
  bool m_digiUpd;
  bool m_clusUpd;
  bool m_hypoUpd;
  bool m_provUpd;
  std::vector<std::string> m_digiLoc;
  std::vector<std::string> m_clusLoc;
  std::vector<std::string> m_hypoLoc;
};
#endif // CALOGETTERTOOL_H
