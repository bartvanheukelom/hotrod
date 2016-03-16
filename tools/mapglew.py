#!/usr/bin/python

# Parses a glew.h file and writes the constants and functions declared in it to a JSON file.
# Original from https://github.com/philogb/v8-gl TODO proper attribution and license copy
# Modified for modern OpenGL by Bart van Heukelom TODO which version is that?

import re
import json

PATH_GL = '/usr/include/GL/glew.h'
FILE_JSON = 'glew_h.json'

#constant and function patterns
constant_pattern = """
    .+define[\s]+       #define C/C++ keyword
    (?P<name>GL_[^\s]+) #Constant name
    [\s]+
    (?P<value>[^\s]+)   #Constant value
    [\s]*
"""
function_pattern = """
    (typedef|GLAPI)[\\s]+
    (?P<return_type>[A-Za-z\\*]+)  #Function return type
    [\\s]+\\(?GLAPIENTRY(\\s\\*)?\\s
    (?P<bigname>[A-Za-z0-1]+) #Function name
    \\)?\\s\\(
    (?P<parameters>.*)  #Function parameters
    \\);
"""
bigname_pattern = "GLEW_FUN_EXPORT\\s(?P<bigname>[A-Z0-1]+)\\s(?P<smallname>.+);"
#precompile regexps
constant = re.compile(constant_pattern, re.VERBOSE)
function = re.compile(function_pattern, re.VERBOSE)
bigname = re.compile(bigname_pattern, re.VERBOSE)

def main():
    json_c, json_f = [], []
    constants = []
    constantvals = {}
    bignames = {}
    #open input file
    with open(PATH_GL, 'r') as fin:
        #line accumulator. Function definitions
        #can be spread into multiple lines
        l = ''
        #get function/constant prototype
        for cont in fin:
            l += cont.replace('\n', '')

            #is constant declaration
            mat = re.match(constant, l)
            if mat and not mat.group('name') in constants:
                print("match CONSTANT " + l)
                constants.append(mat.group('name'))
                constantvals[mat.group('name')] = mat.group('value')
                json_c.append({
                    'name': mat.group('name'),
                    'value': mat.group('value')
                })
            else:
                #is function declaration
                mat = re.match(function, l)
                if mat:
                    print("match FUNCTION " + l)
                    fj = {
                        'bigname': mat.group('bigname'),
                        'return_type': mat.group('return_type'),
                        'parameters': get_parameters(mat.group('parameters'))
                    }
                    fj['always'] = (not mat.group('bigname').startswith('PFNGL'))
                    if (fj['always']):
                        bignames[mat.group('bigname')] = mat.group('bigname')
                    json_f.append(fj)
                else:
                    mat = re.match(bigname, l)
                    if mat:
                        print("match BIGNAME " + l)
                        bignames[mat.group('bigname')] = mat.group('smallname') \
                            .replace("__glew", "gl")


            l = '' #empty line accumulator

    for f in json_f:
        f['name'] = bignames[f['bigname']]

    for c in json_c:
        while (not is_int_string(c['value'])):
            c['value'] = constantvals[c['value']]

    #dump as JSON
    with open(FILE_JSON, 'w') as fout:
        fout.write(json.dumps({
            'constants': json_c,
            'functions': json_f
        }, indent=4))

def is_int_string(s):
    if (s.startswith('0x')): return True
    try:
        int(s)
        return True
    except ValueError:
        return False

par_pattern = """
    (?P<type>
        const\\s?
        [A-Za-z0-9_]+
        \\s|\\*|\\[\\]*
    )
    (?P<name>
        [A-Za-z0-9_]+
    )
    (?P<aftername>\\[\\])?
    ;
"""
parp = re.compile(par_pattern, re.VERBOSE)

def get_parameters(params):
    """Returns an ordered list of parameter types"""

    if (params.strip() == 'void'): return []

    params_list = []
    for par in params.split(','):

        full = par.strip()

        # cheats
        if (full == 'const GLvoid*binary'): type = 'const GLvoid*'
        elif (full == 'GLvoid*binary'): type = 'GLvoid*'
        elif (full == 'GLvoid*column'): type = 'GLvoid*'
        elif (full == 'GLvoid*span'): type = 'GLvoid*'
        elif (full == 'const GLvoid*coords'): type = 'const GLvoid*'
        elif (full == 'const GLvoid*charcodes'): type = 'const GLvoid*'
        elif (full == 'const GLvoid*getProcAddress'): type = 'const GLvoid*'
        else:
            #mat = re.match(parp, full)
            #if (mat):
            #    print("matasdas dasd asd     "  + full)

            parparts = full.split(' ')
            if (len(parparts) == 1 or (len(parparts) == 2 and parparts[0] == 'const')):
                type = full
            else:
                partype = parparts[:-1]
                parname = parparts[-1]
                type = ' '.join(partype) \
                    + ('*' * (parname.count('*') + parname.count('[')))

        params_list.append({'full': full, 'type': type})

    return params_list


def balanced(l):
    return l.count('(') == l.count(')')

if __name__ == '__main__': main()
