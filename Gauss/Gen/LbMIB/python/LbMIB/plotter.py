#!/usr/bin/env python
from ROOT import gROOT, TCanvas, TH1D, TH2D, gStyle, TFile

counter = 0

# Simplified plotter for gas tool
class plotter(object):
  def __init__(self, type, bins, start, end):
    global counter
    gROOT.Reset()
    self.canv = TCanvas('c'+str(counter), 'c'+str(counter), 200, 10, 700, 700)
    self.canv.SetGridx()
    self.canv.SetGridy()
    self.type = type
    self.updrate = -1
    self.hist = None
    if self.type == '1D':
      self.hist = TH1D('H'+str(counter), 'H'+str(counter), bins, start, end)
      self.canv.SetLogy()
    elif self.type == '2D':
      self.hist = TH2D('H'+str(counter), 'H'+str(counter), bins, start, end, bins, start, end)
      self.canv.SetLogz()
      gStyle.SetPalette(1)
    else: 
      print 'False format. Use 1D or 2D.'
    counter = counter + 1

  # Set plot to update itself
  def setupdate(self, rate):
    self.updrate = rate

  # Put in a value
  def fill(self, values):
    if self.type == '1D':
      self.hist.Fill(values[0], values[1])
    elif self.type == '2D':
      self.hist.Fill(values[0], values[1], values[2])
    else:
      print 'Fill failed.'
    if self.hist.GetEntries() % self.updrate == 0 and self.updrate > 0:
      self.draw()

  # Draw 
  def draw(self):
    self.canv.cd()
    if self.type == '1D':
      self.hist.Draw()
    elif self.type == '2D':
      self.hist.Draw('colz')
    else:
      print 'Draw failed.'
    self.canv.Update()

  # Save as image
  def save(self):
    self.draw()
    self.canv.SaveAs(self.hist.GetTitle()+'.gif')

  # Save to root
  def rootsave(self):
    f = TFile.Open(self.hist.GetTitle()+'.root', 'recreate')
    f.WriteTObject(self.hist)
    f.close()


def main():
  print 'Simple draw too for gastool.'


if __name__ == '__main__':main()
