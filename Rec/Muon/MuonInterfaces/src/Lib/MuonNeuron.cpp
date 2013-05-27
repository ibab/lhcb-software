// $Id: MuonNeuron.cpp,v 1.1 2010-02-10 19:07:50 ggiacomo Exp $
#include "MuonInterfaces/MuonNeuron.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
//=============================================================================

MuonNeuron::MuonNeuron() {
}


MuonNeuron::MuonNeuron(MuonHit *h, 
		       MuonHit *t) : 
     m_head( h ),
     m_tail( t ),
     m_status(1.0),
     m_station(-1),
     m_region(-1),
     m_headID(-1),
     m_tailID(-1),
     m_ID(-1)
{
  m_neuron = *t - *h;
  m_neuronXZ.SetX(m_neuron.X());
  m_neuronXZ.SetY(0.);
  m_neuronXZ.SetZ(m_neuron.Z());
  m_neuronYZ.SetX(0.);
  m_neuronYZ.SetY(m_neuron.Y());
  m_neuronYZ.SetZ(m_neuron.Z());
}

//=============================================================================

MuonNeuron::MuonNeuron(MuonHit *h, 
		       MuonHit *t,
		       int s,
		       int r) : 
     m_head( h ),
     m_tail( t ),
     m_status(1.0),
     m_station( s ),
     m_region( r ),
     m_headID(-1),
     m_tailID(-1),
     m_ID(-1)
{
  //  m_head = h; 
  //  m_tail = t;
  m_neuron = *t - *h;
  m_neuronXZ.SetX(m_neuron.X());
  m_neuronXZ.SetY(0.);
  m_neuronXZ.SetZ(m_neuron.Z());
  m_neuronYZ.SetX(0.);
  m_neuronYZ.SetY(m_neuron.Y());
  m_neuronYZ.SetZ(m_neuron.Z());
}

//=============================================================================

MuonNeuron::MuonNeuron(MuonHit *h, 
	     MuonHit *t,
	     int hID,
	     int tID,
	     int s,
	     int r) : 
     m_head( h ),
     m_tail( t ),
     m_status(1.0),
     m_station( s ),
     m_region( r ),
     m_headID( hID ),
     m_tailID( tID ),
     m_ID(-1)
{
  //  m_headID = hID;
  //  m_tailID = tID;
  //  m_station = s;
  //  m_region = r;
  m_neuron = *t - *h;
  m_neuronXZ.SetX(m_neuron.X());
  m_neuronXZ.SetY(0.);
  m_neuronXZ.SetZ(m_neuron.Z());
  m_neuronYZ.SetX(0.);
  m_neuronYZ.SetY(m_neuron.Y());
  m_neuronYZ.SetZ(m_neuron.Z());
}
//=============================================================================

//=============================================================================
// Destructor
//=============================================================================
MuonNeuron::~MuonNeuron() {} 

//=============================================================================
/// clean up weights
void MuonNeuron::cleanupWeights(){
  std::list< std::pair< MuonNeuron*,double > >::iterator iw=m_weight_list.begin();
  
  double wmaxTH = -9999;
  double wmaxHT = -9999;
  MuonNeuron* pmaxTH=0;
  MuonNeuron* pmaxHT=0;
  while( (iw != m_weight_list.end()) && (! m_weight_list.empty())){

    MuonNeuron* ConnNeur = iw->first;
    if(tailHead(*ConnNeur)) {
      if(iw->second >= wmaxTH) {
        wmaxTH = iw->second;
        pmaxTH = iw->first;
      }
      iw = m_weight_list.erase(iw);
      (*ConnNeur).removeWeight(this);
      
    } else if(headTail(*ConnNeur)) {
      if(iw->second >= wmaxHT) {
        wmaxHT = iw->second;
        pmaxHT = iw->first;
      }
      iw = m_weight_list.erase(iw);
      (*ConnNeur).removeWeight(this);
      
    } else {
      
      iw++;
    }
  }
  if( pmaxTH !=0 ) {
    this->setWeight(pmaxTH,wmaxTH);
    pmaxTH->setWeight(this,wmaxTH);
  }
  
  if( pmaxHT !=0 ) {
    this->setWeight(pmaxHT,wmaxHT);
    pmaxHT->setWeight(this,wmaxHT);
  }
}
/// kill double length neurons if there is a unit length one
void MuonNeuron::killDoubleLength(){
  std::list< std::pair< MuonNeuron*,double > >::iterator iw=m_weight_list.begin();
  bool go = false;

  // first check if there is at least 1 unit length neuron connected to this
  for(; iw != m_weight_list.end() ; iw ++){
    if(iw->first->stationDifference() == 1){
      go = true;
      break;
    }
  }

  if( !go ) return;
  iw=m_weight_list.begin();
  
  while( (iw != m_weight_list.end()) && (! m_weight_list.empty())){

    MuonNeuron* ConnNeur = iw->first;
    if(headHead(*ConnNeur) || tailTail(*ConnNeur)) {
      
      if(ConnNeur->stationDifference() > 1){
        iw = m_weight_list.erase(iw);
        (*ConnNeur).removeWeight(this);
      } else {
        
        iw++;
      }
    } else {
      
      iw++;
    }
  }
}

//
void MuonNeuron::removeWeight( MuonNeuron* pn ){
  std::list< std::pair< MuonNeuron*,double > >::iterator iw=m_weight_list.begin();
  for( ; iw != m_weight_list.end(); iw++){
    if( pn == iw->first ) {
      iw = m_weight_list.erase(iw);
      break;
    }
  }
}

/// kill double length neurons if there is a unit length one
void MuonNeuron::killDoubleLength( const float angcut ){
  MuonNeuron* ConnNeur=0;
  MuonNeuron* ConnNeur1=0;
  MuonNeuron* ConnNeur2=0;
  std::list< std::pair< MuonNeuron*,double > >::iterator iw, iw1, iw2;

  if(this->stationDifference() != 1 ) return;

  // find L2 neurons in HH or TT and kill them if angleXZ smaller than 0.1 

  iw=m_weight_list.begin();
  
  while( (iw != m_weight_list.end()) && (! m_weight_list.empty())){

    ConnNeur = iw->first;
    if       ( headHead(*ConnNeur) || tailTail(*ConnNeur) ) {
      if(ConnNeur->stationDifference() > 1 && this->angleXZ(*ConnNeur) < angcut){
        iw = m_weight_list.erase(iw);
        ConnNeur->removeWeight(this);
      } else {
        
        iw++;
      }

    } else {
      iw++;
    }
  }
  // find L1 neurons in HT. Then search for L2 neurons in HT close in angleXZ
  // to the L1 one and kill them if angleXZ smaller than 0.1 

  iw1=m_weight_list.begin();
  
  while( (iw1 != m_weight_list.end()) && (! m_weight_list.empty())){

    ConnNeur1 = iw1->first;
    if( headTail(*ConnNeur1) && ConnNeur1->stationDifference() == 1) { 
      // ok found an L1 HT with this. now search for an L2 HT
      iw2 = m_weight_list.begin();
      while( (iw2 != m_weight_list.end()) && (! m_weight_list.empty())) {
        ConnNeur2 = iw2->first;
        if(headTail(*ConnNeur2) &&
           ConnNeur2->stationDifference() > 1 && 
           ConnNeur2->angleXZ(*ConnNeur1) < angcut){
          iw2 = m_weight_list.erase(iw2); // remove L2 from the list
          ConnNeur2->removeWeight(this); // remove this from L2
        
        } else {
          iw2++;  // next one
        }
      }
      iw1++;
      
    } else {
      iw1++; //look for next L1
    }
  }
  // find L1 neurons in TH. Then search for L2 neurons in TH close in angleXZ
  // to the L1 one and kill them if angleXZ smaller than 0.1 

  iw1=m_weight_list.begin();
  
  while( (iw1 != m_weight_list.end()) && (! m_weight_list.empty())){

    ConnNeur1 = iw1->first;

    if( tailHead(*ConnNeur1) && ConnNeur1->stationDifference() == 1) { 
      // ok found an L1 HT with this. now search for an L2 HT
      iw2 = m_weight_list.begin();
      while( (iw2 != m_weight_list.end()) && (! m_weight_list.empty())) {
        ConnNeur2 = iw2->first;

        if( tailHead(*ConnNeur2) &&
            ConnNeur2->stationDifference() > 1 && 
            ConnNeur2->angleXZ(*ConnNeur1) < angcut){
          iw2 = m_weight_list.erase(iw2); // remove L2 from the list
          ConnNeur2->removeWeight(this); // remove this from L2
        
        } else {
          iw2++;  // next one
        }
      }
      iw1++;
      
    } else {
      iw1++; //look for next L1
    }
  }
}





/// neuron length in terms of stations crossed
int MuonNeuron::stationDifference()
{
  return ( m_head->station() - m_tail->station() );
}

/// return neuron length
double MuonNeuron::len() const {
  return sqrt(m_neuron.Mag2());
}

/// return neuron length along Z
double MuonNeuron::deltaZ() const {
  return (m_head->Z()-m_tail->Z());
}

/// return distance along Z between the neuron station and the closest one
double MuonNeuron::deltaZ( const int st ) const {
  double zsta[5]={12100, 15200, 16400, 17600, 18800}; 
  return (zsta[st]-m_tail->Z());
}

/// return neuron length in the xz plane
double MuonNeuron::lenXZ() const {
  return sqrt(m_neuronXZ.Mag2());
}

/// return neuron length in the yz plane
double MuonNeuron::lenYZ() const {
  return sqrt(m_neuronYZ.Mag2());
}

/// angle with another neuron in space
double MuonNeuron::angle( const MuonNeuron &n ) {
  return ROOT::Math::VectorUtil::Angle(m_neuron, n.m_neuron);
}
 
/// angle with another neuron in XZ
double MuonNeuron::angleXZ( const MuonNeuron &n ) {
  return ROOT::Math::VectorUtil::Angle(m_neuronXZ, n.m_neuronXZ);
} 

/// angle with another neuron in YZ
double MuonNeuron::angleYZ( const MuonNeuron &n ) {
  return ROOT::Math::VectorUtil::Angle(m_neuronYZ, n.m_neuronYZ);
} 

/// check if this is th with n
bool MuonNeuron::tailHead(const MuonNeuron& n) const {
  return (m_tail->hitID() == n.m_head->hitID());
}

/// check if this is ht with n
bool MuonNeuron::headTail(const MuonNeuron& n) const {
  return (m_head->hitID() == n.m_tail->hitID());
}

/// check if this is tt with n
bool MuonNeuron::tailTail(const MuonNeuron& n) const {
  return (m_tail->hitID() == n.m_tail->hitID());
}

/// check if this is hh with n
bool MuonNeuron::headHead(const MuonNeuron& n) const {
  return (m_head->hitID() == n.m_head->hitID());
}

/// check if this is connected  to n
bool MuonNeuron::connectedTo( const MuonNeuron& n) const{
  return ( tailTail(n) || headHead(n) || tailHead(n) || headTail(n) );
}

/// return the neuron projection XZ
ROOT::Math::XYZVector MuonNeuron::projXZ(){
  return m_neuronXZ;
}

/// return the neuron projection YZ
ROOT::Math::XYZVector MuonNeuron::projYZ(){
  return m_neuronYZ;
}

/// store the weights
void MuonNeuron::setWeight( MuonNeuron* n, double ww ){
  m_weight.first = n;
  m_weight.second = ww;
  m_weight_list.push_back(m_weight);
}

/// retireve the weights
std::list< std::pair< MuonNeuron*,double > >  MuonNeuron::getWeights(){
  return m_weight_list;
}

/// set neuron status
void MuonNeuron::setStatus( double st ){
  m_status = st;
}

/// return  neuron status
double MuonNeuron::status(){
  return m_status;
}

/// Neuron ID
void MuonNeuron::setNeuronID( int id ){
  m_ID = id;
}

/// retireve ID
int MuonNeuron::neuronID(){
  return m_ID;
}

/// assign an ID to head and tail points
void MuonNeuron::setHeadTailID( int hID, int tID ){
  m_headID = hID;
  m_tailID = tID;
}

/// retireve head and tail IDs. First = head, Second = tail
std::pair <int, int> MuonNeuron::headTailID(){
  std::pair<int, int> tmp;
  tmp.first = m_headID;
  tmp.second = m_tailID;
  return tmp;
}


