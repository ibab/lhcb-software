// $Id: LoKiSeedConeJetMaker.h,v 1.2 2009-12-14 12:34:33 cocov Exp $
// ============================================================================
#ifndef LOKISEEDCONEJETMAKER_H 
#define LOKISEEDCONEJETMAKER_H 1
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
// ============================================================================
// SeedJet
// ============================================================================
#include "Kernel/IParticleTransporter.h"
#include "GaudiAlg/ITupleTool.h"
#include "Event/RecVertex.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class SeedConeJetMaker
   *
   *  The most trivial, SeedConeJet based  implementaion of interface IJetMaker 
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
    class SeedConeJetMaker 
    : public virtual IJetMaker
    , public         GaudiTool
  {
    // the friend factory fo instantiation 
    friend class ToolFactory<LoKi::SeedConeJetMaker> ;
  public:
    /** The main method: jet-finding procedure 
     * 
     *  @code 
     *
     *  // get the tool
     *  const IJetMaker* jetMaker = tool<IJetMaker> ( .... ) ;
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
     *  IJetMaker::Jets   jets ;
     *
     *  // find the jets! 
     *  StatusCode sc = jetMaker -> makeJets ( input , jets ) ;
     *
     *  // make  a loop over jets:
     *  for ( IJetMaker::Jets::const_iterator iJet = jets.begin() ; 
     *        jets.end() != iJet ; ++iJet ) 
     *    {
     *        // get the jet 
     *        LHCb::Particle* jet = *iJet ;
     *    }
     *
     *  @endcode 
     *
     *  @attention It is a responsibility of users (e.g. the algorithm) 
     *             to take care about the ownership of jets *AND* their 
     *             vertices). The tool is not intended to do it! 
     *  
     *  @param input contaainer of input particles 
     *  @param jets  container of  output jets 
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
     *        according to EPFL/UB/CERN studies. 
     *
     */
    SeedConeJetMaker
      ( const std::string& type   ,
	const std::string& name   ,
	const IInterface*  parent ) 
      : GaudiTool ( type , name , parent )
	// 
	, m_jetID        ( 10098  )
	//
	, m_r            ( 0.7 )
	, m_ptmin        ( 8000  )
	//
	, m_sort         ( 4  )
	, m_sortSeed     ( 4   )

	// 
	, m_combinerName ( "MomentumCombiner"  )
	, m_combiner     ( 0   )

	, m_seedFinderName ( "LoKi__SeedFinder"  )
	, m_seedFinder     ( 0   )
	, m_uniquetrk     ( false )

	{ 
	  //
	  declareInterface <IJetMaker> ( this ) ;
	  //
	  declareProperty 
	    ( "JetID"          , 
	      m_jetID          ,
	      "Particle ID for the Jet") ;
	  declareProperty ( "RParameter"     , m_r      ) ;
	  declareProperty ( "PtMin"          , m_ptmin  ) ;
	  //
	  declareProperty 
	    ( "SortJet"             , 
	      m_sort             , 
	      "Sorting Criteria for jets [0:none,1:pt,2:E, 3:eta,4:DauThenPt, default:DauThenPt]" ) ;

	  declareProperty 
	    ( "SortSeed"             , 
	      m_sort             , 
	      "Sorting Criteria for Seeds [0:none,1:pt,2:E, 3:eta,4:DauThenPt, default:DauThenPt]" ) ;
	  // define momentum combiner
	  declareProperty ( "ParticleCombiner", m_combinerName ) ;
	  declareProperty ( "SeedFinder",m_seedFinderName, "VVSeedFinder, UBSeedFinder, ..." );

	  declareProperty ( "ConeUniqueTrk", m_uniquetrk   , "Cone With unique trk ON/OFF" );

	} 
      /// destructor
      virtual ~SeedConeJetMaker( ){}
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
      SeedConeJetMaker () ;
      // the copy constructor is disabled 
      SeedConeJetMaker           ( const  SeedConeJetMaker& )  ;
      // the assignement operator is disabled
      SeedConeJetMaker& operator=( const  SeedConeJetMaker& )  ;
  protected:
      // proposed jet ID 
      int    m_jetID     ; ///< proposed jet ID
      // R-parameter 
      double m_r         ; ///< R-parameters
      // ptMin-parameter 
      double m_ptmin     ; ///< pt-min parameter 
      // jet sorting criteria 
      int    m_sort      ; ///< jet sorting criteria
      int    m_sortSeed  ; ///< seed sorting criteria

      // combiner 
      std::string                m_combinerName ;
      mutable IParticleCombiner* m_combiner ; ///< combiner to be used 

      std::string                m_seedFinderName ;
      mutable IJetMaker*       m_seedFinder ; ///< combiner to be used 

      bool m_uniquetrk   ;


      ITupleTool *m_tuple;



      IJetMaker::Jets JetCone( const  double &Rmax,
			       IJetMaker::Jets &seeds,
			       const IJetMaker::Input &inputs) const;

      IJetMaker::Jets JetConePurged( const  double &Rmax,
			       IJetMaker::Jets &seeds,
			       const IJetMaker::Input &inputs) const;

  
    
    
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
#endif // LOKISEEDCONEJETMAKER_H
// ============================================================================

