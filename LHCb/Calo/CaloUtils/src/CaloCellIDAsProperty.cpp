// $Id: CaloCellIDAsProperty.cpp,v 1.3 2009-11-28 19:12:40 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// LHCbKernel
// ============================================================================
#include "Kernel/CaloCellID.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/CaloCellIDAsProperty.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/bind.hpp"
// ============================================================================
// Grammars 
// ============================================================================
#include "GaudiKernel/Grammars.h"
// ============================================================================
// GaudiKernel 
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
/** @file 
 *  Implementation file for streaming&parsing function for class LHCb::CaloCellID 
 *  @see LHCb::CaloCellID 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
/*  put CellID into the output stream 
 *  @see LHCb::CaloCellID
 *  @see LHCb::CaloCellID::fillStream 
 *  @param object (INPUT)   object to be streamed 
 *  @param stream (OUTPUT) the stream to be updated 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
std::ostream& Gaudi::Utils::toStream 
( LHCb::CaloCellID& object , 
  std::ostream&     stream ) { return object.fillStream ( stream  ) ; }
// ============================================================================
/*  convert cellID into string 
 *  @param object (INPUT)   object to be streamed 
 *  @param stream (OUTPUT) the stream to be updated 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
std::string Gaudi::Utils::toString ( const LHCb::CaloCellID& object ) 
{ return object.toString() ; }
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Parsers 
  {
    // ========================================================================
    /** @class CCIDGrammar
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-
     */
    class CCIDGrammar : public grammar
    <CCIDGrammar,ClosureGrammar<LHCb::CaloCellID>::context_t>
    {
    public:
      // ======================================================================
      /// the actual type of parsed result 
      typedef LHCb::CaloCellID ResultT;     // the actual type of parsed result 
      typedef LHCb::CaloCellID::ContentType ContentT ; 
      // ======================================================================
    public:
      // ======================================================================
      void matchCalo1  ( const unsigned short v ) const 
      { this->val().setCalo ( v ) ; }
      void matchCalo2  ( const std::string&   v ) const 
      { this->val().setCalo ( v ) ; }
      void matchArea2  ( const std::string&   v ) const 
      { matchArea1 ( CaloCellCode::caloArea ( this->val().calo() , v ) ); }
      void matchArea1  ( const unsigned short v ) const 
      { match ( v , CaloCellCode::ShiftArea , CaloCellCode::MaskArea ) ; }
      void matchRow    ( const unsigned short v ) const 
      { match ( v , CaloCellCode::ShiftRow  , CaloCellCode::MaskRow  ) ; }
      void matchCol    ( const unsigned short v ) const 
      { match ( v , CaloCellCode::ShiftCol  , CaloCellCode::MaskCol  ) ; }
      void matchAll    ( const ContentT       v ) const 
      { this->val().setAll  ( v ) ; }
      // ======================================================================
    private:
      // ======================================================================
      void match ( const unsigned short Value , 
                   const unsigned int   Shift , 
                   const unsigned int   Mask  ) const 
      {
        ContentT tmp = this->val().all() ;
        matchAll ( CaloCellCode::setField ( tmp , Value , Shift , Mask ) )  ;
      }
      // ======================================================================
    public:
      // ======================================================================
      template <typename ScannerT>
      struct definition
      {
        definition ( const CCIDGrammar& cell ) 
        {   
          result = 
            ( !( str_p("LHCb.CaloCellID") | "CaloCellID" )
              >> '(' 
              >>
              ( max_limit_d(4u) [ uint_p [ boost::bind(&CCIDGrammar::matchCalo1 , &cell , _1) ] ] | 
                calo                     [ boost::bind(&CCIDGrammar::matchCalo2 , &cell , _1) ] )
              >> ',' >> 
              ( max_limit_d(4u) [ uint_p [ boost::bind(&CCIDGrammar::matchArea1, &cell , _1 ) ] ] |
                area                     [ boost::bind(&CCIDGrammar::matchArea2, &cell , _1) ] )
              >> ',' >> 
              max_limit_d(64u) [ uint_p [ boost::bind(&CCIDGrammar::matchRow , &cell , _1 ) ] ] 
              >> ',' >> 
              max_limit_d(64u) [ uint_p [ boost::bind(&CCIDGrammar::matchCol , &cell , _1 ) ] ] 
              >> ')'
              )
            |
            ( str_p("(") >> 
              uint_p [ boost::bind(&CCIDGrammar::matchAll , &cell , _1) ] 
              >> ')' ) 
            |
            uint_p [ boost::bind(&CCIDGrammar::matchAll , &cell , _1) ] ;
        }
        // return the final contructed rule 
        const rule<ScannerT>& start() const { return result ; }
        // 
        StringGrammar   calo   ;
        StringGrammar   area   ;
        rule<ScannerT>  result ;
        //
      } ;
      // ======================================================================
    } ;
    // ========================================================================
  } //                                          end of namespace Gaudi::Parsers
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
/*  parse cellID from the string 
 *  @param result (OUPUT) the parsed cellID 
 *  @param input  (INPUT) the input string
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
StatusCode Gaudi::Parsers::parse 
( LHCb::CaloCellID&  result , 
  const std::string& input  ) 
{
  CCIDGrammar gr ;
  return parse ( input.begin ()               , 
                 input.end   ()               , 
                 gr [ var ( result ) = arg1 ] , 
                 SkipperGrammar()             ).full ;
}
// ============================================================================
/*  parse the vector of cellIDs from the string 
 *  @param result (OUPUT) the parsed vector of cellIDs 
 *  @param input  (INPUT) the input string
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
StatusCode Gaudi::Parsers::parse 
( LHCb::CaloCellID::Vector& result , 
  const std::string&        input  ) 
{
  VectorGrammar < CCIDGrammar > gr ;
  return parse ( input.begin ()               , 
                 input.end   ()               , 
                 gr [ var ( result ) = arg1 ] , 
                 SkipperGrammar()             ).full ;
}
// ============================================================================
/*  parse the vector of cellIDs from the string 
 *  @param result (OUPUT) the parsed vector of cellIDs 
 *  @param input  (INPUT) the input string
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
StatusCode Gaudi::Parsers::parse 
( LHCb::CaloCellID::Set&    result , 
  const std::string&        input  ) 
{
  LHCb::CaloCellID::Vector tmp ;
  StatusCode sc = parse ( tmp , input ) ;
  if ( sc.isFailure() ) { return sc ; }
  //
  result.clear() ;
  result.insert (  tmp.begin() , tmp.end() ) ;
  return StatusCode::SUCCESS ;
}
// ============================================================================
/*  parse the map of  { cellID : double } from the string 
 *  @param result (OUPUT) the parsed map { cellID : double } 
 *  @param input  (INPUT) the input string
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
StatusCode Gaudi::Parsers::parse 
( std::map<LHCb::CaloCellID,double>& result , 
  const std::string&                 input  ) 
{
  MapGrammar < CCIDGrammar , RealGrammar<double> > gr ;
  return parse ( input.begin ()               , 
                 input.end   ()               , 
                 gr [ var ( result ) = arg1 ] , 
                 SkipperGrammar()             ).full ;
}
// ============================================================================
/* parse the map of  { cellID : vector<double> } from the string 
 *  @param result (OUPUT) the parsed map { cellID : vetcor<double> } 
 *  @param input  (INPUT) the input string
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
StatusCode Gaudi::Parsers::parse 
( std::map<LHCb::CaloCellID,std::vector<double> >& result , 
  const std::string&                               input  ) 
{
  MapGrammar < CCIDGrammar , VectorGrammar< RealGrammar<double> > > gr ;
  return parse ( input.begin ()               , 
                 input.end   ()               , 
                 gr [ var ( result ) = arg1 ] , 
                 SkipperGrammar()             ).full ;
}
// ============================================================================
/*  parse cellID from the string 
 *  @param result (OUPUT) the parsed cellID 
 *  @param input  (INPUT) the input string
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
StatusCode CaloCellCode::Cell2String::cellFromString 
( LHCb::CaloCellID& cell , const std::string& input )
{
  StatusCode sc =  Gaudi::Parsers::parse ( cell , input ) ;
  if ( sc.isFailure() ) { cell.setAll ( 0 ) ; }
  return sc ;
}
// ============================================================================
/*  parse cellIDs from the string 
 *  @param result (OUPUT) the parsed cellIDs 
 *  @param input  (INPUT) the input string
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
StatusCode CaloCellCode::Cell2String::cellFromString 
( LHCb::CaloCellID::Set& cell , const std::string& input )
{
  StatusCode sc =  Gaudi::Parsers::parse ( cell , input ) ;
  if ( sc.isFailure() ) { cell.clear(); }
  return sc ;
}
// ============================================================================
/*  parse cellIDs from the string 
 *  @param result (OUPUT) the parsed cellIDs 
 *  @param input  (INPUT) the input string
 *  @return status code 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */
// ============================================================================
StatusCode CaloCellCode::Cell2String::cellFromString 
( LHCb::CaloCellID::Vector& cell , const std::string& input )
{
  StatusCode sc =  Gaudi::Parsers::parse ( cell , input ) ;
  if ( sc.isFailure() ) { cell.clear(); }
  return sc ;
}
// ============================================================================
/* convert cellID into string 
 * @param  cell (INPUT) cell to be converted 
 * @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 * @date 2009-09-29
 */
// ============================================================================
std::string CaloCellCode::Cell2String::cellToString 
( const LHCb::CaloCellID& cell ) 
{ return Gaudi::Utils::toString ( cell ) ; }
// ============================================================================
/* convert cellIDs into string 
 * @param  cells (INPUT) cells to be converted 
 * @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 * @date 2009-09-29
 */
// ============================================================================
std::string CaloCellCode::Cell2String::cellToString 
( const LHCb::CaloCellID::Set& cell ) 
{ return Gaudi::Utils::toString ( cell ) ; }
// ============================================================================
/* convert cellIDs into string 
 * @param  cells (INPUT) cells to be converted 
 * @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 * @date 2009-09-29
 */
// ============================================================================
std::string CaloCellCode::Cell2String::cellToString 
( const LHCb::CaloCellID::Vector& cell ) 
{ return Gaudi::Utils::toString ( cell ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

