// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL
// ============================================================================
#include <sstream>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/UpgradeConf.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hlt1::UpgradeConf
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @date 2008-11-12 
 *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$
 */
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::UpgradeConf::UpgradeConf 
( std::string         trTool   ,     //        ITrackFromTrack  tool name
  std::string         address  ,     //   TES location of upgraded tracks 
  LHCb::Track::Types  trType   ,     //                        track type 
  bool                owner    ,     //                           owner ? 
  bool                moveIDs  ,     //                    transfer IDs ? 
  bool                moveAncs ,     //            transfer anscendents ? 
  bool                moveInfo ,     //             transfer extra info ?
  bool                ptOrder  )     //                         order pt?
  : LoKi::Hlt1::TrackTool () 
  , m_trTool   { std::move(trTool)   }
  , m_address  { std::move(address)  }
  , m_trType   { trType   }
  , m_owner    { owner    }
  , m_moveIDs  { moveIDs  } 
  , m_moveAncs { moveAncs } 
  , m_moveInfo { moveInfo } 
  , m_ptOrder  { ptOrder  }
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::UpgradeConf::UpgradeConf 
( std::string         trTool   ,     //        ITrackFromTrack  tool name
  std::string         address  ,     //   TES location of upgraded tracks 
  LHCb::Track::Types  trType   ,     //                        track type 
  bool                owner    ,     //                           owner ? 
  bool                moveIDs  ,     //                    transfer IDs ? 
  bool                moveAncs ,     //            transfer anscendents ? 
  bool                moveInfo ,     //             transfer extra info ?
  bool                ptOrder  ,     //                         order pt?
  const LoKi::Functor<const LHCb::Track*,bool>& cut )   //       functor
  : LoKi::Hlt1::TrackTool ( cut ) 
  , m_trTool   { std::move(trTool)  } 
  , m_address  { std::move(address) }
  , m_trType   { trType   }
  , m_owner    { owner    }
  , m_moveIDs  { moveIDs  } 
  , m_moveAncs { moveAncs } 
  , m_moveInfo { moveInfo } 
  , m_ptOrder  { ptOrder  }
//
{}  
// ============================================================================
// output operator 
// ============================================================================
std::ostream& LoKi::Hlt1::UpgradeConf::fillStream ( std::ostream& s ) const 
{
  s << "LoKi.Hlt1.UpgradeConf('" << trTool  () << "'"
    << ",'"                      << address () << "'"
    << ",LHCb.Track."            << trType  ()
    << "," <<  ( owner    () ? "True" : "False" ) 
    << "," <<  ( moveIDs  () ? "True" : "False" ) 
    << "," <<  ( moveAncs () ? "True" : "False" ) 
    << "," <<  ( moveInfo () ? "True" : "False" ) 
    << "," <<  ( ptOrder  () ? "True" : "False" ) ;
  //
  if ( hasFilter() ) { s << ", " << filter() ; }
  //
  return s << " )" ;  
}
// ============================================================================
// conversion to string 
// ============================================================================
std::string LoKi::Hlt1::UpgradeConf::toString () const 
{ 
  std::ostringstream s ;
  fillStream ( s ) ;
  return s.str() ;
}
// ============================================================================
// The END 
// ============================================================================
