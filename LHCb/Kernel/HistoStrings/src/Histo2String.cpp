// $Id: Histo2String.cpp,v 1.1.1.1 2009-10-22 09:17:02 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// ROOT 
// ============================================================================
#include "TH1D.h"
#include "TH2D.h"
#include "TH1F.h"
#include "TH2F.h"
// ============================================================================
// AIDA 
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// GaudiUtils
// ============================================================================
#include "GaudiUtils/Aida2ROOT.h"
// ============================================================================
// local
// ============================================================================
#include "Kernel/Histo2String.h"
#include "Kernel/HistoXML.h"
// ============================================================================
/** @file 
 *  Implementation file for utilities Histo -> string 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-10-21
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  template <class HISTO>
  std::ostream& _toStream_1D_
  ( const HISTO&   histo  , 
    std::ostream& stream , 
    const bool    asXML  )
  {
    if ( asXML ) { return Gaudi::Utils::Histos::toXml ( histo , stream ) ; }
    //
    stream << "{ " ;
    //
    stream << "'name'  : "  ;
    Gaudi::Utils::toStream ( std::string ( histo.GetName () ) , stream ) << " , " ;
    stream << "'title' : "  ;
    Gaudi::Utils::toStream ( std::string ( histo.GetTitle() ) , stream ) << " , " ;
    //
    const TAxis*       axis  = histo.GetXaxis() ;
    const unsigned int nBins = axis->GetNbins() ;
    if ( axis->IsVariableBinSize() ) 
    {
      const TArrayD*     xbins = axis->GetXbins() ;
      const unsigned int xsize = xbins->GetSize() ;
      std::vector<double> edges ;
      for  ( unsigned int iBin = 0 ; iBin < xsize ; ++iBin )
      { edges.push_back ( xbins->At( iBin ) ) ; }
      // the edges 
      stream << std::endl ;
      stream << "'edges' : "  ;
      Gaudi::Utils::toStream (  edges , stream ) << " ," << std::endl;
    }
    else 
    {
      stream << "'nbins' : "  ;
      Gaudi::Utils::toStream (  nBins            , stream ) << " , " ;
      stream << "'low'   : "  ;
      Gaudi::Utils::toStream (  axis->GetXmin () , stream ) << " , " ;
      stream << "'high'  : "  ;
      Gaudi::Utils::toStream (  axis->GetXmax () , stream ) << " , " << std::endl ;
    }
    // finally: the content 
    std::vector<std::pair<double,double> > bins ;
    for ( unsigned int iBin = 0 ; iBin <= nBins + 1 ; ++iBin ) 
    {
      bins.push_back ( std::make_pair( histo.GetBinContent( iBin ) , 
                                       histo.GetBinError  ( iBin ) ) ) ;
    }
    stream << "'bins'  : "  ;
    Gaudi::Utils::toStream ( bins , stream ) ;
    //
    stream << " }" ;
    //
    return stream ;
  }
  // ==========================================================================
  template <class HISTO>
  std::ostream& _toStream_2D_
  ( const HISTO&   histo , 
    std::ostream& stream , 
    const bool    asXML  )
  {
    if ( asXML ) { return Gaudi::Utils::Histos::toXml ( histo , stream ) ; }
    //
    stream << "{ " ;
    //
    stream << "'name'  : "  ;
    Gaudi::Utils::toStream ( std::string( histo.GetName () ) , stream ) << " , " ;
    stream << "'title' : "  ;
    Gaudi::Utils::toStream ( std::string( histo.GetTitle() ) , stream ) << " , " ;
    //
    const TAxis* xaxis = histo.GetXaxis() ;
    const int    xBins = xaxis->GetNbins() ;
    //
    stream << std::endl << "'X' : { " ;
    if ( xaxis->IsVariableBinSize() ) 
    {
      const TArrayD*     xbins = xaxis->GetXbins() ;
      const unsigned int xsize = xbins->GetSize() ;
      std::vector<double> edges ;
      for  ( unsigned int iBin = 0 ; iBin < xsize ; ++iBin )
      { edges.push_back ( xbins->At( iBin ) ) ; }
      // the edges 
      stream << "'edges' : "  ;
      Gaudi::Utils::toStream (  edges , stream ) << " }," << std::endl;
    }
    else 
    {
      stream << "'nbins' : "  ;
      Gaudi::Utils::toStream (  xBins            , stream ) << " , " ;
      stream << "'low'   : "  ;
      Gaudi::Utils::toStream (  xaxis->GetXmin () , stream ) << " , " ;
      stream << "'high'  : "  ;
      Gaudi::Utils::toStream (  xaxis->GetXmax () , stream ) << " }, " << std::endl ;
    }
    //
    const TAxis* yaxis = histo.GetYaxis() ;
    const int    yBins = yaxis->GetNbins() ;
    //
    stream << std::endl << "'Y' : { " ;
    if ( yaxis->IsVariableBinSize() ) 
    {
      const TArrayD*     ybins = yaxis->GetXbins() ;
      const unsigned int ysize = ybins->GetSize() ;
      std::vector<double> edges ;
      for  ( unsigned int iBin = 0 ; iBin < ysize ; ++iBin )
      { edges.push_back ( ybins->At( iBin ) ) ; }
      // the edges 
      stream << " 'edges' : "  ;
      Gaudi::Utils::toStream (  edges , stream ) << " }," << std::endl;
    }
    else 
    {
      stream << "'nbins' : "  ;
      Gaudi::Utils::toStream (  yBins            , stream ) << " , " ;
      stream << "'low'   : "  ;
      Gaudi::Utils::toStream (  yaxis->GetXmin () , stream ) << " , " ;
      stream << "'high'  : "  ;
      Gaudi::Utils::toStream (  yaxis->GetXmax () , stream ) << " }, " << std::endl ;
    }
    //
    // finally: the content 
    stream << "'bins' : " << std::endl << " [ " ;
    for ( int jBin = yBins + 1  ; jBin >= 0 ; --jBin ) 
    {
      if ( yBins + 1 != jBin ) { stream << std::endl ; }
      for ( int iBin = 0 ; iBin <= xBins + 1 ; ++iBin ) 
      {
        //
        Gaudi::Utils::toStream ( std::make_pair
                                 ( histo.GetBinContent( iBin , jBin ) , 
                                   histo.GetBinError  ( iBin , jBin ) ) , stream ) ;
        //
        if ( xBins + 1 != iBin || 0 != jBin ) { stream << " , " ; }
      }
    }
    stream << " ]" ;
    //
    stream << " }" ;
    //
    return stream ;
  }
  // ==========================================================================
} //                                                 end of anonymous namespace  
// ============================================================================
/*  stream the ROOT histogram into output stream 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 *  @param asXML  (INPUT)  use XML-format 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::toStream 
( const TH1D&   histo  , 
  std::ostream& stream , 
  const bool    asXML  ) { return _toStream_1D_ ( histo , stream , asXML ) ; }
// ============================================================================
/*  stream the ROOT histogram into output stream 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 *  @param asXML  (INPUT)  use XML-format 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::toStream 
( const TH1F&   histo  , 
  std::ostream& stream , 
  const bool    asXML  ) { return _toStream_1D_ ( histo , stream , asXML ) ; }
// ============================================================================
/*  stream the ROOT histogram into output stream 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 *  @param asXML  (INPUT)  use XML-format 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::toStream 
( const TH2D&   histo  , 
  std::ostream& stream , 
  const bool    asXML  ) { return _toStream_2D_ ( histo , stream , asXML ) ; }
// ============================================================================
/*  stream the ROOT histogram into output stream 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 *  @param asXML  (INPUT)  use XML-format 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::toStream 
( const TH2F&   histo  , 
  std::ostream& stream , 
  const bool    asXML  ) { return _toStream_2D_ ( histo , stream , asXML ) ; }
// ============================================================================
/*  stream the AIDA histogram into output stream 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 *  @param asXML  (INPUT)  use XML-format 
 */
// ============================================================================
std::ostream& Gaudi::Utils::toStream 
( const AIDA::IHistogram1D& histo  , 
  std::ostream&             stream , 
  const bool                asXML  ) 
{
  //
  AIDA::IHistogram1D* aida = const_cast<AIDA::IHistogram1D*> ( &histo ) ;
  //
  const TH1D* root = Gaudi::Utils::Aida2ROOT::aida2root ( aida ) ;
  if ( 0 == root ) { return stream ; }                                // RETURN 
  //
  return toStream  ( *root , stream , asXML ) ;
}
// ============================================================================
/*  stream the AIDA histogram into output stream 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 *  @param asXML  (INPUT)  use XML-format 
 */
// ============================================================================
std::ostream& Gaudi::Utils::toStream 
( const AIDA::IHistogram2D& histo  , 
  std::ostream&             stream , 
  const bool                asXML  ) 
{
  //
  AIDA::IHistogram2D* aida = const_cast<AIDA::IHistogram2D*> ( &histo ) ;
  //
  const TH2D* root = Gaudi::Utils::Aida2ROOT::aida2root ( aida ) ;
  if ( 0 == root ) { return stream ; }                                // RETURN 
  //
  return toStream  ( *root , stream , asXML ) ;
}
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// ============================================================================
std::string Gaudi::Utils::toString 
( const TH1D&               histo , 
  const bool                asXML ) 
{
  std::ostringstream o ;
  toStream ( histo , o , asXML ) ;
  return o.str() ;
}
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// ============================================================================
std::string Gaudi::Utils::toString 
( const TH1F&               histo , 
  const bool                asXML ) 
{
  std::ostringstream o ;
  toStream ( histo , o , asXML ) ;
  return o.str() ;
}
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// ============================================================================
std::string Gaudi::Utils::toString 
( const TH2D&               histo , 
  const bool                asXML ) 
{
  std::ostringstream o ;
  toStream ( histo , o , asXML ) ;
  return o.str() ;
}
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// ============================================================================
std::string Gaudi::Utils::toString 
( const TH2F&               histo , 
  const bool                asXML ) 
{
  std::ostringstream o ;
  toStream ( histo , o , asXML ) ;
  return o.str() ;
}
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// ============================================================================
std::string Gaudi::Utils::toString 
( const AIDA::IHistogram1D& histo , 
  const bool                asXML ) 
{
  std::ostringstream o ;
  toStream ( histo , o , asXML ) ;
  return o.str() ;
}    
// =============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// ============================================================================
std::string Gaudi::Utils::toString 
( const AIDA::IHistogram2D& histo , 
  const bool                asXML ) 
{
  std::ostringstream o ;
  toStream ( histo , o , asXML ) ;
  return o.str() ;
}    
// =============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// =============================================================================
std::string Gaudi::Utils::toString ( const AIDA::IHistogram1D* histo ) 
{
  if ( 0 == histo ) { return std::string ("{}") ; }
  return toString ( *histo ) ;
}
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// =============================================================================
std::string Gaudi::Utils::toString ( AIDA::IHistogram1D* histo ) 
{
  if ( 0 == histo ) { return std::string ("{}") ; }
  return toString ( *histo ) ;
}
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// =============================================================================
std::string Gaudi::Utils::toString ( const TH1D* histo ) 
{
  if ( 0 == histo ) { return std::string ("{}") ; }
  return toString ( *histo ) ;
}
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// =============================================================================
std::string Gaudi::Utils::toString ( const TH2D* histo ) 
{
  if ( 0 == histo ) { return std::string ("{}") ; }
  return toString ( *histo ) ;
}
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// =============================================================================
std::string Gaudi::Utils::toString ( TH1D* histo ) 
{
  if ( 0 == histo ) { return std::string ("{}") ; }
  return toString ( *histo ) ;
}
// ============================================================================
/*  convert the histogram into the string 
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param asXML  (INPUT)  use XML-format 
 *  @return the string representation of the histogram 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-26
 */
// =============================================================================
std::string Gaudi::Utils::toString ( TH2D* histo ) 
{
  if ( 0 == histo ) { return std::string ("{}") ; }
  return toString ( *histo ) ;
}
// ============================================================================
// The END 
// ============================================================================
