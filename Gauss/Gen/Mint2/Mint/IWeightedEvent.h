#ifndef IWeightedEvent_HH
#define IWeightedEvent_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 2 March 2015

// The point of this class is to communicate to
// Neg2LL if the events in the eventlist are weighted
// or not. Events that are weighted should inherit
// from this abstract base class.
// Using the getWeight functions, Neg2LL
// (and possibly other methods) gets events with
// weight 1 for any other type of event, without
// having to know about the event type in anyw
// way, so it keeps it completely general,
// people can still use doubles or whatever
// they want as event type w/o having
// to implement a weight function if they
// don't want one. If you do want an event weight, though,
// and you want to use Neg2LL to implement that weight,
// you have to make sure your event class
// inherits from IWeightedEvent.
// 


namespace MINT{
  class IWeightedEvent{
  public:
    virtual double getWeight() const=0;
  };
  
  template<typename EVT_TYPE>
    double getWeight(const EVT_TYPE& ){
    return 1;
  }
  template<> inline
    double getWeight(const MINT::IWeightedEvent& evt){
    return evt.getWeight();
  }

}//namespace MINT

#endif
//
