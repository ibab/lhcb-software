#ifndef CALORELATIONSGETTER_H 
#define CALORELATIONSGETTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloInterfaces/ICaloRelationsGetter.h"            // Interface
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/Incident.h" 
// from LHCb
#include "Relations/Relation2D.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/IRelationWeighted2D.h"
#include "Event/Track.h"
#include "Event/CaloHypo.h"
#include "CaloUtils/Calo2Track.h"



/** @class CaloRelationsGetter CaloRelationsGetter.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2013-10-04
 */
class CaloRelationsGetter : public GaudiTool, virtual public ICaloRelationsGetter, virtual public IIncidentListener   {
public: 
  /// Standard constructor
  CaloRelationsGetter( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);
  virtual ~CaloRelationsGetter( ); ///< Destructor


  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual void handle(const Incident&  ) { 
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << "IIncident Svc reset" << endmsg; 
    clean();
  } 

  // getters 
  LHCb::Calo2Track::ITrHypoTable2D* getTrHypoTable2D(std::string location);
  LHCb::Calo2Track::IHypoEvalTable* getHypoEvalTable(std::string location);
  LHCb::Calo2Track::IClusTrTable*   getClusTrTable  (std::string location);


protected:

private:

  std::map<std::string,LHCb::Calo2Track::ITrHypoTable2D*> m_trHypo2D;
  std::map<std::string,LHCb::Calo2Track::IHypoEvalTable*> m_hypoEval;
  std::map<std::string,LHCb::Calo2Track::IClusTrTable*>   m_clusTr;
  void clean();
};
#endif // CALORELATIONSGETTER_H
