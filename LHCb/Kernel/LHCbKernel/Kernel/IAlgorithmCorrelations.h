#ifndef KERNEL_IALGORITHMCORRELATIONS_H
#define KERNEL_IALGORITHMCORRELATIONS_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_IAlgorithmCorrelations ( "IAlgorithmCorrelations", 1, 0 );

/** @class IAlgorithmCorrelations IAlgorithmCorrelations.h Kernel/IAlgorithmCorrelations.h
 *
 *  Tool to print a table of correlations of Bernoullian variables.
 *    The list of algorithms (or any string) is set at initialization with
 *      StatusCode algorithms(const std::vector<std::string>&)
 *    At each event, for each algorithm the result is given by:
 *      StatusCode fillResult(const std::string& algo,const bool& result)
 *    Then at the end of each event on needs to trigger the correlations:
 *       StatusCode endEvent()
 *    The statistics are printed at finalization (or anywhere) with
 *       StatusCode printTable(void)
 *       StatusCode printList(void) (for debugging)
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-04-19
 */
class IAlgorithmCorrelations : virtual public IAlgTool {
public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IAlgorithmCorrelations; }
  ///< Set algorithms for which correlations are wanted (column, subject)
  virtual StatusCode algorithms(const std::vector<std::string>&) = 0 ;
  ///< (optional) Set algorithms to check correlations against (row, object)
  virtual StatusCode algorithmsRow(const std::vector<std::string>&) = 0 ;

  ///< Fill results one by one for each algorithm
  virtual StatusCode fillResult(const std::string& algo,const bool& result) = 0 ;
  ///< Actually tell the tool that we reached the end of the event
  virtual StatusCode endEvent() = 0 ;

  ///< Print correlation table
  virtual StatusCode printTable(void) = 0 ;
  ///< Print list of results
  virtual StatusCode printList(void) = 0 ;

};
#endif // KERNEL_IALGORITHMCORRELATIONS_H
