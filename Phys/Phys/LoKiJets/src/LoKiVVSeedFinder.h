// $Id: LoKiVVSeedFinder.h,v 1.2 2009-12-14 12:34:33 potterat Exp $
// ============================================================================
#ifndef LOKIVVSEEDFINDER_H 
#define LOKIVVSEEDFINDER_H 1
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IJetMaker.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Geometry.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/IVertexFit.h"
// ============================================================================
// SeedJet
// ============================================================================
#include "Kernel/IParticleTransporter.h"
#include "Event/RecVertex.h"
#include "LHCbMath/LHCbMath.h"
#include "GaudiAlg/ITupleTool.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class VVSeedFinder
   *
   *  The VVSeedFinder (EPFL) implementaion of interface IJetMaker 
   *  @see IJetMaker 
   *
   *  This file is a part of LoKi project - 
   *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
   *
   *  The package has been designed with the kind help from
   *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
   *  contributions and advices from G.Raven, J.van Tilburg, 
   *  A.Golutvin, P.Koppenburg have been used in the design.
   *
   *  @author Cedric POTTERAT  cedric.potterat@cern.ch
   *  @date   2011-01-31
   */
  class VVSeedFinder 
    : public virtual IJetMaker
    , public         GaudiTool
  {
    // the friend factory fo instantiation 
    friend class ToolFactory<LoKi::VVSeedFinder> ;
  public:
    /** The main method: seed-finding procedure 
     * 
     *  @code 
     *
     *  // get the tool
     *  const IJetMaker* seedMaker = tool<IJetMaker> ( .... ) ;
     *
     *  // input particles 
     *  IJetMaker::Inputs input = ... 
     *  // 1) 
     *  // const Particles* particles = .... ;
     *  // // create the input container 
     *  // IJetMaker::Inputs input( particles->begin() , particles->end() ) ;
     *  // 2) 
     *  // LHCb::Particle::ConstVector particles = .... ;
     *  // // create the input container 
     *  // IJetMaker::Inputs input( particles.begin() , particles.end() ) ;
     *  // 3) 
     *  // LoKi::Range particles = .... ;
     *  // // create the input container 
     *  // IJetMaker::Inputs input( particles.begin() , particles.end() ) ;
     *
     *  // placeholder for "output" jets 
     *  IJetMaker::Jets   seeds ;
     *
     *  // find the jets! 
     *  StatusCode sc = seedMaker -> makeJets ( input , seeds ) ;
     *
     *  // make  a loop over jets:
     *  for ( IJetMaker::Jets::const_iterator iSeed = seeds.begin() ; 
     *        seeds.end() != iSeed ; ++iSeed ) 
     *    {
     *        // get the jet 
     *        LHCb::Particle* seed = *iSeed ;
     *    }
     *
     *  @endcode 
     *
     *  @attention It is a responsibility of users (e.g. the algorithm) 
     *             to take care about the ownership of jets *AND* their 
     *             vertices). The tool is not intended to do it! 
     *  
     *  @param input contaainer of input particles 
     *  @param seeds  container of  output seeds (of type Particle)
     *  @return status code 
     */
    virtual StatusCode makeJets 
      ( const IJetMaker::Input& input , IJetMaker::Jets& jets ) const ;
    virtual StatusCode makeJets 
      ( const IJetMaker::Input& input , const LHCb::RecVertex& vtx , IJetMaker::Jets& jets ) const ;
    // ========================================================================
  protected:  
    /** the standard constructor
     * 
     *  @todo The default values for configuration parameters 
     *        (especially for R-parameter) need to be adjusted 
     *        according to EPFL studies. 
     *        @link http://cdsweb.cern.ch/record/1211038/files/LHCb-INT-2009-023.pdf LHCb-INT-2009-023 - Bay/Potterat  @endlink
     *        @link http://cdsweb.cern.ch/record/1266883/files/CERN-THESIS-2010-074.pdf CERN_THESIS-2010-074 - Potterat @endlink
     *
     */
    VVSeedFinder
      ( const std::string& type   ,
	const std::string& name   ,
	const IInterface*  parent ) 
      : GaudiTool ( type , name , parent )
	// 
	,   m_seedID           ( 20098  )
	//
	,   m_r                ( 0.15   )
	//
	,   m_sort             ( 4      )
	// 
	,   m_combinerName     ( "MomentumCombiner"  )
	,   m_combiner         ( 0      )
	,   m_dist             ( 0      )
	,   m_fitter           ( 0      )
	,   m_PtTrackMin       ( 600.0 )
	,   m_PTrackMin        ( 1000.0)
	,   m_IPmin            ( 0.1    )
	,   m_Signif           ( 2.5    )  	
	,   m_DMK0             ( 10.0   )   	
	,   m_TseedVtxMin      ( 1.0    ) 	
	,   m_TseedVtxMax      ( 200.0  )
	,   m_TseedVtxMinAnyPV ( 0.1    ) 
	,   m_DtrakMax         ( 0.5    )     	
	,   m_PtSeedsMin       ( 1000.   )  
	,   m_PtMergedSeedsMin ( 1000.   ) 
	,   m_SeedsMaxChi2DoF  ( 50.    ) 
	,   m_Triplets         ( false  )   
	,   m_DRmin            ( 0.1    )   
	,   m_DRmax            ( 10.    )  
	,   m_TrkChi2DoF       ( 2.5    )
	,   m_DeltaRSeeds      ( 0.5    )
	,   m_preFilter        ( true   )   
	,   m_jetFilter        ( true   )   
	,   m_PVveto           ( true   )   
	{ 
	  //
	  declareInterface <IJetMaker> ( this ) ;
	  //
	  declareProperty ("SeedID" , m_seedID , "Particle ID for the Seed") ;
	  declareProperty ( "SeedRParameter"  , m_r      ) ;
	  declareProperty 
	    ( "Sort"             , 
	      m_sort             , 
	      "Sorting Criteria for jets [0:none,1:pt,2:E,3:eta, 4:ProtoSeed the Dau then Pt, default:4]" ) ;
	  // define momentum combiner
	  declareProperty("ParticleCombiner"  , m_combinerName ) ;
	  declareProperty("SeedPtTrackMin"    ,   m_PtTrackMin      , "pt of the track used for Vertexing" );
	  declareProperty("SeedPTrackMin"     ,   m_PTrackMin      , "p of the track used for Vertexing" );
	  declareProperty("SeedIPmin"         ,   m_IPmin           , "ip of the track used for Vertexing");
	  declareProperty("SeedSignif"        ,   m_Signif           , "signif oft he track used for Vertexing");  	
	  declareProperty("SeedDMK0"          ,   m_DMK0          ,"mass window for Ks"  );   	
	  declareProperty("SeedTseedVtxMin"   ,   m_TseedVtxMin     , "min distance btw PV and the  vtx" ); 	
	  declareProperty("SeedTseedVtxMax"   ,   m_TseedVtxMax     , "max distance btw PV and the  vtx" );
	  declareProperty("SeedTseedVtxMinAnyPV"   ,   m_TseedVtxMinAnyPV     , "min distance btw any PV and the  vtx" ); 	

	  declareProperty("SeedDtrakMax"      ,   m_DtrakMax         ,"dca window for vtx" );     	
	  declareProperty("SeedPtSeedsMin"    ,   m_PtSeedsMin     ,"min pt of the seeds" );  
	  declareProperty ( "SeedVtxMaxChi2PerDoF" , m_SeedsMaxChi2DoF     ,"max chi2 per dof for the vtx fit of the seed" );   
	  declareProperty("PtMergedSeedsMin"    ,   m_PtMergedSeedsMin     ,"min pt of the merged seeds" );  	

	  declareProperty("SeedTriplets"      ,   m_Triplets      ,"built Vtx with 3 tracks"  );
	  declareProperty("SeedDRmin"         ,   m_DRmin         , "min positon in R of the vtx"  );   
	  declareProperty("SeedDRmax"         ,   m_DRmax          , "max positon in R of the vtx"  ); 
	  declareProperty("SeedTrkChi2PerDoF" ,   m_TrkChi2DoF     , "max chi2PerDoF for the track used for the vtx"  ); 
	  declareProperty("SeedsDeltaR" ,   m_DeltaRSeeds    , "min DR btw 2 seeds otherwise:merge"  ); 
	  declareProperty("SeedpreFilter"    ,   m_preFilter    ,"pre filter the protoseeds"      );
	  declareProperty("SeedFilter"    ,   m_jetFilter    ,"filter the protoseeds"     ); 
	  declareProperty("vetoPV"    ,   m_PVveto ,"exclude vertex near to any PV with distance fixe by 'SeedTseedVtxMinAnyPV' and exclude the trk associated to any PV to construct a secondary vtx"     ); 

	} 
      /// destructor
      virtual ~VVSeedFinder( ){}
  public:
      /** standard initialization of the tool
       *  @return status code 
       */
      virtual StatusCode initialize () ;
  protected:
      /// make the detailed check of all parameters

      
      inline StatusCode  check() const 
	{
	  if ( 0 > m_ptmin ) { Warning ( "PtMin is negative " ) ; }	
	  return StatusCode( StatusCode::SUCCESS , true ) ;
	}
      
  protected:
      int to_user_index   ( const int index ) const { return index + 10000 ; }
      int from_user_index ( const int index ) const { return index - 10000 ; }
  private:
      // the default constructor is disabled 
      VVSeedFinder () ;
      // the copy constructor is disabled 
      VVSeedFinder           ( const  VVSeedFinder& )  ;
      // the assignement operator is disabled
      VVSeedFinder& operator=( const  VVSeedFinder& )  ;
  protected:
      // proposed jet ID 
      int    m_seedID     ; ///< proposed jet ID
      // R-parameter 
      double m_r         ; ///< R-parameters
      // ptMin-parameter 
      double m_ptmin     ; ///< pt-min parameter 
      // jet sorting criteria 
      int    m_sort      ; ///< jet sorting criteria
      // combiner 
      std::string                m_combinerName ;
      mutable IParticleCombiner* m_combiner ; ///< combiner to be used 

      IDistanceCalculator *m_dist;
      IVertexFit          *m_fitter;

      
      double   m_Rmax           ;
      double   m_PtTrackMin     ;
      double   m_PTrackMin     ;
      double   m_IPmin          ;
      double   m_Signif         ;  	
      double   m_DMK0           ;   	
      double   m_TseedVtxMin    ; 	
      double   m_TseedVtxMax    ;
      double   m_TseedVtxMinAnyPV   ;
      double   m_DtrakMax       ;     	
      double   m_PtSeedsMin     ;  
      double   m_PtMergedSeedsMin;
      double   m_SeedsMaxChi2DoF ;
      bool     m_Triplets ;
      double   m_DRmin    ;	
      double   m_DRmax   ;	
      double   m_TrkChi2DoF ;
      double   m_DeltaRSeeds;
      bool     m_preFilter;
      bool     m_jetFilter;
      bool     m_PVveto;



      IJetMaker::Jets JetCone( const  double &Rmax,
			       IJetMaker::Jets &seeds,
			       const IJetMaker::Input &inputs) const;



      IJetMaker::Jets FilterProtoJets(const double &DeltaRSeeds, 
				      IJetMaker::Jets  &ProtoJets) const;
    

      double getDeltaR(LHCb::Particle *p1,LHCb::Particle *p2) const;

      void RemoveTracks(LHCb::Particle::ConstVector & particles, 
			const LHCb::RecVertex PV ) const;
  };       
}
 






class sortDauPt {
 public:
  inline bool operator() ( LHCb::Particle* obj1 ,
			   LHCb::Particle* obj2) {
    if(obj1->weight() == obj2->weight())
      if(obj1->daughtersVector().size() == obj2->daughtersVector().size())
	return obj1->pt() > obj2->pt() ;
      else
	return obj1->daughtersVector().size() > obj2->daughtersVector().size();
    else
      return obj1->weight() > obj2->weight();  
  }
};



class sortE {
 public:
  inline bool operator() ( LHCb::Particle* obj1 ,
			   LHCb::Particle* obj2) { 
    return obj1->momentum().E() > obj2->momentum().E() ;      
  }
};

class sortEta {
 public:
  inline bool operator() ( LHCb::Particle* obj1 ,
			   LHCb::Particle* obj2) { 
    return obj1->momentum().Eta() > obj2->momentum().Eta() ;      
  }
}; 

class sortPt {
 public:
  inline bool operator() ( LHCb::Particle* obj1 ,
			   LHCb::Particle* obj2) { 
    return obj1->pt() > obj2->pt();      
  }
}; 


// End of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKIVVSEEDFINDER_H
// ============================================================================

