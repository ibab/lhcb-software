#!/usr/bin/env python
# =============================================================================
# @file Beauty2Charm_ComboEngine.py
# @author Albert Puig Navarro (albert.puig@cern.ch)
# @date 2011-07-15
# =============================================================================
"""Define the ComboEngine class and provide a joinFunctors function.

ComboEngine usage:
  1) Create ComboEngine object from a list of decays we want to study and a reference
     decay.
  2) If one wants to calculate the WM functor of all sign-valid combinations of the
     list of decays, use getWMFunctor method (with lowMass and highMass)
  3) If one want to create a Selection for all the possible decays, use the
     mergedSelection with name, mass limits and an inputSelection. It will return
     a MergedSelection with the WM cut + SubstitutePID chain for each of the possible
     decays.
     
joinFunctors usage:
  1) Create several functors with the ComboEngine.
  2) Input them into the function, you will get a new functor which merges the 
     input functors removing repeated ones.
     
"""

__author__ = "Albert Puig (albert.puig@cern.ch)"

import itertools

from Gaudi.Configuration import *
from Configurables import FilterDesktop
from Configurables import SubstitutePID
from PhysSelPython.Wrappers import Selection, SelectionSequence, MultiSelectionSequence, MergedSelection

class ComboEngine(object):
  """Filter a given set of decays with the given mass window.
  
  Explanation goes here.
  """
  def __init__(self, decayList, inputDecay):
    decayList = [tuple(d) for d in decayList]
    self._decayCombos = self._getValidCombos(decayList, inputDecay)
    self.decayList = decayList
    self.referenceDecay = inputDecay

  def mergedSelection(self, name, massLow, massHigh, inputSelection):
    return MergedSelection("%s" % name, RequiredSelections=self.unmergedSelection(name, massLow, massHigh, inputSelection).values())
    
  def unmergedSelection(self, name, massLow, massHigh, inputSelection):
    return self._buildAlgorithmChain(name, self._decayCombos, massLow, massHigh, inputSelection)

  def getWMFunctor(self, massLow, massHigh):
    return self._buildUnsignedWMFunctor(self._decayCombos, massLow, massHigh)
  
  def _getValidCombos(self, decayList, inputDecay):
    inputDecaySigns = self._extractSigns(inputDecay)
    temp = {}
    for decay in decayList:
      tDecay = tuple(decay)
      for permutation in itertools.permutations(decay):
        if not tDecay in temp:
          temp[tDecay] = []
        if self._extractSigns(permutation) == inputDecaySigns:
          temp[tDecay].append(permutation)
    output = {}
    for key, decayList in temp.items():
      if decayList:
        output[key] = self._listToSet(decayList)
    return output
    
  def _extractSigns(self, particles):
    return ''.join([d[-1] for d in particles])
    
  def _reorder(self, lst, combination):
    newLst = range(len(lst))
    for i, newpos in enumerate(combination):
      newLst[i] = lst[newpos]
    return newLst
    
  def _unsignDecay(self, decay, combination=None):
    """Remove signs from a decay.
    
    @arg  decay: daughters of the decay
    @type decay: list
    
    @return: string
    
    """
    if not combination:
      combination = range(len(decay))
    myDecay = self._reorder(decay, combination)
    return [d.replace('+','').replace('-','') for d in myDecay]
    
  def _buildAlgorithmChain(self, name, decayCombos, massLow, massHigh, inputSelection):
    print 'build shit!!!!'
    uniqueCombos = []
    for nominalDecay, validDecays in decayCombos.items():
      uniqueCombos.extend(validDecays)
    uniqueCombos = list(set(uniqueCombos))
    wmSelections = {}
    for combo in uniqueCombos:
      comboName = ''.join(self._unsignDecay(combo))
      functor = self._buildUnsignedWMFunctor({combo: [combo]}, massLow, massHigh)
      _filter = FilterDesktop("%s_WMFilter_%s" % (name, comboName),
                              Code=functor)
      print '_filter', _filter
      wmSelections[comboName] = Selection("%s_WMSel_%s" % (name, comboName),
                                      Algorithm=_filter,
                                      RequiredSelections=[inputSelection])
    mySelections = {}
    for nominalDecay, validDecays in decayCombos.items():
      nominalDecayName = ''.join(nominalDecay)
      nDaughters = len(nominalDecay)
      algoList = []
      for validDecay in validDecays:
        validDecayName = ''.join(validDecay)
        wmSel = wmSelections[''.join(self._unsignDecay(validDecay))]
        _subsAlgo = SubstitutePID("%s_SubsAlg_%s" % (name, validDecayName),
                                  Code="DECTREE( 'Charm -> %s' )" % (' '.join(self.referenceDecay)), 
                                  Substitutions={})
        _subsAlgo.MaxChi2PerDoF = -666
        mySubs = {}
        for i, validDaughter in enumerate(validDecay):
          #if validDaughter == self.referenceDecay[i]:
          #  continue
          if not validDaughter in mySubs:
            mySubs[validDaughter] = self.referenceDecay[:]
          mySubs[validDaughter][i] = '^'+ mySubs[validDaughter][i]
        for sub, descriptor in mySubs.items():
          subsString = 'Charm -> ' + ' '.join(descriptor)
          _subsAlgo.Substitutions[subsString] = sub
        #print 'Combo:', 'alg =', _subsAlgo
        print 'subs:', _subsAlgo.Substitutions
        print 'code:', _subsAlgo.Code
        sel = Selection("%sSel_%s" % (name, validDecayName), Algorithm=_subsAlgo, RequiredSelections=[wmSel])
        sel = Selection(name+'Sel_'+validDecayName+'Filter',
                        Algorithm=FilterDesktop('DummyName',Code='(in_range(1764.84, MM, 1964.84))'),
                        RequiredSelections=[sel]) # hi
        algoList.append(sel)
        #mySelections[validDecayName] = sel
      print 'algoList',algoList
      mySelections[nominalDecayName] = MergedSelection("%sMSel_%s" % (name, nominalDecayName), RequiredSelections=algoList)
    print 'mySelections', mySelections
    return mySelections
  
  def _buildUnsignedWMFunctor(self, decayCombos, massLow, massHigh):
    listOfCombos = []
    for decayList in decayCombos.values():
      listOfCombos.extend(['(in_range(%s, %s, %s))' % (massLow, self._buildFunctorString(decay), massHigh) for decay in decayList])
    return ' | '.join(set(listOfCombos))
        
  def _buildFunctorString(self, daughters):
    """Build the WM functor for the given daughter particles.

    @arg  daughters: daughter combination for the D
    @type daughters: list of strings

    @return: string

    """
    return "WM(%s)" % ','.join(["'%s+'" % d for d in self._unsignDecay(daughters)])

  def _listToSet(self, lst):
    """Convert list to set.
    
    @arg  lst: list to convert
    @type lst: list
    
    @return: set
    """
    return set(itertools.chain([tuple(l) for l in lst]))

def joinFunctors(*functorList):
  listOfFunctors = []
  for functor in functorList:
    listOfFunctors.extend(functor.split(' | '))
  return ' | '.join(set(listOfFunctors))

def example():
  mn = 1864.84
  mx = 1964.84
  ce = ComboEngine([('pi+','pi-'),('K+','pi-'),('pi+','K-'),('K+','K-')],
                   ['pi+','pi-'])
  mass = ce.getWMFunctor(min,max)
  emptyFilter = FilterDesktop("Empty", Code="ALL")
  return ce.mergedSelection("Combo",mn,mx,Selection("EmptySel", Algorithm=emptyFilter))  
  return ce

if __name__ == '__main__':
  print "DecayFilter.py"
  print __author__
  print __doc__

# EOF




