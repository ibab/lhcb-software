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

/** @class BremAdder BremAdder.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-10-25
 */
class BremAdder : public GaudiTool, virtual public IBremAdder {
public: 
  /// Standard constructor
  BremAdder( const std::string& type, 
             const std::string& name,
             const IInterface* parent);

  virtual ~BremAdder( ); ///< Destructor
  virtual StatusCode initialize();

  bool addBrem   ( LHCb::Particle* particle,bool force=false ) const {return brem4particle( particle, "add" ,force  ); };
  bool removeBrem( LHCb::Particle* particle,bool force=false ) const {return brem4particle( particle, "remove",force); };
  const std::vector<const LHCb::CaloHypo*> bremList(const LHCb::Particle* particle)const;

protected:

  bool brem4particle( LHCb::Particle* particle , std::string what,bool force=false) const;
  const std::pair<Gaudi::XYZPoint,Gaudi::SymMatrix3x3> getPos(const LHCb::ProtoParticle* proto, LHCb::State::Location state
                                                              , double zcalo, double def=0)const;
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
};
#endif // BREMADDER_H
