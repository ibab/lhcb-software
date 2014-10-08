// $Id: HltSelectionContainer.h,v 1.7 2009-12-24 14:13:19 graven Exp $
#ifndef HLTBASE_HLTSELECTIONCONTAINER_H
#define HLTBASE_HLTSELECTIONCONTAINER_H 1
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/StringKey.h"
#include "HltBase/HltSelection.h"
#include "HltBase/HltAlgorithm.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/GaudiException.h"
#include "boost/utility.hpp"
#include <vector>
#include <utility>
#include <tuple>
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

namespace Hlt
{
namespace detail
{

template <size_t... I>
struct index_sequence
{
};

// push N-1 onto the 'pack', lower N by one, and recurse...
template <size_t N, size_t... Is>
struct make_index_sequence : make_index_sequence<N - 1, N - 1, Is...>
{
};

// end of recursion -- 0 items to add...
template <size_t... Is>
struct make_index_sequence<0, Is...> : index_sequence<Is...>
{
};

// -- end of recursion
template <typename F, typename Tuple>
void map_impl( F&, Tuple&, index_sequence<> )
{
}
// -- recursively walk through the tuple...
template <typename F, typename Tuple, size_t I, size_t... J>
void map_impl( F& f, Tuple& t, index_sequence<I, J...> )
{
    f( std::get<I>( t ) );
    map_impl( f, t, index_sequence<J...>{} );
}

/// map functor f on each item in a tuple: 
/// starting with f(get<0>), f(get<1>),... f(get<N-2>), f(get<N-1>)
template <typename F, typename Tuple>
void map( F& f, Tuple& t )
{
    map_impl( f, t, make_index_sequence<std::tuple_size<Tuple>::value>{} );
}

template <typename Candidate>
struct data_
{
    using selection_type = Hlt::TSelection<Candidate>;
    using candidate_type = Candidate;
    selection_type* selection;
    Gaudi::StringKey property;
    data_() : selection{nullptr}
    {
    }
};

template <>
struct data_<void>
{
    using selection_type = Selection;
    using candidate_type = void;
    selection_type* selection;
    Gaudi::StringKey property;
    data_() : selection{nullptr}
    {
    }
};

template <typename U>
inline void register_( data_<U>& d, HltAlgorithm& owner )
{
    d.selection = &owner.registerTSelection<U>( d.property );
}

template <>
inline void register_( data_<void>& d, HltAlgorithm& owner )
{
    d.selection = &owner.registerSelection( d.property );
}

template <typename Candidate>
struct cdata_
{
    using selection_type = const Hlt::TSelection<Candidate>;
    using candidate_type = Candidate;
    selection_type* selection;
    Gaudi::StringKey property;
    cdata_() : selection{nullptr}
    {
    }
};

/// retrieve selection
class retrieve_
{
    HltAlgorithm& m_owner;

  public:
    retrieve_( HltAlgorithm& owner ) : m_owner( owner )
    {
    }
    // specialization for output -- do nothing
    template <typename U>
    void operator()( detail::data_<U>& )
    {
    }

    // specialization for input -- retrieve
    template <typename U>
    void operator()( detail::cdata_<U>& u )
    {
        if ( u.selection ) {
            throw GaudiException( m_owner.name() +
                                      "::retrieve_ selection already present..",
                                  "", StatusCode::FAILURE );
        }
        u.selection = &m_owner.retrieveTSelection<U>( u.property );
    }
};

/// zero
struct zero_
{
    template <typename U>
    void operator()( U& u )
    {
        u.selection = nullptr;
    }
};

//
/// declare property -- this fixes the naming convention for properties!
template <unsigned N>
class declare_
{
    HltAlgorithm& m_alg;
    unsigned int m_counter;
    std::map<int, std::string> m_defs;

  public:
    declare_( HltAlgorithm& alg, std::map<int, std::string>&& defaults )
        : m_alg( alg ), m_counter{0u}, m_defs{std::move( defaults )}
    {
    }
    template <typename U>
    void operator()( U& t )
    {
        std::string def = m_defs[m_counter];
        if ( m_counter == 0 ) {
            t.property = Gaudi::StringKey(
                def.empty() ? m_alg.name() : def ); // set default output name...
            m_alg.declareProperty( "OutputSelection", t.property );
        } else {
            std::string prop{"InputSelection"};
            if ( N > 2 ) prop += std::to_string( m_counter );
            if ( !def.empty() )
                t.property = Gaudi::StringKey( def );  // set default input name...
            m_alg.declareProperty( prop, t.property ); // TODO: add callback, locked
                                                       // as soon as corresponding
                                                       // TSelection* is non-zero...
                                                       // TODO: add help entry
        }
        ++m_counter;
    }
};
}

template <typename T1, typename... Tn>
class SelectionContainer : private boost::noncopyable
{
    HltAlgorithm& m_owner; // The algorithm which owns us
    using Tuple = std::tuple<detail::data_<T1>, detail::cdata_<Tn>...>;
    Tuple m_data;

  public:
    SelectionContainer( HltAlgorithm& alg ) : m_owner( alg )
    {
        detail::zero_ x{};
        map( x, m_data );
    }

    using map_t = std::map<int, std::string>;
    void declareProperties( map_t defaults = map_t() )
    {
        detail::declare_<std::tuple_size<Tuple>::value> x( m_owner,
                                                           std::move( defaults ) );
        map( x, m_data );
    }
    // could move to postInitialize hook
    void registerSelection()
    {
        auto& d = std::get<0>( m_data );
        if ( d.selection ) {
            throw GaudiException(
                m_owner.name() + "::registerSelection selection already present..",
                "", StatusCode::FAILURE );
        }
        register_( d, m_owner );
    }

    // could move to preInitialize hook
    void retrieveSelections()
    {
        detail::retrieve_ x{m_owner};
        map( x, m_data );
    }
    // TODO: check if register/retrieve has been called...
    typename std::tuple_element<0, Tuple>::type::selection_type* output()
    {
        return std::get<0>( m_data ).selection;
    }

    // template <unsigned N, typename std::enable_if< (N>0) >::type*>
    template <unsigned N>
    typename std::tuple_element<N, Tuple>::type::selection_type* input()
    {
        static_assert(
            N > 0,
            "input() : N>0 for input -- N=0 corresponds to the required output" );
        auto& d = std::get<N>( m_data );
        if ( !d.selection->processed() ) {
            m_owner.debug() << "requesting stale selection " << d.selection->id()
                            << endmsg;
            m_owner.warning()
                << "requesting stale selection...  most likely a misconfiguration"
                << endmsg;
        }
        return d.selection;
    }
};
}

#endif
