#ifndef REPORTCHECKTOOL_H 
#define REPORTCHECKTOOL_H 1

#include "IReportCheck.h"
// Include files
// from STL
#include <string>
#include <map>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// From PhysEvent
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"

#include "Event/HltDecReports.h"
#include "Event/HltSelReports.h"
#include "Event/HltVertexReports.h"
#include "Event/HltDecReports.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"

using std::map;
using std::string;

/** @class ReportCheckTool ReportCheckTool.h
 *  
 *  Tool to check if version number of the HltSelReports 
 *
 *  @author Sean Benson
 *  @date   02/10/2014
 */
class ReportCheckTool : public GaudiTool, virtual public IReportCheck
{
  public:
    /// Standard constructor
    ReportCheckTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);
    ///  Destructor
    virtual ~ReportCheckTool( ) { };
  
    //===========================================================================
    /// Check version of the reports from an HltSelReports object.
    int VersionTopLevel(string,string); 
    //===========================================================================
    /// Check version of the reports from an HltSelReports object.
    int checkBankVersion();
    //===========================================================================
    /// Check TCK.
    unsigned int getTCK();

  protected:
  
  private:
    map<string,int> version_map_particle
      ,version_map_vertex
      ,version_map_rpid
      ,version_map_track;
  
}; // End of class header. 

#endif // REPORTCHECKTOOL_H
