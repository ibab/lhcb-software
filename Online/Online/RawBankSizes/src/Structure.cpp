// $Id: Structure.cpp,v 1.1 2010-02-15 16:02:54 jost Exp $
// =====================================================================-------
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <map>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Grammars.h"
#include "GaudiKernel/Parsers.icpp"
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
           << "detector" //o.det 
           << " )" ;
}
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Parsers 
  {
    // ========================================================================
    class BeatGrammar : public grammar
    < BeatGrammar, ClosureGrammar<HParam::HistParams>::context_t >
    {
    public:
      // ======================================================================
      typedef HParam::HistParams ResultT;
      // ======================================================================
    public:
      // ======================================================================
      /// callback
      void matchBin ( const int bin ) const 
      { 
      	this->val().n_bin = bin ; 
      }
      /// callback
      void matchMin ( const float x ) const 
      {
       this->val().xmin = x ; 
      }
      /// callback
      void matchMax ( const float x ) const 
      {
       this->val().xmax = x ; 
      }
      /// callback
      void matchsmin ( const int smin ) const 
      {
       this->val().s_min = smin ; 
      }
      /// callback
      void matchsmax ( const int smax ) const 
      {
       this->val().s_max = smax ; 
      }
      void matchdet ( const std::string d ) const 
      {
       this->val().det = d ; 
      }
      // ======================================================================
    public:
      template <typename ScannerT>
      struct definition
      {
        definition( BeatGrammar const &self)
        {
          para = str_p("(")   
            >> bin   [boost::bind(&BeatGrammar::matchBin,&self,_1)]
            >> ','
            >> x_min [boost::bind(&BeatGrammar::matchMin,&self,_1)]
            >> ','
            >> x_max [boost::bind(&BeatGrammar::matchMax,&self,_1)]
            >> ','
            >> s_min    [boost::bind(&BeatGrammar::matchsmin,&self,_1)]
            >> ','
            >> s_max   [boost::bind(&BeatGrammar::matchsmax,&self,_1)]
             >> ','
            >> det   [boost::bind(&BeatGrammar::matchdet,&self,_1)]
            >> ')' ;
        }
        rule<ScannerT> const& start() const { return para; }
        rule<ScannerT>     para  ;
        IntGrammar<int>    bin   ;
        RealGrammar<float> x_min ;
        RealGrammar<float> x_max ;
        IntGrammar<int>    s_min ;
        IntGrammar<int>    s_max ;
        StringGrammar det;
      };
      // ======================================================================
    };
    // ========================================================================
  } //                                         end of namespace Gaudi::Parsers 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
// parse it!
// ============================================================================
StatusCode  Gaudi::Parsers::parse 
( HParam::HistParams& result , 
  const std::string&          input  ) 
{
  BeatGrammar gr ;
  return parse
    ( createIterator(input), 
      IteratorT(),
      gr[var(result)=arg1],
      SkipperGrammar()).full;
}
// ============================================================================
// The END 
// ============================================================================

