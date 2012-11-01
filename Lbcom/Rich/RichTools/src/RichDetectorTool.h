
//-----------------------------------------------------------------------------
/** @file RichDetectorTool.h
 *
 *  Header file for tool : Rich::DetectorTool
 *
 *  @author Antonis Papanestis
 *  @date   2012-10-26
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHDETECTORTOOL_H
#define RICHTOOLS_RICHDETECTORTOOL_H 1

// STL
#include <vector>

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichDetectorTool.h"

// RichDet
#include "RichDet/DeRich.h"

namespace Rich
{
  //---------------------------------------------------------------------------
  /** @class DetectorTool RichDetectorTool.h
   *
   *  A tool to preform the manipulation of RichDetector channel identifiers
   *
   *  @author Antonis Papanestis
   *  @date   2012-11-01
   *
   */
  //---------------------------------------------------------------------------

  class DetectorTool : public Rich::ToolBase,
                       virtual public IDetectorTool
  {

  public: // Methods for Gaudi Framework

    /// Standard constructor
    DetectorTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent );

    virtual ~DetectorTool( ); ///< Destructor

    // Initialization of the tool after creation
    virtual StatusCode initialize();

  public: // methods (and doxygen comments) inherited from interface

    // method to return pointer to existing Rich detectors
    const std::vector<DeRich*>& deRichDetectors() const;

  private:

    /// pointers to DeRich detector objects
    mutable std::vector<DeRich*> m_deRichDets;

  };

}

#endif // RICHDETTOOLS_RICHDETECTORTOOL_H
