#!/usr/bin/python

# Parses a gl.h file and writes the constants and functions declared in it to a JSON file.
# Original from https://github.com/philogb/v8-gl TODO proper attribution and license copy
# Modified for modern OpenGL by Bart van Heukelom TODO which version is that?

import re
import json

PATH_GL = '/usr/include/GL/gl.h'
FILE_JSON = 'gl_h.json'

#constant and function patterns
constant_pattern = """
    .+define[\s]+       #define C/C++ keyword
    (?P<name>GL_[^\s]+) #Constant name
    [\s]+
    (?P<value>[^\s]+)   #Constant value
    [\s]*
"""
function_pattern = """
    GLAPI[\s]+
    (?P<return_type>[^\s]+)  #Function return type
    [\s]+GLAPIENTRY[\s]+
    (?P<name>gl[A-Za-z0-9]+) #Function name
    [\s]*
    \((?P<parameters>.*)\);  #Function parameters
"""
#precompile regexps
constant = re.compile(constant_pattern, re.VERBOSE)
function = re.compile(function_pattern, re.VERBOSE)

def main():
    json_c, json_f = [], []
    constants = []
    #open input file
    with open(PATH_GL, 'r') as fin:
        #line accumulator. Function definitions
        #can be spread into multiple lines
        l = ''
        #get function/constant prototype
        for cont in fin:
           l += cont.replace('\n', '')
           if not balanced(l):
              continue
           #is constant declaration
           mat = re.match(constant, l)
           if mat and not mat.group('name') in constants:
               print("match CONSTANT " + l)
               constants.append(mat.group('name'))
               json_c.append({
                   'name': mat.group('name'),
                   'value': mat.group('value')
               })
           else:
               #is function declaration
               mat = re.match(function, l)
               if mat:
                   print("match FUNCTION " + l)
                   json_f.append({
                       'name': mat.group('name'),
                       'return_type': mat.group('return_type'),
                       'parameters': get_parameters(mat.group('parameters'))
                   })

           l = '' #empty line accumulator

    #dump as JSON
    with open(FILE_JSON, 'w') as fout:
        fout.write(json.dumps({
            'constants': json_c,
            'functions': json_f
        }, indent=4))

def get_parameters(params):
    """Returns an ordered list of parameter types"""

    params_list = []
    params_aux = params.split(',')
    passed = False
    for par in params_aux:
        if passed and not balanced(params_list[-1]):
            params_list[-1] += ',' + par
        else:
            #magic
            param = ' '.join(par.strip().split(' ')[:-1]) + ('*' * (par.count('*') + par.count('[')))
            if param.strip() != '': params_list.append(param)
        passed = True

    return params_list


def balanced(l):
    return l.count('(') == l.count(')')

if __name__ == '__main__': main()
