#####################################
#
# AlPlotter
#
# A class for plotting alignment
# variables from different alignment
# jobs
#
#####################################

# general imports
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from matplotlib.ticker import Formatter, FixedLocator, ScalarFormatter
from matplotlib.backends.backend_pdf import PdfPages
import datetime
import numpy as np
import re

# usr imports
from AlignOutput import AlignIter

# Dicts
units= {'Delta':'mm', 'DeltaErr':'mm', 'Cur': 'mm', 'CurTot': 'mm' }
colors=['red','blue','black','orange','green','magenta','yellow','cyan','gray','purple','crimson','chocolate','pink']

# BEGIN ALPLOTTER
class AlPlotter:
    """
    A class to plot various results from few alignment jobs
    """
    def __init__(self, alignJobs= None):
        """
            Initilisation
        """
        self.AlignJobs = alignJobs if alignJobs != None else []
        self.Plots = []
        self.xAxis = {'title': 'iterations', 'min': 0, 'max': len(self.AlignJobs), 'labels': xrange(len(self.AlignJobs))}
        return
    
    # Show plots
    def Show(self):
        """
            Plot all the created figures
        """
        plt.show()
        return
    
    # Save to PDF
    def SavePdf(self, fname):
        """
            Save to a single PDF all the created figures
        """
        Pdf = PdfPages(fname)
        for pl in self.Plots: Pdf.savefig(pl)
        self.SetPdfInfo(Pdf)
        return
    
    # Set the label and format the X axis
    def SetPltAxis(self,fig):
        """
            Set the label and format the X axis
        """
        ax=plt.gca()
        if self.xAxis['title'] == 'time':
            # matplotlib date format object
            hfmt = mdates.DateFormatter('%Y-%m-%d %H:%M')
            ax.xaxis.set_major_formatter(hfmt)
            ax.xaxis_date()
            # rotate and align the tick labels so they look better
            fig.autofmt_xdate()
        # set tick names
        if self.xAxis['title'] == 'variable' or self.xAxis.has_key('ticknames'):
            ax.set_xticks(xrange(0,len(self.xAxis['ticknames'])))
            ax.set_xticklabels(self.xAxis['ticknames'])
            ax.set_xlim(self.xAxis['min'], self.xAxis['max'])
        # ax.get_xaxis().set_major_formatter(ScalarFormatter())
        return
    
    # Create the plot frame
    def Frame(self, title = 'a plot', ylabel = 'u.a.', xlog=False, ylog=False):
        """
            Create a frame in where to plot the data
        """
        fig = plt.figure()
        # labels
        plt.title(title)
        plt.ylabel(ylabel)
        plt.xlabel(self.xAxis['title'])
        # Set X axis
        self.SetPltAxis(fig)
        # log axis
        if xlog: plt.xscale('log')
        if ylog: plt.yscale('log')
        return fig

    # Resize the axes
    def ResizeAxes(self, yenlarge=0.07, yrange=None):
        """
            Resize the axes for seeing the full data points in the plot
        """
        # enlarge by 7% on each side the plot limits
        plt.xlim(plt.xlim()[0]-yenlarge*(plt.xlim()[1]-plt.xlim()[0]),plt.xlim()[1]+yenlarge*(plt.xlim()[1]-plt.xlim()[0]))
        if yrange != None: plt.ylim(yrange['min'],yrange['max'])
        else: plt.ylim(plt.ylim()[0]-yenlarge*(plt.ylim()[1]-plt.ylim()[0]),plt.ylim()[1]+yenlarge*(plt.ylim()[1]-plt.ylim()[0]))
        return
    
    # Set x-axis title and labels
    def SetAxis(self, title, labels):
        """
            Set X-Axis title and labels
        """
        self.xAxis = {'title': 'variable', 'min': 0, 'max': len(labels), 'labels': xrange(0,len(labels)) ,'ticknames':labels }
        return
    
    # Create a figure out of an array
    def Plot(self, y, yerr=None, yrange=None, zero=False, legend=False, title = None, addToPDF=False):
        """
            Plot the content of an array
        """
        # Check data input type (multiple plots at once if dictionary)
        if type(y) == dict:
            pltErr = (yerr != None)
            if yerr!=None and y.keys() != yerr.keys():
                print 'Mismatch: Cannot plot error bars!'
                pltErr = False
            i = 0
            for name in y.keys():
                if type(y[name]) not in [list, np.ndarray]:
                    print 'ERROR: a list of values is requested as input!'
                    return
                if len(y[name]) == 0: continue
                if pltErr: plt.errorbar(self.xAxis['labels'], y[name], yerr=yerr[name], marker='o', color = colors[i], label=name, linestyle='None')
                else: plt.plot(self.xAxis['labels'], y[name], marker='o', color = colors[i], label=name, linestyle='None')
                i = i + 1
        else:
            if type(y) not in [list, np.ndarray]:
                print 'ERROR: a list of values is requested as input!'
                return
            if len(y) == 0: return
            if yerr != None: plt.errorbar(self.xAxis['labels'], y, yerr=yerr, marker='o', linestyle='None')
            else: plt.plot(self.xAxis['labels'], y,marker='o', linestyle='None')
        # add a dashed line for zero
        if zero: plt.axhline(ls='--',c='black')
        # add a legend
        if legend:
            if len(y) > 6:
                # Shrink current axis by 20%
                ax=plt.gca()
                box = ax.get_position()
                ax.set_position([box.x0, box.y0, box.width * 0.6, box.height])
                # Put a legend to the right of the current axis
                ax.legend(loc='center left', bbox_to_anchor=(1, 0.5))
            else:
                plt.legend(loc='best')
        # change title
        if title != None: plt.title(title)
        return
    
    # Plot general variable of iteration
    def PlotExpression(self, expr, yrange=None, xlog=False, ylog=False):
        """
            Plot a common variable of iteration that can be extracted from a general expression
        """
        # Get array with values
        x = self.DecodeExpression(expr)
        # Define the frame
        self.Plots += [ self.Frame( expr, expr + (' ('+units[expr]+')' if expr in units.keys() else ''), xlog=xlog, ylog=ylog) ]
        # Plot
        self.Plot(x)
        self.ResizeAxes(yrange=yrange)
        return
    
    # Plot alignable property
    def PlotAlignable(self, name, key, yrange=None, xlog=False, ylog=False):
        """
            Plot a property of an alignable for the stored jobs
        """
        # store values for specific key of alignable
        x = [self.GetAttr(key, alJob.Alignables[name]) for alJob in self.AlignJobs]
        self.Plots += [ self.Frame( name, name + (' ('+units[name]+')' if name in units.keys() else ''), xlog=xlog, ylog=ylog) ]
        self.Plot(x)
        self.ResizeAxes(yrange=yrange)
        return
    
    # plot parameter for specific dof
    def PlotAlignableDof(self, name, dof, corr=None, par=None, plotErr=True, yrange=None, addToPDF=False, xlog=False, ylog=False):
        """
            Plot the parameter for a specific degree of freedom
                name: alignable name (IT/Station2/TopBox)
                dof: dof name (Tx,Ty,Tz,Rx,Ry,Rz)
                corr: specific correction name
                par: parameter name (Cur, CurTot, Delta, DeltaErr, GCC, Chi2)
                plotErr: plot error bars
                xaxis: a dictionary with informations on what to show on the xaxis, i.e. {'title': 'iterations', 'min': 0, 'max': len(y)}
        """
        if corr == None and par== None: return
        y = self.GetAlignableDofCorrection(name, dof, corr) if par == None else {'vals': [self.GetAttr(par, self.GetAlignableDof(alJob, name, dof)) for alJob in self.AlignJobs], 'err': [self.GetAttr('DeltaErr', self.GetAlignableDof(alJob, name, dof)) for alJob in self.AlignJobs] if plotErr and par in ['Cur', 'CurTot', 'Delta'] else [] }
 #       # plot error?
 #       parErr = 'DeltaErr' if plotErr and par in ['Cur', 'CurTot', 'Delta'] else None
 #       # store values for specific key of alignable
 #       y = [self.GetAttr(par, self.GetAlignableDof(alJob, name, dof)) for alJob in self.AlignJobs]
 #       yErr = [self.GetAttr(parErr, self.GetAlignableDof(alJob, name, dof)) for alJob in self.AlignJobs]
        # add figure
        if par != None and corr==None:
            self.Plots += [ self.Frame( name+'.'+dof, par + (' ('+units[par]+')' if units[par] != None else ''), xlog=xlog, ylog=ylog) ]
        else:
            self.Plots += [ self.Frame( name+'.'+dof, corr + ' (mm)', xlog=ylog, ylog=ylog) ]
        self.Plot(y['vals'], yerr= y['err'] if len(y['err']) != 0 else None, zero=True)
        self.ResizeAxes(yrange=yrange)
        return
    
    # plot parameter for specific dofs from different (or same) alignables
    def PlotAlignablesDofs(self, names, corr=None, par=None, plotErr=True, xaxis=None, title=None, yrange=None, addToPDF=False, xlog=False, ylog=False):
        """
            Plot a parameter for two or more dofs from same or different alignables
                names: list of alignable names + dofs (['IT/Station2/TopBox.Tx', 'IT/Station2/TopBox.Tz'])
                par: parameter name (Cur, CurTot, Delta, DeltaErr, GCC, Chi2)
                plotErr: plot error bars
                xaxis: a dictionary with informations on what to show on the xaxis, i.e. {'title': 'iterations', 'min': 0, 'max': len(y)}
                title: assign a title
        """
        if corr == None and par== None: return
        parErr = 'DeltaErr' if plotErr and (par in ['Cur', 'CurTot', 'Delta'] or corr!=None) else None
        # store values for specific key of alignable
        yvals, yerrs = {}, {}
        for name in names:
            alName = name.split('.')[0]
            dof = name.split('.')[1]
            if par != None and corr==None:
                # plot error?
                yvals.update( {name: [self.GetAttr(par, self.GetAlignableDof(alJob, alName, dof)) for alJob in self.AlignJobs]} )
                if parErr!=None:
                    yerrs.update( {name: [self.GetAttr(parErr, self.GetAlignableDof(alJob, alName, dof)) for alJob in self.AlignJobs]} )
            else:
                ytmp= self.GetAlignableDofCorrection(alName, dof, corr)
                yvals.update( {name: ytmp['vals']} )
                yerrs.update( {name: ytmp['err']} )
        # add figure
        if par != None and corr==None:
            self.Plots += [ self.Frame( title if title!=None else '', par + (' ('+units[par]+')' if units[par] != None else ''), xlog=xlog, ylog=ylog) ]
        else:
            self.Plots += [ self.Frame( name+'.'+dof, corr + ' (mm)', xlog=xlog, ylog=ylog) ]
              #        print yvals, yerrs, parErr
        self.Plot(yvals, yerr= yerrs if parErr != None else None, zero=True, legend=True, title=title)
        self.ResizeAxes(yrange=yrange)
        return

    # plot alignment correction, initial or survey parameter
    def PlotAlignableDofCorrection(self, name, dof, type='correction to initial', plotErr=True, addToPDF=False):
        """
            Depending of the choice, plot the alignment correction, or the stored alignment dof
            Plot options:
            - correction to initial ('Delta')
            - correction to nominal ('Al')
            - correction to initial including mother (CurTot+Delta) pout-pin+pintot(CurTot)
            - correction to nominal including mother (Delta) pouttot(CurTot+Delta)-pintot
            - initial (Al-Delta) pin
            - correction to survey (Al-Sur) pout- sur
            - survey (Sur) sur
            - correction of initial to survey (Al-Delta-Sur) pin-sur
            - correction of nominal including mother to survey (CurTot+Delta-Sur) pouttot-sur
            Options:
                name: alignable name (IT/Station2/TopBox)
                dof: dof name (Tx,Ty,Tz,Rx,Ry,Rz)
                par: parameter name (Cur, CurTot, Delta, DeltaErr, GCC, Chi2)
                plotErr: plot error bars
                xaxis: a dictionary with informations on what to show on the xaxis, i.e. {'title': 'iterations', 'min': 0, 'max': len(y)}
        """
        # plot error?
        parErr = 'DeltaErr' if plotErr else None
        # store values for specific key of alignable
        y, yErr = [], []
        if type == 'correction to initial':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y += [self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) ]
            yErr += [self.GetAttr(parErr, self.GetAlignableDof(alJob, name, dof)) ]
        if type == 'correction to nominal':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y += [self.GetAttr(dof,alJob.Alignables[name].AlignmentPars) ]
            yErr += [0]
        if type == 'correction to initial including mother':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y += [(self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof))+
                 self.GetAttr('CurTot', self.GetAlignableDof(alJob, name, dof)) if self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof))!=None else None) ]
            yErr += [self.GetAttr(parErr, self.GetAlignableDof(alJob, name, dof))]
        if type == 'correction to nominal including mother':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y += [self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) ]
            yErr += [self.GetAttr(parErr, self.GetAlignableDof(alJob, name, dof)) ]
        if type == 'initial':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y += [self.GetAttr(dof,alJob.Alignables[name].AlignmentPars)-
                 self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) if self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) != None else 0]
            yErr += [0]
        if type == 'correction to survey':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y += [self.GetAttr(dof,alJob.Alignables[name].AlignmentPars)-self.GetAttr(dof,alJob.Alignables[name].SurveyPars)]
            yErr += [self.GetAttr(dof+'Err',alJob.Alignables[name].SurveyPars)]
        if type == 'survey':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y += [self.GetAttr(dof,alJob.Alignables[name].SurveyPars)]
            yErr += [self.GetAttr(dof+'Err',alJob.Alignables[name].SurveyPars)]
        if type == 'correction of initial to survey':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y += [self.GetAttr(dof,alJob.Alignables[name].AlignmentPars)-
                 self.GetAttr(dof,alJob.Alignables[name].SurveyPars)+
                 self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) if self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) != None else 0]
            yErr += [ np.sqrt(np.power(self.GetAttr(dof+'Err',alJob.Alignables[name].SurveyPars),2)+
                             np.power(self.GetAttr('DeltaErr', self.GetAlignableDof(alJob, name, dof)) if self.GetAttr('DeltaErr', self.GetAlignableDof(alJob, name, dof)) != None else 0,2) )]
        if type == 'correction of nominal including mother to survey':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y += [-self.GetAttr(dof,alJob.Alignables[name].SurveyPars)+
                 (self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof))+self.GetAttr('CurTot', self.GetAlignableDof(alJob, name, dof))) if self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) != None else 0]
            yErr += [ np.sqrt(np.power(self.GetAttr(dof+'Err',alJob.Alignables[name].SurveyPars),2)+
                             np.power(self.GetAttr('DeltaErr', self.GetAlignableDof(alJob, name, dof)) if self.GetAttr('DeltaErr', self.GetAlignableDof(alJob, name, dof)) != None else 0,2) )]
        # add figure
        self.Plots += [ plt.figure() ]
        plt.title(type)
        plt.ylabel(type+' (mm)')
        plt.xlabel(self.xAxis['title'])
        # set time axis
        if self.xAxis['title'] == 'time':
            # matplotlib date format object
            hfmt = mdates.DateFormatter('%Y-%m-%d %H:%M')
            ax=plt.gca()
            ax.xaxis.set_major_formatter(hfmt)
            ax.xaxis_date()
            # rotate and align the tick labels so they look better
            self.Plots[-1].autofmt_xdate()
        # set time axis
        if self.xAxis['title'] == 'variable':
            # matplotlib date format object
            ax=plt.gca()
#            ax.xaxis.set_major_formatter(hfmt)
            ax.set_xticklabels(self.xAxis['ticknames'])
            # rotate and align the tick labels so they look better
#            self.Plots[-1].autofmt_xdate()
        # set tick names
        if self.xAxis['title'] == 'variable':
            ax=plt.gca()
            ax.set_xticks(xrange(0,len(self.xAxis['ticknames'])))
            ax.set_xticklabels(self.xAxis['ticknames'])
        if plotErr: plt.errorbar(self.xAxis['labels'], y, yerr=yErr, marker='o', label=name+'.'+dof)
        else: plt.plot(self.xAxis['labels'], y,marker='o', label=name+'.'+dof)
        # enlarge by 7% on each side the plot limits
        plt.xlim(plt.xlim()[0]-0.07*(plt.xlim()[1]-plt.xlim()[0]),plt.xlim()[1]+0.07*(plt.xlim()[1]-plt.xlim()[0]))
        # draw dashed line at y=0
        plt.axhline(ls='--',c='black')
        # add a legend
        plt.legend(loc='best')
        return
    
    # UTILITIES
    # Get attribute from class
    def GetAttr(self, attr, head=None):
        """
            Helper function: Get attribute from class
        """
        # Returns an array with the values from each alignment job
        if head == None: head = self.AlignJobs
        x = head.__getattribute__(attr)
        return x
    # Get values of different items
    def GetValues(self, name, head=None):
        """
            Helper function: Get values of different items in head
        """
        # Returns an array with the values from each alignment job
        if head == None: head = self.AlignJobs
        x = [item.__getattribute__(name) for item in head]
        return np.array(x)
    # Decode regular expression
    def DecodeExpression(self, expr):
        """
            Helper function: decode a regular expression using regex
        """
        # remove the mathematical operations and flatten list
        flatList = [x+'!' for x in re.split(r'[+-/*() ]+',expr) if x != '']
        # remove duplicates
        flatList = list(set(flatList))
        # sort list by string length
        flatList.sort(key=len, reverse=True)
        expression = expr.replace(' ','')
        expression = expression + '!'
        for op in ['+','-','*','/','(',')']: expression = expression.replace(op, '!'+op)
        for op in ['+','-','*','/','(',')']:
            for op1 in ['+','-','*','/','(',')']: expression = expression.replace(op+'!'+op1, op+op1)
        # associate array to value
        vals = {}
        for x in flatList:
            vals.update( {x : self.GetValues(x.split('!')[0])} )
            expression = expression.replace(x, 'vals["'+x+'"]')
        return eval(expression)
    # Get alignable specific dof
    def GetAlignableDof(self, alJob, name, dof):
        """
            Look for a specific degree of freedom in the alignable
        """
        # First look for the values in the alJob
        for alDof in alJob.Alignables[name].AlignmentDOFs:
            if alDof.Name == dof: return alDof
        return None # if not found
    # Set X axis
    def SetXaxis(self, kind, labels=None):
        if kind == 'iterations':
            self.xAxis = {'title': 'iterations', 'min': 0, 'max': len(self.AlignJobs), 'labels': xrange(0,len(self.AlignJobs))}
        if kind == 'time':
            # get list of times from alignjobs
            times = [alJob.Times['initialize'].Machine/1000/1000/1000 for alJob in self.AlignJobs]
            dts = map(datetime.datetime.fromtimestamp, times)
            fds = mdates.date2num(dts) # converted
            self.xAxis = {'title': 'time', 'min': 0, 'max': len(self.AlignJobs), 'labels':dts}
        if kind == 'variable':
            self.xAxis = {'title': 'variable', 'min': 0, 'max': len(self.AlignJobs), 'labels': xrange(0,len(self.AlignJobs)) ,'ticknames':labels}
        return

    # Get list of values for specific alignment correction
    def GetAlignableDofCorrection(self, name, dof, type='correction to initial', plotErr=True):
        # plot error?
        parErr = 'DeltaErr' if plotErr else None
        y ={'vals': [], 'err':[]}
        if type == 'correction to initial':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y['vals'] += [self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof))]
            y['err'] += [self.GetAttr(parErr, self.GetAlignableDof(alJob, name, dof))]
        if type == 'correction to nominal':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y['vals'] += [self.GetAttr(dof,alJob.Alignables[name].AlignmentPars)]
            y['err'] += [0]
        if type == 'correction to initial including mother':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y['vals'] += [(self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof))+
                 self.GetAttr('CurTot', self.GetAlignableDof(alJob, name, dof)) if self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof))!=None else None) ]
            y['err'] += [self.GetAttr(parErr, self.GetAlignableDof(alJob, name, dof))]
        if type == 'correction to nominal including mother':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y['vals'] += [self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof))]
            y['err'] += [self.GetAttr(parErr, self.GetAlignableDof(alJob, name, dof))]
        if type == 'initial':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y['vals'] += [self.GetAttr(dof,alJob.Alignables[name].AlignmentPars)-
                 self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) if self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) != None else 0]
            y['err'] += [0]
        if type == 'correction to survey':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y['vals'] += [self.GetAttr(dof,alJob.Alignables[name].AlignmentPars)-self.GetAttr(dof,alJob.Alignables[name].SurveyPars) ]
            y['err'] += [self.GetAttr(dof+'Err',alJob.Alignables[name].SurveyPars) ]
        if type == 'survey':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y['vals'] += [self.GetAttr(dof,alJob.Alignables[name].SurveyPars) ]
            y['err'] += [self.GetAttr(dof+'Err',alJob.Alignables[name].SurveyPars) ]
        if type == 'correction of initial to survey':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y['vals'] += [self.GetAttr(dof,alJob.Alignables[name].AlignmentPars)-
                 self.GetAttr(dof,alJob.Alignables[name].SurveyPars)+
                 self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) if self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) != None else 0 ]
            y['err'] += [ np.sqrt(np.power(self.GetAttr(dof+'Err',alJob.Alignables[name].SurveyPars),2)+
                             np.power(self.GetAttr('DeltaErr', self.GetAlignableDof(alJob, name, dof)) if self.GetAttr('DeltaErr', self.GetAlignableDof(alJob, name, dof)) != None else 0,2) )]
        if type == 'correction of nominal including mother to survey':
          for alJob in self.AlignJobs:
            if alJob.Alignables[name].EnoughHits == False:
              print 'Alignable',name,'has insufficient statistic'
              continue
            y['vals'] += [-self.GetAttr(dof,alJob.Alignables[name].SurveyPars)+
                 (self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof))+self.GetAttr('CurTot', self.GetAlignableDof(alJob, name, dof))) if self.GetAttr('Delta', self.GetAlignableDof(alJob, name, dof)) != None else 0 ]
            y['err'] += [ np.sqrt(np.power(self.GetAttr(dof+'Err',alJob.Alignables[name].SurveyPars),2)+
                             np.power(self.GetAttr('DeltaErr', self.GetAlignableDof(alJob, name, dof)) if self.GetAttr('DeltaErr', self.GetAlignableDof(alJob, name, dof)) != None else 0,2) ) ]
        return y
    
    # Set PDF info
    def SetPdfInfo(self, pdf, title='AlignLogOutput', author='Aligner',
                   subject='Output of AlignLog', keywords='alignlog alignment'):
        """
            Plot all the created figures
        """
        d = pdf.infodict()
        d['Title'] = title
        d['Author'] = author
        d['Subject'] = subject
        d['Keywords'] = keywords
        d['CreationDate'] = datetime.datetime.today()
        d['ModDate'] = datetime.datetime.today()
        return


# END ALPLOTTER
