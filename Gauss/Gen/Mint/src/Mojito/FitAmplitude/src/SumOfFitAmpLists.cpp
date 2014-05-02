#include "Mint/SumOfFitAmpLists.h"

using namespace std;
using namespace MINT;

SumOfFitAmpLists::SumOfFitAmpLists()
{
}

SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list){
  addList(list);
}
SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list_1
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_2){
  addList(list_1);
  addList(list_2);
}
SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list_1
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_2
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_3){
  addList(list_1);
  addList(list_2);
  addList(list_3);
}
SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list_1
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_2
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_3
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_4){
  addList(list_1);
  addList(list_2);
  addList(list_3);
  addList(list_4);
}
SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list_1
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_2
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_3
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_4
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_5){
  addList(list_1);
  addList(list_2);
  addList(list_3);
  addList(list_4);
  addList(list_5);
}
SumOfFitAmpLists::SumOfFitAmpLists(const counted_ptr<ILookLikeFitAmpSum>& 
				   list_1
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_2
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_3
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_4
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_5
				   ,const counted_ptr<ILookLikeFitAmpSum>&
				   list_6){
  addList(list_1);
  addList(list_2);
  addList(list_3);
  addList(list_4);
  addList(list_5);
  addList(list_6);
}

void SumOfFitAmpLists::addList(const counted_ptr<ILookLikeFitAmpSum>& 
			       list){
  _listOfLists.push_back(list);
}

counted_ptr<IIntegrationCalculator> 
SumOfFitAmpLists::makeIntegrationCalculator(){
  return makeIntegCalculator();
}

counted_ptr<IntegCalculator> 
SumOfFitAmpLists::makeIntegCalculator(){
  counted_ptr<IntegCalculator> ptr(0);
  if(_listOfLists.empty()) return ptr;

  ptr = _listOfLists[0]->makeIntegCalculator();
  if(_listOfLists.size() == 1) return ptr;

  for(unsigned int i=1; i < _listOfLists.size(); i++){
    ptr->append(_listOfLists[i]->makeIntegCalculator());
  }
  return ptr;
}

double SumOfFitAmpLists::RealVal(IDalitzEvent& evt){
  double sum=0;
  if(_listOfLists.empty()) return 0;

  for(unsigned int i=0; i < _listOfLists.size(); i++){
    sum += _listOfLists[i]->RealVal(evt);
  }
  return sum;
}

DalitzBWBoxSet SumOfFitAmpLists::makeBWBoxes(const DalitzEventPattern& pat, TRandom* rnd){
  if(_listOfLists.empty()){
    DalitzBWBoxSet emptyBoxes;
    return emptyBoxes;
  }
  DalitzBWBoxSet boxset(_listOfLists[0]->makeBWBoxes(pat, rnd));
  if(_listOfLists.size() == 1) return boxset;

  for(unsigned int i=1; i < _listOfLists.size(); i++){
    DalitzBWBoxSet boxes(_listOfLists[i]->makeBWBoxes(pat, rnd));
    boxset.add(boxes);
  }
  return boxset;
}

MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
SumOfFitAmpLists::makeEventGenerator(const DalitzEventPattern& pat, TRandom* rnd){
  MINT::counted_ptr<MINT::IUnweightedEventGenerator<IDalitzEvent> > 
    ptr(new DalitzBWBoxSet(makeBWBoxes(pat, rnd)));
  return ptr;
}

void SumOfFitAmpLists::print(std::ostream& os) const{
  os << "SumOfFitAmpList with " << _listOfLists.size()
     << " components:" << endl;
  for(unsigned int i=0; i < _listOfLists.size(); i++){
    os << " ------ " << endl;
    if(0 != _listOfLists[i]) _listOfLists[i]->print(os);
  }
}
void SumOfFitAmpLists::printNonZero(std::ostream& os) const{
  os << "SumOfFitAmpList with " << _listOfLists.size()
     << " components:" << endl;
  for(unsigned int i=0; i < _listOfLists.size(); i++){
    os << " ------ " <<  i << " ----- "  << endl;
    if(0 != _listOfLists[i]) _listOfLists[i]->printNonZero(os);
  }
  os << " =========================== " << endl;
}

// ------

std::ostream& operator<<(std::ostream& os, const SumOfFitAmpLists& sofal){
  sofal.print(os);
  return os;
}

//===========
//

