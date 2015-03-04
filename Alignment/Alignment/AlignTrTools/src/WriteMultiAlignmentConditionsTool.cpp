//#include "AlignmentInterfaces/IWriteMultiAlignmentConditionsTool.h"
#include "GaudiAlg/GaudiTool.h"

// STL
#include <vector>

// from AlignmentInterfaces
#include "AlignmentInterfaces/IWriteAlignmentConditionsTool.h"

class WriteMultiAlignmentConditionsTool 
  : public GaudiTool, virtual public IWriteAlignmentConditionsTool
{
public:

  /// constructor
  WriteMultiAlignmentConditionsTool( const std::string& type,
				     const std::string& name,
				     const IInterface* parent); ///< Standard constructor
  
  /// destructor
  virtual ~WriteMultiAlignmentConditionsTool();

  /// initialize
  StatusCode initialize();

  // Everything configured via options
  StatusCode write() const ;

  // write with given version
  StatusCode write( const std::string& version ) const ;

private:

  std::vector<std::string>          m_xmlWriterNames ;
  std::vector<IWriteAlignmentConditionsTool*> m_xmlWriters ;
};


/*********************************************************************************/
/* Implementation                                                                */
/*********************************************************************************/

// std
//#include <fcntl.h>
#include <sys/stat.h>
//#include <errno.h>
#include <sstream>
#include <fstream>
//#include <boost/filesystem.hpp>
#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_TOOL_FACTORY( WriteMultiAlignmentConditionsTool )

//--------------------------------------------------------------------
//
//  WriteMultiAlignmentConditionsTool : Handler for different Alignment conditions tools
//
//--------------------------------------------------------------------

WriteMultiAlignmentConditionsTool::WriteMultiAlignmentConditionsTool( const std::string& type,
								      const std::string& name,
								      const IInterface* parent)
: GaudiTool( type,name,parent)
{
  // interface
  declareInterface<IWriteAlignmentConditionsTool>(this);
  // constructer
  declareProperty("XmlWriters",m_xmlWriterNames) ;
}

WriteMultiAlignmentConditionsTool::~WriteMultiAlignmentConditionsTool()
{
  // destructor
}

StatusCode WriteMultiAlignmentConditionsTool::initialize() 
{
  StatusCode sc = GaudiTool::initialize();

  std::stringstream msg("Calling the following xml writers:\n");
  for (const auto& wr: m_xmlWriterNames) msg << "\t" << wr;
  info() << msg << endreq;

  // loading the xml writers
  for(const auto& i : m_xmlWriterNames )
    m_xmlWriters.push_back( tool<IWriteAlignmentConditionsTool>(i,this) ) ;

  return sc;
}


StatusCode WriteMultiAlignmentConditionsTool::write() const
{
  // write conditions for each writer
  for( auto& i: m_xmlWriters )
    if (i->write() == StatusCode::FAILURE) return StatusCode::FAILURE ;
  return StatusCode::SUCCESS ;
}


 // version, description and author as arguments
StatusCode WriteMultiAlignmentConditionsTool::write( const std::string& version ) const 
{
  for( auto& i: m_xmlWriters )
    if (i->write(version) == StatusCode::FAILURE) 
      return StatusCode::FAILURE ;
  return StatusCode::SUCCESS ;
}
