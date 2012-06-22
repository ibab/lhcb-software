// $Id: HighPtIsoLeptonAndTagPV.h,v 1.1 2007/12/10 15:38:23 ibelyaev Exp $
// ============================================================================
#ifndef HIGHPTISOLEPTONANDTAGPV_H 
#define HIGHPTISOLEPTONANDTAGPV_H 1
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/ITupleTool.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/DVAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/DeclareFactoryEntries.h"
//#include "Kernel/IContextTool.h"
#include <Kernel/IDistanceCalculator.h>
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"
#include "Kernel/IParticleTransporter.h"
#include "Kernel/IVertexFit.h"
//#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IOnOffline.h" 
#include "Kernel/DVAlgorithm.h"
#include "MuonDAQ/IMuonRawBuffer.h"
//To recreate track measurements
#include "TrackInterfaces/IMeasurementProvider.h"
#include "GaudiKernel/IParticlePropertySvc.h"

class IDistanceCalculator;

class  HighPtIsoLeptonAndTagPV : public DVAlgorithm {

  // the friend factory fo instantiation 
  friend class AlgFactory<HighPtIsoLeptonAndTagPV> ;
 public:
  HighPtIsoLeptonAndTagPV
    (const std::string& name, ISvcLocator* pSvcLocator)
    : DVAlgorithm(name,pSvcLocator)
 
    ,   m_FilterPart       ( true )
    ,   m_LeptID           ( 10099  )
    ,   m_niLeptID         ( 10199  )
    ,   m_LeptEJoverEL     ( 1.3   )     
    ,   m_LeptRmax         ( 0.45    )
    ,   m_LeptMuPtTrackMin ( 5000.0 )
    ,   m_LeptElPtTrackMin ( 5000.0 )

    ,   m_LeptIPmax        ( 0.1    )
    ,   m_LeptSignif       ( 1.    )

    ,   m_LeptjetMomRaw    ( false ) 
    ,   m_onOffline (0) 
    ,   m_ipTool (0) 
    ,   m_pVertexFit (0) 
 
    ,   m_combinerName ( "MomentumCombiner"  )
    ,   m_combiner     ( 0   )

    ,   m_transporterName ( "ParticleTransporter:PUBLIC" ) // The name of particle transpoter tool
    ,   m_transporter ( 0 )

      ,   m_TrkChi2DoF     ( 2.5    )

    { 
	
	
 
      declareProperty("FilterPart"    ,   m_FilterPart ); 

	
      declareProperty( "IsoLeptID"  ,     m_LeptID      ,   "Particle ID for the Lepton") ;
      declareProperty( "NoIsoLeptID"  ,     m_niLeptID      ,   "Particle ID for the Lepton") ;
	
      declareProperty("LeptEJoverEL"    ,   m_LeptEJoverEL      );	
      declareProperty("LeptJetR"          ,   m_LeptRmax            );
      declareProperty("LeptMuPtTrackMin"    ,   m_LeptMuPtTrackMin      );
      declareProperty("LeptElPtTrackMin"    ,   m_LeptElPtTrackMin      );
      declareProperty("LeptIPmax"    ,   m_LeptIPmax      );
      declareProperty("LeptSignif"    , m_LeptSignif  );
      declareProperty("LeptJetRawEnergy"   ,   m_LeptjetMomRaw          ); 	
      declareProperty("VertexFitter", m_typeVertexFit = "Default");


      // define momentum combiner
      declareProperty ( "ParticleCombiner", m_combinerName ) ;

      declareProperty      ( "Transporter"      , m_transporterName     , 
			     "The Particle Transporter tool to be used" );

	  declareProperty ( "SeedTrkChi2PerDoF" , m_TrkChi2DoF     , "max chi2PerDoF for the track used for the vtx"  ); 


    } 
  /// destructor
  virtual ~HighPtIsoLeptonAndTagPV( ){}
  /** standard initialization of the tool
   *  @return status code 
   */
  virtual StatusCode initialize () ;
      
  virtual StatusCode execute();

  virtual StatusCode finalize();

 protected:
  /// make the detailed check of all parameters
  inline StatusCode  check() const 
    {
      
      return StatusCode( StatusCode::SUCCESS , true ) ;
    }
 protected:
 
  LHCb::Particle JetCone( const double&,
			  Gaudi::XYZPoint,
			  Gaudi::XYZPoint,
			  const int&,
			  LHCb::Particle::ConstVector&)const  ;
  
  StatusCode GetIsoLeptons(LHCb::Particle::ConstVector InLeptons,
			   LHCb::Particle::ConstVector inputParts,
			   std::vector<LHCb::Particle>& Leptons ,
			   int& ) const;

  void RemoveTracks( LHCb::Particle::ConstVector & particles,
		     const LHCb::RecVertex  PV )const;




 private:
  // the default constructor is disabled 
  HighPtIsoLeptonAndTagPV () ;
  // the copy constructor is disabled 
  HighPtIsoLeptonAndTagPV           ( const  HighPtIsoLeptonAndTagPV& )  ;
  // the assignement operator is disabled
  HighPtIsoLeptonAndTagPV& operator=( const  HighPtIsoLeptonAndTagPV& )  ;
 protected:


  
  bool  m_FilterPart;


  int    m_LeptID     ; ///< proposed jet ID
  int    m_niLeptID     ; ///< proposed jet ID

  bool    m_useNN  ;
  double   m_LeptEJoverEL        ;	
  double   m_LeptRmax           ;
  double   m_LeptMuPtTrackMin     ;
  double   m_LeptElPtTrackMin     ;

  double   m_LeptIPmax           ;

  double   m_LeptSignif    ;
  bool  m_LeptjetMomRaw;


  //  IContextTool* m_context;
  
  const IOnOffline*     m_onOffline ;
  IMeasurementProvider * m_measProvider;
  ITupleTool* m_tuple;
  const IDistanceCalculator* m_ipTool; 
  const IVertexFit* m_pVertexFit;  
          
  std::string                m_combinerName ;
  mutable IParticleCombiner* m_combiner ; ///< combiner to be used 


  std::string  m_transporterName ;

  mutable IParticleTransporter* m_transporter;
  IParticlePropertySvc* m_svcProp;

  std::string m_geomToolName ; 
  std::string m_typeVertexFit; 

  const IDistanceCalculator* distanceCalculator;

  const IDistanceCalculator* m_dist;

  double   m_TrkChi2DoF     ;

   

  typedef LHCb::Relation1D< LHCb::Particle,LHCb::Particle >  Table ;



};



DECLARE_ALGORITHM_FACTORY(HighPtIsoLeptonAndTagPV);
// ============================================================================
// The END 
// ============================================================================
#endif // HIGHPTISOLEPTONANDTAGPV_H
// ============================================================================

