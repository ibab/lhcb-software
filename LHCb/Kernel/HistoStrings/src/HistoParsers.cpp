// $Id: HistoParsers.cpp,v 1.1.1.1 2009-10-22 09:17:02 ibelyaev Exp $
// =============================================================================
// Include files 
// =============================================================================
// STD & STL 
// =============================================================================
#include <map>
#include <memory>
// =============================================================================
// AIDA 
// ============================================================================
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"
// ============================================================================
// Boost
// ============================================================================
#include "boost/bind.hpp"
// ============================================================================
// ROOT 
// ============================================================================
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TAxis.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Grammars.h"
// ============================================================================
// GaudiUtils 
// ============================================================================
#include "GaudiUtils/Aida2ROOT.h"
// ============================================================================
// local
// ============================================================================
#include "Kernel/HistoParsers.h"
#include "Kernel/HistoXML.h"
// ============================================================================
// Local 
// ============================================================================
#include "H1.h"
// ============================================================================

namespace Gaudi
{
  // ==========================================================================
  namespace Parsers
  {
    // ========================================================================
    /** @class EdgeGrammar 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-26
     */
    class EdgeGrammar : public grammar 
    <EdgeGrammar,ClosureGrammar<Edges>::context_t>
    {
      // ======================================================================
    public:
      // ======================================================================
      typedef Edges ResultT;
      // ======================================================================
    public:
      // ======================================================================
      template <class ScannerT>
      struct definition
      {
        definition ( const EdgeGrammar& top ) 
        {
          inner = 
            ( ( str_p("edges") | "'edges'" | "\"edges\"" ) 
              >> ":" >> edges  [ top.val *= arg1 ] )
            | ( 
               ( str_p("nbins")  | "'nbins'"  | "\"nbins\""  ) 
               >> ":" >> nbins [ top.val /= arg1 ]  >> ',' >>
               ( str_p("low")    | "'low'"    | "\"low\""    ) 
               >> ":" >> low   [ top.val -= arg1 ]  >> ',' >>
               ( str_p("high")   | "'high'"   | "\"high\""   ) 
               >> ":" >> high  [ top.val += arg1 ]  
               ) ;
          
          result =            
            ( str_p("{") >> inner >> '}' ) |
            ( str_p("(") >> inner >> ')' ) |
            ( str_p("[") >> inner >> ']' ) | inner ;
        }
        //
        rule<ScannerT> const& start() const { return result ; }
        //
        VectorGrammar< RealGrammar< double > > edges  ;
        RealGrammar<double>                    low    ;
        RealGrammar<double>                    high   ;
        IntGrammar<unsigned int>               nbins  ;
        rule<ScannerT>                         inner  ;
        rule<ScannerT>                         result ;
      };  
      // ======================================================================
    } ;
    // ========================================================================
    class H1Grammar : public grammar 
    <H1Grammar,ClosureGrammar<H1>::context_t>
    {
      // ======================================================================
    public:
      // ======================================================================
      typedef H1 ResultT;
      // ======================================================================
    public:
      // ======================================================================
      template <class ScannerT>
      struct definition
      {
        definition ( const H1Grammar& top ) 
        {
          inner = 
            ( str_p("name")  | "'name'"  | "\"name\""  ) 
            >> ":" >> name   [ top.val *= arg1 ] >> ',' >>
            ( str_p("title") | "'title'" | "\"title\"" ) 
            >> ":" >> title  [ top.val /= arg1 ] >> ',' >> 
            !( ( str_p("X") | "'X'" | "\"X\"" | "x" | "'x'" | "\"x\"" ) >> ':') 
            >> edges [ top.val &= arg1 ] >> ',' >> 
            ( str_p("bins") | "'bins'" | "\"bins\"" ) 
            >> ':' >> bins  [ top.val += arg1 ] ;
          
          result =            
            ( str_p("{") >> inner >> '}' ) |
            ( str_p("(") >> inner >> ')' ) |
            ( str_p("[") >> inner >> ']' ) | inner ;
        }
        //
        rule<ScannerT> const& start() const { return result ; }
        //
        
        StringGrammar   name  ;
        StringGrammar   title ;
        EdgeGrammar     edges ;
        VectorGrammar< PairGrammar<RealGrammar<double>,RealGrammar<double> > > bins ;
        rule<ScannerT>                         inner  ;
        rule<ScannerT>                         result ;
      };  
      // ======================================================================
    } ;
    // ========================================================================
    class H2Grammar : public grammar 
    <H2Grammar,ClosureGrammar<H2>::context_t>
    {
      // ======================================================================
    public:
      // ======================================================================
      typedef H2 ResultT;
      // ======================================================================
    public:
      // ======================================================================
      template <class ScannerT>
      struct definition
      {
        definition ( const H2Grammar& top ) 
        {
          inner = 
            ( str_p("name")  | "'name'"  | "\"name\""  ) 
            >> ":" >> name   [ top.val *= arg1 ] >> ',' >>
            ( str_p("title") | "'title'" | "\"title\"" ) 
            >> ":" >> title  [ top.val /= arg1 ] >> ',' >> 
            !( ( str_p("X") | "'X'" | "\"X\"" | "x" | "'x'" | "\"x\"" ) >> ':') 
            >> edges [ top.val &= arg1 ] >> ',' >>
            !( ( str_p("Y") | "'Y'" | "\"Y\"" | "y" | "'y'" | "\"y\"" ) >> ':') 
            >> edges [ top.val |= arg1 ] >> ',' >> 
            ( str_p("bins") | "'bins'" | "\"bins\"" ) 
            >> ':' >> bins  [ top.val += arg1 ] ;
          
          result =            
            ( str_p("{") >> inner >> '}' ) |
            ( str_p("(") >> inner >> ')' ) |
            ( str_p("[") >> inner >> ']' ) | inner ;
        }
        //
        rule<ScannerT> const& start() const { return result ; }
        //
        
        StringGrammar   name  ;
        StringGrammar   title ;
        EdgeGrammar     edges ;
        VectorGrammar< PairGrammar<RealGrammar<double>,RealGrammar<double> > > bins ;
        rule<ScannerT>                         inner  ;
        rule<ScannerT>                         result ;
      };  
      // ======================================================================
    } ;
    // ========================================================================
  } //                                          end of namespace Gaudi::Parsers 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
namespace 
{
  // ==========================================================================
  /// parse the histogram 
  StatusCode _parse ( H1& h1 , const std::string& input ) 
  {
    //
    using namespace Gaudi::Parsers ;
    //
    H1Grammar gr ;
    // check the parsing
    StatusCode sc  = 
      parse ( input.begin() , input.end() ,
              gr [var(h1) = arg1 ] ,
              SkipperGrammar() ).full  ;
    
    if ( sc.isFailure () ){ return sc ; }                  // REUTRN 
    
    //
    return h1.ok() ? StatusCode::SUCCESS : StatusCode::FAILURE ;
  }
  // ==========================================================================
  /// parse the histogram 
  StatusCode _parse ( H2& h2 , const std::string& input ) 
  {
    //
    using namespace Gaudi::Parsers ;
    //
    H2Grammar gr ;
    // check the parsing
    StatusCode sc  = 
      parse ( input.begin() , input.end() ,
              gr [var(h2) = arg1 ] ,
              SkipperGrammar() ).full  ;

    //
    if ( sc.isFailure () ){ return sc ; }                  // REUTRN 
    
    //
    return h2.ok() ? StatusCode::SUCCESS : StatusCode::FAILURE ;
  }
  // ==========================================================================
  typedef std::auto_ptr<TH1D>   H1P ;
  // ==========================================================================
  H1P _parse_1D ( const std::string& input , std::string& name ) 
  {
    //
    // 1) parse the custom format
    //
    H1        h1 ;
    StatusCode sc = _parse ( h1 , input ) ;
    if ( sc.isFailure() || !h1.ok() ) { return H1P() ; }   // RETURN
    //
    // 2) create the histogram 
    //
    H1P histo 
      ( h1.m_edges.edges.empty() ?          // FIXED binning? 
        new TH1D ( "" , //h1.m_name.c_str   ()         ,           // NAME  
                   h1.m_title.c_str() ,           // TITLE 
                   h1.m_edges.nbins   ,           // #bins 
                   h1.m_edges.low     ,           // low edge 
                   h1.m_edges.high    ) :         // high  edge 
        new TH1D ( "" , // h1.m_name .c_str ()          ,     // NAME 
                   h1.m_title .c_str()  ,            // TITLE 
                   h1.m_edges.edges.size() -1  ,     // #bins 
                   &h1.m_edges.edges.front()   ) ) ; // vector of edges 
    
    // fill the histogram 
    for ( unsigned int ibin = 0 ; ibin < h1.m_bins.size() ; ++ibin ) 
    {
      histo -> SetBinContent ( ibin , h1.m_bins[ibin].first  ) ;
      histo -> SetBinError   ( ibin , h1.m_bins[ibin].second ) ;
    }  
    //
    name = h1.m_name ;
    //
    return histo ;
  }
  // ==========================================================================
  typedef std::auto_ptr<TH2D>   H2P ;
  // ==========================================================================
  H2P _parse_2D ( const std::string& input , std::string& name ) 
  {
    //
    // 1) parse the custom format
    //
    H2        h2 ;
    StatusCode sc = _parse ( h2 , input ) ;
    if ( sc.isFailure() || !h2.ok() ) { return H2P() ; }   // RETURN
    //
    // 2) create the histogram 
    //
    H2P histo 
      ( h2.m_xedges.edges.empty() && 
        h2.m_yedges.edges.empty()     ?            // FIXED binning? 
        new TH2D ( "" , //h1.m_name.c_str   ()         ,           // NAME  
                   h2.m_title.c_str() ,            // TITLE 
                   h2.m_xedges.nbins   ,           // #bins 
                   h2.m_xedges.low     ,           // low edge 
                   h2.m_xedges.high    ,           // high edge 
                   h2.m_yedges.nbins   ,           // #bins 
                   h2.m_yedges.low     ,           // low edge 
                   h2.m_yedges.high    ) :
        h2.m_xedges.edges.empty() && !h2.m_xedges.edges.empty() ?
        new TH2D ( "" , //h1.m_name.c_str   ()         ,           // NAME  
                   h2.m_title.c_str() ,            // TITLE 
                   h2.m_xedges.nbins   ,           // #bins 
                   h2.m_xedges.low     ,           // low edge 
                   h2.m_xedges.high    ,           // high edge 
                   h2.m_yedges.nBins() ,           // #bins 
                   &h2.m_yedges.edges.front() ) : // vector of edges 
        !h2.m_xedges.edges.empty() && h2.m_xedges.edges.empty() ?
        new TH2D ( "" , //h1.m_name.c_str   ()         ,           // NAME  
                   h2.m_title.c_str() ,            // TITLE 
                   h2.m_xedges.nBins() ,           // #bins 
                   &h2.m_xedges.edges.front() ,    // vector of edges 
                   h2.m_yedges.nbins   ,           // #bins 
                   h2.m_yedges.low     ,           // low edge 
                   h2.m_yedges.high    )        :         // high edge 
        new TH2D ( "" , //h1.m_name.c_str   ()         ,           // NAME  
                   h2.m_title.c_str() ,            // TITLE 
                   h2.m_xedges.nBins() ,           // #bins 
                   &h2.m_xedges.edges.front() ,    // vector of edges 
                   h2.m_yedges.nBins() ,           // #bins 
                   &h2.m_yedges.edges.front() ) ) ; // vector of edges
    
    int ibin = 0 ;
    const int xBins = h2.m_xedges.nBins() ;
    const int yBins = h2.m_yedges.nBins() ;
    
    for ( int jBin = yBins + 1 ; jBin >= 0 ; --jBin ) 
    {
      for ( int iBin = 0 ; iBin <= xBins + 1  ; ++iBin ) 
      {
        histo -> SetBinContent ( iBin , jBin , h2.m_bins[ibin].first  ) ;
        histo -> SetBinError   ( iBin , jBin , h2.m_bins[ibin].second ) ;
        ++ibin ;
      }
    }
    //
    name = h2.m_name ;
    //
    return histo ;
  }
  // ==========================================================================
} //                                                 end of anonymous namespace 
// ============================================================================
/*  parse ROOT histogram from text representation
 *  @param result (OUTPUT) the histogram
 *  @param input  (INPUT)  the input to be parsed 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Parsers::parse 
( TH1D& result , const std::string& input ) 
{
  // 1) check the parsing
  std::string name ;
  H1P h1 = _parse_1D ( input , name ) ;
  if ( 0 != h1.get() ) 
  {
    result.Reset() ;
    h1->Copy ( result )  ;                            // ASSIGN
    result.SetName ( name.c_str () ) ;
    return StatusCode::SUCCESS ;                           // RETURN 
  }
  //
  // XML-like text?
  if ( std::string::npos != input.find('<') ) 
  { return Gaudi::Utils::Histos::fromXml ( result , input ) ; }
  //
  return StatusCode::FAILURE ;
}
// ============================================================================
/*  parse ROOT histogram from text representation
 *  @param result (OUTPUT) the histogram
 *  @param input  (INPUT)  the input to be parsed 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Parsers::parse 
( TH2D& result , const std::string& input ) 
{
  // 1) check the parsing
  std::string name ;
  H2P h2 = _parse_2D ( input , name ) ;
  if ( 0 != h2.get() ) 
  {
    result.Reset() ;
    h2->Copy ( result )  ;                            // ASSIGN
    result.SetName ( name.c_str () ) ;
    return StatusCode::SUCCESS ;                           // RETURN 
  }
  //
  // XML-like text?
  if ( std::string::npos != input.find('<') ) 
  { return Gaudi::Utils::Histos::fromXml ( result , input ) ; }
  //
  return StatusCode::FAILURE ;
}
// ============================================================================
/*  parse ROOT histogram from text representation
 *  @param result (OUTPUT) the histogram
 *  @param input  (INPUT)  the input to be parsed 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Parsers::parse 
( TH1D*& result , const std::string& input ) 
{
  if ( 0 != result ) { return parse ( *result , input ) ; } // RETURN  
  
  // 1) check the parsing 
  std::string name ;
  H1P h1 = _parse_1D  ( input , name ) ;
  if ( 0 != h1.get() ) 
  {
    result = h1.release() ; 
    result->SetName ( name.c_str() ) ;
    return StatusCode::SUCCESS ;                           // RETURN 
  }
  //
  // XML-like text?
  if ( std::string::npos != input.find('<') ) 
  { return Gaudi::Utils::Histos::fromXml ( result , input ) ; }
  //
  return StatusCode::FAILURE ;
}
// ============================================================================
/*  parse ROOT histogram from text representation
 *  @param result (OUTPUT) the histogram
 *  @param input  (INPUT)  the input to be parsed 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Parsers::parse 
( TH2D*& result , const std::string& input ) 
{
  if ( 0 != result ) { return parse ( *result , input ) ; } // RETURN  
  
  // 1) check the parsing 
  std::string name ;
  H2P h2 = _parse_2D  ( input , name ) ;
  if ( 0 != h2.get() ) 
  {
    result = h2.release() ; 
    result->SetName ( name.c_str() ) ;
    return StatusCode::SUCCESS ;                           // RETURN 
  }
  //
  // XML-like text?
  if ( std::string::npos != input.find('<') ) 
  { return Gaudi::Utils::Histos::fromXml ( result , input ) ; }
  //
  return StatusCode::FAILURE ;
}
// ============================================================================
/*  parse AIDA histogram from text representation
 *  @param result (OUTPUT) the histogram
 *  @param input  (INPUT)  the input to be parsed 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Parsers::parse 
( AIDA::IHistogram1D& result , const std::string& input ) 
{
  // 1) convert to ROOT 
  TH1D* root = Gaudi::Utils::Aida2ROOT::aida2root ( &result ) ;
  if ( 0 == root ) { return StatusCode::FAILURE ; }
  // 2) read ROOT histogram 
  return parse ( *root , input ) ;
}
// ============================================================================
/*  parse AIDA histogram from text representation
 *  @param result (OUTPUT) the histogram
 *  @param input  (INPUT)  the input to be parsed 
 *  @return status code 
 */
// ============================================================================
StatusCode Gaudi::Parsers::parse 
( AIDA::IHistogram2D& result , const std::string& input ) 
{
  // 1) convert to ROOT 
  TH2D* root = Gaudi::Utils::Aida2ROOT::aida2root ( &result ) ;
  if ( 0 == root ) { return StatusCode::FAILURE ; }
  // 2) read ROOT histogram 
  return parse ( *root , input ) ;
}
// ============================================================================
// The END
// ============================================================================
