# =============================================================================
""" 
@namespace PyTree
@brief TTree derived class with simplified booking and setting methods

See PyTreeExample() for examples on how to use this module.

@author Gabriel Guerrer - Gabriel.Guerrer@cern.ch
@date 2007-06-27
"""
# =============================================================================

from ROOT import TTree, std
from array import array

#---------------------------------------------------
# PyTree
#---------------------------------------------------
class PyTree(TTree):
	""" 
	TTree derived class with simplified booking and setting methods.
	PyTree is stored as TTree in a ROOT file.	
	
    @author Gabriel Guerrer - Gabriel.Guerrer@cern.ch
	"""	

	varnames = []	# internal branch names

	#---------------------------------------------------
	def book(self, label): 			#general label = "varname[size]/type"
		""" 
		Simplified booking. Possible uses:		

		@param label "varname/type"
		@param label "varname[100]/type"
		@param label "varname[othervarname]/type"
		@param label "varname<>/type"
    	
		Where type = I (integer), F (float), C (character - for a string)
		The last example shows the possibility of working with std::vector<type>
		
		Label can be a list of strings as well
		
		An internal class member is created with name varname to hold the var values

		"""	

		# If a list of labels is given as input, call book n times
		if isinstance(label, list):
			for ilabel in label:
				self.book(ilabel)
		else:
	
			# Start parsing
			i0 = label.find("/")
			i1 = label.find("[")
			i2 = label.find("]")
			i3 = label.find("<>") # vector

			# type check
			typ = label[i0+1:]		
			if not (typ == 'I' or typ == 'F' or typ == 'C'):
				print 'PyTree ERROR - var type ' + typ + ' not defined. Please use I, F or C.'		
				return
			typ = typ.lower()	# compatibility between array and TTree:Branch		

			# label = "varname<>/type"
			if i3 > -1:
				varname = label[:i3]
				if len(varname) == 0:
					print 'PyTree ERROR - you must supply a varname.'		
					return						
				if		typ == 'i': typ = 'int'
				elif 	typ == 'f': typ = 'float'			
				var = std.vector(typ)()				
				setattr(self, varname, var) # create internal varname to hold var
				self.Branch(varname, 'vector<'+typ+'>', var)				

			# label = "varname/type"
			elif i1 == -1 and i2 == -1:
				varname = label[:i0]
				if len(varname) == 0:
					print 'PyTree ERROR - you must supply a varname.'		
					return								
				if 	typ == 'c':
					var = array(typ, ' ')
				else:
					var = array(typ, [0])
				setattr(self, varname, var)
				self.Branch(varname, var, label)
				self.setEmpty(varname) # trick, cannot branch an empty array
				
			#label = "varname[100]/type" or "varname[othervarname]/type"
			else:	
				size = label[i1+1:i2]
				varname = label[:i1]
				if len(varname) == 0 or len(size) == 0:
					print 'PyTree ERROR - you must supply varname and size.'		
					return	
				
				var = array(typ, [0])									
				setattr(self, varname, var)
				self.Branch(varname, var, label)
				self.setEmpty(varname) # trick, cannot branch an empty array

			# append to internal branch names
			self.varnames.append(varname)		

	#---------------------------------------------------
	def get(self, varname):
		"""
		Return content of varname
		
		@param varname
		"""
		value = getattr(self, varname) 
		if len(value) == 1:
			value = value[0]
		return value
		
	#---------------------------------------------------
	def set(self, varname, values, opts=''):
		"""
		Set values in varname. An extra fill() call is necessary to write values in the Tree
		For setting arrays and vectors, one can pass python lists
		
		@param varname
		@param values
		@param opts = 'app' appends data to varname
		"""
	
		# check if varname exists
		if not hasattr(self, varname):
			print 'PyTree ERROR - setting an inexistent var ', varname
			return 

		var = getattr(self, varname)				
		# label = "varname<>/type"
		if str(type(var)).find('vector') != -1:		
			# values == int, float			
			if isinstance(values,int) or isinstance(values,float):
				if opts.find('app') == -1:
					self.setEmpty(varname)
				var.push_back(values)
			# values == list, vector			
			else:			
				if opts.find('app') == -1:
					self.setEmpty(varname)
				for value in values:
					var.push_back(value)
		
		# label = "varname[]/type" or "varname/type"
		else:		
			#if leafsize != len(values) and values != 0:
			#	print 'PyTree - ERROR: trying to set ', len(values), ' dim value in ',  varname,  ', leafsize == ',  leafsize	
			
			# should append data?
			#leafsize = self.GetLeaf(varname).GetLen()
			if opts.find('app') == -1: 				
				self.setEmpty(varname)
			if isinstance(values,str) or isinstance(values,list) or isinstance(values,tuple):
				var.extend(values)
			else:
				var.append(values)				
				
			if var.typecode == 'c':
#				if leafsize > len(var):
#					for i in range(leafsize-len(var)-1):
				var.extend('\0')

	#---------------------------------------------------
	def setEmpty(self, varname):
		"""		
		Set varname to empty content [] or ''
		
		@param varname
		"""
		var = getattr(self, varname)
		
		if str(type(var)).find('vector') != -1:
			var.clear()
		else:
			while len(var) > 0:
				var.pop()

	#---------------------------------------------------
	def setEmptyAll(self):
		"""	
		Set all varnames empty
		"""
		for varname in self.varnames:
			self.setEmpty(varname)
		
	#---------------------------------------------------
	def emptyTo0(self):
		"""		
		Should be used before the TTree Fill(). It fills the empty branches that have not been set 
		to 0, to prevent that branch receving the same value of previous event
		"""	
		for varname in self.varnames:
			var = getattr(self, varname)
			
			if len(var) == 0:

				if str(type(var)).find('vector') != -1:
					self.set(varname, [0])

				else: # array
					leafsize = self.GetLeaf(varname).GetLen()
					if var.typecode == 'c':
						self.set(varname, ' ')
					else:
						self.set(varname, leafsize*[0])
						
	#---------------------------------------------------
	def updateBranches(self):
		"""		
		After changing the size of array in emptyTo0 a new address could have been assigned so update it
		"""
		for varname in self.varnames:
			var = getattr(self, varname)
			self.SetBranchAddress(varname, var)				
				
	#---------------------------------------------------
	def fill(self):	
		"""		
		Fills PyTree in a proper way
		Always use it in the end of event loop
		"""
		self.emptyTo0()
		self.updateBranches()
		self.Fill()		
		self.setEmptyAll()


#-----------------------------------------------
# Example
#-----------------------------------------------		
def PyTreeExample():
	"""	
	Example for booking and filling
	"""
	
	t = PyTree('t', '')
	tree_vars = ['n/I', 'a/F', 'x[2]/F', 'y[n]/F', 'z<>/F', 'str/C']

	# book vars
	t.book(tree_vars)

	# loop over your events ...
	for i in range(10):			

		t.set('n', 3+i)
		t.set('a', 3.141592*i)
		t.set('x', [0, 1.+i])
		n = t.get('n')
		t.set('y', n*[1.])
		t.set('str', 'nice string'+str(i))
		
		# for setting the std::vector, one could pass a Python list
		t.set('z', [0., 1., 2., 3., 4., 5.])
		
		# or a std::vector as well
		#z = std.vector(float)()
		#z.push_back(0) # ...
		#t.set('z', z)

		t.fill()
		
	# the t object returned has type TTree, so it can be written to a TFile and
	# inspected using Show(i), Scan(), Print() ...
	return t
