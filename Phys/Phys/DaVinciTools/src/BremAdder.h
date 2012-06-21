// $Id$
#ifndef BREMADDER_H
#define BREMADDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// from LHCb
#include "Kernel/IBremAdder.h"            // Interface
#include "CaloDet/DeCalorimeter.h"
#include "Kernel/IParticle2State.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/Incident.h" 

/** @class BremAdder BremAdder.h
 *
 *  Adds Brem corrections to particles
 *
 *  @author Olivier Deschamps
 *  @date   2006-10-25
 */
class BremAdder : public GaudiTool, virtual public IBremAdder, virtual public IIncidentListener  {
public:
  /// Standard constructor
  BremAdder( const std::string& type,
             const std::string& name,
             const IInterface* parent);

  virtual ~BremAdder( ); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  // paranoid cleaning - reset all caches at each event
  virtual void handle(const Incident&  ) { 
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << "IIncident Svc reset" << endmsg; 
    m_list.clear();
    m_list2.clear();
    m_key=NULL;
    m_key2=NULL;
  }

  bool addBrem   ( LHCb::Particle* particle,bool force=false );
  bool removeBrem( LHCb::Particle* particle,bool force=false );
  bool addBrem2Pair( LHCb::Particle* p1, LHCb::Particle* p2 , bool force=false);
  bool hasBrem(const LHCb::Particle* particle);
  const LHCb::CaloMomentum bremMomentum(const LHCb::Particle* particle,std::string flag="");
  const std::pair<LHCb::CaloMomentum,LHCb::CaloMomentum> bremMomenta(const LHCb::Particle* p1,
                                                                     const LHCb::Particle* p2,
                                                                     std::string flag="");

protected:
  bool brem4particle( LHCb::Particle* particle ,
                      const std::string& what,
                      const std::vector<const LHCb::CaloHypo*>& brems,
                      bool force=false);

  const std::vector<const LHCb::CaloHypo*> bremList(const LHCb::Particle* particle,
                                                    std::string flag="");

  const std::pair<std::vector<const LHCb::CaloHypo*>,
                  std::vector<const LHCb::CaloHypo*> > bremLists(const LHCb::Particle* p1,
                                                                 const LHCb::Particle* p2,
                                                                 std::string flag="");

  const std::vector<const LHCb::CaloHypo*> getBrem(const LHCb::Particle* particle);

  const std::pair<Gaudi::XYZPoint,Gaudi::SymMatrix3x3> 
  getPos(const LHCb::ProtoParticle* proto,
         const LHCb::State::Location& state,
         double zcalo, double def=0 ) const;

  const LHCb::State* usedState( const LHCb::Track* track) const;

private:

  double m_bremid;
  double m_dllBrem;
  double m_chi2Brem;
  std::string m_bInput;
  DeCalorimeter* m_calo;
  ITrackExtrapolator*  m_extrapolator;
  std::string m_extrapolatorType;
  //  double m_ctol;
  double m_stol;
  double m_ptg;
  const IParticle2State* m_p2s ;
  int m_method;
  const LHCb::Particle* m_key;
  const LHCb::Particle* m_key2;
  std::vector<const LHCb::CaloHypo*> m_list;
  std::vector<const LHCb::CaloHypo*> m_list2;
  double m_z0;

};

#endif // BREMADDER_H
