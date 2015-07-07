# BEGIN POSITION
class Position:
    """
        Position of the detector element
    """
    def __init__(self, x,y,z):
        self.X = x
        self.Y = y
        self.Z = z
    def Print(self):
        return '(%.2f,%.2f,%.2f)' % (self.X,self.Y,self.Z)
    # Get Attributes
    def __getattribute__(self, name):
        return self.__dict__[name] if name in self.__dict__.keys() else None
# END POSITION

# BEGIN ALIGN_PARAMETERS
class AlignParameters:
    """
        Alignment Parameters
    """
    def __init__(self, Tx,Ty,Tz,Rx,Ry,Rz):
        self.Tx = Tx
        self.Ty = Ty
        self.Tz = Tz
        self.Rx = Rx
        self.Ry = Ry
        self.Rz = Rz
    def Print(self):
        return 'T( %.2f, %.2f, %.2f) R( %.2f, %.2f, %.2f)' % (self.Tx,self.Ty,self.Tz,self.Rx,self.Ry,self.Rz)
    # Get Attributes
    def __getattribute__(self, name):
        return self.__dict__[name] if name in self.__dict__.keys() else None
# END ALIGN_PARAMETERS

# BEGIN SURVEY_PARAMETERS
class SurveyParameters(AlignParameters):
    """
        Survey Parameters
    """
    def __init__(self,
                 Tx,Ty,Tz,Rx,Ry,Rz,
                 TxErr=0,TyErr=0,TzErr=0,RxErr=0,RyErr=0,RzErr=0):
        AlignParameters.__init__(self,Tx,Ty,Tz,Rx,Ry,Rz)
        self.TxErr = TxErr
        self.TyErr = TyErr
        self.TzErr = TzErr
        self.RxErr = RxErr
        self.RyErr = RyErr
        self.RzErr = RzErr
    def Print(self):
        out ='T( %.2f +/- %.2f, %.2f +/- %.2f, %.2f +/- %.2f)' % (self.Tx,self.TxErr, self.Ty, self.TyErr, self.Tz, self.TzErr)
        out = out +'  R( %.2f +/- %.2f, %.2f +/- %.2f, %.2f +/- %.2f)' % (self.Rx,self.RxErr,self.Ry,self.RyErr,self.Rz,self.RzErr)
        return out
    # Get Attributes
    def __getattribute__(self, name):
        return self.__dict__[name] if name in self.__dict__.keys() else None

# END SURVEY_PARAMETERS

# BEGIN LHCB_TIME
class LHCbTime:
    """
        LHCb Time
    """
    def __init__(self, machine,date,time):
        self.Machine = machine # ns
        self.Date = date
        self.Time = time
        self.Hours = time.split(':')[0]
        self.Minutes = time.split(':')[1]
        self.Seconds = time.split(':')[2]
        self.Period = time.split(' ')[1]
# END LHCB_TIME

# BEGIN ALIGNABLE
class Alignable:
    """
        Status of the single detector element
    """
    def __init__(self, name,
                 GlbPos=None, AvePos=None,
                 nTracks=None, nHits=None, nOutliers=None,
                 local_to_global_diag = None, local_delta_chi2= 0,
                 surveyChi2Before=None, surveyChi2After=None,
                 AlignDofs=None, enough_hits = True,
                 AlignPars=None, SurveyPars=None,
                 absolute_hit_error_contribution=None, relative_hit_error_contribution=None):
        self.Name = name
        self.GlobalPosition = GlbPos
        self.AveragePosition = AvePos
        self.NumTracks = nTracks
        self.NumHits = nHits
        self.NumOutliers = nOutliers
        self.EnoughHits = enough_hits
        self.LocatToGlobalDiagonal = local_to_global_diag if local_to_global_diag != None else []
        self.LocalDeltaChi2 = local_delta_chi2
        self.SurveyChi2Before = surveyChi2Before
        self.SurveyChi2After = surveyChi2After
        self.AlignmentDOFs = AlignDofs if AlignDofs != None else []
        self.AlignmentPars = AlignPars
        self.SurveyPars = SurveyPars
        self.HitErrorContribution = {'absolute':absolute_hit_error_contribution, 'relative': relative_hit_error_contribution}
    def Print(self):
        print 'Alignable:', self.Name
        print 'Global position:', self.GlobalPosition.Print()
        print 'Number of tracks/hits/outliers seen:', self.NumTracks, self.NumHits, self.NumOutliers
        print 'Local-to-global diagonal:', self.LocalToGlobalDiagonal
        print 'survey chi2 / dof (before/after): (%.2f / %.2f) / %d' % (self.SurveyChi2Before, self.SurveyChi2After, len(self.AlignmentDOFs))
        print 'align pars:', self.AlignmentPars.Print()
        print 'survey pars:', self.SurveyPars.Print()
        return
    # Get Attributes
    def __getattribute__(self, name):
        return self.__dict__[name] if name in self.__dict__.keys() else None
# END ALIGNABLE

# BEGIN DEGREE_OF_FREEDOM
class Dof:
    """
        Information regarding the single degree of freedom
    """
    def __init__(self, alignable, name, chi2, delta, deltaErr):
        """ Initialise class """
        dofnames = {'0': 'Tx', '1': 'Ty', '2': 'Tz', '3': 'Rx', '4': 'Ry', '5': 'Rz'}
        self.Alignable = alignable
        self.Name = dofnames[name] if name in ['%d' % x for x in xrange(0,6) ] else name
        self.Chi2 = chi2
        self.Delta = delta
        self.DeltaErr = deltaErr
    def Print(self):
        print '%s %s chi2= %.2f delta= %.2f +/- %.2f' % (self.Alignable, self.Name, self.Chi2, self.Delta, self.DeltaErr)
        return
    # Get Attributes
    def __getattribute__(self, name):
        return self.__dict__[name] if name in self.__dict__.keys() else None
# END DEGREE_OF_FREEDOM

# BEGIN DEGREE_OF_FREEDOM
class AlignDof(Dof):
    """
        Information regarding the single degree of freedom
        """
    def __init__(self, alignable, name, chi2, delta, deltaErr,
                 curtot, cur, gcc):
        """ Initialise class """
        Dof.__init__(self, alignable, name, chi2, delta, deltaErr)
        self.CurTot = curtot
        self.Cur = cur
        self.GCC = gcc
    def Print(self):
        print '%s - %s: chi2=%.2f delta=%.2f+-%.2f' % (self.Alignable, self.Name, self.Chi2, self.Delta, self.DeltaErr)
    # Get Attributes
    def __getattribute__(self, name):
        return self.__dict__[name] if name in self.__dict__.keys() else None
# END DEGREE_OF_FREEDOM

# BEGIN CONSTRAINT
class Constraint:
    """
        Information about the constraint
    """
    def __init__(self, name, value, error, active):
        self.Name = name
        self.Value = value
        self.Error = error
        self.Active = active
    def Print(self):
        print '%s %.6f +/- %.6f %s' % (self.Name, self.Value, self.Error, '(active)' if self.Active else '')
        return
    # Get Attributes
    def __getattribute__(self, name):
        return self.__dict__[name] if name in self.__dict__.keys() else None
# END CONSTRAINT

# BEGIN SURVEY CONSTRAINT
class SurveyConstraint(Dof):
    """
        Information about the survey constraint
    """
    def __init__(self, alignable, name, active, chi2,
                 survey, surveyErr, delta, deltaErr):
        Dof.__init__(self, alignable, name, chi2, delta, deltaErr)
        self.Active = active
        self.Survey = survey
        self.SurveyErr = surveyErr
    def Print(self):
        print '%s %s chi2= %.2f survey= %.2f +/- %.2f delta= %.2f +/- %.2f %s' % (self.Alignable, self.Name, self.Chi2,
                                                                                self.Survey, self. SurveyErr, self.Delta, self.DeltaErr,
                                                                                '(active)' if self.Active else '')
        return
    # Get Attributes
    def __getattribute__(self, name):
        return self.__dict__[name] if name in self.__dict__.keys() else None
# END SURVEY CONSTRAINT

# BEGIN ALIGNMENT ITERATION
class AlignIter:
    """
        Information about the survey constraint
    """
    def __init__(self, index=None, nevs=None,
                 time_first_event=None, time_last_event=None, time_initialise=None,
                 first_runnr=None, last_runnr=None,
                 nVertices=None, nParticles=None, nTracks=None, nHits=None, nHitsExternal=None,
                 chi2=None, nDofs=None, ave_trchi2=None, trchi2_ndof=None, vchi2_ndof=None,
                 survey_const_chi2=None, survey_const_ndof=None,
                 most_important_dofs=None,
                 nAlignablesInsufficient=None, nLagrangeConst=None, nChi2Const=None, nActivePars=None,
                 deltaChi2=None, deltaChi2Dofs=None, normalised_change_chi2=None, deltaChi2_trackDofs=None, local_delta_chi2=None,
                 canonical_constraint_chi2=None, canonical_constraint_equations=None,
                 survey_constraints_largest=None,
                 alignables=None):
        self.Index = index
        self.Nevs = nevs
        self.Times = {'first': time_first_event, 'last': time_last_event, 'initialize': time_initialise}
        self.Runs = {'first': first_runnr, 'last': last_runnr}
        self.Nums = {'vertices': nVertices, 'particles': nParticles, 'tracks': nTracks, 'hits': nHits,
                     'hitsExternal': nHitsExternal, 'alignables with insufficient statistics': nAlignablesInsufficient,
                     'lagrange constraints': nLagrangeConst, 'chisq constraints': nChi2Const, 'active parameters':nActivePars}
        self.Chi2 = chi2
        self.NDOFs = nDofs
        self.AveTrChi2 = ave_trchi2
        self.TrChi2nDof = trchi2_ndof
        self.VChi2nDof = vchi2_ndof
        self.SurveyConstrChi2 = survey_const_chi2
        self.SurveyConstrNDofs = survey_const_ndof
        self.MostImpDofs = most_important_dofs if most_important_dofs!= None else []
        self.DeltaChi2 = deltaChi2
        self.LocalDeltaChi2 = local_delta_chi2
        self.DeltaChi2nDofs = deltaChi2Dofs
        self.NormalisedChi2Change = normalised_change_chi2
        self.DeltaChi2overNDofs = deltaChi2_trackDofs
        self.CanonicalConstraints = {'chi2': canonical_constraint_chi2,
                                    'equations': canonical_constraint_equations if canonical_constraint_equations != None else []}
        self.SurveyConstrLargest = survey_constraints_largest if survey_constraints_largest != None else []
        self.Alignables = alignables if alignables!= None else {}
    # Get Attributes
    def __getattribute__(self, name):
        return self.__dict__[name] if name in self.__dict__.keys() else None
    # Info
    def AlignablesWithInsufficientStatistic(self, details=True, moredetails=False):
        notAligned = []
        for name, alignable in self.Alignables.iteritems():
            if not alignable.EnoughHits:
                notAligned += [alignable.Name]
                if moredetails: alignable.Print()
                if details: print alignable.Name
        return notAligned
    def PrintAlignable(self, name):
        if name in self.Alignables.keys():
            self.Alignables[name].Print()
        return
# END ALIGNMENT ITERATION

class AlignOutput:
    """
        A class for storing the output of an alignment job
    """
    def __init__(self, fileName):
        self.FileName = fileName
        self.logstatus = None
        self.alignableName = None
        self.AlignIterations = []
        self.index = None
    # Getters
    def Iteration(self, idx):
        return self.AlignIterations[idx] if idx < len(self.AlignIterations) else None
    # Parser
    def Parse(self):
        '''
        A funcion to parse information to the objects
        '''
        f = open(self.FileName,'r')
        # parse each line of the file to extract the relevant informations
        for line in f.readlines():
            if 'ALIGNMENT LOG' in line: continue
            if self.SetLogStatus(line) == True: continue
            if self.logstatus == 'general':
                self.ParseGeneral(line)
                continue
            if self.logstatus == 'dofs':
                self.ParseMostImportantDofs(line)
                continue
            if self.logstatus == 'canonical_constraints':
                self.ParseCanonicalConstraints(line)
                continue
            if self.logstatus == 'survey_constraints':
                #self.ParseSurveyConstraints(line)
                continue
            if self.logstatus == 'alignables':
                self.ParseAlignable(line)
                continue
        return
    # Set Log Status
    def SetLogStatus(self, line):
        prev_log = self.logstatus
        if 'Iteration:' in line:
            self.index =int(float(line.split(' ')[1].split('\n')[0]))
            self.AlignIterations+=[AlignIter(self.index)]
            self.logstatus = 'general'
            return (self.logstatus != prev_log)
        if 'Most important dofs, including survey:' in line:
            self.logstatus = 'dofs'
            return (self.logstatus != prev_log)
        if 'Number of alignables with insufficient statistics:' in line:
            self.logstatus = 'general'
            self.ParseGeneral(line)
            return (self.logstatus != prev_log)
        if 'Canonical constraint values:' in line:
            self.logstatus = 'canonical_constraints'
            return (self.logstatus != prev_log)
        if 'total local delta chi2 / dof:' in line:
            self.logstatus = 'general'
            self.ParseGeneral(line)
            return (self.logstatus != prev_log)
        if 'Survey constraints with largest chisquare contribution:' in line:
            self.logstatus = 'survey_constraints'
            return (self.logstatus != prev_log)
        if 'Alignable: Tracker' in line or 'Alignable: Velo' in line or 'Alignable: Muon' in line:
            self.logstatus = 'alignables'
            self.ParseAlignable(line)
            return (self.logstatus != prev_log)
    # General Info Parser
    def ParseGeneral(self, line):
        '''
        A funcion to parse general information to the AlignIter object
        '''
        words = line.split('\n')[0].split(' ')
        if 'Total number of events:' in line:
            self.AlignIterations[self.index].Nevs = int(float(words[4]))
            return
        for step in ['first','last']:
            if 'Time of '+step+' event [ns]' in line:
                self.AlignIterations[self.index].Times[step]=self.DecodeTime(words)
                return
        if 'Time at initialize [ns]' in line:
            self.AlignIterations[self.index].Times['initialize']=self.DecodeTime(words)
            return
        if 'First/last run number:' in line:
            self.AlignIterations[self.index].Runs = [words[-1].split('/')[0],words[-1].split('/')[1]]
            return
        for obj in ['vertices','particles','tracks','hits']:
            if obj+' for alignment' in line and 'Used' in line:
                self.AlignIterations[self.index].Nums[obj] = int(float(words[1]))
                return
        if 'Total chisquare/dofs:' in line:
            self.AlignIterations[self.index].Chi2 = float(words[5])
            self.AlignIterations[self.index].NDOFs = float(words[7])
            return
        if 'Average track chisquare:' in line:
            self.AlignIterations[self.index].AveTrChi2 = float(words[-1])
            return
        if 'Track chisquare/dof:' in line:
            self.AlignIterations[self.index].TrChi2nDof = float(words[-1])
            return
        if 'Vertex chisquare/dof:' in line:
            self.AlignIterations[self.index].VChi2nDof = float(words[-1])
            return
        if 'Total number of hits in external detectors:' in line:
            self.AlignIterations[self.index].Nums['hitsExternal'] = int(float(words[-1]))
            return
        if 'Total chisquare of survey constraints:' in line:
            self.AlignIterations[self.index].SurveyConstrChi2 = float(words[-3])
            self.AlignIterations[self.index].SurveyConstrNDofs = float(words[-1])
            return
        for obj in ['alignables with insufficient statistics', 'lagrange constraints', 'chisq constraints', 'active parameters']:
            if 'Number of '+obj+':' in line:
                self.AlignIterations[self.index].Nums[obj] = int(float(words[-1]))
                return
        if 'Alignment delta chisquare/dof:' in line:
            self.AlignIterations[self.index].DeltaChi2 = float(words[-3])
            self.AlignIterations[self.index].DeltaChi2nDofs = float(words[-1])
            return
        if 'Normalised alignment change chisquare:' in line:
            self.AlignIterations[self.index].NormalisedChi2Change = float(words[-1])
            return
        if 'Alignment delta chisquare/track dof:' in line:
            self.AlignIterations[self.index].DeltaChi2overNDofs = float(words[-1])
            return
        if 'total local delta chi2 / dof:' in line:
            self.AlignIterations[self.index].LocalDeltaChi2 = float(words[-3])
            return

        return
    # Parse Most Important DOFs
    def ParseMostImportantDofs(self, line):
        '''
        Parse the most important dofs
        '''
        words = line.split('\n')[0].split(' ')
        dof = Dof(words[3], words[4].split('=')[-1], float(words[6]), float(words[8]), float(words[10]))
        self.AlignIterations[self.index].MostImpDofs += [dof]
        return
    # Parse Canonical Constraints
    def ParseCanonicalConstraints(self, line):
        '''
        Parse canonical constraints
        '''
        words = [x for x in line.split('\n')[0].split(' ') if x!='']
        if 'Canonical constraint chisquare:' in line:
            self.AlignIterations[self.index].CanonicalConstraints['chi2'] = float(words[-1])
            return
        elif 'Values of constraint equations after solution (X=active,O=inactive):' in line: return
        else:
            self.AlignIterations[self.index].CanonicalConstraints['equations']+=[Constraint(words[0], float(words[2]), float(words[4]), words[1]=='O')]
            return
        return
    # Parse Survey Constraints
    def ParseSurveyConstraints(self, line):
        '''
        Parse survey constraints
        '''
        words = [x for x in line.split('\n')[0].split(' ') if x!='']
        self.AlignIterations[self.index].SurveyConstrLargest+=[SurveyConstraint(words[1], words[2].split('=')[-1],
                                                         words[3].split('=')[-1]=='1', float(words[4].split('=')[-1]),
                                                         float(words[5].split('=')[-1]), float(words[6].split('=')[-1]),
                                                         float(words[7].split('=')[-1]), float(words[8].split('=')[-1]))]
        return
    # Parse alignable
    def ParseAlignable(self, line):
        words = [x for x in line.split('\n')[0].split(' ') if x!='']
        if 'Alignable:' in line:
            self.alignableName = words[1]
            self.AlignIterations[self.index].Alignables.update({ self.alignableName: Alignable(self.alignableName)})
            return
        else:
            if 'Global position:' in line:
                self.AlignIterations[self.index].Alignables[self.alignableName].GlobalPosition = self.DecodePosition(words[2])
                return
            if 'Number of tracks/hits/outliers seen:' in line:
                self.AlignIterations[self.index].Alignables[self.alignableName].NumTracks = int(float(words[4]))
                self.AlignIterations[self.index].Alignables[self.alignableName].NumHits = int(float(words[5]))
                self.AlignIterations[self.index].Alignables[self.alignableName].NumOutliers = int(float(words[6]))
                return
            if 'Local-to-global diagonal:' in line:
                self.AlignIterations[self.index].Alignables[self.alignableName].LocalToGlobalDiagonal = [int(float(words[2+i])) for i in xrange(0,6)]
                return
            if 'Not enough hits for alignment. Skipping update.' in line:
                self.AlignIterations[self.index].Alignables[self.alignableName].EnoughHits = False
                return
            if 'survey chi2 / dof (before/after):' in line:
                self.AlignIterations[self.index].Alignables[self.alignableName].SurveyChi2Before = float(words[5])
                self.AlignIterations[self.index].Alignables[self.alignableName].SurveyChi2After = float(words[6])
                return
            if 'align pars:' in line:
                words = [x.split(',')[0] for x in words if x!='']
                self.AlignIterations[self.index].Alignables[self.alignableName].AlignmentPars = AlignParameters(float(words[2]),float(words[3]),float(words[4]),
                                                                   float(words[5]),float(words[6]),float(words[7]))
                return
            if 'survey pars:' in line:
                words = [x.split(',')[0] for x in words if x!='']
                self.AlignIterations[self.index].Alignables[self.alignableName].SurveyPars = SurveyParameters(float(words[2]),float(words[3]),float(words[4]),
                                                                    float(words[5]),float(words[6]),float(words[7]))
                return
            if 'survey errors:' in line:
                words = [x.split(',')[0] for x in words if x!='']
                self.AlignIterations[self.index].Alignables[self.alignableName].SurveyPars.TxErr = float(words[2])
                self.AlignIterations[self.index].Alignables[self.alignableName].SurveyPars.TyErr = float(words[3])
                self.AlignIterations[self.index].Alignables[self.alignableName].SurveyPars.TzErr = float(words[4])
                self.AlignIterations[self.index].Alignables[self.alignableName].SurveyPars.RxErr = float(words[5])
                self.AlignIterations[self.index].Alignables[self.alignableName].SurveyPars.RyErr = float(words[6])
                self.AlignIterations[self.index].Alignables[self.alignableName].SurveyPars.RzErr = float(words[7])
                return
            if words[0] in ['Tx','Ty','Tz','Rx','Ry','Rz']:
                self.AlignIterations[self.index].Alignables[self.alignableName].AlignmentDOFs += [self.DecodeAlignDof(words)]
                return
            if 'contribution to hit error (absolute/relative):' in line:
                self.AlignIterations[self.index].Alignables[self.alignableName].HitErrorContribution['absolute'] = float(words[-2])
                self.AlignIterations[self.index].Alignables[self.alignableName].HitErrorContribution['relative'] = float(words[-1])
                return
            if 'local delta chi2 / dof:' in line:
                self.AlignIterations[self.index].Alignables[self.alignableName].LocalDeltaChi2 = float(words[-3])
                return
        return
    # Decode Time
    def DecodeTime(self,words):
        '''
        Decode Time from logfile
        '''
        words = [x for x in words if x != ':']
        return LHCbTime( int(float(words[-5])), words[-3], words[-2]+' '+words[-1])
    # Decode Position
    def DecodePosition(self,word):
        '''
        Decode Position from logfile
        '''
        pos = word.split('(')[1].split(')')[0].split(',')
        return Position( float(pos[0]), float(pos[1]), float(pos[2]) )
    # Decode Alignment DOF
    def DecodeAlignDof(self,words):
        '''
        Decode alignment dof from logfile
        '''
        return AlignDof(self.AlignIterations[self.index].Alignables[self.alignableName].Name,
                        words[0], 0, float(words[7]), float(words[9]), float(words[3]), float(words[5]), float(words[9]))




