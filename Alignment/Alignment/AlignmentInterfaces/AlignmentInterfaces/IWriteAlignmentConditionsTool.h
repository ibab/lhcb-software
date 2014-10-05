// $Id: IWriteAlignmentConditionsTool.h,v 1.0 2014-10-03 12:00:00 wouter Exp $
#ifndef AlIGNMENTINTERFACES_IWRITEALIGNMENTCONDITIONSTOOL
#define AlIGNMENTINTERFACES_IWRITEALIGNMENTCONDITIONSTOOL

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from AlignKernel
#include "AlignKernel/AlVec.h"
#include "AlignKernel/AlSymMat.h"
#include "AlignKernel/AlMat.h"

static const InterfaceID IID_IWriteAlignmentConditionsTool ( "IWriteAlignmentConditionsTool", 1, 0 );

/** @class IWriteAlignmentConditionsTool
 *  
 *
 *  @author Wouter Hulsbergen
 *  @date   2014-10-03
 */

class IWriteAlignmentConditionsTool : virtual public IAlgTool {
public: 
  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IWriteAlignmentConditionsTool; }

  virtual StatusCode write() const = 0 ;
  
  virtual StatusCode write( const std::string& filename,
			    const std::string& topelement,
			    const std::vector<unsigned int>& depths,
			    const std::string& tag = "",
			    const std::string& author = "",
			    const std::string& description = "") const = 0 ;

};
#endif
