// $Id: MicroDSTTool.h,v 1.4 2009-07-30 10:05:04 jpalac Exp $
#ifndef MICRODST_MICRODSTTOOL_H 
#define MICRODST_MICRODSTTOOL_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiTool.h>
#include <MicroDST/MicroDSTCommon.h>

/** @class MicroDSTTool MicroDSTTool.h MicroDST/MicroDSTTool.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-04
 */
class GAUDI_API MicroDSTTool : public MicroDSTCommon<GaudiTool>
{

public:
  
  /// Standard constructor
  MicroDSTTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);

  virtual StatusCode initialize();

  virtual ~MicroDSTTool( ); ///< Destructor

};

#endif // MICRODST_MICRODSTTOOL_H
