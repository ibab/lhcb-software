// $Id: Structure.cpp,v 1.1 2010-02-15 16:02:54 jost Exp $
// =====================================================================-------
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#ifdef _WIN32
#pragma warning (disable : 4244 )
#endif
#include <map>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ParsersFactory.h"
// ============================================================================
// local
// ============================================================================
#include "Structure.h"
// ============================================================================

// ============================================================================
// stream it!
// ============================================================================

std::ostream& Gaudi::Utils::toStream
( const HParam::HistParams& o ,
  std::ostream& s )
{
  return s << "( "
           << o.n_bin << " , "
           << o.xmin  << " , "
           << o.xmax  << " , "
    		   << o.s_min << " , "
		       << o.s_max << " , "
           << "\"detector\"" //o.det
           << " )" ;
}
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Parsers
  {
    // ========================================================================
    template< typename Iterator, typename Skipper>
    class BeatGrammar:
      public qi::grammar<Iterator, HParam::HistParams(), Skipper>
    {
    public:
      // ======================================================================
      typedef HParam::HistParams ResultT;
      // ======================================================================
    public:
      struct tag_bin{};
      struct tag_min{};
      struct tag_max{};
      struct tag_smin{};
      struct tag_smax{};
      struct Operations {
        // Some magic:
        template <typename A, typename B = boost::fusion::unused_type,
            typename C = boost::fusion::unused_type,
            typename D = boost::fusion::unused_type>
        struct result { typedef void type; };
        // Actions:
        // --------------------------------------------------------------------
        void operator()(HParam::HistParams& val, const int bin,
            tag_bin) const {
         val.n_bin = bin;
        }

        void operator()(HParam::HistParams& val, const float x,
            tag_min) const {
         val.xmin = x;
        }

        void operator()(HParam::HistParams& val, const float x,
            tag_max) const {
         val.xmax = x;
        }

        void operator()(HParam::HistParams& val, const int smin,
            tag_smin) const {
         val.s_min = smin;
        }

        void operator()(HParam::HistParams& val, const int smax,
            tag_smax) const {
         val.s_max = smax;
        }

        void operator()(HParam::HistParams& val,
            const std::string& d) const {
         val.det = d;
        }

      };
    public:
      BeatGrammar() : BeatGrammar::base_type(para)
      {
          para = qi::lit('[')
            >> qi::int_[op(qi::_val, qi::_1, tag_bin())]
            >> ','
            >> qi::float_[op(qi::_val, qi::_1, tag_min())]
            >> ','
            >> qi::float_[op(qi::_val, qi::_1, tag_max())]
            >> ','
            >> qi::int_[op(qi::_val, qi::_1, tag_smin())]
            >> ','
            >> qi::int_[op(qi::_val, qi::_1, tag_smax())]
             >> ','
            >> det   [op(qi::_val, qi::_1)]
            >> ']' ;
        }
        qi::rule<Iterator, HParam::HistParams(), Skipper> para;
        StringGrammar<Iterator, Skipper> det;
        ph::function<Operations> op;
      // ======================================================================
    };
    REGISTER_GRAMMAR(HParam::HistParams, BeatGrammar);
    // ========================================================================
  } //                                         end of namespace Gaudi::Parsers
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
// parse it!
// ============================================================================
StatusCode  Gaudi::Parsers::parse
( HParam::HistParams& result ,
  const std::string& input  )
{
  return parse_(result, input);
}
// ============================================================================
// The END
// ============================================================================

