// $Id: P2VVTupleTool.h,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
#ifndef PSI2STUPLETOOL_H 
#define PSI2STUPLETOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

typedef std::vector< const HepMC::GenParticle* > GenPartVector;

/** @class P2VVTupleTool P2VVTupleTool.h
 *  
 *
 *  @author Thomas Blake
 *  @date   2008-06-17
 */
class P2VVTupleTool : public GaudiTupleAlg { //Algorithm {

public: 
  /// Standard constructor
  P2VVTupleTool( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~P2VVTupleTool( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  std::string formatName( const int pid ) const;
  void fillParticle( Tuple& tuple, const HepMC::GenParticle* particle ) const ;
  int fillCandidate( const HepMC::GenParticle* particle, const int process_type );
  void addDaughter( const HepMC::GenParticle* particle )  ;
  bool isLongLivedParticle( const HepMC::GenParticle* particle ) const;
  void fillP2VVAngles( Tuple& tuple, const HepMC::GenParticle* parent ) const ;
  
  
private:
  
  IParticlePropertySvc* m_ppsvc;
  int m_candidates;
  int m_events;
  
  IP2VVAngleCalculator* m_angletool;
  
  std::map< int, std::string > m_map;
  
  int m_daughterOne;
  int m_daughterTwo;
  int m_mother;
  std::string m_name;

  const HepMC::GenParticle* m_muplus;
  const HepMC::GenParticle* m_muminus;
  const HepMC::GenParticle* m_dtwo;
  const HepMC::GenParticle* m_done;
  
  
  
};
#endif // PSI2STUPLETOOL_H
