// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"                     // Event/TrackEvent 
#include "Event/L0CaloCandidate.h"           // Event/L0Event 
#include "Event/HltCandidate.h"              // Hlt/HltBase 
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/ITrack2CandidateMatch.h"
// ============================================================================
namespace Hlt
{
// ==========================================================================
/** @class MatchVeloL0Calo 
 *  
 *  @see LHCb::Track
 *  @see LHCb::L0CaloCandidate 
 *  @see Hlt::Candidate 
 *  @see Hlt::ITrack2CandidateMatch
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @author Mariusz Witek Mariusz.Witek@cern.ch
 *  @date 2011-02-11
 */
class MatchVeloL0Calo 
   : virtual public extends1<GaudiTool,Hlt::ITrack2CandidateMatch>
{
   // ========================================================================
   /// friend factory for instantiation 
   friend class ToolFactory<Hlt::MatchVeloL0Calo> ;
   // ========================================================================
public:  // ITrack2CandidateMatch
   // ========================================================================
   /** match a track with candidate 
    *  @see Hlt::ITrack2CandidateMatch
    *  @see ITrackMatch
    *  @param track     (INPUT)  the input track 
    *  @param candidate (INPUT)  the input candidate 
    *  @param matched   (OUTPUT) the matched track 
    *  @param quality   (OUTPUT) the matching quality
    *  @param quality2  (OUTPUT) the matching quality-2
    *  @return status code 
    */ 
   virtual StatusCode match 
   ( const LHCb::Track&    track     , 
     const Hlt::Candidate& candidate ,
     LHCb::Track&          matched   , 
     double&               quality   ,
     double&               quality2  ) const ;
   // ========================================================================
   /** match a track with candidate ("filter-mode")
    *  @see Hlt::ITrack2CandidateMatch
    *  @param track    (INPUT)  the input track 
    *  @param candidat (INPUT)  the input candidate 
    *  @return true if track and candidate are "matched"
    */ 
   virtual bool match 
   ( const LHCb::Track*    track      , 
     const Hlt::Candidate* candidate  ,
     const double          quality    , 
     const double          quality2   ) const ;
   // ========================================================================    
public:  // from IAlgTool
   // ========================================================================    
   /// the standard tool initialization 
   virtual StatusCode initialize () ;
   /// the standard tool finalization  
   virtual StatusCode finalize   () ;
   // ========================================================================    
protected: // AlgTool technicalities 
   // ========================================================================    
   /** standard constructor 
    *  @param type   tool type (?)
    *  @param name   tool instance name 
    *  @param parent tool parent 
    */
   MatchVeloL0Calo 
   ( const std::string& type   ,   // tool type ?
     const std::string& name   ,   // tool instance name 
     const IInterface*  parent ) ; // the parent 
   /// protected and virtual destructor 
   virtual ~MatchVeloL0Calo () ;
   // ========================================================================    
private:
   // ========================================================================    
   /// the default constructor is disabled 
   MatchVeloL0Calo () ;
   /// copy constructor is disabled 
   MatchVeloL0Calo           ( const MatchVeloL0Calo& ) ;
   /// assignement operator is disabled 
   MatchVeloL0Calo& operator=( const MatchVeloL0Calo& ) ;
   // ========================================================================    
private:
   // Properties
   double m_maxChi2;
} ; 
   // ==========================================================================
} // end of namespace Hlt
// ============================================================================
/*  standard constructor 
 *  @param type   tool type (?)
 *  @param name   tool instance name 
 *  @param parent tool parent 
 */
// ============================================================================
Hlt::MatchVeloL0Calo::MatchVeloL0Calo 
( const std::string& type   ,   // tool type ?
  const std::string& name   ,   // tool instance name 
  const IInterface*  parent )   // the parent 
   : base_class ( type , name , parent ) 
{
   declareProperty( "MaxMatchChi2", m_maxChi2 = 50 );
}  
// ============================================================================
// protected and virtual destructor 
// ============================================================================
Hlt::MatchVeloL0Calo::~MatchVeloL0Calo()
{

}
// ============================================================================
// the standard tool initialization 
// ============================================================================
StatusCode Hlt::MatchVeloL0Calo::initialize () 
{
   return GaudiTool::initialize();
}
// ============================================================================
// the standard tool finalization 
// ============================================================================
StatusCode Hlt::MatchVeloL0Calo::finalize   () 
{
   return GaudiTool::finalize();
}
// ============================================================================
/*  match a track with candidate 
 *  @see Hlt::ITrack2CandidateMatch
 *  @see ITrackMatch
 *  @param track     (INPUT)  the input track 
 *  @param candidate (INPUT)  the input candidate 
 *  @param matched   (OUTPUT) the matched track 
 *  @param quality   (OUTPUT) the matching quality
 *  @param quality2  (OUTPUT) the matching quality-2
 *  @return status code 
 */ 
// ============================================================================
StatusCode Hlt::MatchVeloL0Calo::match 
( const LHCb::Track&    track, 
  const Hlt::Candidate& candidate,
  LHCb::Track&          matched, 
  double&            /* quality  */,
  double&            /* quality2 */  ) const 
{
   // ==========================================================================
   // get L0Calo from Hlt-candidate 
   const LHCb::L0CaloCandidate* l0 = candidate.get<LHCb::L0CaloCandidate>() ;
   if ( 0 == l0 ) 
   { return Error ("HltCandidate is NOT L0Calo!") ; }

   //variables used later [from HltVeloEcalMatch constructor]
   double m_ptkickConstant = 1.263;
   double m_zKick    = 525.00;
   double m_eres0    = 0.60;
   double m_eres1    = 0.70;

   //get energy and position of L0 calo candidate [from HltVeloTCaloMatch::match]
   static const double s = double(4)/sqrt(double(12));
   double x      = l0->position().x()/Gaudi::Units::cm;
   double y      = l0->position().y()/Gaudi::Units::cm;
   double z      = l0->position().z()/Gaudi::Units::cm;
   double ex     = l0->posTol()*s/Gaudi::Units::cm;
   double ey     = l0->posTol()*s/Gaudi::Units::cm;
   double et     = l0->et()/Gaudi::Units::GeV;
   double e      = fabs(et) *( sqrt(x*x + y*y + z*z)/
                               sqrt(x*x + y*y));

   //matching [from HltVeloEcalMatch::match]                                            
   double matchChi2 = 10.e+10; 

   // get track slopes
   double trackDxDz = track.firstState().tx();
   double trackDyDz = track.firstState().ty();
   // Absolute energy uncertainty:
   double de = e*(sqrt( m_eres0*m_eres0 + m_eres1*m_eres1/e ));
   double deltaZ = z - m_zKick;
   double xkick  = deltaZ * m_ptkickConstant/e;
   double exkick = fabs( xkick/e)*de;
   // Calculate the slopes and their uncertainties:
   double edxdz  = sqrt(ex*ex + exkick*exkick)/z;
   double dydz   = y/z;
   double edydz  = ey/z;
   // loop for -1 and +1 charge
   double q[2]={-1.,1.};
   for (int i= 0; i< 2; ++i) {
      double dxdz = (x + q[i]*xkick)/z;
      // calculate chi2 
      double deltaX = q[i]*(dxdz - trackDxDz)/edxdz;
      double deltaY = (dydz/fabs(dydz))*(dydz - trackDyDz)/edydz;
      double chi2 = deltaX*deltaX + deltaY*deltaY;
      if (chi2 < matchChi2) {
         matchChi2      = chi2;
         if ( msgLevel(MSG::DEBUG) ) {
            debug() << "Best so far: q = " << q[i] << "\tchi2 = " << matchChi2
                    << " (" << deltaX*deltaX << " + " << deltaY*deltaY << ")" << endmsg;
         }
      } // end if chi2 < matchChi2
   } // end loop for -1 and +1 charge
   if ( matchChi2 < m_maxChi2 ) {
      matched.copy( track );
      return StatusCode::SUCCESS;
   } else {
      return StatusCode::FAILURE;
   }
}
// ============================================================================
/*  match a track with candidate ("filter-mode")
 *  @see Hlt::ITrack2CandidateMatch
 *  @param track    (INPUT)  the input track 
 *  @param candidat (INPUT)  the input candidate 
 *  @return true if track and candidate are "matched"
 */ 
// ============================================================================
bool Hlt::MatchVeloL0Calo::match 
( const LHCb::Track*    track       , 
  const Hlt::Candidate* candidate   ,
  const double       /* quality  */ , 
  const double       /* quality2 */ ) const 
{
   // 
   if ( 0 == track     ) 
   {
      Error( "LHCb::Track*    points to NULL, result false").ignore();
      return false ;                                                    // RETURN 
   }
   //
   if ( 0 == candidate ) 
   {
      Error( "Hlt::Candidate* points to NULL, result false").ignore();
      return false ;                                                    // RETURN 
   }
   //
   // get L0Calo from Hlt-candidate 
   const LHCb::L0CaloCandidate* l0 = candidate->get<LHCb::L0CaloCandidate>() ;
   if ( 0 == l0 ) 
   { 
      Error ("HltCandidate is NOT L0Calo!").ignore() ;
      return false ;                                                  // RETURN 
   }
   LHCb::Track matched_track;
   // if everything is fine, do the matching
   double q1 = 0., q2 = 0.;
   return match( *track, *candidate, matched_track, q1, q2 );
}
// ============================================================================
//                                                the factory for instantiation 
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(Hlt,MatchVeloL0Calo)
// ============================================================================
  
// ============================================================================
// The END 
// ============================================================================
