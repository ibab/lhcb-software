#ifndef SRC_CONVERSIONDODMAPPER_H
#define SRC_CONVERSIONDODMAPPER_H 1

// base class
#include "MapperToolBase.h"

#include <boost/regex.hpp>

class IJobOptionsSvc;

/** @class ConversionDODMapper ConversionDODMapper.h
 *
 * Tool for automatic conversions in the transient store.
 *
 * Implements the IDODAlgMapper interface to dynamically instruct the DataOnDemandSvc
 * to call conversion algorithms that will convert some input object in the T.S.
 * the the requested one.
 *
 * The Tool must be configured with a list of path transformation rules
 * (property "Transformations") in the form of pairs of strings, where the first
 * one is a regular expression matching a possible path to produce and the
 * second one is the format of the corresponding source path.
 * E.g.: to generate the entries in ".../Phys/..." from the corresponding path
 * in ".../pPhys/...", tha pair to use is ("(.*)/Phys/(.*)", "$1/pPhys/$2").
 *
 * In addition to the path transformation rules, a mapping from source ClassID
 * to the name of the converter algorithm must be set via the property
 * "Algorithms".
 *
 * It is possible to change the default OutputLevel for all the instances of an
 * algorithm type triggered by the tool using the property
 * "AlgorithmsOutputLevels", which is a map from algorithm class name to
 * MSG::Level.
 *
 * @author Marco Clemencic
 * @date 17/01/2012
 */
class ConversionDODMapper : public MapperToolBase
{

public:

  /// Standard constructor
  ConversionDODMapper( const std::string& type, 
                       const std::string& name, 
                       const IInterface* parent );

  /// Destructor
  virtual ~ConversionDODMapper(); 

  /// Initialize the tool instance.
  virtual StatusCode initialize();

public:

  /// Return the algorithm type/name to produce the requested entry.
  ///
  /// For the given path in the transient store, try to transform it to a source
  /// location and, if it is possible, load the source object to find the type
  /// of the conversion algorithm.
  /// A unique name for the algorithm instance is chosen and the JobOptionsSvc
  /// is fed with the InputName and OutputName properties for that instance.
  ///
  /// Then the TypeNameString of the algorithm instance is returned.
  ///
  /// @see IDODAlgMapper
  virtual Gaudi::Utils::TypeNameString algorithmForPath(const std::string &path);

public:

  /// Instruct the DataOnDemandSvc to create the DataObjects for the
  /// intermediate levels of a path we can handle.
  ///
  /// If the requested path can be transformed via the known rules and the
  /// source object is a trivial DataObject, we tell the DataOnDemandSvc to
  /// create the node.
  ///
  /// @see IDODNodeMapper
  virtual std::string nodeTypeForPath(const std::string &path);

private:

  /// Convert a string using the configured mapping rules.
  /// All the rules are tried until one matches. If there is no match an empty
  /// string is returned.
  std::string transform(const std::string &input) const;

  /// Helper function to get the source candidate.
  DataObject *candidate(const std::string &path) const;

private:

  /// @{
  /// Data members corresponding to properties
  typedef std::vector<std::pair<std::string, std::string> > RulesMapProp;
  RulesMapProp m_pathTransfRules; ///!< Transformations

  typedef std::map<CLID, std::string> AlgForTypeMap;
  AlgForTypeMap m_algTypes; ///!< Algorithms

  typedef std::map<std::string, unsigned int> OutLevelsMap;
  OutLevelsMap m_algOutLevels; ///!< AlgorithmsOutputLevels
  /// @}

private:

  /// Helper class to manage the regex translation rules.
  class Rule 
  {
  public:
  
    /// Constructor.
    inline Rule(const std::string& _regexp, const std::string& _format):
      regexp(_regexp), format(_format) {}
    
    /// Apply the conversion rule to the input string.
    /// If the regex does not match the input, an empty string is returned.
    inline std::string apply(const std::string &input) const
    {
      return boost::regex_replace(input, regexp, format, boost::match_default | boost::format_no_copy);
    }
    
    /// Helper to create a Rule from a pair of strings.
    inline static Rule make(const std::pair<std::string, std::string> &p) 
    {
      return Rule(p.first, p.second);
    }
 
  private:
    
    /// Regular expression object.
    boost::regex regexp;
  
    /// Format string (see Boost documentation).
    std::string format;
  };

  ///@{
  /// List of translation rules
  typedef std::list<Rule> RulesList;
  RulesList m_rules;
  ///@}
};

#endif // SRC_CONVERSIONDODMAPPER_H
