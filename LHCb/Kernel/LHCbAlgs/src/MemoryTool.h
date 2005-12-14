// $Id: MemoryTool.h,v 1.1 2005-12-14 13:22:21 cattanem Exp $
#ifndef MEMORYTOOL_H 
#define MEMORYTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "Kernel/IGenericTool.h"            // Interface


/** @class MemoryTool MemoryTool.h
 *  Tool to plot memory usage of the application at each call
 *
 *  @author Marco Cattaneo
 *  @date   2005-12-14
 */
class MemoryTool : public GaudiHistoTool, virtual public IGenericTool {
public: 
  /// Standard constructor
  MemoryTool( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  virtual ~MemoryTool( ); ///< Destructor

  void execute(); ///< Plot the current memory usage 

protected:

private:
  unsigned int m_counter; ///< Counter of calls to the tool
  unsigned int m_bins;    ///< Number of bins of histogram (Property HistoSize)  
};
#endif // MEMORYTOOL_H
