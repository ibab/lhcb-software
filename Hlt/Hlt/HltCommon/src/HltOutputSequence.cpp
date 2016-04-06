// STD & STL
#include <vector>
#include <string>
#include <sstream>
#include <map>

// Gaudi
#include <GaudiKernel/AlgFactory.h>
#include <GaudiKernel/IJobOptionsSvc.h>
#include <GaudiKernel/IAlgManager.h>

// HltBase
#include <HltBase/IHltOutputSvc.h>

// boost
#include <boost/format.hpp>

// local
#include "HltOutputSequence.h"

// This came from GaudiSequencer, might be better of in a public place.
namespace {

   using std::unique_ptr;
   using std::string;
   using std::forward;
   using std::vector;
   using std::map;
   using std::make_pair;
   using boost::io::group;

   //TODO: this  adds C++14 'make_unique'... remove once we move to C++14...
   template<typename T, typename ...Args>
   unique_ptr<T> make_unique_( Args&& ...args ) {
      return unique_ptr<T>( new T( forward<Args>(args)... ) );
   }

   bool isDefault(const string& s) { return s.empty(); }
   constexpr bool isDefault(double x) { return x == 0; }

   // utility class to populate some properties in the job options service
   // for a given instance name in case those options are not explicitly
   // set a-priori (effectively inheriting their values from the parent)
   class populate_JobOptionsSvc_t {
      vector<unique_ptr<Property>> m_props;
      IJobOptionsSvc* m_jos;
      string m_name;

      template <typename T> void process(T&& t) {
         static_assert( std::tuple_size<T>::value == 2, "Expecting an std::tuple key-value pair" );
         using type = typename std::decay<typename std::tuple_element<1,T>::type>::type;
         using prop_t = SimpleProperty<type>;
         if (!isDefault(std::get<1>(t))) m_props.push_back( make_unique_<prop_t>( std::get<0>(t), std::get<1>(t) ) ) ;
      }
      template <typename T, typename... Args> void process(T&& t, Args&&... args) {
         process(forward<T>(t)); process(forward<Args>(args)...);
      }
      void check_veto() { // avoid changing properties expliclty present in the JOS...
         const auto* props = m_jos->getProperties(m_name);
         if (!props) return;
         for ( const auto& i : *props ) {
            auto j = std::find_if( begin(m_props), end(m_props),
                                   [&i](const unique_ptr<Property>& prop) {
                                      return prop->name() == i->name();
                                   } );
            if (j==end(m_props)) continue;
            m_props.erase( j );
            if (m_props.empty()) break; // done!
         }
      }

   public:
      template <typename... Args>
      populate_JobOptionsSvc_t( string name, IJobOptionsSvc* jos, Args&&... args )
         : m_jos{jos}, m_name{ std::move(name) }   {
         process(forward<Args>(args)...);
         if (!m_props.empty()) check_veto();
         std::for_each( begin(m_props), end(m_props), [&](const unique_ptr<Property>& i ) {
               m_jos->addPropertyToCatalogue( m_name, *i ).ignore();
            } );
      }
      ~populate_JobOptionsSvc_t() {
         std::for_each( begin(m_props), end(m_props), [&](const unique_ptr<Property>& i ) {
               m_jos->removePropertyFromCatalogue( m_name, i->name() ).ignore();
            } );
      }
   };

   template <typename Stream, typename Container, typename Separator, typename Transform>
   Stream& ostream_joiner(Stream& os, const Container& c, Separator sep, Transform trans )
   {
      auto first = begin(c); auto last = end(c);
      if (first!=last) { os << trans(*first); ++first; }
      for (;first!=last;++first) os << sep << trans(*first);
      return os;
   }

}


//-----------------------------------------------------------------------------
// Implementation file for class : HltOutputSequence
//
// 2016-03-26 : Roel Aaij
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltOutputSequence )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltOutputSequence::HltOutputSequence( const string& name,
                                      ISvcLocator* svcLocator)
: GaudiHistoAlg (name , svcLocator)
{
  declareProperty("WriterProperties", m_writerProperties,
                  "Properties of the writers to be set.");
  declareProperty("StreamProperties", m_streamProperties,
                  "Properties of the writer to be set, where the stream name "
                  "should be substituted in the value first.");
  declareProperty("GroupProperties", m_groupProperties,
                  "Properties of the writer to be set, where the stream group name "
                  "should be substituted in the value first.");
  declareProperty("WriterType", m_writerType, "Type of writer algorithm to instantiate.");
  declareProperty("OutputPerGroup", m_outputPerGroup = true);
  declareProperty("IgnoreFilters", m_ignoreFilters = false);
  declareProperty("ForceStreams", m_forcedStreams);
}

//=============================================================================
StatusCode HltOutputSequence::initialize() {
   GaudiHistoAlg::initialize();

   StatusCode status = decodeNames();
   if ( !status.isSuccess() ) return status;

   //== Initialize the algorithms
   for (auto&  entry : m_entries) {
      status = entry.algorithm()->sysInitialize();
      if (!status.isSuccess()) {
         return Error("Can not initialize " + entry.algorithm()->name(), status);
      }
   }
   return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltOutputSequence::execute()
{
   StatusCode result = StatusCode::SUCCESS;

   for (auto&  entry : m_entries) {
      Algorithm* alg = entry.algorithm();
      if (!alg->isEnabled()) continue;
      // Check filters
      auto execWriter = true;
      for (const auto& filter : entry.filters()) {
         if (!filter->isEnabled()) {
            result = Error(string{"Filter Algorithm "} + filter->name() + " is not enabled!",
                           StatusCode::FAILURE);
            break;
         } else if (!filter->isExecuted()) {
            result = Error(string{"Filter Algorithm "} + filter->name() + " was not executed!",
                           StatusCode::FAILURE);
            break;
         } else if (!filter->filterPassed()) {
            execWriter = false;
            break;
         }
      }
      if (!result.isSuccess()) break;

      if (execWriter && !alg->isExecuted()) {
         result = alg->sysExecute();
         alg->setExecuted(true);
         if (!result.isSuccess()) break;  //== Abort and return bad status
      }
   }

   setExecuted(true);
   return result;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltOutputSequence::finalize()
{
   return GaudiHistoAlg::finalize();
}

//=========================================================================
//  Decode the input names and fills the m_algs vector.
//=========================================================================
StatusCode HltOutputSequence::decodeNames()
{
  StatusCode final = StatusCode::SUCCESS;
  m_entries.clear();

  //== Get the "Context" option if in the file...
  auto jos = service<IJobOptionsSvc>("JobOptionsSvc");

  //= Get the Application manager, to see if algorithm exist
  auto appMgr = svc<IAlgManager>("ApplicationMgr");
  auto outputSvc = svc<IHltOutputSvc>("HltOutputSvc");
  if (!outputSvc.isValid()) {
     return Error("Could not obtain HltOutputSvc", StatusCode::FAILURE);
  }

  // Get all streams we want to output, either the groups (online), or all
  // logical streams (offline)
  map<string, string> streams;
  if (m_outputPerGroup) {
     auto groups = outputSvc->groups();
     std::transform(begin(groups), end(groups), std::inserter(streams, begin(streams)),
                    [](const string& group) {
                       return make_pair(group, group);
                    });
  } else {
     for (const auto& group : outputSvc->groups()) {
        auto substreams = outputSvc->logicalStreams(group);
        std::transform(begin(substreams), end(substreams), std::inserter(streams, begin(streams)),
                       [group](const string& stream) {
                          return make_pair(stream, group);
                       });
     }
  }

  if (msgLevel(MSG::DEBUG)) {
     debug () << "Streams:";
     for (const auto& s : streams) {
        debug() << " " << s;
     }
     debug() << endmsg;
  }

  // If the ForceStreams property is not empty, the used streams are taken from
  // whatever is in there.
  if (!m_forcedStreams.empty()) {
     streams.clear();
     for (auto entry : m_forcedStreams) {
        streams.emplace(entry.first, entry.first);
     }
  }

  // Loop over streams and instantiate writers
  for (const auto& entry : streams) {
     auto stream = entry.first;
     auto group = entry.second;
     string name = stream + "StreamOutput";
     string typeName = m_writerType + "/" + name;

     //== Check wether the specified algorithm already exists. If not, create it
     StatusCode result = StatusCode::SUCCESS;
     SmartIF<IAlgorithm> ialg = appMgr->algorithm(typeName, false); // do not create it now
     if (!ialg) {
        // ensure some magic properties are set while we create the subalgorithm so
        // that it effectively inherites 'our' settings -- if they have non-default
        // values... and are not set explicitly already.
        populate_JobOptionsSvc_t populate_guard{name, jos,
              std::forward_as_tuple( "Context",          context() ),
              std::forward_as_tuple( "RootInTES",        rootInTES() )
              };
        Algorithm *alg = nullptr;
        result = createSubAlgorithm(m_writerType, name, alg);
        ialg = alg; // ensure that ialg.isValid() from here onwards!

        // Lambdas to apply properties with a substitution transformation.
        using trans = std::function<string(const string&)>;
        auto applyProperties = [this, &alg](const PropMap& pm, const trans& subs) -> StatusCode {
           StatusCode r = StatusCode::SUCCESS;
           for (const auto& entry: pm) {
              if (!alg->hasProperty(entry.first))
                 return Error(string{"Writer "} + m_writerType + " does not have property " + entry.first,
                              StatusCode::FAILURE);
              r = alg->setProperty(entry.first, subs(entry.second));
              if (!r.isSuccess()) return r;
           }
           return r;
        };

        // Apply the writer properties and the stream properties, where the name of the stream is
        // substituted in the stream properties
        using trans = std::function<string(const string&)>;
        trans substream = [stream](const string& val) -> string { return (boost::format(val) % stream).str(); };
        trans subgroup = [group](const string& val) -> string { return (boost::format(val) % group).str(); };
        trans nosub = [](const string& val) -> string { return val; };
        for (const auto& propEntry : {make_pair(m_writerProperties, nosub),
                                      make_pair(m_streamProperties, substream),
                                      make_pair(m_groupProperties, subgroup)}) {
           result = applyProperties(propEntry.first, propEntry.second);
        }
     } else {
        return Error(string{"Writer algoritm "} + typeName + " already exists, it shouldn't!", StatusCode::FAILURE);
     }

     // propagate the sub-algorithm into own state.
     if (result.isSuccess () &&
         Gaudi::StateMachine::INITIALIZED <= FSMState() &&
         ialg &&
         Gaudi::StateMachine::INITIALIZED  > ialg->FSMState()) {
        StatusCode sc = ialg->sysInitialize();
        if (sc.isFailure()) result = sc;
     }

     // propagate the sub-algorithm into own state.
     if (result.isSuccess () &&
         Gaudi::StateMachine::RUNNING <= FSMState() &&
         ialg &&
         Gaudi::StateMachine::RUNNING > ialg->FSMState()) {
        StatusCode sc = ialg->sysStart();
        if (sc.isFailure()) result = sc;
     }

     // Find filters from HltOutputSvc
     vector<SmartIF<IAlgorithm>> filters{};
     auto filterNames = m_forcedStreams.empty() ? outputSvc->filters(stream) : m_forcedStreams[stream];
     for (const auto& f : filterNames) {
        SmartIF<IAlgorithm> ialg = appMgr->algorithm(f, false); // do not create it now
        if (!ialg) {
           return Error(string{"Filter "} + f + " does not exist, it should!", StatusCode::FAILURE);
        }
        filters.emplace_back(std::move(ialg));
     }

     //== Is it an Algorithm ?  Strange test...
     if ( result.isSuccess() ) {
        // TODO: (MCl) it is possible to avoid the dynamic_cast in most of the
        //             cases by keeping the result of createSubAlgorithm.
        Algorithm* alg = dynamic_cast<Algorithm*>(ialg.get());
        if (alg) {
           // Note: The reference counting is kept by the system of sub-algorithms
           // If IgnoreFilters, don't pass them in here, so they are not
           m_entries.emplace_back(alg, m_ignoreFilters ? decltype(filters){} : std::move(filters));
           if (msgLevel(MSG::DEBUG)) debug () << "Added algorithm " << name << endmsg;
        } else {
           warning() << name << " is not an Algorithm - failed dynamic_cast"
                     << endmsg;
           final = StatusCode::FAILURE;
        }
     } else {
        warning() << "Unable to find or create " << name << endmsg;
        final = result;
     }
  }

  //== Print the list of algorithms
  MsgStream& msg = info();
  msg << "Member list:" << endmsg;
  ostream_joiner( msg, m_entries, endmsg, [](const AlgorithmEntry& e) -> string {
        std::stringstream m;
        auto print = [&m](const IAlgorithm* alg) {
           string typ = alg->type();
           m << ((alg->name() == typ) ? alg->name() : (typ + "/" + alg->name())) << " ";
        };
        std::for_each(begin(e.filters()), end(e.filters()),
                      [&print](const SmartIF<IAlgorithm>& e) { print(e.get());} );
        print(e.algorithm());
        return m.str();
     } );
  if ( !isDefault(context())  ) msg << ", with context '" << context() << "'";
  if ( !isDefault(rootInTES()) ) msg << ", with rootInTES '" << rootInTES() << "'";
  msg << endmsg;

  return final;
}
