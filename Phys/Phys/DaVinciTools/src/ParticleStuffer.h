// $Id: ParticleStuffer.h,v 1.5 2004-03-25 16:46:00 pkoppenb Exp $ 
#ifndef PARTICLESTUFFER_H  
#define PARTICLESTUFFER_H 1 

// Include files 
// from STL 
#include <vector>
#include <string> 

// from Gaudi 
#include "GaudiKernel/AlgTool.h" 
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"

// from DaVinciTools 
#include "DaVinciTools/IParticleStuffer.h" 
#include "DaVinciTools/IParticleTransporter.h" 
#include "Event/Vertex.h"
#include "Event/Particle.h"

// Forward declarations 
class IParticlePropertySvc; 
class IParticleTransporter;

/** @class ParticleStuffer ParticleStuffer.h 
 *  Fill a particle given a vertex and a particle ID 
 *  @author Paul Colrain 
 *  @date   14/03/2002 
 *  Modified by S. Amato to transport parameters to the vertex position 
 *  11/10/202  
*/ 

class ParticleStuffer : public AlgTool,  
                        virtual public IParticleStuffer { 
  
public: 
  
  /// Standard constructor 
  ParticleStuffer( const std::string& type,  
                   const std::string& name, 
                   const IInterface* parent); 
  
  /// Desctructor 
  virtual ~ParticleStuffer( ){ };  
  
  /// Retrieve  the ParticlePropertyService. 
  StatusCode initialize();     
  
  
  /// Fill Composite Particle from Vertex 
  StatusCode fillParticle( const Vertex&, Particle&,  
                           const ParticleID& ); 
  
  // Fill Particle from ProtoParticle 
  //StatusCode fillParticle( const ProtoParticle&, Particle&,  
  //                         const ParticleID& ); 
  
  
private: 
  
  void calcSumpz( int const SIIDIM , HepVector &e, HepVector &pzvec,  
                  HepVector &SII, 
                  double &sumpz, double &sum1pz, double &sum2pz ); 
  
  void calcPar1Par2( int const SIIDIM ,HepMatrix &posErrTemp, HepVector &e, 
                     HepVector &sumh,  
                     HepVector &sum, 
                     double &par1, double &par2 ); 
  
  void calcSumhDeriv( int const SIIDIM ,HepMatrix &posErrTemp, HepVector &e,  
                      HepMatrix &sumhDeriv ); 
  
  void calcParDeriv( int const SIIDIM , HepVector& sum, HepVector& sumh,  
                     HepMatrix &sumhDeriv,  
                     HepMatrix &parDeriv ); 
  
  void calcSumpzDeriv( int const SIIDIM , HepVector &e, HepVector& SII,
                       HepVector& pzvec, 
                       HepMatrix &sumpzDeriv   ); 
  
  
  void buildTeMatrix( int const SIIDIM , HepMatrix &sumhDeriv, HepMatrix
                      &sumpzDeriv, 
                      HepMatrix &parDeriv, 
                      HepVector &sumh, HepVector &sum, 
                      double par1, double par2, double sumpz, double sum1pz, 
                      double sum2pz, HepMatrix &Te ); 
  
  StatusCode calcCeEPosErr( const Vertex& vtx, HepSymMatrix &Ce,
                            HepVector &e, HepMatrix &posErrTemp ); 
  
  
  
  
  IParticlePropertySvc* m_ppSvc;        ///< Reference to ParticlePropertySvc 
  IToolSvc* m_pToolSvc; 
  IParticleTransporter* m_pTransporter; ///< Reference to ParticleTransporter 
  std::string m_transporterType;        ///< Type of transporter to use       
  
}; 

#endif // PARTICLESTUFFER_H 
