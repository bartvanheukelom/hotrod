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
constant = re.compile(constant_pattern, re.VERBOSE)

function_pattern = """
    typedef\\s+
    (?P<return_type>.+)
    \\s*\\(GLAPIENTRY\\s+\\*\\s+
    (?P<bigname>PFNGL[A-Za-z0-9]+PROC)
    \\)\\s*\\(
    (?P<parameters>.*)
    \\);
"""
function = re.compile(function_pattern, re.VERBOSE)

functiongl1_pattern = """
    GLAPI\\s+
    (?P<return_type>.+)
    \\s+GLAPIENTRY\\s+
    (?P<name>gl[A-Za-z0-9]+)
    \\s+\\(
    (?P<parameters>.*)
    \\);
"""
functiongl1 = re.compile(functiongl1_pattern, re.VERBOSE)

bigname_pattern = "GLEW_FUN_EXPORT\\s+(?P<bigname>PFNGL[A-Z0-9]+PROC)\\s+(?P<smallname>__glew.+);"
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

            if (l.startswith('/* --')): print(l)
            #if (l.count('GLAPI') != 0): print(l)

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
                        'always': False,
                        'bigname': mat.group('bigname').strip(),
                        'return_type': mat.group('return_type').strip(),
                        'parameters': get_parameters(mat.group('parameters'))
                    }
                    json_f.append(fj)
                else:
                    mat = re.match(functiongl1, l)
                    if mat:
                        print("match FUNCTIONGL1 " + l)
                        fj = {
                            'always': True,
                            'name': mat.group('name').strip(),
                            'return_type': mat.group('return_type').strip(),
                            'parameters': get_parameters(mat.group('parameters'))
                        }
                        json_f.append(fj)
                    else:
                        mat = re.match(bigname, l)
                        if mat:
                            print("match BIGNAME " + l)
                            bignames[mat.group('bigname')] = mat.group('smallname') \
                                .replace("__glew", "gl")


            l = '' #empty line accumulator

    # resolve bignames to smallnames
    for f in json_f:
        if 'name' in f: continue
        f['name'] = bignames.pop(f['bigname'])
        del f['bigname']

    if len(bignames) != 0:
        print("Unused names")
        print(bignames)

    # resolve constants to literal values
    for c in json_c:
        while (not is_int_string(c['value'])):
            c['value'] = constantvals[c['value']]

    # output as JSON
    with open(FILE_JSON, 'w') as fout:
        fout.write(json.dumps({
            'constants': json_c,
            'functions': json_f
        }, indent=4))

def is_int_string(s):
    "Determine whether the given string represents a literal number."
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





if __name__ == '__main__': main()
