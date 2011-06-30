// $Id: $
#ifndef VELOSELECTCHANNELIDONTRACK_H 
#define VELOSELECTCHANNELIDONTRACK_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"

// Kernel/LHCbKernel
#include "Kernel/IVeloChannelIDSelector.h"            // Interface

// Forward declarations
namespace LHCb {
  class VeloChannelID;
}

/** @class VeloSelectChannelIDOnTrack VeloSelectChannelIDOnTrack.h
 *  
 *  Tool for selecting channelIDs on a set of input tracks
 *  (largely copied from STSelectChannelIDOnTrack)
 *
 *  @author Eduardo Rodrigues
 *  @date   2011-06-28
 */
class VeloSelectChannelIDOnTrack : public GaudiTool,
                                   virtual public IVeloChannelIDSelector,
                                   virtual public IIncidentListener
{
public: 
  /// Standard constructor
  VeloSelectChannelIDOnTrack( const std::string& type, 
                              const std::string& name,
                              const IInterface* parent);
  
  virtual ~VeloSelectChannelIDOnTrack( ); ///< Destructor
  
  /** "select"/"preselect" method 
   *  @param  channelID pointer to object to be selected 
   *  @return true if channelID is selected
   */
  virtual bool select( const LHCb::VeloChannelID& cluster ) const ;
  
  /** "select"/"preselect" method (functor interface)
   *  @param  channelID pointer to object to be selected 
   *  @return true if channelID is selected
   */
  virtual bool operator() ( const LHCb::VeloChannelID& cluster ) const ;
  
  /// Intialize
  StatusCode initialize();
  
  /** Implement the handle method for the Incident service.
   *  This is used to inform the tool of software incidents.
   *
   *  @param incident The incident identifier
   */
  void handle( const Incident& incident );
  
protected:

private:
  /// Default constructor is private 
  VeloSelectChannelIDOnTrack();
  /// Copy constructor is private 
  VeloSelectChannelIDOnTrack( const VeloSelectChannelIDOnTrack& );
  /// Assignement operator is private 
  VeloSelectChannelIDOnTrack& operator=( const VeloSelectChannelIDOnTrack& );

  /// initEvent
  void initEvent() const;
  
private:
  std::string m_trackLocation;

  mutable std::vector<LHCb::VeloChannelID> m_ids;
  mutable bool m_configured;

};
#endif // VELOSELECTCHANNELIDONTRACK_H
