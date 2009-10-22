// $Id: HistoStrings.cpp,v 1.1.1.1 2009-10-22 09:17:02 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <sstream>
// ============================================================================
// local
// ============================================================================
#include "Kernel/HistoStrings.h"
#include "Kernel/Histo2String.h"
#include "Kernel/HistoParsers.h"
#include "Kernel/HistoXML.h"
// ============================================================================
/** @file 
 *  Implementation file for utilities Histo -> string 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-10-21
 */
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// ============================================================================
std::string Gaudi::Utils::Histos::HistoStrings::toString 
( const TH1D&               histo , 
  const bool                asXML ) 
{ return Gaudi::Utils::toString ( histo , asXML ) ; }
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// ============================================================================
std::string Gaudi::Utils::Histos::HistoStrings::toString 
( const TH2D&               histo , 
  const bool                asXML ) 
{ return Gaudi::Utils::toString ( histo , asXML ) ; }
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// ============================================================================
std::string Gaudi::Utils::Histos::HistoStrings::toString 
( const AIDA::IHistogram1D& histo , 
  const bool                asXML ) 
{ return Gaudi::Utils::toString ( histo , asXML ) ; }
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// ============================================================================
std::string Gaudi::Utils::Histos::HistoStrings::toString 
( const AIDA::IHistogram2D& histo , 
  const bool                asXML ) 
{ return Gaudi::Utils::toString ( histo , asXML ) ; }
// ============================================================================
/*  parse the histogram from string
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::HistoStrings::fromString
( TH1D& result , const std::string& input ) 
{ return Gaudi::Parsers::parse ( result , input ) ; }
// ============================================================================
/*  parse the histogram from string
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::HistoStrings::fromString
( TH2D& result , const std::string& input ) 
{ return Gaudi::Parsers::parse ( result , input ) ; }
// ============================================================================
/*  parse the histogram from string
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::HistoStrings::fromString
( AIDA::IHistogram1D& result , const std::string& input ) 
{ return Gaudi::Parsers::parse ( result , input ) ; }
// ============================================================================
/*  parse the histogram from string
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::HistoStrings::fromString
( AIDA::IHistogram2D& result , const std::string& input ) 
{ return Gaudi::Parsers::parse ( result , input ) ; }
// ============================================================================
namespace 
{
  // ==========================================================================
  template <class TYPE>
  std::string _toXml ( const TYPE& object ) 
  {
    std::ostringstream s ;
    Gaudi::Utils::Histos::toXml ( object , s ) ;
    return s.str() ;
  }
  // ==========================================================================
  template <class TYPE>
  StatusCode _fromXml ( TYPE& object , const std::string& input ) 
  {
    return Gaudi::Utils::Histos::fromXml ( object , input ) ;
  }
  // ==========================================================================
} //                                                end of anonymous namespace 
// ============================================================================
std::string Gaudi::Utils::Histos::HistoStrings::toXml
( const TH1D&               histo ) { return _toXml ( histo ) ; }
std::string Gaudi::Utils::Histos::HistoStrings::toXml
( const TH2D&               histo ) { return _toXml ( histo ) ; }
std::string Gaudi::Utils::Histos::HistoStrings::toXml
( const TH3D&               histo ) { return _toXml ( histo ) ; }
std::string Gaudi::Utils::Histos::HistoStrings::toXml
( const TProfile&           histo ) { return _toXml ( histo ) ; }
std::string Gaudi::Utils::Histos::HistoStrings::toXml
( const TProfile2D&         histo ) { return _toXml ( histo ) ; }
// ============================================================================
std::string Gaudi::Utils::Histos::HistoStrings::toXml
( const AIDA::IHistogram1D& histo ) { return _toXml ( histo ) ; }
std::string Gaudi::Utils::Histos::HistoStrings::toXml
( const AIDA::IHistogram2D& histo ) { return _toXml ( histo ) ; }
std::string Gaudi::Utils::Histos::HistoStrings::toXml
( const AIDA::IHistogram3D& histo ) { return _toXml ( histo ) ; }
std::string Gaudi::Utils::Histos::HistoStrings::toXml
( const AIDA::IProfile1D&   histo ) { return _toXml ( histo ) ; }
std::string Gaudi::Utils::Histos::HistoStrings::toXml
( const AIDA::IProfile2D&   histo ) { return _toXml ( histo ) ; }
// ============================================================================
StatusCode Gaudi::Utils::Histos::HistoStrings::fromXml 
( TH1D&               result , const std::string& input ) 
{ return _fromXml ( result , input ) ; }
StatusCode Gaudi::Utils::Histos::HistoStrings::fromXml 
( TH2D&               result , const std::string& input ) 
{ return _fromXml ( result , input ) ; }
StatusCode Gaudi::Utils::Histos::HistoStrings::fromXml 
( TH3D&               result , const std::string& input ) 
{ return _fromXml ( result , input ) ; }
StatusCode Gaudi::Utils::Histos::HistoStrings::fromXml 
( TProfile&           result , const std::string& input ) 
{ return _fromXml ( result , input ) ; }
StatusCode Gaudi::Utils::Histos::HistoStrings::fromXml 
( TProfile2D&         result , const std::string& input ) 
{ return _fromXml ( result , input ) ; }
// ============================================================================
StatusCode Gaudi::Utils::Histos::HistoStrings::fromXml 
( AIDA::IHistogram1D& result , const std::string& input ) 
{ return _fromXml ( result , input ) ; }
StatusCode Gaudi::Utils::Histos::HistoStrings::fromXml 
( AIDA::IHistogram2D& result , const std::string& input ) 
{ return _fromXml ( result , input ) ; }
StatusCode Gaudi::Utils::Histos::HistoStrings::fromXml 
( AIDA::IHistogram3D& result , const std::string& input ) 
{ return _fromXml ( result , input ) ; }
StatusCode Gaudi::Utils::Histos::HistoStrings::fromXml 
( AIDA::IProfile1D&   result , const std::string& input ) 
{ return _fromXml ( result , input ) ; }
StatusCode Gaudi::Utils::Histos::HistoStrings::fromXml 
( AIDA::IProfile2D&   result , const std::string& input ) 
{ return _fromXml ( result , input ) ; }
// ============================================================================




// ============================================================================
// The END 
// ============================================================================
