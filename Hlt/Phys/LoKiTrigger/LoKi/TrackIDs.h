// $Id: TrackIDs.h,v 1.2 2009-12-27 19:10:25 graven Exp $
// ============================================================================
#ifndef LOKI_TRACKIDS_H 
#define LOKI_TRACKIDS_H 1
// ============================================================================
// Include files
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/OTChannelID.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrackTypes.h"
// ============================================================================
/** @file LoKi/TrackIDs.h
 *  Collection of functors that deals with LHCbIDs for Tracks 
 *  (on request from Wouter Hulsbergen)
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-12-17
 */
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Tracks 
  {
    // ========================================================================
    /** @class CountIDs
     *  Simple functor to count LHCbIDs that satisfy certain criteria 
     *  @author  Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-12-17
     */
    class CountIDs : public LoKi::BasicFunctors<LHCb::Track>::Function
    {
    public:
      // ======================================================================
      // the actual type of pointer to member function
      typedef bool (LHCb::LHCbID::*PMF)() const ;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from pointer to member function 
      CountIDs ( PMF fun ) ;
      /// constructor from pointer to member function name  
      CountIDs ( const std::string& name ) ;
      /// MANDATORY: virtual destructor
      virtual ~CountIDs() ;
      /// MANDATORY: clone method ("virtual consttructor")
      virtual  CountIDs* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default contructor is disabled 
      CountIDs() ;                        // The default contructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the pointer to member function 
      PMF  m_pmf ;                            // the pointer to member function
      /// the actual fuinctor name 
      std::string m_nick ;                          // the actual fuinctor name
      // ======================================================================
    } ;
    // ========================================================================
    /** @class CountOTIDs
     *  Simple functor to count OTChannelID that satisfy certain criteria 
     *  @author  Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2009-12-17
     */
    class CountOTIDs : public LoKi::BasicFunctors<LHCb::Track>::Function
    {
    public:
      // ======================================================================
      // the actual type of pointer to member function
      typedef unsigned int (LHCb::OTChannelID::*PMF)() const ;

      // Uncomment the next line, and get a bad crash from Reflex...
      // in the global c'tor invoked when libLoKiTrigger.so is loaded.
      // BUT only if someother shared lib is loaded first...
      //typedef std::vector<unsigned int> UInts;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from pointer to member function 
      CountOTIDs ( PMF fun , const unsigned int i ) ;
      /// constructor from pointer to member function & list  
      CountOTIDs ( PMF fun , const std::vector<unsigned int>& i ) ;
      /// constructor from pointer to member function & range 
      CountOTIDs ( PMF                fun , 
                   const unsigned int imin , 
                   const unsigned int imax ) ;
      /// constructor from pointer to member function name  
      CountOTIDs ( const std::string& name , const unsigned int i ) ;
      /// constructor from pointer to member function name  & list 
      CountOTIDs ( const std::string& name , const std::vector<unsigned int>& i  ) ;
      /// constructor from pointer to member function name  & range
      CountOTIDs ( const std::string& nick , 
                   const unsigned int imin , 
                   const unsigned int imax ) ;
      /// MANDATORY: virtual destructor
      virtual ~CountOTIDs() ;
      /// MANDATORY: clone method ("virtual consttructor")
      virtual  CountOTIDs* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument t ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// The default contructor is disabled 
      CountOTIDs() ;                        // The default contructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the pointer to member function 
      PMF  m_pmf ;                            // the pointer to member function
      /// the list of values 
      std::vector<unsigned int>   m_uints ;  //       the list of values 
      /// the low edge 
      unsigned int m_imin ;                           // the low edge 
      /// the high edge 
      unsigned int m_imax ;                          // the high edge      
      /// the actual fuinctor name 
      std::string m_nick ;                          // the actual fuinctor name
      // ======================================================================
    } ;
    // ========================================================================
  } //                                            end of namespace LoKi::Tracks
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRACKIDS_H
// ============================================================================
