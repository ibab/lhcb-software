// $Id: BasePlotTool.h,v 1.5 2009-02-15 22:03:51 jonrob Exp $
#ifndef BASEPLOTTOOL_H
#define BASEPLOTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoTool.h"
#include "Kernel/IPlotTool.h"            // Interface
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/Escape.h"

/** @class BasePlotTool BasePlotTool.h
 *
 *  Base class for all plot tools
 *
 *  @author Patrick Koppenburg
 *  @date   2008-12-05
 */
class BasePlotTool : public GaudiHistoTool,
                     virtual public IPlotTool 
{

public:

  /// Standard constructor
  BasePlotTool( const std::string& type,
                const std::string& name,
                const IInterface* parent);

  virtual ~BasePlotTool( ); ///< Destructor

  //=============================================================================

  /// Fill plots using a single Particle
  virtual StatusCode fillPlots(const LHCb::Particle*,const std::string where = "");

  /// Fill plots using a LHCb::Particle::ConstVector
  virtual StatusCode fillPlots( const LHCb::Particle::ConstVector& pv,
                                const std::string where = "" );

  virtual StatusCode setPath ( const std::string& path )
  {
    setHistoDir ( path ) ;
    return StatusCode::SUCCESS;
  }

  /// Tool initialise
  StatusCode initialize();

protected:

  /// Fill plots
  virtual StatusCode fillImpl( const LHCb::Particle *p,
                               const std::string & trailer = "" ) = 0;

  /// Fill final state PID plots
  std::string histoName( const std::string & var, 
                         const LHCb::ParticleProperty* pp,
                         const std::string & trailer ) const
  {
    if (!pp) Exception("Null ParticleProperty");
    //return std::string(var+"_"+Decays::escape(pp->name())+"_"+trailer);
    // CRJ : Too many plots at one level ??
    return ( trailer.empty() ? 
             std::string(var+"_"+Decays::escape(pp->name())) :
             std::string(trailer+"/"+var+"_"+Decays::escape(pp->name())) );
  }

  LHCb::IParticlePropertySvc* ppSvc() const { return m_ppSvc; }

  /// Number of bins
  unsigned int nBins() const { return m_bins; }

  /// Returns the ParticleProperty object for a given ParticleID
  const LHCb::ParticleProperty * particleProperty( const LHCb::ParticleID& id ) const;

  /// Fill Likelihood plots
  void llPlots( const double var,
                const std::string & varName,
                const LHCb::Particle* particle,
                const LHCb::ParticleProperty* pp,
                const std::string & trailer,
                const double dllLimit = 30 ) const;

private:

  LHCb::IParticlePropertySvc* m_ppSvc; ///< PPSvc.

  unsigned int m_bins;                 ///< Number of bins

  double m_dllCut;                     ///< DLL cut value

};

#endif // BASEPLOTTOOL_H
