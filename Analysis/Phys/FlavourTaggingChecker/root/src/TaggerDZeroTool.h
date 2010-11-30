#ifndef TaggerDZeroTool_H
#define TaggerDZeroTool_H 1

#include <iostream>
#include <vector>
#include <TLorentzVector.h>
#include "Tagger.h"
#include "Event.h"
#include "GenericTool.h"
#include "SVertexOneSeedTool.h"
#include "NNetTool_MLP.h"

using namespace std;

/** @class TaggerDZeroTool
 *
 *  Tagger Tool for the D0 tagger
 *
 *  @author Marco Musy
 *  @date   2010-11-22
 */

class TaggerDZeroTool  : public GenericTool {

 public:

  TaggerDZeroTool( );
  ~TaggerDZeroTool( ){ delete m_svtool; delete tDZ; } ///< Destructor

  Tagger* tag(Event& );

 private:

  SVertexOneSeedTool* m_svtool;
  NNetTool_MLP nnet;
  Tagger* tDZ;

  TH1F* hD_doca_r ;
  TH1F* hD_doca_w ;
  TH1F* hD_ipSV_r ;
  TH1F* hD_ipSV_w ;
  TH1F* hD_IPs_r  ;
  TH1F* hD_IPs_w  ;
  TH1F* hD_mass   ;
  TH1F* hD_mass_r ;

};
#endif 
