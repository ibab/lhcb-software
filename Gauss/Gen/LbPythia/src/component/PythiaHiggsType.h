#ifndef LBPYTHIA_PythiaHiggsType_H
#define LBPYTHIA_PythiaHiggsType_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from Generators
#include "Generators/IGenCutTool.h"

/** @class PythiaHiggsType PythiaHiggsType.h component/PythiaHiggsType.h
 *
 *  Tool to select events with, 1 or 2 b quarks in the acceptance coming 
 *  from a given mother, and/or 1 or 2 leptons in the acceptance with given 
 *  minimal pt and comming (or not) from
 *  a W or Z boson
 *
 *  @author Victor Coco
 *  @date   2007-01-21
 */
class PythiaHiggsType : public GaudiTool , virtual public IGenCutTool {
public:
  /// Standard constructor
  PythiaHiggsType( const std::string & type , const std::string & name ,
                   const IInterface * parent ) ;
  
  virtual ~PythiaHiggsType( ); ///< Destructor  

  /// initialize function
  virtual StatusCode initialize( );

  /* Accept events following criteria given in options. 
   * Implements IGenCutTool::applyCut 
   */
  virtual bool applyCut( ParticleVector & theParticleVector ,
                         const HepMC::GenEvent * theEvent ,
                         const LHCb::GenCollision * theCollision ) const ;

private:

  /// Return true if the particle is a b quark
  bool Isb( const HepMC::GenParticle * p ) const ;

  /// Return true if the particle is a lepton
  bool IsLepton( const HepMC::GenParticle * p ) const ;

  /// Max theta angle to define the LHCb acceptance
  double m_thetaMax ;

  /// Number of leptons required in the acceptance
  int m_nbLepton;

  /// Minimum pT of the lepton
  double m_ptMin ;

  /// If true, the lepton is required to come from a W or a Z
  bool m_leptonWZ;

  /// Number of quarks required to be in the acceptance
  int m_nbbquarks;

  /// PDG id of the mother of the b quarks
  int m_motherofb_id;
};
#endif // COMPONENT_PythiaHiggsType_H
