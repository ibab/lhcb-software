// $Id: HltSelectionContainer.h,v 1.7 2009-12-24 14:13:19 graven Exp $
#ifndef HLTBASE_HLTSELECTIONCONTAINER_H
#define HLTBASE_HLTSELECTIONCONTAINER_H 1
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "HltBase/HltSelection.h"
#include "HltBase/HltAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/GaudiException.h"
#include "boost/lexical_cast.hpp"
#include "boost/utility.hpp"
#include "boost/tuple/tuple.hpp"
#include "boost/static_assert.hpp"
// #include "boost/assign/list_of.hpp"
#include <vector>
#include <utility>
#include <memory>
#include <cassert>
#include <map>
//
//  This file declares the templates SelectionContainerN<T1,T2,...TN> where
//  T1,T2, ..., TN are the type of candidate lists.
//
//  These classes are utilities intended to be member data of HltAlgorithms,
//  and will deal with setting up of input and output selections, and will
//  insure they contain valid data when input<N> and output are called.
//
//  The provide the following member functions:
//
//            void declareProperties()
//            void retrieveSelections()
//            void registerSelection()
//            TSelection<T1>* output()
//            TSelection<TN>* input<N>() for (N>1)
//
//  In an HltAlgorithm implementation, you should initialize the contained
//  container in the initializer list, passing it '*this', and call declareProperties
//  in the body of the constructor,  retrieveSelections & registerSelection during
// initialize, and
//  you can access the selections using output() and input<N>() during execute.
//
// an example will go here...
//
//
//

namespace Hlt {
namespace detail {

template <typename Candidate> struct data_ {
    typedef Hlt::TSelection<Candidate> selection_type;
    typedef Candidate candidate_type;
    selection_type *selection;
    Gaudi::StringKey property;
    data_() : selection( nullptr ) {}
};

template <typename Candidate> struct cdata_ {
    typedef const Hlt::TSelection<Candidate> selection_type;
    typedef Candidate candidate_type;
    selection_type *selection;
    Gaudi::StringKey property;
    cdata_() : selection( nullptr ) {}
};

/// retrieve selection
class retrieve_ {
  public:
    retrieve_( HltAlgorithm &owner ) : m_owner( owner ) {}
    template <typename U> void operator()( U &u ) {
        if ( u.selection ) {
            throw GaudiException( m_owner.name() +
                                      "::retrieve_ selection already present..",
                                  "", StatusCode::FAILURE );
        }
        u.selection =
            &m_owner.retrieveTSelection<typename U::candidate_type>( u.property );
    }

  private:
    HltAlgorithm &m_owner;
};

/// zero
struct zero_ {
    template <typename U> void operator()( U &u ) { u.selection = nullptr; }
};
//
/// declare property -- this fixes the naming convention for properties!
template <unsigned N> class declare_ {
  public:
    declare_( HltAlgorithm &alg, std::map<int, std::string> &&defaults )
        : m_alg( alg ), m_counter( 0u ), m_defs( std::move( defaults ) ) {}
    template <typename U> void operator()( U &t ) {
        std::string def = m_defs[m_counter];
        if ( m_counter == 0 ) {
            t.property = Gaudi::StringKey(
                def.empty() ? m_alg.name() : def ); // set default output name...
            m_alg.declareProperty( "OutputSelection", t.property );
        } else {
            std::string prop{"InputSelection"};
            if ( N > 2 ) prop += boost::lexical_cast<std::string>( m_counter );
            if ( !def.empty() )
                t.property = Gaudi::StringKey( def ); // set default input name...
            m_alg.declareProperty( prop, t.property ); // TODO: add callback, locked
                                                       // as soon as corresponding
                                                       // TSelection* is non-zero...
            // TODO: add help entry
        }
        ++m_counter;
    }

  private:
    HltAlgorithm &m_alg;
    unsigned int m_counter;
    std::map<int, std::string> m_defs;
};

template <typename T> class SelectionContainer_ : private boost::noncopyable {
    template <unsigned N> struct helper_ {
        typedef typename boost::tuples::element<N, T>::type data_type; // tuple(N) ->
                                                                       // data<X>
        typedef typename data_type::candidate_type candidate_type; // data<X> -> X
        typedef typename data_type::selection_type selection_type; // data<X> ->
                                                                   // TSelection<X>
    };

  protected:
    // make sure only derived classes can be instantiated...
    SelectionContainer_( HltAlgorithm &alg ) : m_owner( alg ) {
        zero_ x;
        for_each_selection( m_data, x );
    }

  public:
    typedef std::map<int, std::string> map_t;
    void declareProperties( map_t defaults = map_t() ) {
        declare_<boost::tuples::length<T>::value> x( m_owner,
                                                     std::move( defaults ) );
        for_each_selection( m_data, x );
    }
    // could move to postInitialize hook
    void registerSelection() {
        typename helper_<0>::data_type &d = boost::get<0>( m_data );
        if ( d.selection ) {
            throw GaudiException(
                m_owner.name() + "::registerSelection selection already present..",
                "", StatusCode::FAILURE );
        }
        d.selection =
            &m_owner.registerTSelection<typename helper_<0>::candidate_type>(
                 d.property );
    }

    // could move to preInitialize hook
    void retrieveSelections() {
        // start recursion at 2nd element, because the first is the output
        retrieve_ x( m_owner );
        for_each_selection( m_data.get_tail(), x );
    }
    // TODO: check if register/retrieve has been called...
    typename helper_<0>::selection_type *output() {
        return boost::get<0>( m_data ).selection;
    }

    template <unsigned N> typename helper_<N>::selection_type *input() {
        BOOST_STATIC_ASSERT( N > 0 ); // N=0 corresponds to the output....
        typename helper_<N>::data_type &d = boost::get<N>( m_data );
        if ( !d.selection->processed() ) {
            m_owner.debug() << "requesting stale selection " << d.selection->id()
                            << endmsg;
            m_owner.warning()
                << "requesting stale selection...  most likely a misconfiguration"
                << endmsg;
        }
        return d.selection;
    }

  private:
    /// apply a functor on each item: recursively walk through the tuple...
    template <typename Function>
    void for_each_selection( boost::tuples::null_type, Function ) {};
    template <typename Tuple, typename Function>
    void for_each_selection( Tuple &tuple, Function &f ) {
        f( tuple.get_head() );
        for_each_selection( tuple.get_tail(), f );
    };

    // The algorithm which owns us
    HltAlgorithm &m_owner;
    // the actual data: a boost::tuple<U1,U2,...UN> where Ui = detail::data_<Ti>
    // with Ti eg. LHCb::Track for a tracklist, LHCb::RecVertex for a vertex list,
    // etc.
    T m_data;
};
}

template <typename T1, typename T2, typename T3, typename T4, typename T5>
struct SelectionContainer5
    : public detail::SelectionContainer_<
          boost::tuple<detail::data_<T1>, detail::cdata_<T2>, detail::cdata_<T3>,
                       detail::cdata_<T4>, detail::cdata_<T5>>> {
    SelectionContainer5( HltAlgorithm &owner )
        : detail::SelectionContainer_<
              boost::tuple<detail::data_<T1>, detail::cdata_<T2>, detail::cdata_<T3>,
                           detail::cdata_<T4>, detail::cdata_<T5>>>( owner ) {}
};

template <typename T1, typename T2, typename T3, typename T4>
struct SelectionContainer4
    : public detail::SelectionContainer_<
          boost::tuple<detail::data_<T1>, detail::cdata_<T2>, detail::cdata_<T3>,
                       detail::cdata_<T4>>> {
    SelectionContainer4( HltAlgorithm &owner )
        : detail::SelectionContainer_<
              boost::tuple<detail::data_<T1>, detail::cdata_<T2>, detail::cdata_<T3>,
                           detail::cdata_<T4>>>( owner ) {}
};

template <typename T1, typename T2, typename T3>
struct SelectionContainer3
    : public detail::SelectionContainer_<
          boost::tuple<detail::data_<T1>, detail::cdata_<T2>, detail::cdata_<T3>>> {
    SelectionContainer3( HltAlgorithm &owner )
        : detail::SelectionContainer_<boost::tuple<
              detail::data_<T1>, detail::cdata_<T2>, detail::cdata_<T3>>>( owner ) {}
};

template <typename T1, typename T2>
struct SelectionContainer2
    : public detail::SelectionContainer_<
          boost::tuple<detail::data_<T1>, detail::cdata_<T2>>> {
    SelectionContainer2( HltAlgorithm &owner )
        : detail::SelectionContainer_<
              boost::tuple<detail::data_<T1>, detail::cdata_<T2>>>( owner ) {}
};

template <typename T1>
struct SelectionContainer1
    : public detail::SelectionContainer_<boost::tuple<detail::data_<T1>>> {
    SelectionContainer1( HltAlgorithm &owner )
        : detail::SelectionContainer_<boost::tuple<detail::data_<T1>>>( owner ) {}
};

// 0 is a special case of 1 ;-): no input, one output, but output has no explicit
// candidates...
class SelectionContainer0 : boost::noncopyable {
  public:
    SelectionContainer0( HltAlgorithm &owner )
        : m_output( nullptr ), m_owner( owner ) {}
    void declareProperties() {
        m_property = Gaudi::StringKey( m_owner.name() ); // set default output name
                                                         // to the instance name of
                                                         // the algorithm
        m_owner.declareProperty( "OutputSelection", m_property );
    }
    void registerSelection() { m_output = &m_owner.registerSelection( m_property ); }
    Selection *output() { return m_output; }

  private:
    Gaudi::StringKey m_property;
    Selection *m_output;
    HltAlgorithm &m_owner;
};
}

#endif
