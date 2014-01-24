// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Hlt1Wrappers.h"
#include "LoKi/Constants.h"
// ============================================================================
// Local
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file
 *  Implementation file for classes from the file LoKi/Hlt1Wrappers.h
 *
 *  This file is part of LoKi project:
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2009-03-31
 */
// ============================================================================
// construct from the configuration tool
// ============================================================================
LoKi::Tracks::TrFunction::TrFunction( const LoKi::Hlt1::TrackFunction& tool )
    : LoKi::TrackTypes::TrFunc(), m_config( tool ), m_tool()
{
    init();
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Tracks::TrFunction* LoKi::Tracks::TrFunction::clone() const
{
    return new TrFunction( *this );
}
// ============================================================================
// initialize
// ============================================================================
void LoKi::Tracks::TrFunction::init()
{
    // get GaudiAlgorithm
    GaudiAlgorithm* alg = LoKi::Hlt1::Utils::getGaudiAlg( *this );
    Assert( 0 != alg, "GaudiAlgorithm points to NULL!" );
    // get IAlgorithm
    IAlgorithm* ialg = LoKi::Hlt1::Utils::getAlg( *this );
    Assert( alg == ialg, "GaudiAlgorithm is not *my* IAlgorithm" );

    // retrive the tool
    m_tool = m_config.pub() ? alg->tool<ITrackFunctionTool>( m_config.tool() )
                            : alg->tool<ITrackFunctionTool>( m_config.tool(), alg );

    /// it must be private tool!
    Assert( m_tool->parent() == alg, "ITrackFunctionTool tool must be private!" );
}
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Tracks::TrFunction::result_type LoKi::Tracks::TrFunction::
operator()( LoKi::Tracks::TrFunction::argument a ) const
{
    //
    if ( !a ) {
        Error( "LHCb::Track* points to NULL, return -inf" );
        return LoKi::Constants::NegativeInfinity;
    }
    //
    Assert( !( !m_tool ), "Invalid ITrackFunctionTool!" );
    //
    return m_tool->function( *a );
}
// ============================================================================
// OPTIONAL: nice printout
// ============================================================================
std::ostream& LoKi::Tracks::TrFunction::fillStream( std::ostream& s ) const
{
    return s << "TrFUN(" << m_tool << ")";
}
// ============================================================================

// // ============================================================================
// // construct from the configuration tool
// // ============================================================================
// LoKi::Tracks::TTrFunction::TTrFunction
// ( const LoKi::Hlt1::TrackBiFunction& tool )
//   : LoKi::TrackTypes::TTrFunc ()
//     , m_config ( tool )
//   , m_tool ()
// {
//   init () ;
// }
// // ============================================================================
// // MANDATORY: clone method ("virtual constructor")
// // ============================================================================
// LoKi::Tracks::TTrFunction*
// LoKi::Tracks::TTrFunction::clone() const
// { return new TTrFunction(*this) ; }
// // ============================================================================
// // initialize
// // ============================================================================
// void LoKi::Tracks::TTrFunction::init()
// {
//   // get GaudiAlgorithm
//   GaudiAlgorithm* alg = LoKi::Hlt1::Utils::getGaudiAlg ( *this ) ;
//   Assert ( 0 != alg , "GaudiAlgorithm points to NULL!" ) ;
//   // get IAlgorithm
//   IAlgorithm* ialg = LoKi::Hlt1::Utils::getAlg ( *this ) ;
//   Assert ( alg == ialg , "GaudiAlgorithm is not *my* IAlgorithm" ) ;

//   // retrive the tool
//   m_tool = m_config.pub() ?
//     alg->tool<ITrackBiFunctionTool> ( m_config.tool()       ) :
//     alg->tool<ITrackBiFunctionTool> ( m_config.tool() , alg ) ;

//   /// it must be private tool!
//   Assert ( m_tool->parent() == alg ,
//            "ITrackBiFunctionTool tool must be private!");

// }
// // ============================================================================
// // MANDATORY: the only one essential method
// // ============================================================================
// LoKi::Tracks::TTrFunction::result_type
// LoKi::Tracks::TTrFunction::operator()
//   ( LoKi::Tracks::TTrFunction::argument a ) const
// {
//   //
//   Assert ( !(!m_tool) , "Invalid ITrackBiFunctionTool!" ) ;
//   //
//   return m_tool->function ( a.first , a.second ) ;
// }
// // ============================================================================
// // OPTIONAL: nice printout
// // ============================================================================
// std::ostream&
// LoKi::Tracks::TTrFunction::fillStream ( std::ostream& s ) const
// { return s << "TTrFUN(" << m_tool << ")" ; }
// // ============================================================================

// // ============================================================================
// // construct from the configuration tool
// // ============================================================================
// LoKi::Tracks::TTrFunction2::TTrFunction2
// ( const LoKi::Hlt1::TrackBiFunction& tool )
//   : LoKi::Tracks::TTrFunction  (tool )
// {}
// // ============================================================================
// // MANDATORY: clone method ("virtual constructor")
// // ============================================================================
// LoKi::Tracks::TTrFunction2*
// LoKi::Tracks::TTrFunction2::clone() const
// { return new TTrFunction2(*this) ; }
// // ============================================================================
// // MANDATORY: the only one essential method
// // ============================================================================
// LoKi::Tracks::TTrFunction2::result_type
// LoKi::Tracks::TTrFunction2::operator()
//   ( LoKi::Tracks::TTrFunction2::argument a ) const
// {
//   //
//   Assert ( !(!m_tool) , "Invalid ITrackFunctionTool!" ) ;
//   //
//   return m_tool->function ( a.second , a.first ) ;
// }
// // ============================================================================
// // OPTIONAL: nice printout
// // ============================================================================
// std::ostream&
// LoKi::Tracks::TTrFunction2::fillStream ( std::ostream& s ) const
// { return s << "TTrFUN2(" << m_tool << ")" ; }
// // ============================================================================

// // ============================================================================
// // construct from the configuration tool
// // ============================================================================
// LoKi::Tracks::TrVFunction::TrVFunction
// ( const LoKi::Hlt1::TrackVertexFunction& tool )
//   : LoKi::TrackTypes::TrVFunc ()
//   , m_config ( tool )
//   , m_tool ()
// {
//   init () ;
// }
// // ============================================================================
// // MANDATORY: clone method ("virtual constructor")
// // ============================================================================
// LoKi::Tracks::TrVFunction*
// LoKi::Tracks::TrVFunction::clone() const
// { return new TrVFunction(*this) ; }
// // ============================================================================
// // initialize
// // ============================================================================
// void LoKi::Tracks::TrVFunction::init()
// {
//   // get GaudiAlgorithm
//   GaudiAlgorithm* alg = LoKi::Hlt1::Utils::getGaudiAlg ( *this ) ;
//   Assert ( 0 != alg , "GaudiAlgorithm points to NULL!" ) ;
//   // get IAlgorithm
//   IAlgorithm* ialg = LoKi::Hlt1::Utils::getAlg ( *this ) ;
//   Assert ( alg == ialg , "GaudiAlgorithm is not *my* IAlgorithm" ) ;

//   // retrive the tool
//   m_tool = m_config.pub() ?
//     alg->tool<ITrackVertexBiFunctionTool> ( m_config.tool()       ) :
//     alg->tool<ITrackVertexBiFunctionTool> ( m_config.tool() , alg ) ;

//   /// it must be private tool!
//   Assert ( m_tool->parent() == alg ,
//            "ITrackVertexBiFunctionTool tool must be private!");

// }
// // ============================================================================
// // MANDATORY: the only one essential method
// // ============================================================================
// LoKi::Tracks::TrVFunction::result_type
// LoKi::Tracks::TrVFunction::operator()
//   ( LoKi::Tracks::TrVFunction::argument a ) const
// {
//   //
//   Assert ( !(!m_tool) , "Invalid ITrackVertexBiFunctionTool!" ) ;
//   //
//   return m_tool->function ( a.first , a.second ) ;
// }
// // ============================================================================
// // OPTIONAL: nice printout
// // ============================================================================
// std::ostream&
// LoKi::Tracks::TrVFunction::fillStream ( std::ostream& s ) const
// { return s << "TrVFUN(" << m_tool << ")" ; }
// // ============================================================================

// ============================================================================
// The END
// ============================================================================
