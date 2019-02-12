
#-------------------------------------------------------------------
# argument parser for single valued parameters or list of parameters
#
#-------------------------------------------------------------------

class argument_parser:
#_____________________________________________________________________________
    def __init__(self):
        #dictionaries for parameter values to be loaded by parse function
        self.inp_dict = {}
        self.out_dict = {}

#_____________________________________________________________________________
    def add_parameter(self, name, type=str):
        self.inp_dict[name] = type
        if type == list:
            self.out_dict[name] = []

#_____________________________________________________________________________
    def set_default(self, name, val):
        #default value for a parameter
        self.out_dict[name] = val

#_____________________________________________________________________________
    def parse(self, config):
        #loop over configuration file
        for line in open(config):
            line = line.lstrip()
            line = line.strip("\n")

            #skip comments and empty lines
            if line == "" or line[0] == "#": continue

            lpar = line.split()

            #name of the parameter
            arg = lpar.pop(0)

            #load value for the parameter
            if arg in self.inp_dict:
                if self.inp_dict.get(arg) == list:
                    #read into output list until reaching a comment
                    vlist = []
                    while len(lpar) > 0:
                        val = lpar.pop(0)
                        if val[0] == "#": break
                        vlist.append(val)
                    self.out_dict.get(arg).append(vlist)
                else:
                    #read single-valued parameter
                    self.out_dict[arg] = self.inp_dict.get(arg)( lpar.pop(0) )

#_____________________________________________________________________________
    def get(self, name):

        #retrieve parameter value from output dictionary

        return self.out_dict.get(name)












