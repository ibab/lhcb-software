#!/usr/bin/env python

from bisect import bisect

filedict = {'pos':0,'H2':1,'CH4':2,'CO':3,'CO2':4}

# Class holding the gas map
class gasmap(object):
  def __init__(self):
    self.Z = []
    self.dens = {'H':[],'C':[],'O':[]}
    self.buffer = 0

  # Read in a map file
  def parse(self, path):
    for entry in self.getfile(path):
      self.fill(entry)

  # Get gas entries from file
  def getfile(self, filename):
    f = open(filename,'r')
    lines = f.readlines()
    f.close()
    entries = map(lambda x: map(float, x.split()),lines)
    return entries

  # Parse gas entries
  def fill(self, entry):
    # Set position on entry
    self.Z.append(entry[filedict['pos']])
    # Set densities in atoms per m3
    self.dens['H'].append(2 * entry[filedict['H2']] + 4 * entry[filedict['CH4']])
    self.dens['C'].append(entry[filedict['CH4']] + entry[filedict['CO']] + entry[filedict['CO2']])
    self.dens['O'].append(entry[filedict['CO']] + 2 * entry[filedict['CO2']])

  # Find closest entry
  def getclosest(self, pos):
    closest = None
    # Check last entry (i.e. the "buffer")
    if pos>=self.Z[self.buffer] and self.buffer == len(self.Z)-1:
      closest = self.buffer 
    elif pos>=self.Z[self.buffer] and pos<self.Z[self.buffer+1]:
      closest = self.buffer
    else:
      closest = bisect(self.Z, pos)
      # to avoid out of index:
      if closest >= len(self.Z):
        closest = len(self.Z)-1
      self.buffer = closest
    return {'H':self.dens['H'][closest],'C':self.dens['C'][closest],'O':self.dens['O'][closest]}
      
  # Set defaults outside of the described
  def setdefault(self, h2, ch4, co, co2):
    self.Z.insert(0,self.Z[0]-0.01)
    self.Z.append(self.Z[len(self.Z)-1]+0.01)

    self.dens['H'].insert(0, 2*h2 + 4*ch4)
    self.dens['H'].append(2*h2 + 4*ch4)

    self.dens['C'].insert(0, ch4 + co + co2)
    self.dens['C'].append(ch4 + co + co2)

    self.dens['O'].insert(0, co + 2*co2)
    self.dens['O'].append(co + 2*co2)


def main():
  print 'Classes for defining gas densities'


if __name__ == '__main__':main()

