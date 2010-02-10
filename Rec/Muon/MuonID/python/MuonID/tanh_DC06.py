#===========================================================
# Mapping of distances using hyperbolic tangent (DLL_flag=3)
# Use nMupBins+1 for each region, to deal with last p-bin scaling
#===========================================================

#print "Loading hyperbolic tangent parameters"

#  Scale factors (The effective scale factors are these divided by atanh(0.5) )
tanhScaleFactorsR1 = [470.8,103.2,42.4,22.,13.8,9.5, 4.4]
tanhScaleFactorsR2 = [215.7,34.9, 12.6,7.2, 3.3]
tanhScaleFactorsR3 = [226.4,34.1,10.3,4., 2.1]
tanhScaleFactorsR4 = [ 73.2,14.8,8.7,4.9, 2.3]

# Cumulative tanh(dist2) histograms content for muon and combinatorial non-muons
# For the moment, non-muons dist gets contribution form any momentum 
# (So, only distfac makes it different for a given p-bin)
nDistBins = 50

#print "Loading tanh parameterss for signal muons distributions" 
# Automatically printed by perfDLL::ReadDLLPar()
# Signal muons:
#Region 1 ( 3 < p < 10 ):
tanhCumulHistoMuonR1_1 = [0, 0.106092, 0.220588, 0.306197, 0.370273, 0.429622, 0.486345, 0.536765, 0.577731, 0.620273, 0.652311, 0.676996, 0.702731, 0.725315, 0.75, 0.77416, 0.794118, 0.811975, 0.831933, 0.846113, 0.856618, 0.871324, 0.877626, 0.883403, 0.893908, 0.89916, 0.906513, 0.913866, 0.921744, 0.924895, 0.927521, 0.933824, 0.939601, 0.942752, 0.948004, 0.951681, 0.954832, 0.956408, 0.957983, 0.96271, 0.966387, 0.967962, 0.970063, 0.973214, 0.975315, 0.977416, 0.978992, 0.980567, 0.984769, 0.987395]

#Region 1 ( 10 < p < 20 ):
tanhCumulHistoMuonR1_2 = [0, 0.109433, 0.228936, 0.323766, 0.399127, 0.461732, 0.510574, 0.55287, 0.59332, 0.631923, 0.663478, 0.690668, 0.717355, 0.737999, 0.757637, 0.776771, 0.792212, 0.808996, 0.822424, 0.835851, 0.847096, 0.859013, 0.870594, 0.880665, 0.891071, 0.899295, 0.908191, 0.917086, 0.925814, 0.931856, 0.939241, 0.944948, 0.94948, 0.956865, 0.961229, 0.96811, 0.97197, 0.975327, 0.978684, 0.982209, 0.985062, 0.986573, 0.988755, 0.990433, 0.991944, 0.993118, 0.994965, 0.99614, 0.997482, 0.998322]

#Region 1 ( 20 < p < 30 ):
tanhCumulHistoMuonR1_3 = [0, 0.0944471, 0.224727, 0.323564, 0.408519, 0.471879, 0.524086, 0.566564, 0.6056, 0.641077, 0.670978, 0.697081, 0.719744, 0.742406, 0.76317, 0.777527, 0.793308, 0.807784, 0.821666, 0.837565, 0.84943, 0.859872, 0.871144, 0.879924, 0.890009, 0.899383, 0.907807, 0.915638, 0.922046, 0.928097, 0.933555, 0.938894, 0.943522, 0.94898, 0.953844, 0.95954, 0.963099, 0.966659, 0.970337, 0.974015, 0.979829, 0.98327, 0.986711, 0.988965, 0.99205, 0.994067, 0.996084, 0.997508, 0.998695, 0.999169]

#Region 1 ( 30 < p < 40 ):
tanhCumulHistoMuonR1_4 = [0, 0.0644907, 0.181738, 0.291194, 0.379647, 0.447885, 0.510699, 0.562666, 0.602012, 0.639779, 0.674095, 0.701312, 0.728429, 0.750222, 0.770634, 0.787398, 0.805049, 0.819249, 0.833153, 0.845577, 0.855537, 0.865891, 0.875259, 0.88295, 0.891135, 0.899024, 0.906518, 0.914308, 0.921014, 0.928311, 0.932551, 0.938862, 0.943793, 0.94823, 0.953062, 0.957598, 0.962233, 0.966177, 0.969825, 0.973474, 0.977813, 0.981461, 0.984716, 0.987772, 0.990336, 0.9929, 0.995267, 0.99714, 0.999211, 0.999507]

#Region 1 ( 40 < p < 50 ):
tanhCumulHistoMuonR1_5 = [0, 0.0510469, 0.155457, 0.262646, 0.359274, 0.435334, 0.494905, 0.547341, 0.589124, 0.625903, 0.659904, 0.690569, 0.715397, 0.738095, 0.759774, 0.779044, 0.796924, 0.812118, 0.825644, 0.837688, 0.849639, 0.862053, 0.871873, 0.881138, 0.890587, 0.899389, 0.907634, 0.914767, 0.920141, 0.927923, 0.934593, 0.939411, 0.944877, 0.949231, 0.954141, 0.958959, 0.962757, 0.966278, 0.970539, 0.973689, 0.976839, 0.979711, 0.982676, 0.98527, 0.987864, 0.990365, 0.992959, 0.995738, 0.998147, 0.999074]

#Region 1 ( 50 < p < 60 ):
tanhCumulHistoMuonR1_6 = [0, 0.0294372, 0.124964, 0.21366, 0.312073, 0.39923, 0.465993, 0.522944, 0.574603, 0.615392, 0.64887, 0.67975, 0.705916, 0.731025, 0.752573, 0.772487, 0.791727, 0.807407, 0.822318, 0.83569, 0.846176, 0.859933, 0.870322, 0.881289, 0.889658, 0.899278, 0.907359, 0.91342, 0.921212, 0.927946, 0.932852, 0.938624, 0.943723, 0.947379, 0.951515, 0.956518, 0.961231, 0.965849, 0.969505, 0.972487, 0.976431, 0.979317, 0.982492, 0.984993, 0.988456, 0.990669, 0.993362, 0.995863, 0.997787, 0.999519]

#Region 1 ( 60 < p < 70 ):
tanhCumulHistoMuonR1_7 = [0, 0.0151013, 0.0675503, 0.143644, 0.227866, 0.307532, 0.381709, 0.452573, 0.518985, 0.575455, 0.624642, 0.664319, 0.696973, 0.726174, 0.75151, 0.773653, 0.793034, 0.809672, 0.825394, 0.839564, 0.852128, 0.862897, 0.874012, 0.883729, 0.89165, 0.899676, 0.906924, 0.913638, 0.920058, 0.925753, 0.931328, 0.936454, 0.941269, 0.945635, 0.949536, 0.953919, 0.957958, 0.961858, 0.964706, 0.968106, 0.971437, 0.974474, 0.977253, 0.979842, 0.982914, 0.985227, 0.987988, 0.991008, 0.993666, 0.996548]

#Region 2 ( 3 < p < 10 ):
tanhCumulHistoMuonR2_1 = [0, 0.149861, 0.286478, 0.382551, 0.45783, 0.515102, 0.564126, 0.602463, 0.636501, 0.666938, 0.696561, 0.721538, 0.739428, 0.760572, 0.779507, 0.795539, 0.812965, 0.826673, 0.837825, 0.850372, 0.861408, 0.870586, 0.87895, 0.885572, 0.89138, 0.899164, 0.905669, 0.911826, 0.917751, 0.923211, 0.927277, 0.931924, 0.93448, 0.9375, 0.941101, 0.943541, 0.947607, 0.950511, 0.952951, 0.956668, 0.959224, 0.961431, 0.963522, 0.966078, 0.968518, 0.971073, 0.974675, 0.977463, 0.980367, 0.983852]

#Region 2 ( 10 < p < 20 ):
tanhCumulHistoMuonR2_2 = [0, 0.088294, 0.219717, 0.323275, 0.40292, 0.471019, 0.523541, 0.568119, 0.609017, 0.644006, 0.673359, 0.700481, 0.724668, 0.745685, 0.766858, 0.784862, 0.800634, 0.815037, 0.827795, 0.840437, 0.852217, 0.863293, 0.874212, 0.883449, 0.891824, 0.899339, 0.90654, 0.913467, 0.919573, 0.924543, 0.930414, 0.934875, 0.93922, 0.944073, 0.948808, 0.953192, 0.957888, 0.961293, 0.964659, 0.968455, 0.971978, 0.974874, 0.978357, 0.98094, 0.983915, 0.98728, 0.990177, 0.993268, 0.99593, 0.997691]

#Region 2 ( 20 < p < 30 ):
tanhCumulHistoMuonR2_3 = [0, 0.0413114, 0.138905, 0.240951, 0.335639, 0.414566, 0.477133, 0.533447, 0.578296, 0.616796, 0.651317, 0.681037, 0.707883, 0.730908, 0.752479, 0.77304, 0.791201, 0.807656, 0.822279, 0.836681, 0.849725, 0.860622, 0.871613, 0.881498, 0.890594, 0.89928, 0.906576, 0.913903, 0.921104, 0.926789, 0.932506, 0.938159, 0.943529, 0.947666, 0.952561, 0.957646, 0.962005, 0.966363, 0.97009, 0.973596, 0.976628, 0.979913, 0.98244, 0.984714, 0.98683, 0.989483, 0.991883, 0.994504, 0.996621, 0.998642]

#Region 2 ( 30 < p < 40 ):
tanhCumulHistoMuonR2_4 = [0, 0.0354249, 0.108906, 0.189695, 0.272872, 0.353833, 0.430189, 0.496953, 0.551562, 0.596025, 0.63564, 0.669645, 0.698733, 0.726193, 0.750398, 0.771452, 0.790498, 0.808609, 0.823326, 0.837212, 0.85044, 0.861971, 0.872394, 0.881744, 0.890505, 0.899508, 0.908546, 0.916165, 0.923056, 0.929531, 0.936041, 0.941651, 0.947088, 0.951035, 0.955399, 0.959312, 0.96364, 0.967969, 0.971743, 0.975241, 0.978634, 0.981612, 0.984902, 0.987118, 0.989681, 0.991689, 0.993802, 0.995533, 0.997368, 0.999134]

#Region 2 ( 40 < p < 50 ):
tanhCumulHistoMuonR2_5 = [0, 0.00862237, 0.0411871, 0.0955093, 0.158714, 0.229007, 0.301263, 0.368382, 0.433082, 0.491943, 0.546396, 0.59774, 0.642542, 0.68304, 0.716671, 0.74592, 0.771202, 0.792543, 0.811712, 0.827917, 0.842938, 0.856476, 0.868298, 0.879547, 0.889873, 0.899354, 0.90717, 0.914609, 0.921891, 0.928563, 0.934428, 0.940086, 0.945158, 0.94954, 0.953585, 0.957838, 0.9617, 0.965406, 0.9691, 0.972806, 0.975798, 0.978633, 0.981741, 0.98468, 0.987034, 0.989557, 0.992015, 0.994447, 0.996671, 0.998153]

#Region 3 ( 3 < p < 6 ):
tanhCumulHistoMuonR3_1 = [0, 0.189164, 0.326654, 0.419061, 0.489191, 0.548115, 0.59333, 0.626285, 0.656604, 0.685473, 0.710651, 0.733588, 0.755866, 0.775376, 0.79304, 0.808331, 0.822041, 0.835223, 0.847087, 0.856973, 0.867387, 0.875165, 0.88202, 0.887951, 0.893488, 0.899025, 0.90522, 0.909438, 0.91392, 0.916557, 0.919852, 0.923807, 0.926443, 0.929475, 0.932244, 0.934485, 0.937121, 0.93923, 0.940548, 0.942789, 0.945162, 0.946612, 0.948326, 0.949776, 0.951885, 0.952676, 0.954653, 0.957553, 0.960322, 0.964672]

#Region 3 ( 6 < p < 10 ):
tanhCumulHistoMuonR3_2 = [0, 0.0832973, 0.208772, 0.311332, 0.395542, 0.462507, 0.516549, 0.564635, 0.604458, 0.63967, 0.672095, 0.699669, 0.723495, 0.74516, 0.766105, 0.783734, 0.80074, 0.816016, 0.830379, 0.842629, 0.854638, 0.864966, 0.874333, 0.883028, 0.892107, 0.899169, 0.907239, 0.91531, 0.921891, 0.927944, 0.933612, 0.938512, 0.943123, 0.948071, 0.95153, 0.955901, 0.960225, 0.964164, 0.968151, 0.971273, 0.975261, 0.978335, 0.98165, 0.984484, 0.987654, 0.990729, 0.992938, 0.995148, 0.997118, 0.998751]

#Region 3 ( 10 < p < 20 ):
tanhCumulHistoMuonR3_3 = [0, 0.0442162, 0.13949, 0.238359, 0.334882, 0.418058, 0.486836, 0.543333, 0.590627, 0.631003, 0.665102, 0.695499, 0.721432, 0.74389, 0.764795, 0.782697, 0.798345, 0.814694, 0.82827, 0.841236, 0.852816, 0.864045, 0.873202, 0.882085, 0.890587, 0.898677, 0.906128, 0.913503, 0.919658, 0.926286, 0.931619, 0.936555, 0.941675, 0.946444, 0.950817, 0.955479, 0.959623, 0.963509, 0.967089, 0.97038, 0.97358, 0.976795, 0.979233, 0.981838, 0.984581, 0.987003, 0.989182, 0.991757, 0.994363, 0.996678]

#Region 3 ( 20 < p < 30 ):
tanhCumulHistoMuonR3_4 = [0, 0.0171465, 0.0560427, 0.116804, 0.188635, 0.259293, 0.32514, 0.389169, 0.448779, 0.503763, 0.556054, 0.603328, 0.646252, 0.68066, 0.711708, 0.738728, 0.762779, 0.785404, 0.80446, 0.821768, 0.836774, 0.850399, 0.863012, 0.875348, 0.886557, 0.896729, 0.906258, 0.913807, 0.920481, 0.92688, 0.933117, 0.938365, 0.94421, 0.950517, 0.955396, 0.959562, 0.963543, 0.96741, 0.971507, 0.974614, 0.978434, 0.981703, 0.984442, 0.987457, 0.990057, 0.992681, 0.994384, 0.996295, 0.997883, 0.999033]

#Region 3 ( 30 < p < 40 ):
tanhCumulHistoMuonR3_5 = [0, 0.00280597, 0.0139387, 0.0463349, 0.0947652, 0.146639, 0.19793, 0.253831, 0.315143, 0.374925, 0.427746, 0.477507, 0.525099, 0.571252, 0.613797, 0.655248, 0.694186, 0.729406, 0.759525, 0.78824, 0.814077, 0.835687, 0.854399, 0.871217, 0.885301, 0.89771, 0.908915, 0.918044, 0.926935, 0.933878, 0.941184, 0.947525, 0.952827, 0.957801, 0.963231, 0.967841, 0.971831, 0.975001, 0.978609, 0.981561, 0.984421, 0.987045, 0.989414, 0.991491, 0.993167, 0.994862, 0.996119, 0.997486, 0.998779, 0.999581]

#Region 4 ( 3 < p < 6 ):
tanhCumulHistoMuonR4_1 = [0, 0.195304, 0.359145, 0.466469, 0.542796, 0.596924, 0.63969, 0.673717, 0.702325, 0.724291, 0.744625, 0.760473, 0.778535, 0.793218, 0.807085, 0.819262, 0.8301, 0.839888, 0.849968, 0.858766, 0.865641, 0.87374, 0.881314, 0.888481, 0.894074, 0.899085, 0.905028, 0.910389, 0.914292, 0.91942, 0.923848, 0.927985, 0.932529, 0.935909, 0.939405, 0.9429, 0.946571, 0.949892, 0.952805, 0.955602, 0.95869, 0.961429, 0.964342, 0.966498, 0.968828, 0.971625, 0.974363, 0.9785, 0.981239, 0.985201]

#Region 4 ( 6 < p < 8 ):
tanhCumulHistoMuonR4_2 = [0, 0.0484059, 0.145218, 0.245048, 0.338576, 0.417117, 0.481146, 0.533895, 0.581559, 0.620909, 0.655704, 0.686421, 0.71306, 0.736257, 0.756911, 0.776772, 0.79446, 0.812096, 0.826925, 0.840589, 0.851605, 0.863097, 0.872895, 0.882798, 0.891431, 0.899428, 0.907743, 0.916164, 0.922678, 0.928821, 0.935759, 0.940314, 0.945133, 0.949899, 0.953924, 0.958055, 0.962451, 0.966688, 0.969813, 0.972461, 0.974738, 0.978339, 0.980934, 0.983688, 0.986707, 0.989514, 0.991844, 0.994333, 0.996611, 0.998782]

#Region 4 ( 8 < p < 10 ):
tanhCumulHistoMuonR4_3 = [0, 0.0361972, 0.110412, 0.194144, 0.281361, 0.362856, 0.436031, 0.498908, 0.551799, 0.596942, 0.63652, 0.671573, 0.701217, 0.727117, 0.751196, 0.772883, 0.792438, 0.809757, 0.825567, 0.838153, 0.851311, 0.862544, 0.874454, 0.882983, 0.892344, 0.900094, 0.906907, 0.913928, 0.920377, 0.92771, 0.934159, 0.939255, 0.94534, 0.950073, 0.953505, 0.957458, 0.960422, 0.964843, 0.968379, 0.971604, 0.97436, 0.977637, 0.980237, 0.983358, 0.985698, 0.98861, 0.990691, 0.993395, 0.996203, 0.998648]

#Region 4 ( 10 < p < 15 ):
tanhCumulHistoMuonR4_4 = [0, 0.0225046, 0.0672619, 0.138107, 0.215025, 0.286178, 0.352348, 0.416587, 0.477691, 0.534625, 0.584728, 0.625203, 0.661843, 0.695348, 0.724486, 0.750154, 0.774114, 0.794211, 0.811958, 0.827577, 0.842356, 0.855959, 0.868275, 0.879919, 0.891004, 0.900409, 0.908722, 0.917175, 0.924733, 0.931339, 0.937413, 0.942871, 0.947909, 0.952612, 0.957342, 0.961513, 0.965095, 0.968818, 0.972037, 0.97506, 0.978307, 0.981414, 0.984269, 0.986732, 0.988916, 0.990903, 0.993142, 0.995018, 0.996501, 0.998293]

#Region 4 ( 15 < p < 20 ):
tanhCumulHistoMuonR4_5 = [0, 0.00325542, 0.0188308, 0.0549134, 0.102322, 0.152576, 0.205345, 0.266301, 0.325094, 0.385836, 0.438878, 0.489776, 0.53925, 0.584378, 0.626796, 0.668103, 0.705082, 0.7375, 0.766136, 0.790815, 0.814441, 0.834227, 0.851771, 0.868107, 0.8822, 0.895164, 0.90569, 0.915456, 0.924599, 0.932455, 0.938868, 0.945457, 0.951325, 0.956412, 0.960993, 0.964931, 0.969356, 0.972787, 0.975594, 0.978421, 0.98154, 0.984093, 0.986588, 0.988674, 0.991013, 0.992904, 0.994698, 0.996472, 0.997895, 0.99922]


#print "Loading tanh parameterss for non muons distributions" 
# Non muons:
# From file DVNv22r3_tanh_jpsi-bbinc-closest-isMuonLoose-cumulative-AllMom-PerfDLL.log_options-file-AllMom
#Region 1 ( 3 < p < 10 ):
tanhCumulHistoNonMuonR1_1 = [0, 0.065762, 0.141926, 0.208371, 0.26867, 0.321086, 0.368593, 0.410863, 0.448075, 0.483079, 0.513549, 0.542547, 0.567192, 0.590695, 0.611083, 0.630137, 0.647431, 0.663082, 0.677645, 0.69097, 0.704199, 0.715636, 0.726241, 0.735949, 0.74569, 0.754737, 0.763784, 0.772234, 0.780555, 0.787938, 0.795523, 0.801999, 0.809361, 0.815901, 0.822462, 0.829077, 0.835723, 0.841388, 0.847629, 0.853743, 0.859642, 0.865948, 0.872274, 0.878451, 0.884714, 0.891734, 0.899138, 0.907502, 0.916645, 0.929661]

#Region 1 ( 10 < p < 20 ):
tanhCumulHistoNonMuonR1_2 = [0, 0.00628387, 0.0196304, 0.0372978, 0.0555413, 0.0740622, 0.0913669, 0.108362, 0.125208, 0.14136, 0.15746, 0.172748, 0.18794, 0.203132, 0.217812, 0.23213, 0.247098, 0.26132, 0.274986, 0.288354, 0.302191, 0.315645, 0.329226, 0.342615, 0.355738, 0.368337, 0.381172, 0.393857, 0.406339, 0.41879, 0.431144, 0.443893, 0.4566, 0.469615, 0.48277, 0.495711, 0.508332, 0.521764, 0.535804, 0.549204, 0.562924, 0.57737, 0.59253, 0.607871, 0.624066, 0.641424, 0.65986, 0.680931, 0.706877, 0.740089]

#Region 1 ( 20 < p < 30 ):
tanhCumulHistoNonMuonR1_3 = [0, 0.00144028, 0.00452354, 0.00870567, 0.0144774, 0.020708, 0.0280587, 0.0355908, 0.0430056, 0.0509538, 0.0585179, 0.0664554, 0.0742436, 0.0817224, 0.0892118, 0.0970213, 0.10482, 0.112214, 0.11981, 0.12731, 0.135258, 0.143174, 0.150877, 0.158804, 0.166219, 0.174273, 0.182296, 0.190597, 0.19895, 0.207208, 0.215753, 0.224715, 0.233794, 0.243321, 0.252881, 0.262845, 0.273012, 0.283244, 0.294104, 0.305872, 0.318002, 0.331328, 0.345144, 0.360101, 0.376478, 0.394582, 0.41545, 0.440341, 0.473264, 0.522426]

#Region 1 ( 30 < p < 40 ):
tanhCumulHistoNonMuonR1_4 = [0, 0.000448086, 0.0015683, 0.00314727, 0.00481159, 0.00713737, 0.00948449, 0.0125677, 0.0154696, 0.0187236, 0.022575, 0.0265118, 0.0305232, 0.0345773, 0.0382473, 0.0426749, 0.0472944, 0.0516152, 0.05568, 0.0603209, 0.0649725, 0.069688, 0.0743183, 0.0788631, 0.083536, 0.0882303, 0.0934686, 0.0986003, 0.103732, 0.108693, 0.114177, 0.11965, 0.125485, 0.131289, 0.137328, 0.143996, 0.150568, 0.157492, 0.164426, 0.172172, 0.180696, 0.189743, 0.199409, 0.209811, 0.222112, 0.235683, 0.25224, 0.272287, 0.298244, 0.341292]

#Region 1 ( 40 < p < 50 ):
tanhCumulHistoNonMuonR1_5 = [0, 0.000224043, 0.000650792, 0.00140827, 0.00233645, 0.00336065, 0.00441685, 0.00569709, 0.00720138, 0.00853497, 0.0104127, 0.0124184, 0.0143388, 0.0163551, 0.0184569, 0.0208467, 0.0235352, 0.0262557, 0.0287949, 0.0316541, 0.034524, 0.0372018, 0.0399757, 0.0431336, 0.0463982, 0.0497802, 0.0528635, 0.0561174, 0.0597875, 0.0635215, 0.0671702, 0.0710216, 0.074841, 0.0788631, 0.0830453, 0.0873768, 0.0920923, 0.097032, 0.102484, 0.10769, 0.113504, 0.119831, 0.126723, 0.134511, 0.143548, 0.153181, 0.164864, 0.179843, 0.20006, 0.232695]

#Region 1 ( 50 < p < 60 ):
tanhCumulHistoNonMuonR1_6 = [0, 7.4681e-05, 0.000309393, 0.000725473, 0.00120556, 0.001739, 0.00240046, 0.00323262, 0.00384074, 0.00468357, 0.00560108, 0.00666795, 0.00764947, 0.00860965, 0.00991124, 0.0114262, 0.0128665, 0.0142321, 0.015587, 0.0172406, 0.0189156, 0.020708, 0.0227777, 0.0248901, 0.0268425, 0.0289229, 0.0313874, 0.0335638, 0.0359109, 0.0380553, 0.0404984, 0.0433577, 0.0462595, 0.0492895, 0.0523087, 0.0552319, 0.0588806, 0.062476, 0.0664234, 0.0704029, 0.0747023, 0.0792472, 0.0844109, 0.09014, 0.0968186, 0.104425, 0.113088, 0.124504, 0.139739, 0.165226]

#Region 1 ( 60 < p < 70 ):
tanhCumulHistoNonMuonR1_7 = [0, 3.20061e-05, 7.4681e-05, 0.000213374, 0.000298724, 0.000448086, 0.000629454, 0.000842828, 0.00109888, 0.00133359, 0.0015683, 0.0019097, 0.00226177, 0.00246447, 0.00280587, 0.0033073, 0.00356335, 0.00393676, 0.00440618, 0.00480092, 0.00533436, 0.00580378, 0.00637989, 0.00706269, 0.00751078, 0.00805488, 0.00867367, 0.00950583, 0.0103273, 0.0112022, 0.0119596, 0.0131865, 0.0140827, 0.0150642, 0.0161631, 0.0173473, 0.0186809, 0.0200892, 0.0221162, 0.0237806, 0.0259677, 0.0278987, 0.0304912, 0.0331797, 0.0363696, 0.0396983, 0.0445312, 0.0505484, 0.0586673, 0.0727926]

#Region 2 ( 3 < p < 10 ):
tanhCumulHistoNonMuonR2_1 = [0, 0.0268357, 0.0643925, 0.10224, 0.138991, 0.173594, 0.206537, 0.237671, 0.270208, 0.299442, 0.327381, 0.354673, 0.380065, 0.404254, 0.427687, 0.449735, 0.471227, 0.491573, 0.511107, 0.529811, 0.547328, 0.564123, 0.580619, 0.595589, 0.610052, 0.623843, 0.637178, 0.650322, 0.662413, 0.674362, 0.685531, 0.696161, 0.706691, 0.716773, 0.726598, 0.73609, 0.74565, 0.755043, 0.763806, 0.772759, 0.781671, 0.790459, 0.798989, 0.807719, 0.816822, 0.826323, 0.836363, 0.847707, 0.860494, 0.878708]

#Region 2 ( 10 < p < 20 ):
tanhCumulHistoNonMuonR2_2 = [0, 0.00131938, 0.00427347, 0.00844736, 0.0135008, 0.0194754, 0.0258234, 0.0320718, 0.0383782, 0.0444275, 0.0510078, 0.0570653, 0.063712, 0.0703421, 0.0772212, 0.0837517, 0.0902407, 0.0972193, 0.104082, 0.111077, 0.117923, 0.124736, 0.132046, 0.139498, 0.146875, 0.154401, 0.161778, 0.169304, 0.176963, 0.185062, 0.192953, 0.201567, 0.209997, 0.218279, 0.227373, 0.236991, 0.247529, 0.258773, 0.27005, 0.281252, 0.293915, 0.306619, 0.320751, 0.336625, 0.353818, 0.373186, 0.395034, 0.421912, 0.456597, 0.510036]

#Region 2 ( 20 < p < 30 ):
tanhCumulHistoNonMuonR2_3 = [0, 0.000182556, 0.00074682, 0.00153513, 0.00254749, 0.00364282, 0.0049788, 0.00640605, 0.00807395, 0.00983313, 0.0115923, 0.0136585, 0.0160151, 0.0181062, 0.0207118, 0.0231597, 0.025508, 0.028097, 0.0304454, 0.0332667, 0.0358059, 0.0384695, 0.0410502, 0.0436807, 0.046502, 0.049622, 0.0527919, 0.0555634, 0.0586585, 0.0619197, 0.0653965, 0.0690393, 0.0727734, 0.0767399, 0.0807893, 0.0847557, 0.0890873, 0.0937176, 0.0987711, 0.104347, 0.110222, 0.11623, 0.122835, 0.130727, 0.139581, 0.149704, 0.161496, 0.176084, 0.196563, 0.229448]

#Region 2 ( 30 < p < 40 ):
tanhCumulHistoNonMuonR2_4 = [0, 6.6384e-05, 0.00024894, 0.000589158, 0.000962568, 0.00141066, 0.00185045, 0.00257238, 0.00313664, 0.00390006, 0.00462198, 0.00545178, 0.00633967, 0.00726075, 0.00833119, 0.00934355, 0.0104306, 0.011584, 0.0128038, 0.014314, 0.015733, 0.0170773, 0.0186041, 0.0203052, 0.0219648, 0.0236244, 0.0252757, 0.0271842, 0.028736, 0.0306113, 0.0327771, 0.0348433, 0.0368182, 0.0390006, 0.0412079, 0.043523, 0.0461369, 0.0489499, 0.0519704, 0.0548664, 0.0581441, 0.0618284, 0.0659608, 0.0707404, 0.0760843, 0.0822415, 0.0893943, 0.0987296, 0.111973, 0.134054]

#Region 2 ( 40 < p < 50 ):
tanhCumulHistoNonMuonR2_5 = [0, 1.6596e-05, 6.6384e-05, 9.1278e-05, 0.000199152, 0.000340218, 0.000506178, 0.000647244, 0.00087129, 0.00102065, 0.0012364, 0.00147704, 0.00171769, 0.00190854, 0.00219067, 0.00259727, 0.00283792, 0.00316154, 0.00355154, 0.00394155, 0.00428177, 0.00471326, 0.00521114, 0.00559285, 0.00609073, 0.0066467, 0.00716947, 0.00775033, 0.00839757, 0.00906971, 0.00980823, 0.0104472, 0.0111857, 0.0120487, 0.0130196, 0.0141149, 0.0153513, 0.0164217, 0.0175835, 0.0190688, 0.0207533, 0.0223963, 0.0242467, 0.026454, 0.0287028, 0.0315905, 0.0350756, 0.0394238, 0.0454232, 0.0559534]

#Region 3 ( 3 < p < 6 ):
tanhCumulHistoNonMuonR3_1 = [0, 0.0458268, 0.104545, 0.16541, 0.224144, 0.279795, 0.330441, 0.377383, 0.42194, 0.462723, 0.501081, 0.535046, 0.566974, 0.595167, 0.620873, 0.644782, 0.666003, 0.685573, 0.703718, 0.720003, 0.735243, 0.748693, 0.760577, 0.771881, 0.78217, 0.791591, 0.800779, 0.808775, 0.816112, 0.82324, 0.830011, 0.836317, 0.842275, 0.84838, 0.854013, 0.859173, 0.86434, 0.869268, 0.874187, 0.878999, 0.884197, 0.889202, 0.894114, 0.899158, 0.904449, 0.909563, 0.915668, 0.922238, 0.929706, 0.939453]

#Region 3 ( 6 < p < 10 ):
tanhCumulHistoNonMuonR3_2 = [0, 0.00229328, 0.00774755, 0.0153247, 0.02308, 0.0317572, 0.0405584, 0.0495843, 0.0584475, 0.0672255, 0.0758253, 0.0855872, 0.0948223, 0.104669, 0.114695, 0.124906, 0.135102, 0.145406, 0.155703, 0.165922, 0.176822, 0.187994, 0.198392, 0.209416, 0.220364, 0.231334, 0.242514, 0.254073, 0.265687, 0.278432, 0.290425, 0.302728, 0.315139, 0.327512, 0.340962, 0.354288, 0.367862, 0.38245, 0.397666, 0.413789, 0.430888, 0.448328, 0.467123, 0.487368, 0.508976, 0.532691, 0.559954, 0.590898, 0.628536, 0.681459]

#Region 3 ( 10 < p < 20 ):
tanhCumulHistoNonMuonR3_3 = [0, 0.000240174, 0.000945202, 0.00192914, 0.00330046, 0.00493519, 0.00672488, 0.00867726, 0.0110015, 0.0133103, 0.015681, 0.0181448, 0.0204845, 0.0231574, 0.0259233, 0.0286892, 0.0317185, 0.0343294, 0.0374904, 0.0406437, 0.043487, 0.0469889, 0.0503591, 0.0534581, 0.0569368, 0.0603689, 0.0637934, 0.0675354, 0.0711303, 0.0746554, 0.0791723, 0.0833714, 0.0879657, 0.0925368, 0.0972395, 0.10281, 0.108481, 0.114501, 0.121133, 0.127951, 0.135288, 0.143593, 0.152402, 0.162048, 0.173584, 0.186964, 0.202079, 0.22189, 0.248596, 0.2934]

#Region 3 ( 20 < p < 30 ):
tanhCumulHistoNonMuonR3_4 = [0, 2.32427e-05, 0.000100718, 0.00031765, 0.000557824, 0.000898716, 0.00123186, 0.00163473, 0.00204535, 0.00257219, 0.00315325, 0.00387378, 0.00456331, 0.00520636, 0.00598886, 0.00669389, 0.00747639, 0.00837511, 0.00918085, 0.0101725, 0.0113967, 0.0123496, 0.0134033, 0.0146506, 0.015743, 0.0169129, 0.0181835, 0.0193379, 0.0206085, 0.0221812, 0.0235836, 0.0252028, 0.0267523, 0.0284955, 0.0304634, 0.0324003, 0.0341357, 0.036429, 0.0389315, 0.0414107, 0.0439441, 0.0472368, 0.0506303, 0.0542019, 0.0585947, 0.0634137, 0.0693716, 0.0767163, 0.0881439, 0.10759]

#Region 3 ( 30 < p < 40 ):
tanhCumulHistoNonMuonR3_5 = [0, 1.54951e-05, 2.32427e-05, 6.19804e-05, 0.000116213, 0.000247922, 0.000333145, 0.000457106, 0.000635299, 0.000821241, 0.000983939, 0.00116988, 0.00136357, 0.00158825, 0.00187491, 0.00209184, 0.00235526, 0.00268065, 0.0030138, 0.00340892, 0.00380405, 0.00420692, 0.00465628, 0.00501267, 0.0055395, 0.00599661, 0.00649245, 0.00698829, 0.00749963, 0.00805746, 0.008716, 0.00932031, 0.0101183, 0.011017, 0.011877, 0.012644, 0.013566, 0.0147204, 0.0157585, 0.0169129, 0.0182068, 0.0195161, 0.0211198, 0.0229792, 0.0251098, 0.027589, 0.030905, 0.0345541, 0.0404965, 0.0504366]

#Region 4 ( 3 < p < 6 ):
tanhCumulHistoNonMuonR4_1 = [0, 0.0131964, 0.0365754, 0.0632445, 0.0920629, 0.11932, 0.146818, 0.173943, 0.202281, 0.229923, 0.256868, 0.282853, 0.309918, 0.336551, 0.36208, 0.386263, 0.409594, 0.433946, 0.456976, 0.479359, 0.501645, 0.522791, 0.543816, 0.564793, 0.584438, 0.603314, 0.622418, 0.640874, 0.659498, 0.676501, 0.692831, 0.709006, 0.724988, 0.740706, 0.754995, 0.769368, 0.782637, 0.795917, 0.808405, 0.819933, 0.831472, 0.842387, 0.852738, 0.863305, 0.873079, 0.883369, 0.89378, 0.904227, 0.916246, 0.930439]

#Region 4 ( 6 < p < 8 ):
tanhCumulHistoNonMuonR4_2 = [0, 0.000912584, 0.00318204, 0.00606388, 0.00959414, 0.0134726, 0.0180596, 0.0229827, 0.0274976, 0.0325768, 0.0378842, 0.043708, 0.0494356, 0.0548991, 0.0606148, 0.0667988, 0.0731988, 0.079623, 0.0863713, 0.0929275, 0.0990874, 0.105992, 0.112752, 0.119657, 0.126837, 0.13409, 0.141535, 0.149159, 0.15682, 0.164661, 0.172983, 0.181736, 0.19073, 0.200817, 0.210855, 0.221302, 0.232145, 0.243624, 0.255488, 0.267783, 0.281316, 0.29647, 0.312716, 0.33122, 0.351141, 0.373571, 0.399171, 0.432457, 0.475192, 0.54109]

#Region 4 ( 8 < p < 10 ):
tanhCumulHistoNonMuonR4_3 = [0, 0.000348223, 0.00122478, 0.00256964, 0.00397454, 0.00588377, 0.007805, 0.0101465, 0.0124159, 0.0147574, 0.0177594, 0.0205572, 0.0237032, 0.0263329, 0.029683, 0.0325768, 0.035987, 0.0395293, 0.0429875, 0.046854, 0.0506004, 0.0540946, 0.0579131, 0.0618636, 0.0662944, 0.0705692, 0.0752642, 0.0799472, 0.0849544, 0.0899976, 0.0949568, 0.0997598, 0.105427, 0.111071, 0.116727, 0.123463, 0.129935, 0.13702, 0.144068, 0.151801, 0.160507, 0.169488, 0.180043, 0.191595, 0.205488, 0.221134, 0.239673, 0.2625, 0.294296, 0.348007]

#Region 4 ( 10 < p < 15 ):
tanhCumulHistoNonMuonR4_4 = [0, 9.60615e-05, 0.000432277, 0.000876561, 0.00153698, 0.00235351, 0.00317003, 0.0039025, 0.00492315, 0.00606388, 0.00720461, 0.00840538, 0.00977426, 0.0111431, 0.012476, 0.0139169, 0.0153218, 0.017219, 0.0190082, 0.0207733, 0.0227426, 0.0246638, 0.0262968, 0.0284582, 0.0305716, 0.0325768, 0.0350384, 0.0372358, 0.0400216, 0.0425432, 0.045377, 0.0481028, 0.0510807, 0.0539625, 0.0570245, 0.0604947, 0.0643252, 0.0682757, 0.0727786, 0.0775216, 0.0825288, 0.0883766, 0.0943084, 0.100456, 0.10873, 0.116943, 0.12817, 0.141739, 0.160831, 0.193828]

#Region 4 ( 15 < p < 20 ):
tanhCumulHistoNonMuonR4_5 = [0, 1.20077e-05, 6.00384e-05, 0.000216138, 0.000396254, 0.000576369, 0.000792507, 0.00105668, 0.00135687, 0.0018732, 0.00216138, 0.00250961, 0.00291787, 0.00333814, 0.00371037, 0.00414265, 0.00459894, 0.00522334, 0.00583573, 0.00647214, 0.00698847, 0.00756484, 0.00826129, 0.00901777, 0.00982229, 0.0106508, 0.0114553, 0.0121518, 0.0131004, 0.014025, 0.0149135, 0.0160423, 0.0174472, 0.018684, 0.0198967, 0.0214217, 0.0230548, 0.0245797, 0.0260207, 0.0277498, 0.0300552, 0.0321926, 0.0347502, 0.0375841, 0.0411383, 0.045329, 0.0502041, 0.0560279, 0.0652378, 0.0816763]
