// $Id: WriteAlignmentConditions.cpp,v 1.9 2010-01-12 12:09:19 wouter Exp $


#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "AlignmentInterfaces/IWriteAlignmentConditionsTool.h"

/** @class WriteAlignmentConditions WriteAlignmentConditions.h
 *
 *  Class for printing out the full geometry tree
 *
 *  @author M.Needham
 *  @author J. van Tilburg
 *  @date   21/12/2006
 */

class WriteAlignmentConditions : public GaudiAlgorithm {

public:
 
  /// constructer
  WriteAlignmentConditions(const std::string& name, ISvcLocator *svcloc );

  /// destructer
  virtual ~WriteAlignmentConditions();

  /// initialize
  StatusCode initialize();

  // finalize
  StatusCode finalize();

  /// empty execute
  StatusCode execute() { return StatusCode::SUCCESS ; }

private:

  ToolHandle<IWriteAlignmentConditionsTool> m_tool ;
};

// Gaudi
#include "GaudiKernel/AlgFactory.h"
DECLARE_ALGORITHM_FACTORY( WriteAlignmentConditions )

//--------------------------------------------------------------------
//
//  WriteAlignmentConditions : Prints out the full geometry tree
//
//--------------------------------------------------------------------

WriteAlignmentConditions::WriteAlignmentConditions( const std::string& name, 
						    ISvcLocator* pSvcLocator ) :
  GaudiAlgorithm(name, pSvcLocator),
  m_tool("WriteAlignmentConditionsTool",this)
{
  // constructer
  declareProperty("Tool",m_tool) ;
}

WriteAlignmentConditions::~WriteAlignmentConditions()
{
  // destructer
}

StatusCode WriteAlignmentConditions::initialize() 
{
  StatusCode sc = GaudiAlgorithm::initialize();
  sc = m_tool.retrieve() ;
  return sc;
}

StatusCode WriteAlignmentConditions::finalize()
{
  StatusCode sc = m_tool->write() ;
  m_tool.release().ignore() ;
  if( sc.isSuccess() ) sc = GaudiAlgorithm::finalize() ;
  return sc ;
}
