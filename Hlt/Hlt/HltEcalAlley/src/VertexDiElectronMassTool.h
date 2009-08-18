// $Id: VertexDiElectronMassTool.h,v 1.2 2009-08-18 12:48:47 witekma Exp $
#ifndef VERTEXDIELECTRONMASSTOOL_H 
#define VERTEXDIELECTRONMASSTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Event/Track.h"
#include "Event/RecVertex.h"
#include "TrackInterfaces/IFunctionTool.h"            // Interface
#include "Event/L0CaloCandidate.h"
#include "CaloInterfaces/ICaloClusterization.h"
#include "CaloDet/DeCalorimeter.h"

/** @class VertexDiElectronMassTool VertexDiElectronMassTool.h
 *  
 *
 *  @author Mariusz Witek
 *  @date   2008-07-05
 */
class VertexDiElectronMassTool : public GaudiTool, 
                        virtual public IRecVertexFunctionTool {
public: 
  /// Standard constructor
  VertexDiElectronMassTool( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~VertexDiElectronMassTool( ); ///< Destructor

  StatusCode initialize();
  
  double function(const LHCb::RecVertex& vertex);

private:

};
#endif // VERTEXDIELECTRONMASSTOOL_H
