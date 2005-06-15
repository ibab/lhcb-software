#ifndef TrackKeys_H
#define TrackKeys_H 1

//! @author J.A. Hernando
//! @date 13/04/2005
  

namespace TrackKeys 
{
  enum History {HistoryUnknown=0,
                TrackIdealPR,
                TrSeeding,
                TrVelo,
                TrLong,
                TrVeloTT,
                TrMatching,
                TrKshort,
                TrgForward,
                TrgVeloTT};
    

  enum HistoryFit {FitUnknown=0,
                   Kalman};


  
  enum Types {TypeUnknown=0,
              Velo,
              VeloR,
              Long,
              Upstream,
              Downstream,
              Ttrack};

  enum Status {StatusUnknown=0,
               PatRecIDs,
               PatRecMeas,
               Fitted,
               FitFailed};

  enum Flags {Backward=1,
              Invalid=2,
              Unique=4,
              IPSelected=8,
              PIDSelected=16};
};
#endif
