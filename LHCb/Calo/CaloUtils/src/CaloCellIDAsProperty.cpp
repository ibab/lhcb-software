// $Id: CaloCellIDAsProperty.cpp,v 1.4 2010-03-18 13:58:42 ibelyaev Exp $
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
#include "GaudiKernel/ParsersFactory.h"
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
     *  @author alexander.mazurov@gmail.com
     *  @date 2011-08-29
     */
    template< typename Iterator, typename Skipper>
    class CCIDGrammar :
      public qi::grammar<Iterator,LHCb::CaloCellID(), Skipper>
    {
    public:
      // ======================================================================
      /// the actual type of parsed result
      typedef LHCb::CaloCellID ResultT;     // the actual type of parsed result
      typedef LHCb::CaloCellID::ContentType ContentT ;
      // ======================================================================
    public:
      // ======================================================================
      struct tag_calo {};
      struct tag_area {};
      struct tag_row {};
      struct tag_col {};
      struct Operations {
        template <typename A, typename B = boost::fusion::unused_type,
            typename C = boost::fusion::unused_type,
            typename D = boost::fusion::unused_type>
        struct result { typedef void type; };
        // ====================================================================
        void operator()(LHCb::CaloCellID& val, const ContentT v) const {
          val.setAll(v);
        }
        void match (LHCb::CaloCellID& val, const unsigned short Value,
            const unsigned int Shift, const unsigned int Mask) const {
          ContentT tmp = val.all() ;
          operator()(val, CaloCellCode::setField(tmp, Value, Shift, Mask));
        }
        // ====================================================================
        void operator()(LHCb::CaloCellID& val, unsigned short v,
            tag_calo) const {
          val.setCalo(v);
        }
        void operator()(LHCb::CaloCellID& val, const std::string& v,
            tag_calo) const {
          val.setCalo(v);
        }
        void operator()(LHCb::CaloCellID& val,
            unsigned short v, tag_area) const {
          match(val, v, CaloCellCode::ShiftArea, CaloCellCode::MaskArea);
        }
        void operator()(LHCb::CaloCellID& val,
            const std::string& v, tag_area) const {
          operator()(val, CaloCellCode::caloArea (val.calo() , v ),
              tag_area());
        }

        void operator()(LHCb::CaloCellID& val, unsigned short v,
            tag_row) const {
          match(val, v ,CaloCellCode::ShiftRow, CaloCellCode::MaskRow) ;
        }

        void operator()(LHCb::CaloCellID& val, unsigned short v,
            tag_col) const {
          match (val, v, CaloCellCode::ShiftCol, CaloCellCode::MaskCol);
        }
        // ====================================================================
      };
    public:
      // ======================================================================
      CCIDGrammar(): CCIDGrammar::base_type(result)
      {
          max_limit %=   qi::int_[qi::_a = qi::_1] >>
                    qi::eps( qi::_a <= qi::_r1 && qi::_a >= 0);
          result =
              (-(qi::lit("LHCb.CaloCellID") | qi::lit("CaloCellID"))
              >> qi::lit('(')
              >> (max_limit(4)[op(qi::_val, qi::_1, tag_calo())]
                   |
                   calo[op(qi::_val, qi::_1, tag_calo())]
                 )
              >> ','
              >> (max_limit(4)[op(qi::_val, qi::_1, tag_area())]
                  |
                  area[op(qi::_val, qi::_1, tag_area())]
                 )
              >> ',' >> max_limit(64)[op(qi::_val, qi::_1, tag_row())]
              >> ',' >> max_limit(64)[op(qi::_val, qi::_1, tag_col())]
              >> qi::lit(')')
              )
              |
              (qi::lit('(') >> qi::int_[op(qi::_val, qi::_1)]  >> qi::lit(')'))
              |
              qi::int_[op(qi::_val, qi::_1)]
              ;
        }
        StringGrammar<Iterator, Skipper> area, calo;
        qi::rule<Iterator, LHCb::CaloCellID(), Skipper>  result;
        qi::rule<Iterator,int(int),qi::locals<int>,Skipper>  max_limit;
        ph::function<Operations> op;
      // ======================================================================
    } ;
    REGISTER_GRAMMAR(LHCb::CaloCellID, CCIDGrammar);
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
  return parse_ ( result, input);
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
  return parse_ (result, input);
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
  return parse_(result, input);
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
  return parse_ (result, input);
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

