// Local parser for vector<pair<string,string> >
// Must be done very early.
#include "GaudiKernel/ParsersFactory.h"
namespace Gaudi {
  namespace Parsers {
    static StatusCode parse(std::vector<std::pair<std::string, std::string> >& result, const std::string& input) {
      return parse_(result, input);
    }
  }
}

// Include files
#include <iterator>
// From Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IJobOptionsSvc.h"
// local
#include "ConversionDODMapper.h"

// ----------------------------------------------------------------------------
// Implementation file for class: ConversionDODMapper
//
// 17/01/2012: Marco Clemencic
// ----------------------------------------------------------------------------
DECLARE_TOOL_FACTORY(ConversionDODMapper)

#define ON_VERBOSE if (UNLIKELY(msgLevel(MSG::VERBOSE)))
#define ON_DEBUG   if (UNLIKELY(msgLevel(MSG::DEBUG)))
#define LOG_VERBOSE ON_VERBOSE verbose()
#define LOG_DEBUG   ON_DEBUG debug()

// ============================================================================
// Standard constructor, initializes variables
// ============================================================================
  ConversionDODMapper::ConversionDODMapper(const std::string& type, 
                                           const std::string& name, 
                                           const IInterface* parent)
    : base_class(type, name, parent)
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
}

// ============================================================================
// Destructor
// ============================================================================
ConversionDODMapper::~ConversionDODMapper() {}

StatusCode ConversionDODMapper::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()) return sc;

  // No need to check because it's implicit in the base class initialization
  m_jos = serviceLocator()->service("JobOptionsSvc");

  // convert the map in a list of rules
  std::transform(m_pathTransfRules.begin(), m_pathTransfRules.end(),
                 std::back_inserter(m_rules),
                 ConversionDODMapper::Rule::make);

  // FIXME: we should check if the properties Algorithms and AlgorithmsOutputLevels are consistent
  // In Python it would be:
  // unknown = set(m_algOutLevels.keys()) - set(m_algTypes.values())
  // if unknown:
  //     print "WARNING OutputLevel setting required for unknown algorithms:", list(unknown)
  return sc;
}

DataObject *ConversionDODMapper::candidate(const std::string &path) const {
  if (!path.empty()) {
    if (exist<DataObject>(path)) {
      // ... get the source object...
      DataObject *obj = get<DataObject>(path);
      LOG_VERBOSE
        << "found object of type "<< System::typeinfoName(typeid(*obj))
        << ", classID " << obj->clID() << endmsg;
      return obj;
    } else {
      LOG_VERBOSE << "no source object found" << endmsg;
    }
  }
  return 0;
}

Gaudi::Utils::TypeNameString ConversionDODMapper::algorithmForPath(const std::string & path)
{
  // source path in the transient store
  const std::string src = transform(path);
  // If we have a source path and it points to an actual object get the source object...
  const DataObject *obj = candidate(src);
  if (obj) {
    // ... and choose the correct converter based on the ClassID.
    AlgForTypeMap::iterator item = m_algTypes.find(obj->clID());
    if (item != m_algTypes.end()) {
      const std::string &algType = item->second;
      // Choose a unique name for the algorithm instance
      std::string algName = src + "_Converter";
      std::replace(algName.begin(), algName.end(), '/', '_');
      // Add the configuration of algorithm instance to the JobOptionsSvc
      m_jos->addPropertyToCatalogue(algName, StringProperty("InputName", src));
      m_jos->addPropertyToCatalogue(algName, StringProperty("OutputName", path));
      // ... including the output level
      OutLevelsMap::iterator level = m_algOutLevels.find(algType);
      if (level != m_algOutLevels.end()) {
        std::stringstream lvl; lvl << level->second;
        m_jos->addPropertyToCatalogue(algName, StringProperty("OutputLevel", lvl.str()));
      }
      // Return the algorithm type/name.
      if (UNLIKELY(msgLevel(MSG::VERBOSE)))
        verbose() << "Use algorithm type '" << algType << "'" << endmsg;
      return Gaudi::Utils::TypeNameString(algName, algType);
    }
    else {
      std::ostringstream msg;
      msg << "Unknown packer algorithm for ClassID " << obj->clID()
          << " (" << System::typeinfoName(typeid(*obj)) << "), IGNORED";
      Warning(msg.str()).ignore();
    }
  }
  return "";
}

std::string ConversionDODMapper::nodeTypeForPath(const std::string &path)
{
  // If we have a source path and it points to an actual object...
  // source path in the transient store
  const std::string src = transform(path);
  // If we have a source path and it points to an actual object get the source object...
  const DataObject *obj = candidate(src);
  if (obj) {
    /// handle only plain DataObject instances here
    if (obj->clID() == DataObject::classID()) {
      return "DataObject";
    }
  }
  return "";
}

StatusCode ConversionDODMapper::finalize()
{
  m_jos.reset(); // release JobOptionsSvc
  return GaudiTool::finalize();
}

std::string ConversionDODMapper::transform(const std::string & input) const
{
  LOG_VERBOSE << "destination path '" << input << "'" << endmsg;
  std::string result;
  // try each mapping rule on the input, stopping on the first non-empty result.
  for(RulesList::const_iterator r = m_rules.begin(); r != m_rules.end() && result.empty(); ++r) {
    result = r->apply(input);
  }
  ON_VERBOSE {
    if (result.empty())
      verbose() << "no source candidate" << endmsg;
    else
      verbose() << "source candidate '" << result << "'" << endmsg;
  }
  // either is empty because of no match or not because of early exit.
  return result;
}

// ============================================================================
