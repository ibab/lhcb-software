// $Id: HistoXML.cpp,v 1.1.1.1 2009-10-22 09:17:02 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD&STL 
// ============================================================================
#include <memory>
// ============================================================================
// ROOT
// ============================================================================
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TBufferXML.h"
// ============================================================================
// AIDA 
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IProfile2D.h"
// ============================================================================
// GaudiUtils 
// ============================================================================
#include "GaudiUtils/Aida2ROOT.h"
// ============================================================================
// local
// ============================================================================
#include "Kernel/HistoXML.h"
// ============================================================================
/** @file 
 *  Implementation fil for  histogram ,--> XML conversion functions 
 *  @date 2009-10-21 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /// parse the histogram from xml 
  template <class TYPE> 
  struct _Xml 
  {
    typedef std::auto_ptr<TYPE>  H ;
    //
    H operator() ( const std::string& input ) const 
    {
      // 1) use XML-parser 
      std::auto_ptr<TObject> obj   ( TBufferXML::ConvertFromXML ( input.c_str() ) ) ;
      if ( 0 == obj.get()   ) { return H() ; }      // RETURN
      H histo ( dynamic_cast<TYPE*>( obj.get() ) ) ;
      if ( 0 != histo.get() ) { obj.release() ; }
      return histo ;
    }
    //
  } ;
  // ==========================================================================
} //                                                 end of anonymous namespace
// ============================================================================
/*  stream the ROOT histogram into output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const TH1D&   histo         , 
  std::ostream& stream        ) 
{
  //
  const TObject* obj = &histo ;
  //
  TString s = TBufferXML::ConvertToXML ( const_cast<TObject*> ( obj ) ) ;
  // 
  return stream << s ;
}
// ============================================================================
/*  stream the ROOT histogram into output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const TH2D&   histo         , 
  std::ostream& stream        ) 
{
  //
  const TObject* obj = &histo ;
  //
  TString s = TBufferXML::ConvertToXML ( const_cast<TObject*> ( obj ) ) ;
  // 
  return stream << s ;
}
// ============================================================================
/*  stream the ROOT histogram into output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const TH3D&   histo         , 
  std::ostream& stream        ) 
{
  //
  const TObject* obj = &histo ;
  //
  TString s = TBufferXML::ConvertToXML ( const_cast<TObject*> ( obj ) ) ;
  // 
  return stream << s ;
}
// ============================================================================
/*  stream the ROOT histogram into output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const TH1F&   histo         , 
  std::ostream& stream        ) 
{
  //
  const TObject* obj = &histo ;
  //
  TString s = TBufferXML::ConvertToXML ( const_cast<TObject*> ( obj ) ) ;
  // 
  return stream << s ;
}
// ============================================================================
/*  stream the ROOT histogram into output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const TH2F&   histo         , 
  std::ostream& stream        ) 
{
  //
  const TObject* obj = &histo ;
  //
  TString s = TBufferXML::ConvertToXML ( const_cast<TObject*> ( obj ) ) ;
  // 
  return stream << s ;
}
// ============================================================================
/*  stream the ROOT histogram into output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const TH3F&   histo         , 
  std::ostream& stream        ) 
{
  //
  const TObject* obj = &histo ;
  //
  TString s = TBufferXML::ConvertToXML ( const_cast<TObject*> ( obj ) ) ;
  // 
  return stream << s ;
}
// ============================================================================
/*  stream the ROOT histogram into output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const TProfile& histo  , 
  std::ostream&   stream ) 
{
  //
  const TObject* obj = &histo ;
  //
  TString s = TBufferXML::ConvertToXML ( const_cast<TObject*> ( obj ) ) ;
  // 
  return stream << s ;
}
// ============================================================================
/*  stream the ROOT histogram into output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const TProfile2D& histo  , 
  std::ostream&     stream ) 
{
  //
  const TObject* obj = &histo ;
  //
  TString s = TBufferXML::ConvertToXML ( const_cast<TObject*> ( obj ) ) ;
  // 
  return stream << s ;
}
// ============================================================================
/* stream the ROOT histogram into the output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const AIDA::IHistogram1D& histo  , 
  std::ostream&             stream ) 
{
  // 
  AIDA::IHistogram1D* aida = const_cast<AIDA::IHistogram1D*> ( &histo ) ;
  //
  const TH1D* root = Gaudi::Utils::Aida2ROOT::aida2root ( aida ) ;
  if ( 0 == root ) { return stream ; }                                // RETURN 
  //
  return toXml  ( *root , stream ) ;
}
// ============================================================================
/* stream the ROOT histogram into the output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const AIDA::IHistogram2D& histo  , 
  std::ostream&             stream ) 
{
  // 
  AIDA::IHistogram2D* aida = const_cast<AIDA::IHistogram2D*> ( &histo ) ;
  //
  const TH2D* root = Gaudi::Utils::Aida2ROOT::aida2root ( aida ) ;
  if ( 0 == root ) { return stream ; }                                // RETURN 
  //
  return toXml  ( *root , stream ) ;
}
// ============================================================================
/* stream the ROOT histogram into the output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const AIDA::IHistogram3D& histo  , 
  std::ostream&             stream ) 
{
  // 
  AIDA::IHistogram3D* aida = const_cast<AIDA::IHistogram3D*> ( &histo ) ;
  //
  const TH3D* root = Gaudi::Utils::Aida2ROOT::aida2root ( aida ) ;
  if ( 0 == root ) { return stream ; }                                // RETURN 
  //
  return toXml  ( *root , stream ) ;
}
// ============================================================================
/* stream the ROOT histogram into the output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const AIDA::IProfile1D&   histo  , 
  std::ostream&             stream ) 
{
  // 
  AIDA::IProfile1D* aida = const_cast<AIDA::IProfile1D*> ( &histo ) ;
  //
  const TProfile* root = Gaudi::Utils::Aida2ROOT::aida2root ( aida ) ;
  if ( 0 == root ) { return stream ; }                                // RETURN 
  //
  return toXml  ( *root , stream ) ;
}
// ============================================================================
/* stream the ROOT histogram into the output stream as XML
 *  @param histo  (INPUT)  the histogram to be streamed 
 *  @param stream (OUTPUT) the stream 
 */  
// ============================================================================
std::ostream& Gaudi::Utils::Histos::toXml 
( const AIDA::IProfile2D&   histo  , 
  std::ostream&             stream ) 
{
  // 
  AIDA::IProfile2D* aida = const_cast<AIDA::IProfile2D*> ( &histo ) ;
  //
  const TProfile2D* root = Gaudi::Utils::Aida2ROOT::aida2root ( aida ) ;
  if ( 0 == root ) { return stream ; }                                // RETURN 
  //
  return toXml  ( *root , stream ) ;
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TH1D& result , const std::string& input )
{
  //
  result.Reset() ;                                 // RESET old histogram 
  //
  _Xml<TH1D> _xml ;
  std::auto_ptr<TH1D> histo =  _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }        // RETURN 
  //
  result.Reset() ;
  histo->Copy ( result ) ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TH2D& result , const std::string& input )
{
  //
  result.Reset() ;                                 // RESET old histogram 
  //
  _Xml<TH2D> _xml ;
  std::auto_ptr<TH2D> histo = _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }        // RETURN 
  //
  result.Reset() ;
  histo->Copy ( result ) ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TH3D& result , const std::string& input )
{
  //
  result.Reset() ;                                 // RESET old histogram 
  //
  _Xml<TH3D> _xml ;
  std::auto_ptr<TH3D> histo = _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }        // RETURN 
  //
  result.Reset() ;
  histo->Copy ( result ) ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TH1F& result , const std::string& input )
{
  //
  result.Reset() ;                                 // RESET old histogram 
  //
  _Xml<TH1F> _xml ;
  std::auto_ptr<TH1F> histo =  _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }        // RETURN 
  //
  result.Reset() ;
  histo->Copy ( result ) ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TH2F& result , const std::string& input )
{
  //
  result.Reset() ;                                 // RESET old histogram 
  //
  _Xml<TH2F> _xml ;
  std::auto_ptr<TH2F> histo = _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }        // RETURN 
  //
  result.Reset() ;
  histo->Copy ( result ) ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TH3F& result , const std::string& input )
{
  //
  result.Reset() ;                                 // RESET old histogram 
  //
  _Xml<TH3F> _xml ;
  std::auto_ptr<TH3F> histo = _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }        // RETURN 
  //
  result.Reset() ;
  histo->Copy ( result ) ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TProfile& result , const std::string& input )
{
  //
  result.Reset() ;                                 // RESET old histogram 
  //
  _Xml<TProfile> _xml ;
  std::auto_ptr<TProfile> histo = _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }        // RETURN 
  //
  result.Reset() ;
  histo->Copy ( result ) ;
  //
  return StatusCode::SUCCESS ;  
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TProfile2D& result , const std::string& input )
{
  //
  result.Reset() ;                                 // RESET old histogram 
  //
  _Xml<TProfile2D> _xml ;
  std::auto_ptr<TProfile2D> histo = _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }        // RETURN 
  //
  result.Reset() ;
  histo->Copy ( result ) ;
  //
  return StatusCode::SUCCESS ;  
}


// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TH1D*& result , const std::string& input )
{
  if ( 0 != result ) { return fromXml ( *result , input ) ; }
  //
  _Xml<TH1D> _xml ;
  std::auto_ptr<TH1D> histo = _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }       // RETURN 
  //
  result = histo.release() ;                                     // ASSIGN
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TH2D*& result , const std::string& input )
{
  if ( 0 != result ) { return fromXml ( *result , input ) ; }
  //
  _Xml<TH2D> _xml ;
  std::auto_ptr<TH2D> histo = _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }       // RETURN 
  //
  result = histo.release() ;                                     // ASSIGN
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TH3D*& result , const std::string& input )
{
  if ( 0 != result ) { return fromXml ( *result , input ) ; }
  //
  _Xml<TH3D> _xml ;
  std::auto_ptr<TH3D> histo = _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }       // RETURN 
  //
  result = histo.release() ;                                     // ASSIGN
  //
  return StatusCode::SUCCESS ;
}

// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TProfile*& result , const std::string& input )
{
  if ( 0 != result ) { return fromXml ( *result , input ) ; }
  //
  _Xml<TProfile> _xml ;
  std::auto_ptr<TProfile> histo = _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }       // RETURN 
  //
  result = histo.release() ;                                     // ASSIGN
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( TProfile2D*& result , const std::string& input )
{
  if ( 0 != result ) { return fromXml ( *result , input ) ; }
  //
  _Xml<TProfile2D> _xml ;
  std::auto_ptr<TProfile2D> histo = _xml ( input ) ;
  if ( 0 == histo.get() ) { return StatusCode::FAILURE ; }       // RETURN 
  //
  result = histo.release() ;                                     // ASSIGN
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( AIDA::IHistogram1D& result , const std::string& input )
{
  TH1D* root = Gaudi::Utils::Aida2ROOT::aida2root ( &result ) ;
  if ( 0 == root ) { return StatusCode::FAILURE ; }   // RETURN 
  //
  return fromXml ( *root , input ) ;                  // RETURN 
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( AIDA::IHistogram2D& result , const std::string& input )
{
  TH2D* root = Gaudi::Utils::Aida2ROOT::aida2root ( &result ) ;
  if ( 0 == root ) { return StatusCode::FAILURE ; }   // RETURN 
  //
  return fromXml ( *root , input ) ;                  // RETURN 
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( AIDA::IHistogram3D& result , const std::string& input )
{
  TH3D* root = Gaudi::Utils::Aida2ROOT::aida2root ( &result ) ;
  if ( 0 == root ) { return StatusCode::FAILURE ; }   // RETURN 
  //
  return fromXml ( *root , input ) ;                  // RETURN 
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( AIDA::IProfile1D& result , const std::string& input )
{
  TProfile* root = Gaudi::Utils::Aida2ROOT::aida2root ( &result ) ;
  if ( 0 == root ) { return StatusCode::FAILURE ; }   // RETURN 
  //
  return fromXml ( *root , input ) ;                  // RETURN 
}
// ============================================================================
/*  parse the histogram from standard ROOT XML 
 *  @param result (OUTPUT)  the parsed histogram 
 *  @param input  (INPUT)   the input XML string 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Utils::Histos::fromXml 
( AIDA::IProfile2D& result , const std::string& input )
{
  TProfile2D* root = Gaudi::Utils::Aida2ROOT::aida2root ( &result ) ;
  if ( 0 == root ) { return StatusCode::FAILURE ; }   // RETURN 
  //
  return fromXml ( *root , input ) ;                  // RETURN 
}
// ============================================================================
// The END 
// ============================================================================
