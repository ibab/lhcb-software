// Local parser for vector<pair<string,string> >
// Must be done very early.
#include "GaudiKernel/ParsersFactory.h"
namespace Gaudi
{
  namespace Parsers
  {
    static StatusCode parse( std::vector<std::pair<std::string, std::string> >& result,
                             const std::string& input)
    {
      return parse_(result, input);
    }
  }
}

// Include files
#include <iterator>
// From Gaudi
#include "GaudiKernel/IJobOptionsSvc.h"
// local
#include "ConversionDODMapper.h"

// ----------------------------------------------------------------------------
// Implementation file for class: ConversionDODMapper
//
// 17/01/2012: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(ConversionDODMapper)

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
  ConversionDODMapper::ConversionDODMapper(const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent)
    : MapperToolBase(type, name, parent)
{
  declareProperty("Transformations", m_pathTransfRules,
                  "Dictionary string->string to define the transformation rules. "
                  "The key of each rule is a regular expression to match in the requested "
                  "path and the value is the format of the source path.");
  declareProperty("Algorithms", m_algTypes,
                  "Dictionary ClassID -> AlgorithmType to be used to convert "
                  "sources in destinations");
  declareProperty("AlgorithmsOutputLevels", m_algOutLevels,
                  "Dictionary string->MsgLevel to change the message level "
                  "of a specific converter algorithm type");
  declareProperty( "InputOptionName",  m_inputOptionName  = "InputName"  );
  declareProperty( "OutputOptionName", m_outputOptionName = "OutputName" );
  //setProperty( "OutputLevel", 1 );
}

// ============================================================================
// Destructor
// ============================================================================
ConversionDODMapper::~ConversionDODMapper() {}

// ============================================================================
// Initialize
// ============================================================================
StatusCode ConversionDODMapper::initialize()
{
  const StatusCode sc = MapperToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // convert the map in a list of rules
  std::transform( m_pathTransfRules.begin(),
                  m_pathTransfRules.end(),
                  std::back_inserter(m_rules),
                  ConversionDODMapper::Rule::make );

  // FIXME: we should check if the properties Algorithms and AlgorithmsOutputLevels are consistent
  // In Python it would be:
  // unknown = set(m_algOutLevels.keys()) - set(m_algTypes.values())
  // if unknown:
  //     print "WARNING OutputLevel setting required for unknown algorithms:", list(unknown)
  return sc;
}

// ============================================================================

DataObject*
ConversionDODMapper::candidate(const std::string &path) const
{
  LOG_VERBOSE << " -> ConversionDODMapper::candidate '" << path << "'" << endmsg;
  DataObject * obj(nullptr);
  if ( !path.empty() )
  {
    obj = getIfExists<DataObject>(evtSvc(),path);
    if ( nullptr != obj )
    {
      // ... get the source object...
      LOG_VERBOSE
        << "  -> Found object of type "<< System::typeinfoName(typeid(*obj))
        << ", classID " << obj->clID()
        << endmsg;
    }
    else
    {
      LOG_VERBOSE << "  -> No source object found" << endmsg;
    }
  }
  return obj;
}

// ============================================================================

Gaudi::Utils::TypeNameString
ConversionDODMapper::algorithmForPath(const std::string & path)
{
  LOG_VERBOSE << "ConversionDODMapper::algorithmForPath '" << path << "'" << endmsg;

  // source path in the transient store
  const std::string src = transform(path);

  // If we have a source path and it points to an actual object get the source object...
  const DataObject *obj = candidate(src);
  if ( obj )
  {
    LOG_VERBOSE << " -> Found source data at '" << src << "'" << endmsg;

    // ... and choose the correct converter based on the ClassID.
    AlgForTypeMap::iterator item = m_algTypes.find(obj->clID());
    if ( item != m_algTypes.end() )
    {
      // Get the algorithm type
      const std::string &algType = item->second;

      // Choose a unique name for the algorithm instance
      std::string algName = src + "_Converter";
      std::replace( algName.begin(), algName.end(), '/', '_' );

      // Add the configuration of algorithm instance to the JobOptionsSvc
      joSvc()->addPropertyToCatalogue( algName, StringProperty(m_inputOptionName,src)   );
      joSvc()->addPropertyToCatalogue( algName, StringProperty(m_outputOptionName,path) );
      // ... including the output level
      OutLevelsMap::iterator level = m_algOutLevels.find(algType);
      if ( level != m_algOutLevels.end() )
      {
        std::stringstream lvl;
        lvl << level->second;
        joSvc()->addPropertyToCatalogue( algName, 
                                         StringProperty("OutputLevel",lvl.str()) );
      }

      // Return the algorithm type/name.
      LOG_VERBOSE << " -> Use algorithm type '" << algType << "'"
                  << " name '" << algName << "'"
                  << endmsg;
      return Gaudi::Utils::TypeNameString(algName,algType);

    }
    else
    {

      std::ostringstream msg;
      msg << " -> Unknown packer algorithm for ClassID " << obj->clID()
          << " (" << System::typeinfoName(typeid(*obj)) << "), IGNORED";
      Warning(msg.str(),StatusCode::SUCCESS).ignore();

    }
  }
  else
  {
    LOG_VERBOSE << " -> Source data missing at '" << src << "'" << endmsg;
  }

  return "";
}

// ============================================================================

std::string ConversionDODMapper::nodeTypeForPath(const std::string &path)
{
  LOG_VERBOSE << "ConversionDODMapper::nodeTypeForPath '" << path << "'" << endmsg;

  std::string retS = "";

  // If we have a source path and it points to an actual object get the source object...
  const std::string src = transform(path);
  const DataObject *obj = candidate(src);
  if ( obj )
  {
    // handle only plain DataObject instances here
    if ( obj->clID() == DataObject::classID() ) { retS = "DataObject"; }
  }
  LOG_VERBOSE << " -> NodeType = '" << retS << "'" << endmsg;

  return retS;
}

// ============================================================================

std::string ConversionDODMapper::transform(const std::string & input) const
{
  LOG_VERBOSE << " -> ConversionDODMapper::transform '" << input << "'" << endmsg;
  std::string result;

  // try each mapping rule on the input, stopping on the first non-empty result.
  for ( RulesList::const_iterator r = m_rules.begin();
        r != m_rules.end() && result.empty(); ++r )
  {
    result = r->apply(input);
  }
  
  ON_VERBOSE
  {
    if (result.empty())
    {
      verbose() << "  -> no source candidate" << endmsg;
    }
    else
    {
      verbose() << "  -> source candidate '" << result << "'" << endmsg;
    }
  }
  
  // either is empty because of no match or not because of early exit.
  return result;
}

// ============================================================================
