// $Id: HltSelectionContainer.h,v 1.1 2008-07-30 13:33:16 graven Exp $
#ifndef HLTBASE_HLTSELECTIONCONTAINER_H 
#define HLTBASE_HLTSELECTIONCONTAINER_H 1
#include "HltBase/HltSelection.h"
#include "HltBase/HltAlgorithm.h"
#include "HltBase/stringKey.h"
#include "GaudiKernel/Property.h"
#include "boost/lexical_cast.hpp"
#include "boost/utility.hpp"
#include "boost/tuple/tuple.hpp"
#include "boost/static_assert.hpp"
#include <utility>
#include <cassert>
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
//  in the body of the constructor,  retrieveSelections & registerSelection during initialize, and
//  you can access the selections using output() and input<N>() during execute.
//
// an example will go here...
//
//
//

namespace Hlt {
    namespace detail {
        template <typename T> class callback_ ;

        template <typename Candidate> struct data_ {
                typedef Hlt::TSelection<Candidate> selection_type;
                typedef Candidate                  candidate_type;
                selection_type*             selection;
                stringKey                   property;
                callback_<Candidate>*       callback;
                data_() : selection(0),callback(0) {}
        };

        /// TES callback
        template <typename T> 
        class callback_ : boost::noncopyable {
        public:
            callback_( detail::data_<T>& data, HltAlgorithm& owner) : m_owner(owner), m_data(data) 
            {
                typedef Hlt::TSelection<T> selection_type;
                //FIXME: this doesn't work for multiple clients of a TES selection,
                //       as each client is considered an owner, and multiple
                //       owners are not allowed...
                m_data.selection = new selection_type( m_data.property );
                if (m_owner.registerTESInputSelection( m_data.selection ).isFailure()) {
                    m_owner.error() << " Failed to register TES selection " << endmsg;
                    throw GaudiException("Failed to add Selection",m_data.selection->id().str(),StatusCode::FAILURE);
                }
            }
            ~callback_() { delete m_data.selection; m_data.selection = 0; m_data.callback = 0;} // unregister ourselves...

            void operator()() const {
                assert(m_data.callback==this); // check we are the right callback...
                typedef typename T::Container  container_type;
                assert(std::string(m_data.selection->id().str(),0,4)=="TES:");
                std::string location( m_data.selection->id().str(),4 );
                container_type *obj = m_owner.get<container_type>( m_owner.evtSvc(), location );
                m_data.selection->clean(); //TODO: check if/why this is needed??
                m_data.selection->insert( m_data.selection->end(),obj->begin(),obj->end());
                m_data.selection->setDecision( !m_data.selection->empty() ); // force it processed...
            }
        private:
            HltAlgorithm&       m_owner;
            detail::data_<T>&   m_data;
        };
        
        /// retrieve selection
        class retrieve_ { 
        public:
            retrieve_(HltAlgorithm &owner) : m_owner(owner) {}
            template <typename U> void operator()(U& u) {
                assert(u.selection==0);
                const static std::string tes("TES:");
                if (u.property.str().substr(0,4) == tes ) {
                    u.callback = new callback_<typename U::candidate_type>( u, m_owner );
                }
                u.selection = &m_owner.retrieveTSelection<typename U::candidate_type>(u.property);
            }
        private:
            HltAlgorithm& m_owner;
        };
        
        /// zero
        struct zero_ { template <typename U> void operator()(U& u) { u.selection = 0; } };
        //
        /// declare property -- this fixes the naming convention for properties!
        template <unsigned N>
        class declare_ {
        public:
            declare_(HltAlgorithm &alg) : m_alg(alg), m_counter(0) {}
            template <typename U> void operator()(U& t) {
                if (m_counter==0) {
                    StringProperty x(m_alg.name());
                    t.property.updateHandler( x ); // set default output name...
                    m_alg.declareProperty( "OutputSelection",t.property.property() );
                } else {
                    std::string prop( "InputSelection" );
                    if (N>2) prop += boost::lexical_cast<std::string>(m_counter);
                    m_alg.declareProperty( prop,
                                           t.property.property() ); //TODO: add callback, locked as soon as corresponding TSelection* is non-zero...
                                                                    //WARNING: stringKey already _has_ a callback...
                                                                    //TODO: add help entry
                }
                ++m_counter;
            }
        private:
            HltAlgorithm& m_alg;
            unsigned int m_counter;
        };
                


        template <typename T>
        class SelectionContainer_ : private boost::noncopyable {
            template <unsigned N> struct helper_ { 
                typedef typename boost::tuples::element<N,T>::type data_type;       // tuple(N) -> data<X>
                typedef typename data_type::candidate_type         candidate_type;  // data<X> -> X 
                typedef typename data_type::selection_type         selection_type;  // data<X> -> TSelection<X>
            };
        protected:
            // make sure only derived classes can be instantiated...
            SelectionContainer_(HltAlgorithm& alg) : m_owner(alg) { zero_ x; for_each_selection(m_data,x); }
        public:
            void declareProperties(){
                declare_<boost::tuples::length<T>::value> x(m_owner); for_each_selection(m_data,x); 
            }
            // could move to postInitialize hook
            void registerSelection() {
                typename helper_<0>::data_type& d = boost::get<0>(m_data);
                assert(d.selection==0); // not yet registered...
                d.selection = &m_owner.registerTSelection<typename helper_<0>::candidate_type>(d.property);
            }

            // could move to preInitialize hook
            void retrieveSelections() {  
                // start recursion at 2nd element, because the first is the output
                retrieve_ x(m_owner); for_each_selection(m_data.get_tail(),x);
            }
            //TODO: check if register/retrieve has been called...
            typename helper_<0>::selection_type* output() { return boost::get<0>(m_data).selection; }

            //TODO: this isn't called for 'bound' selections -- MUST move callback into Selection itself...
            //      (or replace the Selection with this class...)
            //      or into from the preExecute hook in HltAlgorithm...
            //      note: can also move a 'flush into TES' hook into the postExecute hook in HltAlgorithm...
            template <unsigned N> 
            typename helper_<N>::selection_type* input() { 
                BOOST_STATIC_ASSERT(N>0); // N=0 corresponds to the output....
                typename helper_<N>::data_type& d = boost::get<N>(m_data); 
                if (!d.selection->processed() && d.callback!=0 ) {
                    (*d.callback)();
                }
                if (!d.selection->processed()) {
                    m_owner.debug() << "requesting stale selection " << d.selection->id() << endmsg;
                    m_owner.warning() << "requesting stale selection...  most likely a misconfiguration" << endmsg;
                }
                return d.selection; 
            }

        private:
            /// apply a functor on each item: recursively walk through the tuple...
            template <typename Function> void for_each_selection( boost::tuples::null_type, Function) {};
            template <typename Tuple, typename Function> void for_each_selection( Tuple& tuple, Function& f) 
            { f(tuple.get_head()); for_each_selection(tuple.get_tail(),f); };

            // The algorithm which owns us
            HltAlgorithm& m_owner;
            // the actual data: a boost::tuple<U1,U2,...UN> where Ui = detail::data_<Ti> 
            // with Ti eg. LHCb::Track for a tracklist, LHCb::RecVertex for a vertex list, etc.
            T       m_data;
        };

    }

    template <typename T1,
              typename T2,
              typename T3,
              typename T4,
              typename T5,
              typename T6,
              typename T7,
              typename T8> struct SelectionContainer8 : public detail::SelectionContainer_< boost::tuple< detail::data_<T1>,
                                                                                                          detail::data_<T2>,
                                                                                                          detail::data_<T3>,
                                                                                                          detail::data_<T4>,
                                                                                                          detail::data_<T5>,
                                                                                                          detail::data_<T6>,
                                                                                                          detail::data_<T7>,
                                                                                                          detail::data_<T8> > > {
        SelectionContainer8(HltAlgorithm& owner) : detail::SelectionContainer_<boost::tuple<detail::data_<T1>,
                                                                                            detail::data_<T2>,
                                                                                            detail::data_<T3>,
                                                                                            detail::data_<T4>,
                                                                                            detail::data_<T5>,
                                                                                            detail::data_<T6>,
                                                                                            detail::data_<T7>,
                                                                                            detail::data_<T8> > > (owner) {}
    };

    template <typename T1,
              typename T2,
              typename T3,
              typename T4,
              typename T5,
              typename T6,
              typename T7> struct SelectionContainer7 : public detail::SelectionContainer_< boost::tuple< detail::data_<T1>,
                                                                                                          detail::data_<T2>,
                                                                                                          detail::data_<T3>,
                                                                                                          detail::data_<T4>,
                                                                                                          detail::data_<T5>,
                                                                                                          detail::data_<T6>,
                                                                                                          detail::data_<T7> > > {
        SelectionContainer7(HltAlgorithm& owner) : detail::SelectionContainer_<boost::tuple<detail::data_<T1>,
                                                                                            detail::data_<T2>,
                                                                                            detail::data_<T3>,
                                                                                            detail::data_<T4>,
                                                                                            detail::data_<T5>,
                                                                                            detail::data_<T6>,
                                                                                            detail::data_<T7> > > (owner) {}
    };

    template <typename T1,
              typename T2,
              typename T3,
              typename T4,
              typename T5,
              typename T6> struct SelectionContainer6 : public detail::SelectionContainer_< boost::tuple< detail::data_<T1>,
                                                                                                         detail::data_<T2>,
                                                                                                         detail::data_<T3>,
                                                                                                         detail::data_<T4>,
                                                                                                         detail::data_<T5>,
                                                                                                         detail::data_<T6> > >{
        SelectionContainer6(HltAlgorithm& owner) : detail::SelectionContainer_<boost::tuple<detail::data_<T1>,
                                                                                            detail::data_<T2>,
                                                                                            detail::data_<T3>,
                                                                                            detail::data_<T4>,
                                                                                            detail::data_<T5>,
                                                                                            detail::data_<T6> > > (owner) {}
    };

    template <typename T1,
              typename T2,
              typename T3,
              typename T4,
              typename T5> struct SelectionContainer5 : public detail::SelectionContainer_< boost::tuple< detail::data_<T1>,
                                                                                                         detail::data_<T2>,
                                                                                                         detail::data_<T3>,
                                                                                                         detail::data_<T4>,
                                                                                                         detail::data_<T5> > >{
        SelectionContainer5(HltAlgorithm& owner) : detail::SelectionContainer_<boost::tuple<detail::data_<T1>,
                                                                                            detail::data_<T2>,
                                                                                            detail::data_<T3>,
                                                                                            detail::data_<T4>,
                                                                                            detail::data_<T5> > > (owner) {}
    };

    template <typename T1,
              typename T2,
              typename T3,
              typename T4> struct SelectionContainer4 : public detail::SelectionContainer_< boost::tuple< detail::data_<T1>,
                                                                                                          detail::data_<T2>,
                                                                                                          detail::data_<T3>,
                                                                                                          detail::data_<T4> > >{
        SelectionContainer4(HltAlgorithm& owner) : detail::SelectionContainer_<boost::tuple<detail::data_<T1>,
                                                                                            detail::data_<T2>,
                                                                                            detail::data_<T3>,
                                                                                            detail::data_<T4> > > (owner) {}
    };

    template <typename T1,                                                    
              typename T2,
              typename T3> struct SelectionContainer3 : public detail::SelectionContainer_< boost::tuple< detail::data_<T1>,
                                                                                                          detail::data_<T2>,
                                                                                                          detail::data_<T3> > >{
        SelectionContainer3(HltAlgorithm& owner) : detail::SelectionContainer_<boost::tuple<detail::data_<T1>,
                                                                                            detail::data_<T2>,
                                                                                            detail::data_<T3> > > (owner) {}
    };

    template <typename T1,
              typename T2> struct SelectionContainer2 : public detail::SelectionContainer_< boost::tuple< detail::data_<T1>,
                                                                                                          detail::data_<T2> > >{
        SelectionContainer2(HltAlgorithm& owner) : detail::SelectionContainer_<boost::tuple<detail::data_<T1>,
                                                                                            detail::data_<T2> > > (owner) {}
    };

    template <typename T1> struct SelectionContainer1 : public detail::SelectionContainer_< boost::tuple< detail::data_<T1> > > {
        SelectionContainer1(HltAlgorithm& owner) : detail::SelectionContainer_<boost::tuple< detail::data_<T1> > > (owner) {}
    };



    // 0 is a special case of 1 ;-): no input, one output, but output has no explicit candidates...
    class SelectionContainer0 : boost::noncopyable {
    public:
        SelectionContainer0(HltAlgorithm& owner) : m_output(0), m_owner(owner) {}
        void declareProperties() { 
            StringProperty x(m_owner.name());
            m_property.updateHandler( x ); // set default output name to the instance name of the algorithm
            m_owner.declareProperty("OutputSelection",m_property.property()); 
        }
        void registerSelection() { m_output = &m_owner.registerSelection(m_property) ; }
        Selection* output() { return m_output; }
    private:
        stringKey   m_property;
        Selection*  m_output;
        HltAlgorithm& m_owner;
    };
}


#endif
