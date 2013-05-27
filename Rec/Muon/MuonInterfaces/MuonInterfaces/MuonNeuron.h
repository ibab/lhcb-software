// $Id: MuonNeuron.h,v 1.1 2010-02-10 19:07:50 ggiacomo Exp $
#ifndef MUONNEURON_H 
#define MUONNEURON_H 1

// Include files
#include "Math/Point3D.h"
#include "Math/Vector3D.h"
#include "Math/GenVector/VectorUtil.h"
#include "MuonInterfaces/MuonHit.h"
#include <list>
#include <iostream>

/** @class MuonNeuron MuonNeuron.h
 *  
 *  A MuonNeuron is an object that describes the neurons used in the NNET
 *  pattern recognition in the Muon Detector. A Neuron is an oriented segment
 *  conventionally pointing outward the intteraction point. It is built out 
 *  of pairs of MuonHits belonging to different Muon stations. It is 
 *  characterised by a tail, a head, a set of neurons to which it is connected
 *  to and the corresponding weights.
 *  By convention, station and region of a neuron are those of the tail
 *
 *  @author Giovanni Passaleva
 *  @date   2007-08-21
 */
class MuonNeuron {
public: 
  /// Standard constructor
  MuonNeuron();

  MuonNeuron(MuonHit *h, 
             MuonHit *t);

  MuonNeuron(MuonHit *h, 
	     MuonHit *t,
	     int s,
	     int r);

  MuonNeuron(MuonHit *h, 
	     MuonHit *t,
	     int hID,
	     int tID,
	     int s,
	     int r);

  ~MuonNeuron( ); ///< Destructor

  /// public member functions

  /// return neuron head
  inline MuonHit* head() const {
    return m_head; 
  }   
  /// returns the neuron tail
  inline MuonHit* tail() const {
    return m_tail; 
  }

  /// return the neuorn station.
  inline int station() const {
    return m_station; 
  }

  ///return neuron region
  inline int region() const {
    return m_region; 
  }

  /// calculate the neuron length in terms of crossed stations
  int stationDifference();
  
  /// return neuron length
  double len() const;
  
  /// return neuron length along z
  double deltaZ() const;
  double deltaZ( const int st ) const;
 
  /// return neuron length in xz projection
  double lenXZ() const;

  /// return neuron length in yz projection
  double lenYZ() const;
  
  /// angle with another neuron in space
  double angle( const MuonNeuron &n );
  
  /// angle with another neuron in XZ
  double angleXZ( const MuonNeuron &n );
  
  /// angle with another neuron in YZ
  double angleYZ( const MuonNeuron &n );
  
  /// check if this is th with n
  bool tailHead( const MuonNeuron& n ) const;
  
  /// check if this is ht with n
  bool headTail( const MuonNeuron& n ) const;
  
  /// check if this is tt with n
  bool tailTail( const MuonNeuron& n ) const;
  
  /// check if this is hh with n
  bool headHead( const MuonNeuron& n ) const;
  
  /// check if this is connected  to n
  bool connectedTo( const MuonNeuron& n) const;
  
  /// return the neuron projection in the xz plan
  ROOT::Math::XYZVector projXZ();
  
  /// return the neuron projection in the yz plan
  ROOT::Math::XYZVector projYZ();
  
  /// store the neuron weights
  void setWeight( MuonNeuron* n, double ww );
  
  /// return the neuron weights
  std::list< std::pair< MuonNeuron*,double > > getWeights();
  
  /// retain only the best HT and TH weights
  void cleanupWeights();
  
  ///kill double length neurons if there is a unit length one
  void killDoubleLength();
  void killDoubleLength( const float angcut);
  
  /// set a neuron status
  void setStatus( double st );
  
  /// return neuron status
  double status( );
  
  /// Neuron ID, a progressive number for debugging purposes
  void setNeuronID( int id );
  
  /// retireve ID
  int neuronID();
  
  /// assign an ID to head and tail points
  void setHeadTailID( int hID, int tID );
  
  /// retireve head and tail IDs. First = head, Second = tail
  std::pair <int, int> headTailID();
  
  
protected:
  
private:
  
  MuonHit* m_head;
  MuonHit* m_tail;
  ROOT::Math::XYZVector m_neuron;
  ROOT::Math::XYZVector m_neuronXZ;
  ROOT::Math::XYZVector m_neuronYZ;
  std::pair< MuonNeuron*,double > m_weight;
  std::list< std::pair< MuonNeuron*,double > > m_weight_list;
  double m_status;
  int m_station;
  int m_region;
  int m_headID;
  int m_tailID;
  int m_ID;
  //
  void removeWeight( MuonNeuron* pn );

};



#endif // MUONNEURON_H
