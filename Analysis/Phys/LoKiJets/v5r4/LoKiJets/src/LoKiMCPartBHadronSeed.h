// $Id: LoKiMCPartBHadronSeed.h,v 1.2 2009-12-14 12:34:33 potterat Exp $
// ============================================================================
#ifndef LOKIMCPARTBHADRONSEED_H 
#define LOKIMCPARTBHADRONSEED_H 1
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
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
// ============================================================================
// SeedJet
// ============================================================================
#include "Kernel/IParticleTransporter.h"
#include "Event/RecVertex.h"
#include "LHCbMath/LHCbMath.h"
#include "GaudiAlg/ITupleTool.h"
#include "Event/MCParticle.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class MCPartBHadronSeed
   *
   *  The SeedFinder implementaion of interface IJetMaker 
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
  class MCPartBHadronSeed 
    : public virtual IJetMaker
    , public         GaudiTool
  {
    // the friend factory fo instantiation 
    friend class ToolFactory<LoKi::MCPartBHadronSeed> ;
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
    // ========================================================================
  protected:  
    /** the standard constructor
     * Creats seeds with the MCParticle B-hadrons that decays
     *
     *  @todo The default values for configuration parameters 
     *        (especially for R-parameter) need to be adjusted 
     *        according to EPFL/UB/CERN studies. 
     *
     */
    MCPartBHadronSeed
      ( const std::string& type   ,
	const std::string& name   ,
	const IInterface*  parent ) 
      : GaudiTool ( type , name , parent )
	// 
	,   m_seedID           ( 90099  )
	,   m_sort             ( 4      )  	
	,   m_PtSeedsMin       ( 1000   )  
	,   m_inAcc            ( true   )  
	,   m_combinerName     ( "MomentumCombiner"  )
	,   m_combiner         ( 0      )  


	{ 
	  //
	  declareInterface <IJetMaker> ( this ) ;
	  //
	  declareProperty ("SeedID" , m_seedID , "Particle ID for the Seed") ;
	  declareProperty 
	    ( "Sort"             , 
	      m_sort             , 
	      "Sorting Criteria for jets [0:none,1:pt,2:E,3:eta, 4:ProtoSeed the Dau then Pt, default:4]" ) ;
	  // define momentum combiner
	
	  declareProperty("SeedPtSeedsMin"    ,   m_PtSeedsMin     ,"min pt of the seeds" );  
	  declareProperty("inLHCbAcc"    ,   m_inAcc     ,"in  LHCbAcc" );  
	  declareProperty("ParticleCombiner"  , m_combinerName ) ;

	} 
      /// destructor
      virtual ~MCPartBHadronSeed( ){}
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
      MCPartBHadronSeed () ;
      // the copy constructor is disabled 
      MCPartBHadronSeed           ( const  MCPartBHadronSeed& )  ;
      // the assignement operator is disabled
      MCPartBHadronSeed& operator=( const  MCPartBHadronSeed& )  ;
  protected:
      // proposed jet ID 
      int    m_seedID     ; ///< proposed jet ID
      // ptMin-parameter 
      double m_ptmin     ; ///< pt-min parameter 
      // jet sorting criteria 
      int    m_sort      ; ///< jet sorting criteri    	
      double   m_PtSeedsMin     ;  
      bool m_inAcc;


      std::string                m_combinerName ;
      mutable IParticleCombiner* m_combiner ; ///< combiner to be used 
    
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
#endif // LOKIMCPARTBHADRONSEED_H
// ============================================================================

