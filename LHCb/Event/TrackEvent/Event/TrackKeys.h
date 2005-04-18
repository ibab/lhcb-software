#ifndef TrackKeys_H
#define TrackKeys_H 1

//! @author J.A. Hernando
//! @date 13/04/2005
  

namespace TrackKeys 
{
  enum History {HistoryUnknown=0,
                TrLong,
                TrSeeding,
                TrKshort,
                TrMatching,
                TrVelo,
                TrVeloTT,
                TrgVelo,
                TrgVeloTT,
                TrgForward};
    

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
               PatRec,
               Ready,
               Fitted,
               Failed};

  enum Flags {Backward=1,
              Valid=2,
              Unique=4,
              IPSelected=8,
              PIDSelected=16};
};
#endif
