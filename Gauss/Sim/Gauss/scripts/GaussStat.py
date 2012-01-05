#!/usr/bin/env python


#==============================================================================================
#==============================================================================================
global script_version
script_version = '20120105'


def float__(s):
    """emulate the behaviour of python float, but makes it valid oalso on
    FORTRAN scientific notation with Double.  1.2D+03 """ 
    from __builtin__ import float
    try:
        return float(s)
    except ValueError:
        return float(s.replace('D', 'E'))
#replacing the native float function
float = float__

# ========================================================================
# Operations for the generation process
# ========================================================================

def operations(result):
    # It takes a dictionary with the counters and makes the operations
    global warnings
    
    calculations = ''
    # Computation of Pile-Up interations
    
    if result.has_key('N2'):
        if result.has_key('L1'):
            calculations += '\nMean Number of generated Pile-Up interactions: %.4f +- %.6f' % ( result['N2']/result['L1'], (result['N2']/result['L1'])*math.sqrt( (1/result['N2']) + (1/result['L1']) ) )
    if result.has_key('N1'):
        calculations += '\nMean Number of non-empty generated Pile-Up interactions: %.4f +- %.6f' % ( result['N2']/result['N1'], (result['N2']/result['N1'])*math.sqrt( (1/result['N2']) + (1/result['N1']) )  )
                                                                                                                             
    if result.has_key('N3') and result.has_key('N4'):
        calculations += '\nMean Number of accepted Pile-Up interactions: %.4f +- %.6f\n' % ( result['N4']/result['N3'], (result['N4']/result['N3'])*math.sqrt( (1/result['N4']) + (1/result['N3']) ) )
                                                                                                                             
    #  Computation of cut efficiencies
                                                                                                                             
    if result.has_key('I2') and result.has_key('I1'):
        calculations += '\nGenerator level cut efficiency: %.4f +- %.6f\n' % ( result['I2']/result['I1'], math.sqrt(result['I2']*(result['I1']-result['I2'])/result['I1']**3) )
                                                                                                                             
    if result.has_key('N20') and result.has_key('N19'):
        calculations += '\nFull level cut efficiency: %.4f +- %.6f\n' % ( result['N20']/result['N19'], math.sqrt(result['N20']*(result['N19']-result['N20'])/result['N19']**3) )
                                                                                                                             
    #  Computation of cross-sections
                                                                                                                             
    if result.has_key('C1') and result.has_key('N2'):
        C1 = result['C1N2']/result['N2']
        if result.has_key('N5'):
            calculations += '\nb cross-section: %.5f +- %.6f' % ( C1*result['N5']/result['N2'], C1*math.sqrt(result['N5']*(result['N2']-result['N5'])/result['N2']**3) )
        if result.has_key('N6'):
            calculations += '\nDouble b cross-section: %.5f +- %.6f' % ( C1*result['N6']/result['N2'], C1*math.sqrt(result['N6']*(result['N2']-result['N6'])/result['N2']**3) )
        if result.has_key('N7'):
            calculations += '\nPrompt B cross-section: %.5f +- %.6f' % ( C1*result['N7']/result['N2'], C1*math.sqrt(result['N7']*(result['N2']-result['N7'])/result['N2']**3) )
        if result.has_key('N8'):
            calculations += '\nc cross-section: %.4f +- %.5f' % ( C1*result['N8']/result['N2'], C1*math.sqrt(result['N8']*(result['N2']-result['N8'])/result['N2']**3) )
        if result.has_key('N9'):
            calculations += '\nDouble c cross-section: %.5f +- %.6f' % ( C1*result['N9']/result['N2'], C1*math.sqrt(result['N9']*(result['N2']-result['N9'])/result['N2']**3) )
        if result.has_key('N10'):
            calculations += '\nPrompt D cross-section: %.5f +- %.6f' % ( C1*result['N10']/result['N2'], C1*math.sqrt(result['N10']*(result['N2']-result['N10'])/result['N2']**3) )
        if result.has_key('N11'):
            calculations += '\nb and c cross-section: %.5f +- %.6f\n' % ( C1*result['N11']/result['N2'], C1*math.sqrt(result['N11']*(result['N2']-result['N11'])/result['N2']**3) )
                                                                                                                             
        #  Computation of B Hadron fraction
        
        if result.has_key('I3') and result.has_key('I5') and result.has_key('I7') and result.has_key('I9'):
            sum = result['I3']+result['I5']+result['I7']+result['I9']
            calculations += '\nFraction of generated B0: %.4f +- %.6f' % ( result['I3']/sum, math.sqrt(result['I3']*(sum - result['I3'])/sum**3) )
            calculations += '\nFraction of generated B+: %.4f +- %.6f' % ( result['I5']/sum, math.sqrt(result['I5']*(sum - result['I5'])/sum**3) )
            calculations += '\nFraction of generated Bs0: %.4f +- %.6f' % ( result['I7']/sum, math.sqrt(result['I7']*(sum - result['I7'])/sum**3) )
            calculations += '\nFraction of generated b-Baryon: %.4f +- %.6f\n' % ( result['I9']/sum, math.sqrt(result['I7']*(sum - result['I9'])/sum**3) )

        if result.has_key('I14') and result.has_key('I16') and result.has_key('I18') and result.has_key('I20'):
            sum = result['I14']+result['I16']+result['I18']+result['I20']
            if sum != 0:
                calculations += '\nFraction of accepted B0: %.4f +- %.6f' % ( result['I14']/sum, math.sqrt(result['I14']*(sum - result['I14'])/sum**3) )
                calculations += '\nFraction of accepted B+: %.4f +- %.6f' % ( result['I16']/sum, math.sqrt(result['I16']*(sum - result['I16'])/sum**3) )
                calculations += '\nFraction of accepted Bs0: %.4f +- %.6f' % ( result['I18']/sum, math.sqrt(result['I18']*(sum - result['I18'])/sum**3) )
                calculations += '\nFraction of accepted b-Baryon: %.4f +- %.6f\n' % ( result['I20']/sum, math.sqrt(result['I20']*(sum - result['I20'])/sum**3) )
            else:
                calculations += '\nFraction of accepted B0: 0 +- 0'
                calculations += '\nFraction of accepted B+: 0 +- 0'
                calculations += '\nFraction of accepted Bs0: 0 +- 0'
                calculations += '\nFraction of accepted b-Baryon: 0 +- 0\n'
                
        #  Computation of D Hadron fraction
                                                                                                                             
        if result.has_key('I25') and result.has_key('I27') and result.has_key('I29') and result.has_key('I31'):
            sum = result['I25']+result['I27']+result['I29']+result['I31']
            calculations += '\nFraction of generated D0: %.4f +- %.6f' % ( result['I25']/sum, math.sqrt(result['I25']*(sum - result['I25'])/sum**3) )
            calculations += '\nFraction of generated D+: %.4f +- %.6f' % ( result['I27']/sum, math.sqrt(result['I27']*(sum - result['I27'])/sum**3) )
            calculations += '\nFraction of generated Ds+: %.4f +- %.6f' % ( result['I29']/sum, math.sqrt(result['I29']*(sum - result['I29'])/sum**3) )
            calculations += '\nFraction of generated c-Baryon: %.4f +- %.6f\n' % ( result['I31']/sum, math.sqrt(result['I31']*(sum - result['I31'])/sum**3) )
                                                                                                                             
        if result.has_key('I34') and result.has_key('I36') and result.has_key('I38') and result.has_key('I40'):
            sum = result['I34']+result['I36']+result['I38']+result['I40']
            if sum != 0:
                calculations += '\nFraction of accepted D0: %.4f +- %.6f' % ( result['I34']/sum, math.sqrt(result['I34']*(sum - result['I34'])/sum**3) )
                calculations += '\nFraction of accepted D+: %.4f +- %.6f' % ( result['I36']/sum, math.sqrt(result['I36']*(sum - result['I36'])/sum**3) )
                calculations += '\nFraction of accepted Ds+: %.4f +- %.6f' % ( result['I38']/sum, math.sqrt(result['I38']*(sum - result['I38'])/sum**3) )
                calculations += '\nFraction of accepted c-Baryon: %.4f +- %.6f\n' % ( result['I40']/sum, math.sqrt(result['I40']*(sum -
                                                                                                                                  result['I40'])/sum**3) )
            else:
                calculations += '\nFraction of accepted D0: 0 +- 0'
                calculations += '\nFraction of accepted D+: 0 +- 0'
                calculations += '\nFraction of accepted Ds+: 0 +- 0'
                calculations += '\nFraction of accepted c-Baryon: 0 +- 0\n'
                
            #  Computation of B** fraction

            if result.has_key('I43') and result.has_key('I44') and result.has_key('I45'):
                sum = result['I43']+result['I44']+result['I45']
                calculations += '\nFraction of generated B: %.4f +- %.6f' % ( result['I43']/sum, math.sqrt(result['I43']*(sum - result['I43'])/sum**3) )
                calculations += '\nFraction of generated B*: %.4f +- %.6f' % ( result['I44']/sum, math.sqrt(result['I44']*(sum - result['I44'])/sum**3) )
                calculations += '\nFraction of generated B**: %.4f +- %.6f' % ( result['I45']/sum, math.sqrt(result['I45']*(sum - result['I45'])/sum**3) )
                
                if result.has_key('I46') and result.has_key('I47') and result.has_key('I48'):
                    sum = result['I46']+result['I47']+result['I48']
                    if sum != 0:
                        calculations += '\nFraction of accepted B: %.4f +- %.6f' % ( result['I46']/sum, math.sqrt(result['I46']*(sum - result['I46'])/sum**3) )
                        calculations += '\nFraction of accepted B*: %.4f +- %.6f' % ( result['I47']/sum, math.sqrt(result['I47']*(sum - result['I47'])/sum**3) )
                        calculations += '\nFraction of accepted B**: %.4f +- %.6f' % ( result['I48']/sum, math.sqrt(result['I48']*(sum - result['I48'])/sum**3) )
                    else:
                        calculations += '\nFraction of accepted B: 0 +- 0'
                        calculations += '\nFraction of accepted B*: 0 +- 0'
                        calculations += '\nFraction of accepted B**: 0 +- 0'
                                                                                                                             
            #  Computation of D** fraction
            
            if result.has_key('I49') and result.has_key('I50') and result.has_key('I51'):
                sum = result['I49']+result['I50']+result['I51']
                if sum != 0:
                    calculations += '\nFraction of generated D: %.4f +- %.6f' % ( result['I49']/sum, math.sqrt(result['I49']*(sum - result['I49'])/sum**3) )
                    calculations += '\nFraction of generated D*: %.4f +- %.6f' % ( result['I50']/sum, math.sqrt(result['I50']*(sum - result['I50'])/sum**3) )
                    calculations += '\nFraction of generated D**: %.4f +- %.6f' % ( result['I51']/sum, math.sqrt(result['I51']*(sum - result['I51'])/sum**3) )
                else:
                    calculations += '\nFraction of generated D: 0 +- 0'
                    calculations += '\nFraction of generated D*: 0 +- 0'
                    calculations += '\nFraction of generated D**: 0 +- 0'

            if result.has_key('I52') and result.has_key('I53') and result.has_key('I54'):
                sum = result['I52']+result['I53']+result['I54']
                if sum != 0:
                    calculations += '\nFraction of accepted D: %.4f +- %.6f' % ( result['I52']/sum, math.sqrt(result['I52']*(sum - result['I52'])/sum**3) )
                    calculations += '\nFraction of accepted D*: %.4f +- %.6f' % ( result['I53']/sum, math.sqrt(result['I53']*(sum - result['I53'])/sum**3) )
                    calculations += '\nFraction of accepted D**: %.4f +- %.6f' % ( result['I54']/sum, math.sqrt(result['I54']*(sum - result['I54'])/sum**3) )
                else:
                    calculations += '\nFraction of accepted D: 0 +- 0'
                    calculations += '\nFraction of accepted D*: 0 +- 0'
                    calculations += '\nFraction of accepted D**: 0 +- 0'

            #  Computation of signal level cut efficiencies

            if result.has_key('S1') and result.has_key('S2'):
                calculations += '\nparticle cut efficiency: %.4f +- %.6f\n' % ( result['S2']/result['S1'], math.sqrt(result['S2']*(result['S1']-result['S2'])/result['S1']**3) )

            if result.has_key('S3') and result.has_key('S4'):
                calculations += '\nanti-particle cut efficiency: %.4f +- %.6f\n' % ( result['S4']/result['S3'], math.sqrt(result['S4']*(result['S3']-result['S4'])/result['S3']**3) )
                
                
    return calculations
# =======================================================================
# search for general parameters 
# =======================================================================

def search_general_info(info):
# It takes a string with the file info and returns a dictionary with the counters
  global gaussversion,DDDB,SIMCOND,luminosity,APPCONFIG,APPCONFIG_vers
  
  # search Gauss version in log file if user did not parse it
  if gaussversion == '':
      test = info.find('Welcome to Gauss version')
      if test != -1:
          gaussversion = info[test:test+100].split(' ')[4].strip().split(' ')[0]  
  else:
      test = info.find('Welcome to Gauss version')
      if test != -1:
          gaussvertmp = info[test:test+100].split(' ')[4].strip().split(' ')[0]
          if str(gaussvertmp) != str(gaussversion):
              print 'INFO: inside the same PRODID, the gauss version you are currently analyzing: %s is different from the previously analyzed log files gauss version %s '%(gaussversion,gaussvertmp)
              
  # search DDDB version
  DDDB = 'dummy'
  test = info.find('DDDB                       INFO Using TAG')
  if test != -1:
     DDDB = info[test:test+100].split('"')[1]

  # search SIMCOND version
  SIMCOND = 'dummy'
  test = info.find('SIMCOND                    INFO Using TAG')
  if test != -1:
     SIMCOND = info[test:test+100].split('"')[1]

  # search APPCONFIG-options
  APPCONFIG = 'dummy'
  test1 = info.find('AppConfig/')
  test2 = info.find('Gauss/')
  if test1 != -1 and test2 != -1:
     APPCONFIG = info[test1:test1+100].split('/')[4].strip().split("'")[0].strip().split(".")[0]
  # search APPCONFIG-version
     APPCONFIG_vers = info[test1:test1+100].split('/')[1]
     
  # search luminosity info
  luminosity = 'dummy'
  test = info.find('-Luminosity')
  if test != -1:
     luminosity = info[test:test+100].split('=')[1].strip().split('(')[0]

  return

# ========================================================================
# Search lines
# ========================================================================

def search_lines(info):
# It takes a string with the file info and returns a dictionary with the counters
  global ParamFiles
  
  result = {}

  # search DecFiles version and ParamsVersion
  DecFiles = 'Error'
  test = info.find('DecFiles')
  if test != -1:
    check = info[test:test+20].find('/')
    if check != -1:
        DecFiles = info[test:test+20].split('/')[1]
    else:
        DecFiles = info[test:test+20].split(' ')[1]
#        print 'Problem with DecFiles ', info[test:test+100]
##      return result
  result['DecFiles'] = DecFiles

  ParamFiles = 'Error'
  test = info.find('ParamFiles')
  if test != -1:
    check = info[test:test+20].find('/')
    if check != -1:
        ParamFiles = info[test:test+20].split('/')[1]
    else:
      print 'Problem with ParamFiles ', info[test:test+100]
##      return result
  result['ParamFiles'] = ParamFiles

## add the visible branching fractions of the signal decay mode info:
  test = info.find('The signal decay mode has visible branching fractions of')
  if test != -1:
    L0 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['L0'] = L0
  else:
    result['L0'] = 'NA'
  test = info.find('Number of all events (including empty events)')
  if test != -1:
    L1 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['L1'] = L1
  test = info.find('Number of events with 0 interaction')
  if test != -1:
    L2 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['L2'] = L2
  test = info.find('All included subprocesses')
  if test != -1:
    C1 = float(string.strip(info[test:test+80].split('I')[2]))
    result['C1'] = C1
  test = info.find('Number of events for generator level cut')
  if test != -1:
  # before
    I1 = float(string.strip(info[test:test+80].split(':')[1].split(',')[0]))
  # after
    I2 = float(string.strip(info[test:test+80].split(':')[2].split()[0]))
    result['I1'] = I1
    result['I2'] = I2
  test = info.find('Number of events for full event cut')
  if test != -1:
  # before
    N19 = float(string.strip(info[test:test+80].split(':')[1].split(',')[0]))
  # after
    N20 = float(string.strip(info[test:test+80].split(':')[2].split()[0]))
    result['N19'] = N19
    result['N20'] = N20
  # only for inclusive productions
  # B Hadron fractions
  test = info.find('Number of generated B0')
  if test != -1:
    I3 = float(string.strip(info[test:test+80].split(':')[1].split('[')[0]))
    result['I3'] = I3
  test = info.find('Number of accepted B0')
  if test != -1:
    I14 = float(string.strip(info[test:test+80].split(':')[1].split('[')[0]))
    result['I14'] = I14
  test = info.find('Number of generated B+')
  if test != -1:
    I5 = float(string.strip(info[test:test+80].split(':')[1].split('[')[0]))
    result['I5'] = I5
  test = info.find('Number of accepted B+')
  if test != -1:
    I16 = float(string.strip(info[test:test+80].split(':')[1].split('[')[0]))
    result['I16'] = I16
  test = info.find('Number of generated Bs0')
  if test != -1:
    I7 = float(string.strip(info[test:test+80].split(':')[1].split('[')[0]))
    result['I7'] = I7
  test = info.find('Number of accepted Bs0')
  if test != -1:
    I18 = float(string.strip(info[test:test+80].split(':')[1].split('[')[0]))
    result['I18'] = I18
  test = info.find('Number of generated b-Baryon')
  if test != -1:
    I9 = float(string.strip(info[test:test+80].split(':')[1].split('[')[0]))
    result['I9'] = I9
  test = info.find('Number of accepted b-Baryon')
  if test != -1:
    I20 = float(string.strip(info[test:test+80].split(':')[1].split('[')[0]))
    result['I20'] = I20
  test = info.find('Number of generated events')
  if test != -1:
    N1 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['N1'] = N1
  test = info.find('Number of generated interactions')
  if test != -1:
    N2 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['N2'] = N2
  test = info.find('Number of accepted events')
  if test != -1:
    N3 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['N3'] = N3
  test = info.find('Number of interactions in accepted events')
  if test != -1:
    N4 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['N4'] = N4
  test = info.find('Number of generated interactions with >= 1b')
  if test != -1:
    N5 = float(string.strip(info[test:test+50].split(':')[1].split()[0]))
    result['N5'] = N5
  test = info.find('Number of generated interactions with >= 3b')
  if test != -1:
    N6 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['N6'] = N6
  test = info.find('Number of generated interactions with 1 prompt B')
  if test != -1:
    N7 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['N7'] = N7
  test = info.find('Number of generated interactions with >= 1c')
  if test != -1:
    N8 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['N8'] = N8
  test = info.find('Number of generated interactions with >= 3c')
  if test != -1:
    N9 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['N9'] = N9
  test = info.find('Number of generated interactions with >= prompt C')
  if test != -1:
    N10 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['N10'] = N10
  test = info.find('Number of generated interactions with b and c')
  if test != -1:
    N11 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['N11'] = N11
  test = info.find('Number of generated D0')
  if test != -1:
    I25 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I25'] = I25
  test = info.find('Number of accepted D0')
  if test != -1:
    I34 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I34'] = I34
  test = info.find('Number of generated D+')
  if test != -1:
    I27 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I27'] = I27
  test = info.find('Number of accepted D+')
  if test != -1:
    I36 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I36'] = I36
  test = info.find('Number of generated Ds+')
  if test != -1:
    I29 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I29'] = I29
  test = info.find('Number of accepted Ds+')
  if test != -1:
    I38 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I38'] = I38
  test = info.find('Number of generated c-Baryon')
  if test != -1:
    I31 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I31'] = I31
  test = info.find('Number of accepted c-Baryon')
  if test != -1:
    I40 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I40'] = I40
  test = info.find('Number of generated B')
  if test != -1:
    I43 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I43'] = I43
  test = info.find('Number of accepted B')
  if test != -1:
    I46 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I46'] = I46
  test = info.find('Number of generated B*')
  if test != -1:
    I44 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I44'] = I44
  test = info.find('Number of accepted B*')
  if test != -1:
    I47 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I47'] = I47
  test = info.find('Number of generated B**')
  if test != -1:
    I45 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I45'] = I45
  test = info.find('Number of accepted B**')
  if test != -1:
    I48 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I48'] = I48
  test = info.find('Number of generated D')
  if test != -1:
    I49 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I49'] = I49
  test = info.find('Number of accepted D')
  if test != -1:
    I52 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I52'] = I52
  test = info.find('Number of generated D*')
  if test != -1:
    I50 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I50'] = I50
  test = info.find('Number of accepted D*')
  if test != -1:
    I53 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I53'] = I53
  test = info.find('Number of generated D**')
  if test != -1:
    I51 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I51'] = I51
  test = info.find('Number of accepted D**')
  if test != -1:
    I54 = float(string.strip(info[test:test+80].split(':')[1].split()[0]))
    result['I54'] = I54
  test = info.find('Number of events for generator particle level cut')
  if test != -1:
  # before
    S1 = float(string.strip(info[test:test+80].split(':')[1].split(',')[0]))
  # after
    S2 = float(string.strip(info[test:test+90].split(':')[2].split()[0]))
    if gaussversion == 'v25r8': S2 = S2/2
    result['S1'] = S1
    result['S2'] = S2
  test = info.find('Number of events for generator anti-particle level cut')
  if test != -1:
  # before
    S3 = float(string.strip(info[test:test+80].split(':')[1].split(',')[0]))
  # after
    S4 = float(string.strip(info[test:test+90].split(':')[2].split()[0]))
    if gaussversion == 'v25r8': S4 = S4/2
    result['S3'] = S3
    result['S4'] = S4
  if result.has_key('N2') and result.has_key('C1'):
    result['C1N2'] = result['C1']*result['N2']

  return result

# ========================================================================
# Store results for the generation process
# ========================================================================

def gen_store_results(jobgen):
  # It builds a html file with the results in friendly tables   


  result = generationResults

# still to check!!!
  wrkflw = gaussversion
  generation = 'Generation'

  evntp = eventtype
  
# perform statistics calculations on the results
  calcs = operations(result)
  
  rc = 'OK'
  if calcs == '':
      rc = 'KO'
      return rc
      
  Warning = '' # Prompt B cross-section control
  count = 0
 
  decfiles = result['DecFiles']
  paramfiles = result['ParamFiles']

  desc_eventtype = readErrorDesc(evntp)
  
  # Construction of the results web page
  # For each production -> 3 tables -> Counters Interaction, Hadron Counters, Signal Counters

  webheader = '<html>\n'
  webheader = '<HEAD>\n'
  webheader += '<link rel=\"stylesheet\" href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/css/lhcb.css\" type=\"text/css\" media=\"screen\">\n'
  webheader += '<link rel=\"stylesheet\" href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/gauss/css/css.css\" type=\"text/css\" media=\"screen\">\n'
  webheader += '<link rel=\"stylesheet\" href=\"css.css\" type=\"text/css\" media=\"screen\">\n'
  webheader += '<title>Table for Generation</title>\n'
  webheader += '</HEAD>\n'
  webheader += '\n<body>\n'

  link = '<a name="%s"> </a>\n' % ( str(evntp) )
  table = '<table>\n<th colspan="2"><font color="#0000FF">%s (%s)</font> - Gauss <font color="#088A08">%s</font> <br> DecFiles <font color="#088A08">%s</font><br>DDDB <font color="#FF0000">%s </font>- SIMCOND <font color="#FF0000">%s</font> <br>APPCONFIG <font color="#088A08">%s</font> <font color="#FF0000">%s</font><br> %s</th>\n' % ( evntp,desc_eventtype, gaussversion, decfiles, DDDB,SIMCOND,APPCONFIG_vers,APPCONFIG,time.ctime())
  table += '<tr>\n'
 
  tr11, tr21, tr31, tr41, tr12, tr22, tr32, tr42, tr13, tr23, tr33, tr43 = '<tr>\n','<tr>\n','<tr>\n','<tr>\n','<tr>\n','<tr>\n','<tr>\n','<tr>\n','<tr>\n', '<tr>\n', '<tr>\n', '<tr>\n'
 
  tr41 += '<td>&nbsp;</td>\n'
  tr42 += '<td>&nbsp;</td>\n'
  tr43 += '<td>&nbsp;</td>\n'
  count1, count2, count3, count4 = 0,0,0,0
  for line in calcs.split('\n'):
  # Table2: Hadron Counters
    if line.find('Fraction') != -1 or line.find('Generator level cut efficiency') != -1:
      if line.find('generated') != -1:
      # generated counters
        count4 += 1
        words = line.split(':')
        tr41 += '<td><font size=2>%s</font></td>\n' % words[0].strip()
        tr42 += '<td>%s</td>\n' % words[1].split('+-')[0].strip()
        if len(words[1].split('+-'))==2:
          tr43 += '<td>&plusmn; %s</td>\n' % words[1].split('+-')[1].strip()
        else:
          tr43 += '<td>&nbsp;</td>\n'
      else:
      # accepted counters
        count2 += 1
        words = line.split(':')
        tr21 += '<td><font size=2>%s</font></td>\n' % words[0].strip()
        tr22 += '<td>%s</td>\n' % words[1].split('+-')[0].strip()
        if len(words[1].split('+-'))==2:
          tr23 += '<td>&plusmn; %s</td>\n' % words[1].split('+-')[1].strip()
        else:
          tr23 += '<td>&nbsp;</td>\n'
  # Table3: Signal Counters
    elif line.find('particle') != -1:
      count3 += 1
      words = line.split(':')
      tr31 += '<td><font size=2>%s</font></td>\n' % words[0].strip()
      tr32 += '<td>%s</td>\n' % words[1].split('+-')[0].strip()
      if len(words[1].split('+-'))==2:
        tr33 += '<td>&plusmn; %s</td>\n' % words[1].split('+-')[1].strip()
      else:
        tr33 += '<td>&nbsp;</td>\n'
  # Table1: Counters Interaction
    else:
      if line != '':
        count1 += 1
        words = line.split(':')
        if line.find('Prompt B cross-section') != -1:
          pbcs = words[1].strip().split(' +- ')
#          if ( float(pbcs[0]) != float(0) ) == 1:
            # Only if Prompt B cross-section value != 0
# removed this warning, no more necessary for MC09
#            Warning = '<p><font color="#FF0000">Warning: Prompt B cross-section = %.4f &plusmn; %.6f</font></p>' % ( float(words[1].split('+-')[0]), float(words[1].split('+-')[1])  )
        else:
          count += 1
          if line[:4] != 'Mean':
            tr11 += '<td><font size=2>%s ( mb )</font></td>\n' % words[0].strip()
          else:
            tr11 += '<td><font size=2>%s</font></td>\n' % words[0].strip()

          tr12 += '<td>%s</td>\n' % words[1].split('+-')[0].strip()

          if len(words[1].split('+-'))==2:
            tr13 += '<td>&plusmn; %s</td>\n' % words[1].split('+-')[1].strip()
          else:
            tr13 += '<td>&nbsp;</td>\n'

  if count1 != 0:
#    table1 = '<tr>\n<td align=center>\n<table border=2>\n<th colspan=%i><div class=firstcell>Counters Interaction</div></th>\n' % count1
    table1 = '<tr>\n<td align=center>\n<table border=2>\n<th colspan=%i><div class=firstcell>Counters Interaction</div></th>\n' % count1
    table1 += tr11 + tr12 + tr13 + '</tr>\n</table>\n</td>\n</tr>'
  else:
    Warning += '<p><font color="#FF0000">Warning: No Counters Interactions lines found</font></p>\n'
    table1 = ''
  if count2 != 0:
    table2 = '<tr>\n<td align=center>\n<table border=2>\n<th colspan=%i><div class=firstcell>Hadron Counters</div></th>\n' % count2
    if count4 != 0:
      table2 += tr21 + tr22 + tr23 + tr41 + tr42 + tr43 + '</tr>\n</table>\n</td>\n</tr>'
    else:
      table2 += tr21 + tr22 + tr23 + '</tr>\n</table>\n</td>\n</tr>'
  else:
    Warning += '<p><font color="#FF0000">Warning: No Hadron Counters lines found</font></p>\n'
    table2 = ''
  if count3 != 0:
    table3 = '<tr>\n<td align=center>\n<table border=2>\n<th colspan=%i><div class=firstcell>Signal Counters</div></th>\n' % count3
    table3 += tr31 + tr32 + tr33 + '</tr>\n</table>\n</td>\n</tr>'
  else:
    Warning += '<p><font color="#FF0000">Warning: No Signal Counters lines found</font></p>\n'
    table3 = ''

  table += table1 + table2 + table3 + '</table>\n'


  web_comments = Warning + '<p>Number of accepted events/generated events: %i / %i = %f<br>Number of interactions in accepted events/generated interactions: %i / %i = %f' % ( result['N3'], result['N1'], result['N3']/result['N1'], result['N4'], result['N2'], result['N4']/result['N2'] )

#  web_comments += '<p>Statistics done (script version %s) with %i jobs from the following ProdIDs: </p>\n<p>' %(script_version,jobgen)
  if result.has_key('L0'):
      web_comments += '<p><font color="#0000FF">Branching fraction of the signal decay mode:</font> %s' % ( str(result['L0']) )

  web_comments += '<p>Statistics done (script version %s) with %i jobs from the following ProdIDs:<font color="#0000FF">' %(script_version,jobgen)

# --- list all the PRODIDs used (without the duplicates):
  prodNoDupes=[]
  for i in prodlist: 
      if not prodNoDupes.count(i):
          prodNoDupes.append(i) 

  for u in prodNoDupes:
     web_comments += str(u) + ' '
  web_comments += '</font></p>\n'
  
#  file = os.path.join(LocationPath, generation+"_%s_%s.html" % ( wrkflw,evntp))
  file = os.path.join(LocationPath, generation+"_%s.html" % ( APPCONFIG))
  if os.path.exists(file):
    print '%s exists '%file  
    f = open(file,'r')
    lines = f.read()
    f.close()
    # Looking for ProdID
    # if we find it, it means we used already this production for another generation
    # it has been tested before anyway
    if lines.find(evntp) == -1:
      # if there is not still a simulation for this production
      pos_list = lines.find('</p>\n<a name=')
      pos_table = lines.find('</body>')
      webn = lines[:pos_list] + ' <a href="%s_%s.html#%s">%s</a>' % ( generation, APPCONFIG, str(evntp), str(evntp) ) + lines[pos_list:pos_table] + link + table + web_comments + lines[pos_table:]
    else:
    # There is already a simulation for this production
    # but if we are here it's because we have to overwrite the previous results
      #print 'This Production was already used for some other statistics'
      pos_table = lines.find('<a name="%s"> </a>\n' % ( str(evntp) ))
      pos_next = lines.find('<a name=', pos_table+1)
      if pos_next == -1:
        pos_next = lines.find('</body>')
      webn = lines[:pos_table] + link + table + web_comments + lines[pos_next:]

  else:
  # the file does not exist
  # the file does not exist, create it
    print '%s does not exist, creating it now'%file
    webheader += '<p>Event Type : <a href="%s_%s.html#%s">%s</a> </p>\n' % ( generation, APPCONFIG, str(evntp), str(evntp) )
    webn = webheader + link + table + web_comments + '</body>\n'

  f = open(file,'w')
  f.write(webn)
  f.close()

  if install_flag ==1:
      install(file)
  if add_flag ==1:
      addToIndex(file)

  return rc

# ========================================================================
# Create and store simulation tables
# ========================================================================

def sim_store_results(jobsim, gevents, averageResults, veloResults, richResults, muonResults):
# It builds a html file with the results in friendly tables
# Construction of the results web page


  count = jobsim
  evntp = eventtype
  if evntp == '30000000': desc = 'minbias'  
  wrkflw = gaussversion
  simulation = 'Simulation'
  rc = 'OK'

  desc_eventtype = readErrorDesc(evntp)
  
  web = '<a name=%s> </a><p><center><b><font color="#0000FF">%s (%s)</font> - Gauss <font color="#088A08">%s</font> <br>DDDB <font color="#FF0000">%s </font>- SIMCOND <font color="#FF0000">%s</font> <br>APPCONFIG <font color="#088A08">%s</font> <font color="#FF0000">%s</font><br> %s</b>\n</center></p>\n' % ( evntp, evntp, desc_eventtype,gaussversion,  DDDB,SIMCOND,APPCONFIG_vers,APPCONFIG,time.ctime())

  # Summary table average
  if averageResults != {}:
      tr11, tr21, tr31 = '<tr>\n','<tr>\n','<tr>\n'
      #web += '<p>\n<table border=1>\n<th colspan=3>Multiplicity:</th>\n'
      web += '<p>\n<table border=1>\n'
      tr11 += '<td><div class=firstcell>Multiplicity:</div></td>\n'
      tr21 += '<td></td>\n'
      tr31 += '<td></td>\n'
      tr11 += '<td><div class=firstcell>MCParticles</div></td>\n'
      tr21 += '<td>%.4f</td>\n' % (float(averageResults['MCParticles']/count))
      tr31 += '<td>&plusmn; %.4f</td>\n' % (math.sqrt(float(averageResults['MCPError'])/count))
      tr11 += '<td><div class=firstcell>MCHits</div></td>\n'
      tr21 += '<td></td>\n'
      tr31 += '<td></td>\n'
      for type in ('TT', 'IT', 'OT', 'Spd', 'Prs', 'Ecal', 'Hcal'):
          tr11 += '<td><center><div class=firstcell>%s</div></center></td>\n' % (type)
          tr21 += '<td>%.4f</td>\n' % (float(averageResults[type]/count))
          tr31 += '<td>&plusmn; %.4f</td>\n' % (math.sqrt(float(averageResults[type+'Error'])/count))
      tr11 += '</tr>\n'
      tr21 += '</tr>\n'
      tr31 += '</tr>\n'
      web += tr11 + tr21 + tr31 + '</table>\n</p>\n'
  else:
      web += '<p><font color="#FF0000">Warning: Impossible to find Multiplicity counters</font></p>\n'
  web += '<p>\n<table celspacing=55555 cellpadding=2 >\n<tr valign=top>\n<td>\n'

  
  # Summary table for RICHes
  # Invalid RICH flags
  if richResults != {}:
      web += '<p>\n<table border=1>\n'
      web += '<tr>\n<td><div class=firstcell> Av. # Invalid RICH flags</div></td>\n<td>%.4f &plusmn; %.4f</td>\n</tr>\n' % ( float(richResults['InvalidRICHflags'])/count, math.sqrt(float(richResults['InvalidRICHflagsError'])/count) )
      web += '</table>\n</p>\n'
                                                                                                                             
      # AV      RICH1   RICH2
      web += '<p>\n<table border=1>\n'
      web += '<tr>\n<td><div class=firstcell>Av. #</div></td>\n<td><div class=firstcell>Rich1</div></td>\n<td><div class=firstcell>Rich2</div></td>\n<tr>\n'
      for av in ('MCRichHits', 'Invalid radiator hits', 'Gas Quartz CK hits', 'HPD Quartz CK hits', 'Nitrogen CK hits'):
          web += '<tr>\n<td>%s</td>\n<td>%.4f &plusmn; %.4f</td>\n<td>%.4f &plusmn; %.4f</td>\n</tr>\n' % (av, float(richResults[av+'Rich1'])/count, math.sqrt(float(richResults[av+'Rich1Error'])/count), float(richResults[av+'Rich2'])/count, math.sqrt(float(richResults[av+'Rich2Error'])/count))
      web += '</table>\n</p>\n'
                                                                                                                             
      # AV      aero    C4F10   CF4
      web += '<p>\n<table border=1>\n'
      web += '<tr>\n<td><div class=firstcell>Av. #</div></td>\n<td><div class=firstcell>Aero</div></td>\n<td><div class=firstcell>C4F10</div></td>\n<td><div class=firstcell>CF4<div></td>\n<tr>\n'
#      for av in ('Signal CK MCRichHits', 'Charged Track hits', 'Scattered hits', 'MCParticle-less hits'):
      
      for av in ('Signal CK MCRichHits', 'scattered hits', 'MCParticle-less hits'):
          web += '<tr>\n<td>%s</td>\n<td>%.4f &plusmn; %.4f</td>\n<td>%.4f &plusmn; %.4f</td>\n<td>%.4f &plusmn; %.4f</td>\n</tr>\n' % (av, float(richResults[av])/count, math.sqrt(float(richResults[av+'Error'])/count), float(richResults[av+'C4F10'])/count, math.sqrt(float(richResults[av+'C4F10Error'])/count), float(richResults[av+'CF4'])/count, math.sqrt(float(richResults[av+'CF4Error'])/count))
      web += '</table>\n</p>\n'
                                                                                                                             
  else:
      web += '<p><font color="#FF0000">Warning: Impossible to find RICH counters</font></p>\n'

  web += '</td>\n<td>\n'

  web += '<p>\n'


  # Summary table for the Velo
  if veloResults != {}:
      web += '<table border=1>\n<th colspan=3><div class=firstcell>Velo Monitoring Table:</div></th>\n'
      web += '<tr>\n<td>Number of MCHits/Event</td>\n<td>%.4f</td>\n<td>&plusmn; %.4f</td>\n</tr>\n' % ( float(veloResults['MCHits'])/count, math.sqrt(veloResults['MCHitsError']/count))
      web += '<tr>\n<td>Number of PileUpMCHits</td>\n<td>%.4f</td>\n<td>&plusmn; %.4f</td>\n</tr>\n' % ( float(veloResults['PileUpMCHits'])/count, math.sqrt(float(veloResults['PileUpMCHitsError'])/count))
      web += '</table>\n'
  else:
      web += '<font color="#FF0000">Warning: Impossible to find Velo counters</font>\n'

  web += '</p>\n<p>\n'


  # Summary table for the MUON
  if muonResults != [[0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0]]:
      web += '<table border=1>\n<th colspan=6><div class=firstcell>Muon Monitoring Table:</div></th>\n'
      web += '<tr>\n<td>M1</td>\n<td>M2</td>\n<td>M3</td>\n<td>M4</td>\n<td>M5</td>\n<td>&nbsp;</td>\n</tr>\n'
      for index in range(4):
          web += '<tr>\n'
          for index2 in range(5):
              web += '<td>%.4f</td>\n' % ( ( float(muonResults[index2][index]) )/count )
          web += '<td>%s</td>\n' % muonResults[5][index]
          web += '</tr>\n'
      web += '<tr>\n<td colspan=6>Integrated over regions</td>\n</tr>\n'
      web += '<tr>\n'
      for index in range(5):
          web += '<td>%.3f</td>\n' % ( ( float(muonResults[index][4]) )/count )
      web += '<td>'+str(muonResults[5][4])+'</td>\n'
      web += '</tr>\n'
      web += '</table>\n'
  else:
      web += '<font color="#FF0000">Warning: Impossible to find MUON counters</font>'


  web += '</p>\n'
  web += '</td>\n</tr>\n</table>\n</p>\n'

  # Number Of Events written
  if gevents != {}:
      if gevents.has_key('N1'):
          web += '<p>Event written / requested: %i / %i = %s </p>\n' % ( int(gevents['N2']), int(gevents['N1']), str(gevents['N2']/gevents['N1']) )
  else:
      web += '<p><font color="#FF0000">Warning: Impossible to find the Events counters</font></p>'
                                                                                                                             
  web += '<p>Statistics (script version %s) done with %i jobs belonging to the following ProdIds: <font color="#0000FF">' %(script_version, count)
# list of ProIds used for this statistics (no duplicates considered)
  prodNoDupes=[]
  for i in prodlist: 
      if not prodNoDupes.count(i):
          prodNoDupes.append(i) 

  for u in prodNoDupes:
     web += str(u) + ' '
  web += '</font></p>\n'

  
                                                                                                                             
  # Where stores it?
  
#  file = os.path.join(LocationPath, simulation  + "_%s_%s.html" % (wrkflw,evntp))
  file = os.path.join(LocationPath, simulation+"_%s.html" % (APPCONFIG))
 
  if os.path.exists(file):
    print '%s exists '%file  
    f = open(file,'r')
    lines = f.read()
    f.close()
    test = lines.find(str(evntp))
    if test == -1:
      print '%s event type does not exist - include it'%(str(evntp))
      # this event type does not exist
      pos_list = lines.find('</p>\n<a name=')
      pos_table = lines.find('</body>')
      webn = lines[:pos_list] + ' <a href="%s_%s.html#%s">%s</a>' % ( simulation, APPCONFIG, str(evntp), str(evntp) ) + lines[pos_list:pos_table] + web + lines[pos_table:]
    else:
    # This event type exist
      print '%s event type exists - do not update'%(str(evntp))
    # just nothing
      pos_table = lines.find('<a name=%s> </a>' % ( str(evntp) ))
      pos_next = lines.find('<a name=', pos_table+1)
      if pos_next == -1:
        pos_next = lines.find('</body>')
      webn = lines[:pos_table] + web + lines[pos_next:]

  else:
  # the file does not exist, create it
    print '%s does not exist, creating it now'%file

    webheader = '<html>\n'
    webheader += '<HEAD>\n'
    webheader += '<link rel=\"stylesheet\" href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/css/lhcb.css\" type=\"text/css\" media=\"screen\">\n'
    webheader += '<link rel=\"stylesheet\" href=\"http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/gauss/css/css.css\" type=\"text/css\" media=\"screen\">\n'
    webheader += '<link rel=\"stylesheet\" href=\"css.css\" type=\"text/css\" media=\"screen\">\n'
    webheader += '<title>Table for Simulation</title>\n'
    webheader += '</HEAD>\n'
    webheader += '\n<body>\n'
    webheader += '\n<p>Event Type : <a href="%s_%s.html#%s">%s</a> </p>\n' % ( simulation, APPCONFIG, str(evntp), str(evntp) )

    webn = webheader + web + '</body>\n'

  f = open(file,'w')
  f.write(webn)
  f.close()

  if install_flag ==1:
      install(file)
##   if add_flag ==1:
##       addToIndex(file)

  return rc


# ========================================================================
# Report the number of events written respect to those requested
# ========================================================================
def NumberOfEvents(info):

    results = {}

    # GaussTape                  INFO Events output: N
    test = info.find('Events output')
    if test != -1:
        N2 = info[test:test+30].split(':')[1].split()[0]
        results['N2'] = float(N2)

    # GaussSim                SUCCESS Requested to process 500 events
    test = info.find("GaussSim                SUCCESS Requested to process")
    test1 = info.find("GaussSim                SUCCESS Requested to process")
    test2 = info.find("GaussSim          SUCCESS Requested to process") 
#    if test != -1:
    if test1 != -1 or test2 != -1:
        if test1 != -1:
            test = test1
        else:
            test = test2
        N1 = info[test:test+80].split('process')[1].split('events')[0].strip()
        results['N1'] = float(N1)
    
    return results
  
# ============================================================================
# Report the type of particles that are not known to LHCb but known to Geant4
#=============================================================================
def ParticlesGeant4(info):

    # Message = 'PDGEncoding does not exist, G4 name is Li7[0.0]
    
    test = info.find('PDGEncoding does not exist')
    if test != -1:
        particle = info[test:test+80].split('G4 name is')[1].split()[0]
    else:
        particle = ''
    
    return particle

# ========================================================================
# Print summary table for average number of hits, particle, etc.
# ========================================================================
def AverageTable(info):

  results = {}

    # MCParticles
  test = info.find('#MCParticles')
  if test != -1:
      value = info[test:test+200].split('|')[3]
      rms = info[test:test+200].split('|')[4]
      results['MCParticles'] = float(value)
      results['MCPError'] = float(rms)

    # MCHits
  #for type in ('VeloPU', 'Velo', 'TT', 'IT', 'OT', 'Rich', 'Rich1', 'Rich2', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Muon'):
  for type in ('IT', 'TT', 'OT', 'Spd', 'Prs', 'Ecal', 'Hcal'):
    test = info.find('%s MCHits' % type)
    if test != -1:
      value = info[test:test+200].split('|')[3]
      rms = info[test:test+200].split('|')[4]
      results[type] = float(value)
      results[type+'Error'] = float(rms)

  return results

# ========================================================================
# Print summary table for the RICHes
# ========================================================================
def RICHesTable(info):

    results = {}

    string1 = 'GetRichHits                INFO Av. #'
    string2 = 'GetRichHits          INFO Av. #'
    test1 = info.find(string1)
    test2 = info.find(string2)
    substring1 = 'GetRichHits                INFO'
    substring2 = 'GetRichHits          INFO'

#    test = info.find("GetRichHits                INFO Av. #")
#    if test != -1:

    if test1 != -1 or test2 != -1:
        if test1 != -1:
            test = test1
            string = string1
            substring = substring1 
        else:
            test = test2
            string = string2
            substring = substring2
        data = info[test:test+4500].split(substring)
#        data = info[test:test+4500].split("GetRichHits                INFO")
        for line in data:
            if line.find("Invalid RICH flags") != -1:
                 results['InvalidRICHflags'] = float(line.split('=')[1].strip().split('+-')[0].strip())
#                 results['InvalidRICHflagsError'] = float(line.split('=')[1].strip().split('+-')[1].strip())**2
                 results['InvalidRICHflagsError'] = float(line.split('=')[1].strip().split('+-')[1].strip().split("\n")[0])**2
            for av in ('MCRichHits', 'Invalid radiator hits', 'Gas Quartz CK hits', 'HPD Quartz CK hits', 'Nitrogen CK hits'):
                if line.find(av) != -1 and line.find('Rich1') != -1:
                    results[av+'Rich1'] = float(line.split('=')[1].split('(')[0].strip().split(' +- ')[0])
                    results[av+'Rich1Error'] = float(line.split('=')[1].split('(')[0].strip().split(' +- ')[1])**2
                    results[av+'Rich2'] = float(line.split('=')[2].strip().split(' +- ')[0])
                    results[av+'Rich2Error'] = float(line.split('=')[2].strip().split(' +- ')[1].strip().split('(')[0])**2
            for aero in ('Signal CK MCRichHits', 'scattered hits', 'MCParticle-less hits'):
                if line.find(aero) != -1 and line.find('Aero') != -1:
                    results[aero] = float(line.split('=')[1].split('(')[0].strip().split(' +- ')[0])
                    results[aero+'Error'] = float(line.split('=')[1].split('(')[0].strip().split(' +- ')[1])**2
                    results[aero+'C4F10'] = float(line.split('=')[2].split('(')[0].strip().split(' +- ')[0])
                    results[aero+'C4F10Error'] = float(line.split('=')[2].split('(')[0].strip().split(' +- ')[1])**2
                    results[aero+'CF4'] = float(line.split('=')[3].strip().split(' +- ')[0])
                    results[aero+'CF4Error'] = float(line.split('=')[3].strip().split(' +- ')[1].strip().split('(')[0])**2
    return results

# ========================================================================
# Print summary table for the VeLo
# ========================================================================
def VeloTable(info):

    results = {}

    # Number of MCHits/Event: 1299.15+/-34.1846
    test = info.find('Number of MCHits/Event')
    if test != -1:
        data = info[test:test+80].split(':')[1].split()[0]
        MCHits = data.split('+/-')[0].strip()
        MCHitsError = data.split('+/-')[1].split()[0]
        results['MCHits'] = float(MCHits)
        results['MCHitsError'] = float(MCHitsError)**2

    test = info.find('Number of PileUpMCHits/Event')
    if test != -1:
        data = info[test:test+80].split(':')[1].split()[0]
        PileUpMCHits = data.split('+/-')[0].strip()
        PileUpMCHitsError = data.split('+/-')[1].split()[0]
        results['PileUpMCHits'] = float(PileUpMCHits)
        results['PileUpMCHitsError'] = float(PileUpMCHitsError)**2

    return results

# ========================================================================
# Print summary table for the MUON
# ========================================================================
def MUONTable(info):

    results = {}

    
    correct, overregions = 'no', 'no'
    M1, M2, M3, M4, M5, M6 = [],[],[],[],[],[]

    # MuonHitChecker             INFO        Muon Monitoring Table
#    test = info.find('MuonHitChecker             INFO -----------------------------------------------------------------')
    string1 = 'MuonHitChecker             INFO -----------------------------------------------------------------'
    string2 = 'MuonHitChecker       INFO -----------------------------------------------------------------'
    test1 = info.find(string1)
    test2 = info.find(string2)
    substring1 = 'MuonHitChecker             INFO'
    substring2 = 'MuonHitChecker       INFO'
#    if test != -1:
    if test1 != -1 or test2 != -1:
        if test1 != -1:
            test = test1
            string = string1
            substring = substring1 
        else:
            test = test2
            string = string2
            substring = substring2
        data = info[test:test+2500].split(substring)
#        data = info[test:test+2500].split('MuonHitChecker             INFO')
        for a in data:
            if overregions == 'yes':
                if a.strip()[0:4] != '----':
                    M1.append(a.split()[0])
                    M2.append(a.split()[1])
                    M3.append(a.split()[2])
                    M4.append(a.split()[3])
                    M5.append(a.split()[4])
                    M6.append(a.split()[5])
                    overregions = 'no'
            else:
                if correct == 'yes':
#                    if a.strip() == '-----------------------------------------------------------------':
                    if a.find('-----------------------------------------------------------------') != -1:
                        correct = 'no'
                        overregions = 'yes'
                    else:
                        M1.append(a.split()[0])
                        M2.append(a.split()[1])
                        M3.append(a.split()[2])
                        M4.append(a.split()[3])
                        M5.append(a.split()[4])
                        M6.append(a.split()[5])
#            if a.strip() == 'M1        M2        M3        M4        M5':
            if a.find('M1        M2        M3        M4        M5') != -1:
                correct = 'yes'
    else:
        M1, M2, M3, M4, M5, M6 = [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0]


    return M1, M2, M3, M4, M5, M6


#===========================================================================
# process one file
#===========================================================================
def process_one_file(file):
    numg = 0
    nums = 0
    # we retrieve the lines compressed in the file
    info = take_info( file )
    # info = '' means the file was not found
    if info != '':
        evt = info.find('EventType')
        if evt == -1:
            pass
        else:
            flr = string.split(info[evt:evt+50],"\n")[0]
            if string.strip(string.split(flr," ")[1]) != '=':                
                evntp = string.strip(string.split(flr," ")[1])  # OK for MC11 MOB
            else:
                evntp = string.strip(string.split(flr,"=")[1])  # OK for MC10 MOB
            
            # GC20080118 remove ; if found
            pos = evntp.find(';')
            if pos > -1:
                evntp = evntp.replace(';','')
            # The eventtype given and this one found in the log file should be equal
            if debug_flag == 1: print 'evntp %s eventtype %s '%(evntp,eventtype)
            if eventtype != evntp and eventtype != '30000000':
                sys.exit('something is wrong with logfile format:evntp %s eventtype %s '%(evntp,eventtype))
                pass
            else:
                # the data are correct, begin the process
                # /////////// GENERATION STATISTICS ///////////
                # results in generationResults
                if debug_flag == 1: print 'execution=',execution
                if execution == 'both' or execution == 'generation':
                    search_general_info(info)
                    numg= Generators(info)
                    if debug_flag == 1: print 'numg=',numg
#                    print 'File ',file,'has ',numg,' events'
                            
                # /////////// SIMULATION TABLES  ///////////
                # results in gevents, averageResults, veloResults, richResults, m1, m2, m3, m4, m5 
                if execution == 'both' or execution =='simulation':
                    search_general_info(info)
                    nums = Simulation(info)
                    if debug_flag == 1: print 'nums=',nums
                    
    return numg,nums

#========================================================================
# generator statistics
#========================================================================

def Generators(info):
  # results in generationResults
  global generationResults
  num = 0
  grAux = search_lines(info)

  if generationResults == {}:
      for index in grAux.keys():
          generationResults[index] = grAux[index]
              
#MY_DEBYG: next line added
          num = 1
  else:
      if grAux != None:
          # if some counters are missing the values are not counted
          if len(grAux.keys()) >= len(generationResults.keys()):
              aux = generationResults
#              Generationresults = {}
              for index in grAux.keys():
#                  if index == 'DecFiles' or index == 'ParamFiles':
                  if index == 'DecFiles' or index == 'ParamFiles' or  index == 'L0':
                      if aux[index] != grAux[index]:
                          if debug_flag == 1: print 'Warning: Different %s version/value found in the logfiles belonging the same production' % index
                          generationResults = aux
                      else:
                          generationResults[index] = grAux[index]
                          num = 1
                  else:
                      generationResults[index] = grAux[index] + aux[index]

  return num
     
#========================================================================
# simulation statistics
#=========================================================================

def Simulation(info):

  # /////////// SIMULATION TABLES  ///////////
  # results in gevents, averageResults, veloResults, richResults, m1, m2, m3, m4, m5, m6
  global geantparticles, gevents, averageResults, veloResults, richResults, m1, m2, m3, m4, m5, m6

  num = 0  
  valid1, valid2, valid3, valid4, valid5 = 0,0,0,0,0

  data = NumberOfEvents(info)
  valid1,gevents = SummaryTable(data,gevents)

  # Type of particles known to Geant4
  particle = ParticlesGeant4(info)
  if particle not in geantparticles:
      geantparticles.append(particle)


  # Summary table average
  averageData = AverageTable(info)
  valid2,averageResults = SummaryTable(averageData,averageResults)
  
  # Summary table for the Velo
  veloData = VeloTable(info)
  valid3,veloResults = SummaryTable(veloData,veloResults)

  # Summary table for RICHes
  richData = RICHesTable(info)
  valid4,richResults = SummaryTable(richData,richResults)

  # Summary table for the MUON
  a1, a2, a3, a4, a5, a6 = MUONTable(info)
  if a1 != [0, 0, 0, 0, 0] and a2 != [0, 0, 0, 0, 0] and a3 != [0, 0, 0, 0, 0] and a3 != [0, 0, 0, 0, 0] and a4 != [0, 0, 0, 0, 0] and a5 != [0, 0, 0, 0, 0]:
    for index in range(5):
        m1[index] += float(a1[index])
        m2[index] += float(a2[index])
        m3[index] += float(a3[index])
        m4[index] += float(a4[index])
        m5[index] += float(a5[index])
        m6[index] = a6[index]
    valid5 = 1

  #print jobs

  if ( valid1 == 1 ) and ( valid2 == 1 ) and ( valid3 == 1 ) and ( valid4 == 1 ) and( valid5 == 1 ):
      num = 1
  return num

# ================================================================================================================
# Summary table
#=================================================================================================================

def SummaryTable(Data,Results):

   # Summary
  if Results == {}:
      for index in Data.keys():
          Results[index] = Data[index]
      valid = 1
  else:
      if len(Data.keys()) >= len(Results.keys()):
          aux = Results
          Results = {}
          for index in Data.keys():
              Results[index] = Data[index] + aux[index]
          valid = 1


  return valid,Results



# ========================================================================
# Retrieve info from the .txt.gz or .log.bz2 logfile.
# ========================================================================
def take_info(file):
    import urllib, tempfile, os, gzip, bz2
    
    print file
    log_file = ''
    
    tmpfile = tempfile.mktemp()
    
    try:
      urllib.urlretrieve(file,tmpfile)
    except IOError:
      print "Error: \tThe url %s is not available or doesn't exist.\n" % file
      exit()
      return log_file

    # Open the log file. First try opening it as a gzip file and if that doesn't work try bz2.
    fc = None
    try:
      fc = gzip.open(tmpfile)
      log_file = fc.read()
    except IOError, e:
      # Close the gzip opened file and reopen it as bz2.
      fc.close()
      try:
        fc = bz2.BZ2File(tmpfile)
        log_file = fc.read()
      except IOError, e:
#        print "Error: %s could not be opened as a bz2 or gzip file. Unknown compression or file could not be found on server." %file
#        print e
# sm
        fc.close()
        try:
            fc = open(tmpfile)
            log_file = fc.read()
        except IOError, e:
            print "Error: %s could not be opened as uncompressed or a bz2 or gzip file. Unknown compression or file could not be found on server." %file
            print e


    if fc:
      fc.close()
      
    os.remove(tmpfile)
    return log_file

# =======================================================================
# Install the produced html pages in the $LHCBDOC area
# =======================================================================
def install(file):

#    dstDir = "/afs/cern.ch/lhcb/software/releases/DOC/gauss/simulation/test_STATISTICS/"
    dstDir = "/afs/cern.ch/lhcb/software/releases/DOC/STATISTICS/MC09STAT/"
    if not os.path.exists(dstDir):
        os.mkdir( dstDir )
    dstFile = os.path.join(dstDir,os.path.basename(file))
    if os.path.exists(dstFile):
        print 'The file you are trying to move existed already: moving existing file to .old!' 
        shutil.move( dstFile , dstFile+'.old')
#    shutil.move( file , dstFile )
    shutil.copy( file , dstFile )

    return

# ========================================================================
# Updated the Statistics Summary web page with the new Conditions entry
# ========================================================================
def addToIndex(file):

# directory where the single stat test are copied
#    dstDir = "/afs/cern.ch/lhcb/software/releases/DOC/gauss/simulation/test_STATISTICS/"
    dstDir = "/afs/cern.ch/lhcb/software/releases/DOC/STATISTICS/MC09STAT/"
# directory where the summary web page will be
#    sumDir = "/afs/cern.ch/lhcb/software/releases/DOC/gauss/simulation/test_STATISTICS/"
    sumDir = "/afs/cern.ch/lhcb/software/releases/DOC/STATISTICS/MC09STAT/"
    shutil.copy(sumDir+"index.html", "index.html")
    f = open("index.html",'r')
    g = open("newindex.html",'w')

    dstFile = os.path.join(dstDir,os.path.basename(file))

    fString = getHTML("http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/STATISTICS/MC09STAT/index.html")
    nameEntry = str(os.path.basename(file).split(".")[0])
# check if there is already this configuration entry in the table 
    isAlready = fString.find(nameEntry)
    if isAlready == -1:
        for line in f:
            g.write(line)
            if line.find('end of page') != -1:
                g.write( "<TR>\n" )
                g.write( "<TD>\n" )
                #          g.write( "<A href=\""+dstFile+"\">"+os.path.basename(file).split(".")[0]+"</A>\n")
                g.write( "<A href=\""+os.path.basename(file)+"\">"+os.path.basename(file).split(".")[0]+"</A>\n")
                g.write( "</TD>\n</TR>" )
    f.close()
    g.close()
    
    if isAlready == -1:
        os.remove( "index.html" )
        os.rename( "newindex.html","index.html")
        shutil.move( "index.html" , sumDir )

    return

#################################################################
def getHTML(url):
        print ">>> URL -> ", url
        usock = urllib.urlopen(url)
        htmlSource = usock.read()
        usock.close()
        return htmlSource

#################################################################
def getHTMLlines(url):

# --- get url content (line by line) ---

#  print ">>> URL -> ", url
  usock = urllib.urlopen(url)
  htmlSource = usock.readlines()
  usock.close()
  return htmlSource

#################################################################
def readErrorDesc(evntp):

  htmlSource = getHTMLlines(url)

  desc_index = 0
  for line in htmlSource:
      if desc_index == 1:
#          desc = line.split('<td>')[1].strip().split('</td>')[0]
          if "href" in line:
              desc = line.split('<b>')[1].strip().split('<br>')[0]
              break
      if evntp in line:
          desc_index = 1
  if desc_index == 0:
      desc = 'dummy'

  return desc
 
# ========================================================================
# Usage
# ========================================================================
def usage():
    import sys
    print "Usage: \tpython GaussStat.py -e EVENTTYPE -f filename.log.gz"
    print "Usage: \tpython GaussStat.py -e EVENTTYPE -v GaussVersion -f filename.log.gz"
    print "Usage: \tpython GaussStat.py -e EVENTTYPE -v GaussVersion --path directory_with_logs"
    print "\t                              [--simulation/--generation]"
    print "\t                              [-h/--help]\n"
    print "\t                              [-d/--debug]\n"
    print "\t                              [-i/--install]\n"
    print "\t                              [-a/--addToIndex]\n"
 
    print "Ex: python GaussStat.py -e 13104011 --path /afs/cern.ch/lhcb/group/gauss/LogChecks/testLog.dir/Gauss_logs_12345/"
    print "    python GaussStat.py -e 13104011 -v v37r3p1 --path /afs/cern.ch/lhcb/group/gauss/LogChecks/testLog.dir/Gauss_logs_12345/"
    print "    python GaussStat.py -e 13104011 -v v37r3p1 -f /afs/cern.ch/lhcb/group/gauss/LogChecks/testLog.dir/Gauss_logs_12345/Gauss_00012345_00012345_1.log.gz"
    print "    python GaussStat.py -e 13104011 --simulation --path /afs/cern.ch/lhcb/group/gauss/LogChecks/testLog.dir/Gauss_logs_12345/"

    sys.exit()

# ========================================================================
# Main
# ========================================================================
if __name__ == "__main__":
    import urllib, gzip, string, sys, math, time, getopt, tempfile, os, shutil

    global eventtype, gaussversion, execution, debug_flag, publish
    global url

    # variables initialization
    filename = ''
    filelist = []
    prodlist = []
    debug_flag = 0
    install_flag = 0
    add_flag = 0
    eventtype = ''
    gaussversion = ''
    execution = '' # generation/simulation

# --- description of eventytpes:
#    url = "http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/gauss/decfiles/releases/latest/table_event.html"
    url = "http://lhcb-release-area.web.cern.ch/LHCb-release-area/DOC/decfiles/releases/latest/table_evttype.html"

# -----------------------------------------------------------------------
# check arguments

    arguments = sys.argv[1:]
    if len(sys.argv) == 1:
      usage()
      sys.exit()
    try:
      keys, values = getopt.getopt(arguments,'aidhgse:v:f:',
                    ['addToIndex','install','debug','help','generation','simulation','evttype','version=','file=','filelist=','path='])

    except getopt.GetoptError:
      usage()
      sys.exit()

    for key,value in keys:
        if key in ('-h','--help'):
            usage()
            sys.exit()
        if key in ('-d','--debug'):
            debug_flag = 1
        if key in ('-i','--install'):
            install_flag = 1
        if key in ('-a','--addToIndex'):
            add_flag = 1
        if key in ('-e','--evttype'):
            eventtype = value
        if key in ('-v','--version'):
            gaussversion = value
        if key in ('-g','--generation'):    
            if execution == '':
                execution = 'generation'
            else:
                execution = 'both'
        if key in ('-s','--simulation'):
            if execution == '':
                execution = 'simulation'
            else:
                execution = 'both'
        if key in ('-f','--file'):
            filename = value
        if key in ('--filelist', ''):
            filelist = get_file_list(value)
        if key in ('--path', ''):
            filelist = [os.path.join(value, f) for f in os.listdir(value)]
    if execution == '': execution = 'both'
    print 'debug_flag %s  eventtype %s gaussversion %s  execution %s install_flag %s add_flag %s'%(debug_flag,eventtype,gaussversion,execution,install_flag,add_flag)

    #-------------------------------------------------------------------------
    # analize only one file if filename is provided with -f options

    if filename != '' or 0 != len(filelist):
        publish = 'no'
        LocationPath = os.getcwd()
        desc = ''
        generationResults = {}
        gevents, averageResults, veloResults, richResults = {}, {}, {}, {}
        geantparticles = []
        m1, m2, m3 =[0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0]
        m4, m5, m6 =[0, 0, 0, 0, 0], [0, 0, 0, 0, 0], [0, 0, 0, 0, 0]

        # Process the files we are given.
        # Can handle either a single file or list of files or both.
        numg, nums = 0, 0
        # Process any file that we are given.
        if '' != filename:
            prodlist.append(string.split(os.path.basename(filename),"_")[1])
            numg,nums = process_one_file(filename)

        # Loop over the list of files given if there are any.
        for fname in filelist:
            prodlist.append(string.split(os.path.basename(fname),"_")[1])
            tmpnumg,tmpnums = process_one_file(fname)
            numg = numg + tmpnumg
            nums = nums + tmpnums

        # compute average and create tables
        if execution == 'both' or execution == 'generation':
            print 'Creating Generation Statistics Tables.... ', eventtype,gaussversion
            # store the results in a file RESULTS_<eventtype>.html
            rc = gen_store_results(numg)
            if rc == 'KO':
                print 'No results for generation statistics %s Gauss %s eventtype %s' %(this_lumi,gaussversion,eventtype)

        if execution == 'both' or execution =='simulation':
            print 'Creating Simulation  Statistics Tables.... ', eventtype,gaussversion
            # store the results in a file RESULTS_<eventtype>.html
            muonResults = [m1, m2, m3, m4, m5, m6]
            rc = sim_store_results(nums,gevents,averageResults,veloResults,richResults,muonResults)

        sys.exit('end of statistics')
        
#########################################################################
