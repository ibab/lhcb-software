// $Id: MicroDSTTool.h,v 1.1.1.1 2007-12-10 09:12:42 jpalac Exp $
#ifndef MICRODST_MICRODSTTOOL_H 
#define MICRODST_MICRODSTTOOL_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiTool.h>
#include <MicroDST/MicroDSTCommon.h>
static const InterfaceID IID_MicroDSTTool ( "MicroDSTTool", 1, 0 );

/** @class MicroDSTTool MicroDSTTool.h MicroDST/MicroDSTTool.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-04
 */
class MicroDSTTool : public MicroDSTCommon<GaudiTool>
{
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_MicroDSTTool; }

  /// Standard constructor
  MicroDSTTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual ~MicroDSTTool( ); ///< Destructor


};
#endif // MICRODST_MICRODSTTOOL_H
