// $Id: RegionCuts.h,v 1.3 2009-10-15 12:41:13 silviam Exp $
#ifndef SIMSVC_REGIONCUTS_H 
#define SIMSVC_REGIONCUTS_H 1

// Include files
#include <string>
#include <vector>

/** @class RegionCuts RegionCuts.h SimSvc/RegionCuts.h
 *  
 *
 *  @author Witold POKORSKI
 *  @date   2003-06-03
 */
class RegionCuts {
public:
  /// Default constructor
  RegionCuts( ):
    m_regionName("unknown"),
    m_volumes(),
    m_gammacut(-1.0),
    m_electroncut(-1.0),
    m_positroncut(-1.0),
    m_protoncut(-1.0),
    m_aprotoncut(-1.0),
    m_neutroncut(-1.0),
    m_aneutroncut(-1.0)
  {}; 
  
  /// Standard constructor
  RegionCuts(std::string name,
             std::vector<std::string> volumes,
             double gammacut,
             double electroncut,
             double positroncut,
             double protoncut,
             double aprotoncut,
             double neutroncut,
             double aneutroncut):
    m_regionName(name),
    m_volumes(volumes),
    m_gammacut(gammacut),
    m_electroncut(electroncut),
    m_positroncut(positroncut),
    m_protoncut(protoncut),
    m_aprotoncut(-aprotoncut),
    m_neutroncut(neutroncut),
    m_aneutroncut(aneutroncut)
  {};
  
  const std::string& region() const 
  {
    return m_regionName;
  }

  const std::vector<std::string>& volumes() const
  {
    return m_volumes;
  }
  
  double gammaCut() const
  {
    return m_gammacut;
  }
  double electronCut() const
  {
    return m_electroncut;
  }
  double positronCut() const
  {
    return m_positroncut;
  }
  double protonCut() const
  {
    return m_protoncut;
  }
  double antiProtonCut() const
  {
    return m_aprotoncut;
  }
  double neutronCut() const
  {
    return m_neutroncut;
  }
  double antiNeutronCut() const
  {
    return m_aneutroncut;
  }
  
private:

  std::string m_regionName;
  std::vector<std::string> m_volumes;

  double m_gammacut;
  double m_electroncut;
  double m_positroncut;
  double m_protoncut;
  double m_aprotoncut;
  double m_neutroncut;
  double m_aneutroncut;  

};
#endif // SIMSVC_REGIONCUTS_H
