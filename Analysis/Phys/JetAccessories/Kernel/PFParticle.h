
#ifndef KERNEL_PFPARTICLE_H 
#define KERNEL_PFPARTICLE_H 1

// Include files
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Kernel/IProtoParticleFilter.h"
#include "Kernel/ParticleProperty.h"
#include "Event/CaloCluster.h"
#include "Kernel/IParticle2State.h"
#include "GaudiKernel/GenericVectorTypes.h"

/** @class PFParticle PFParticle.h
 *  Particle Flow particle is a class inheritting from LHCb::Particle
 *  gathering information from Particle Flow algorithm.
 *
 *  For compatibility, members of PFParticle are also store in its base class Particle extra info
 *
 *
 *  @author Victor Coco
 *  @date   2012-07-23
 */

namespace LHCb {
  
  class PFParticle: public Particle {
    
  public:
    
    typedef std::vector<PFParticle*> Vector;
    typedef std::vector<const PFParticle*> ConstVector;
    
    typedef KeyedContainer<PFParticle, Containers::HashMap> Container;
    
    typedef Gaudi::NamedRange_<ConstVector> Range;
    
    enum PFParticleType{ Unknown=0,
                         // Good Particles
                         Charged = 1,
                         ChargedHadron,
                         Muon,
                         Electron,
                         Neutral = 10,
                         Photon,
                         Pi0,
                         MergedPi0,
                         ResolvedPi0,
                         NeutralHadron,
                         NeutralRecovery ,
                         // Composite particles
                         Composite = 100 ,
                         V0,
                         D,
                         B,
                         // Strange/Bad particles
                         BadParticle = 1000,
                         Charged0Momentum ,
                         ChargedInfMomentum,
                         BadPhotonMatchingT,
                         BadPhoton,
                         IsolatedPhoton,
                         // Conmpostie Daughter
                         Daughter = 10000
			 
    };
    enum ParticleExtraInfo{ Type=900,
                            DaughterType,
                            NSatECAL,
                            NSatHCAL,
                            ClustType,
                            ClustE,
                            ClustECAL,
                            ClustHCAL
			    
    };
    
    enum TrackTag{
		   Keep = 1 ,
		   KeepInfMom,
		   TurnTo0Momentum,
		   Reject
    };
    

    // TDOD: check that all extra info are written in the particle info
  public:

  ///Empty Constructor 
  PFParticle( ) : Particle(),
      m_type( Unknown ),
      m_nbSaturatedECALCaloCell ( 0 ),
      m_nbSaturatedHCALCaloCell ( 0 )
	{
	};

    /// Constructor from particle
    PFParticle( const Particle& p , bool isDaughter = false ) : Particle(p),
      m_type( Unknown ),
      m_nbSaturatedECALCaloCell ( 0 ),
      m_nbSaturatedHCALCaloCell ( 0 )
	{
    int ptype(Unknown);
    
    if(this->particleID().isHadron() && this->particleID().hasCharm() ){
      ptype = D;
    }
    else if(this->particleID().isHadron() && this->particleID().hasBottom() ){
      ptype = B;
    }
    else if ( this->particleID().isHadron() && this->particleID().threeCharge () == 0 && this->particleID().hasStrange() ){
      ptype = V0;
    }
    else if ( this->particleID().isHadron() && this->particleID().threeCharge ()!=0 ){
      ptype = ChargedHadron ;
    }
    else if ( this->particleID().isHadron() && this->particleID().threeCharge () ==0 ){
      ptype = Pi0 ;
    }
    else if ( this->particleID().threeCharge () ==0 ){
      ptype = Photon ;
    }
    else if ( this->particleID().isLepton() ){
      if( this->particleID().abspid()==11 )ptype = Electron;
      else ptype = Muon;
    }
	  //Extra info can be set only if not unknown
    
	  if ( ptype!=Unknown && !isDaughter ) {
      m_type = ptype ;
      this->addInfo(PFParticle::Type,(double)ptype);
    }
    else if ( ptype!=Unknown ){
      m_type = Daughter ;
      m_typeDaug = ptype ;
      this->addInfo(PFParticle::Type,(double)Daughter);
      this->addInfo(PFParticle::DaughterType,(double)ptype);
    }
    
	};

  /// Copy constructor
  PFParticle( const PFParticle& pfp ) : Particle(pfp),
      m_type( pfp.m_type ),
      m_nbSaturatedECALCaloCell( pfp.m_nbSaturatedECALCaloCell ),
      m_nbSaturatedHCALCaloCell( pfp.m_nbSaturatedHCALCaloCell )
	{
	  this->addInfo(PFParticle::NSatECAL,pfp.m_nbSaturatedECALCaloCell);
	  this->addInfo(PFParticle::NSatHCAL,pfp.m_nbSaturatedHCALCaloCell);
	  this->addInfo(PFParticle::Type,pfp.m_type);
	};
    
    /// Neutral Hadron Particle Constructor
    PFParticle( std::vector< const CaloCluster* > clusters , const ProtoParticle * pp  );
    /// Neutral Recovery  Particle Constructor
    PFParticle( Gaudi::Vector6 barycenter , double oldEnergy );
    /// Charged Particle Constructor
    PFParticle( const LHCb::ProtoParticle * pp ,  int type , 
                std::map< std::string , std::pair< const IProtoParticleFilter* ,
                const LHCb::ParticleProperty * > > & protoMap );  
    
    virtual ~PFParticle( ){}; ///< Destructor
    
    /// Private member accessor: set type of PFParticle
    void setPFType( int type ){ 
      this->m_type = type ; 
      this->addInfo(PFParticle::Type,(double)type);
    }
    /// Private member accessor: get type of PFParticle
    int PFType( ) const { return (int)this->info(PFParticle::Type,Unknown); }
    /// Private member accessor: get type of PFParticle
    int PFDaugType( ) const { return (int)this->info(PFParticle::DaughterType,Unknown); }
    
    /// Private member accessor: set number of saturated ECAL cells
    void setNSaturatedCellsECAL( int nsat ){ 
      this->m_nbSaturatedECALCaloCell = nsat ;
      this->addInfo(PFParticle::NSatECAL,nsat); 
    }
    /// Private member accessor: set number of saturated HCAL cells
    void setNSaturatedCellsHCAL( int nsat ){ 
      this->m_nbSaturatedHCALCaloCell = nsat ;
      this->addInfo(PFParticle::NSatHCAL,nsat); 
    }
    /// Private member accessor: get number of saturated ECAL cells
    int nSatECAL ( )const { 
      return this->m_nbSaturatedECALCaloCell ; 
    }
    
    /// Private member accessor: get number of saturated HCAL cells
    int nSatHCAL ( )const { 
      return this->m_nbSaturatedHCALCaloCell ; 
    }
    
    
    
  private:
    int m_type;  ///< PFParticle Type
    int m_typeDaug;  ///< PFParticle Daughter Type
    int m_nbSaturatedECALCaloCell; ///< Number of saturated ECAL cells
    int m_nbSaturatedHCALCaloCell; ///< Number of saturated HCAL cells
  };

} // namespace LHCb;
#endif // KERNEL_PFPARTICLES_H
