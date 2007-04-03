// $Id: DeRichLocations.h,v 1.1 2007-04-03 15:42:32 papanest Exp $
#ifndef RICHDET_DERICHHPDPANELLOCATIONS_H
#define RICHDET_DERICHHPDPANELLOCATIONS_H 1

/** @namespace DeRichHPDPanelLocations
 *
 *  Namespace for the location of Rich Detector Elelements in xml
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 *
 */
namespace DeRichLocations
{
  /// DeRich location
  /// Rich1 location in transient detector store
  static const std::string& Rich1 = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1";

  /// Rich2 location in transient detector store
  static const std::string& Rich2 = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2";

  /// RichSystem location in transient detector store
  static const std::string& RichSystem = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem";

  /// Location of Rich1 top panel
  static const std::string& Rich1Panel0 =
  "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0";

  /// Location of Rich1 bottom panel
  static const std::string& Rich1Panel1 =
  "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel1";

  /// Location of Rich2 left panel
  static const std::string& Rich2Panel0 =
  "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem/HPDPanel0";

  /// Location of Rich2 right panel
  static const std::string& Rich2Panel1 =
  "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem/HPDPanel1";

  /// Multi solid aerogel location
  static const std::string& Aerogel = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Aerogel";

  /// Rich1 gas (C4F10) location
  static const std::string& Rich1Gas = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Rich1Gas";
  static const std::string& C4F10    = Rich1Gas;

  /// Rich2 gas (CF4) location
  static const std::string& Rich2Gas  = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/Rich2Gas";
  static const std::string& CF4       = Rich2Gas;

  /// Rich1 Beampipe location in TDS
  static const std::string& Rich1BeamPipe = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Rich1BeamPipe";
  /// Rich2 Beampipe location in TDS
  static const std::string& Rich2BeamPipe = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/Rich2BeamPipe";}

// ***********************************************************************
// old name spaces for compatibility

/** @namespace DeRichLocation
 *
 *  Namespace for the xml location of the detector elements RichSystem
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
namespace DeRichLocation
{
  /// Rich1 location in transient detector store
  static const std::string& Rich1 = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1";

  /// Rich2 location in transient detector store
  static const std::string& Rich2 = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2";

  /// RichSystem location in transient detector store
  static const std::string& RichSystem = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem";
}

namespace DeRichHPDPanelLocation
{
  /// Location of Rich1 top panel
  static const std::string& Rich1Panel0 =
  "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel0";

  /// Location of Rich1 bottom panel
  static const std::string& Rich1Panel1 =
  "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/PDPanel1";

  /// Location of Rich2 left panel
  static const std::string& Rich2Panel0 =
  "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem/HPDPanel0";

  /// Location of Rich2 right panel
  static const std::string& Rich2Panel1 =
  "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/RichSystem/HPDPanel1";
}

/** @namespace DeRichRadiatorLocation
 *
 *  Namespace for the location of radiators in xml
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
namespace DeRichRadiatorLocation
{
  /// Multi solid aerogel location
  static const std::string& Aerogel = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Aerogel";

  /// Rich1 gas (C4F10) location
  static const std::string& Rich1Gas = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Rich1Gas";
  static const std::string& C4F10    = Rich1Gas;

  /// Rich2 gas (CF4) location
  static const std::string& Rich2Gas  = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/Rich2Gas";
  static const std::string& CF4       = Rich2Gas;
}

/** @namespace DeRichBeamPipeLocation
 *
 *  Namespace for the xml location of the detector elements DeRichBeamPipe
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
namespace DeRichBeamPipeLocation
{
  /// Rich1 Beampipe location in TDS
  static const std::string& Rich1BeamPipe = "/dd/Structure/LHCb/BeforeMagnetRegion/Rich1/Rich1BeamPipe";
  /// Rich2 Beampipe location in TDS
  static const std::string& Rich2BeamPipe = "/dd/Structure/LHCb/AfterMagnetRegion/Rich2/Rich2BeamPipe";
}
#endif // RICHDET_DERICHHPDPANELLOCATIONS_H
