// $Header: 

#ifndef L0MUON_L0MPAD_H     
#define L0MUON_L0MPAD_H     1 

#include "GaudiKernel/ContainedObject.h" 
#include "GaudiKernel/SmartRef.h" 

#include "LHCbEvent/MCParticle.h" 
#include "LHCbEvent/MuonDigit.h"    
#include "L0Muon/MuonTile.h"

// Externals
extern const CLID& CLID_L0mPad;           

class MsgStream;
class MuonLayout;

/** @class L0mPad L0mPad L0Muon/L0mPad.h

   Muon system pads to be used in the L0 trigger logic
   
   @author  Andrei Tsaregorodtsev
   @date  06 April 2001
*/   


class L0mPad : virtual public ContainedObject,
                       public MuonTile  {

public:

    /// Default constructor
    L0mPad();
    /// Constructor setting the L0mPad data members
    L0mPad(int st, int iq, int ir, int nx, int ny, MuonLayout ml);
	      
    ~L0mPad();	       
    /// accessor to the station number
    long station() {return m_station;}
    /// accessor to the pad ID
    long id() {return m_id;}
    /// accessor to the pad X-coordinate
    double x() {return m_x;}
    /// accessor to the pad Y-coordinate
    double y() {return m_y;}
    /// accessor to the pad Z-coordinate
    double z() {return m_z;}
    /// accessor to the X error (pad size)
    double dx() {return m_dx;}
    /// accessor to the Y error (pad size)
    double dy() {return m_dy;}
    /// accessor to the Z error 
    double dz() {return m_dz;}
    
    /// set pad ID
    void setId(long value) {m_id = value;}
    /// set pad X-coordinate
    void setX(double value) {m_x = value;}
    /// set pad Y-coordinate
    void setY(double value) {m_y = value;}
    /// set pad Z-coordinate
    void setZ(double value) {m_z = value;}
    /// set pad X error (pad size)
    void setDx(double value) {m_dx = value;}
    /// set pad Y error (pad size)
    void setDy(double value) {m_dy = value;}
    /// set pad Z error (pad size)
    void setDz(double value) {m_dz = value;}
    
    /// dump the pad contents to the log stream
    void print(MsgStream log);
    
  /// Retrieve pointer to vector of MCParticles (const or non-const)
  const SmartRefVector<MCParticle>& mcParticles() const;
        SmartRefVector<MCParticle>& mcParticles();
  /// Update all MCParticles
  void setMCParticles( const SmartRefVector<MCParticle>& value );
  /// Remove all MCParticles
  void removeMCParticles();
  /// Add single MCParticle to SmartRefVector<MCParticle>
  ///   (by a C++ pointer or a smart reference)
  void addMCParticle( MCParticle* value );
  void addMCParticle( SmartRef<MCParticle> value );       
    
/// Retrieve pointer to class defininition structure
    virtual const CLID& clID() const { return L0mPad::classID(); }
    static const CLID& classID()     { return CLID_L0mPad; }          
    
private:
    long m_station;
    
    double m_x;
    double m_y;
    double m_z;
    double m_dx;
    double m_dy;
    double m_dz;
    
    long m_id;
    SmartRefVector<MCParticle>    m_mcParticles;         

};

/// Retrieve pointer to vector of MCParticles (const or non-const)
inline const SmartRefVector<MCParticle>& L0mPad::mcParticles() const
   {
  return m_mcParticles;
}
inline       SmartRefVector<MCParticle>& L0mPad::mcParticles()
   {
  return m_mcParticles;
}
/// Update all MCParticles
inline void L0mPad::setMCParticles( const SmartRefVector<MCParticle>& value )
   {
  m_mcParticles = value;
}
/// Remove all MCParticles
inline void L0mPad::removeMCParticles()
   {
  m_mcParticles.clear();
}
/// Add single MCParticle to SmartRefVector<MCParticle>
///   (by a C++ pointer or a smart reference)
inline void L0mPad::addMCParticle( MCParticle* value )
   {
  m_mcParticles.push_back(value);
}
inline void L0mPad::addMCParticle( SmartRef<MCParticle> value )
   {
  m_mcParticles.push_back(value);
}  

bool operator==(L0mPad lpd, MuonTile mt);

#endif  // L0MUON_L0MPAD_H 
