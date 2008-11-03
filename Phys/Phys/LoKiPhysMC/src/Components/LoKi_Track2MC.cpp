// $Id: LoKi_Track2MC.cpp,v 1.3 2008-11-03 18:35:49 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $, Version $Revision: 1.3 $
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2007/04/16 16:16:48  pkoppenb
// removed polemic comment
//
// Revision 1.1  2006/08/29 11:40:48  ibelyaev
//  many fixed to simplify MC-match
// 
// ===========================================================================
// Include files 
// ===========================================================================
// GaudiKernel
// ===========================================================================
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/DeclareFactoryEntries.h" 
// ===========================================================================
// GaudiAlg 
// ===========================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ===========================================================================
// Event 
// ===========================================================================
#include "Event/Track.h"
#include "Event/MCParticle.h"
// ===========================================================================
// Relations 
// ===========================================================================
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeighted.h"
#include "Relations/IRelationWeighted2D.h"
#include "Relations/RelationWeighted2D.h"
// ===========================================================================
// LoKiPhysMC 
// ===========================================================================
#include "Kernel/Track2MC.h"
// ===========================================================================
// LinkerEvent 
// ===========================================================================
#include "Linker/LinkedTo.h"
// ===========================================================================
/** @file
 *
 *  Simple algorithm for conversion of Track->MC links from 
 *  "linker" form into usable form of relation table 
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-08-17 
 */
// ============================================================================
namespace LoKi 
{
  /** @class Track2MC
   *
   *  Simple algorithm for conversion of Track->MC links from 
   *  "linker" form into usable form of relation table 
   *
   *  Many thanks to Edwin Bos for kind help!
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-08-17 
   */
  class Track2MC : public GaudiAlgorithm 
  {
    // ========================================================================
    friend class AlgFactory<LoKi::Track2MC> ;
    // ========================================================================
  public:
    // ========================================================================
    /// execution of the algorithm 
    virtual StatusCode execute() ;
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor 
    Track2MC
    ( const std::string& name , 
      ISvcLocator*       pSvc ) 
      : GaudiAlgorithm ( name , pSvc ) 
      , m_tracks () 
      , m_output (  LHCb::Track2MCLocation::Default ) 
    {
      //
      m_tracks.push_back ( LHCb::TrackLocation::Default ) ;
      m_tracks.push_back ( LHCb::TrackLocation::Velo    ) ;
      //
      declareProperty 
        ( "Tracks" ,
          m_tracks , 
          "The list of TES locations of Track->MC contariners/linkers") ;
      declareProperty 
        ( "Output" , 
          m_output ,
          "The TES locations of Track->MC realtiontable (LHcb::Track2MC)");
      //
      setProperty     ( "StatPrint"    , true ) ;
      //
    }
    /// virtual protected destructor 
    virtual ~Track2MC() {} ;
    // ========================================================================
  private:
    // ========================================================================
    /// default constructor is disabled 
    Track2MC () ;                            // default constructor is disabled 
    /// copy  constructor is disabled 
    Track2MC ( const Track2MC& );              // copy  constructor is disabled 
    /// assignement operator is disabled 
    Track2MC& operator=( const Track2MC& ); // assignement operator is disabled 
    // ========================================================================
  private:
    // ========================================================================
    typedef std::vector<std::string> Addresses ;
    /// addresses for the tracks 
    Addresses   m_tracks ;                          // addresses for the tracks 
    /// the address of the output table 
    std::string m_output ;                   // the address of the output table 
    // ========================================================================
  } ;
  // ==========================================================================
} // end of namespace LoKi
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY( LoKi, Track2MC );
// ============================================================================
#define INHERITS(T1,T2) \
     (Relations::IsConvertible<const T1*,const T2*>::value && \
     !Relations::IsConvertible<const T1*,const void*>::same)
// ============================================================================
// execution of the algorithm 
// ============================================================================
StatusCode LoKi::Track2MC::execute() 
{
  // avoid long names 
  typedef LHCb::RelationWeighted2D<LHCb::Track,LHCb::MCParticle,double> Table ;
  // check the inheritance scheme 
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::Track2MC2D));
  
  // create the new relation table and register it in TES 
  Table* table = new Table( 100 ) ;
  put ( table , m_output ) ;
  
  // for statistics 
  size_t nTracks = 0 ;
  // loop over all input track containers 
  for ( Addresses::const_iterator iaddr = 
          m_tracks.begin() ; m_tracks.end() != iaddr ; ++iaddr )
  {
    if ( !exist<LHCb::Tracks> ( *iaddr ) ) 
    {
      Warning ( " No tracks at location '"+ (*iaddr) + "' are found!") ;
      continue ;
    }
    const LHCb::Tracks* tracks = get<LHCb::Tracks>( *iaddr ) ;
    if ( 0 == tracks ) { continue ; }                                 // CONTINUE 
    nTracks += tracks->size() ;// Retrieve the Linker table made by the TrackAssociator
    LinkedTo<LHCb::MCParticle,LHCb::Track> linker ( evtSvc() , msgSvc(), *iaddr );
    if ( linker.notFound() ) 
    {
      Warning ( "The linker table '" + (*iaddr) + "' is not found!" ) ;
      continue ;                                                       // CONTINUE 
    }
    // loop over the tracks:
    for ( LHCb::Tracks::const_iterator itrack = tracks->begin() ; 
          tracks->end() != itrack ; ++itrack ) 
    {
      const LHCb::Track* track = *itrack ;
      if ( 0 == track ) { continue ; }                                // CONTINUE 
      // get the  links form linker object
      const LHCb::MCParticle* mcp = linker.first( track ) ;
      while ( 0 != mcp ) 
      {
        table->i_push ( track , mcp , linker.weight() ) ; // NB! i_push is used! 
        mcp = linker.next() ;
      }
      //
    } // end of the loop over tracks in the container    
  } // end of loop over containers
  
  /// MANDATORY usage of i_sort after i_push 
  table->i_sort() ;                                        // ATTENTION! 
  
  // check for some "strange" status 
  if ( table->i_relations().empty() ) { Warning ( "Empty relation table!") ; }
  
  // a bit of statistics 
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  {
    counter ( "#tracks"   ) += nTracks   ;
    counter ( "#links"    ) += table->i_relations().size() ;
  }
  //
  return StatusCode::SUCCESS;
} 
// ============================================================================
/// The END 
// ============================================================================
