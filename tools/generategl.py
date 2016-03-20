#!/usr/bin/python

import json
import re

#input output files
IN_FILE = 'glew_h.json'
OUT_FILE = '../src/gl.cpp.incl'

#map some OpenGL types to V8 types
unsigned = re.compile('unsigned|ubyte|ushort|uint|bitfield|boolean')
integer = re.compile('int|enum|sizei|short|byte')
double = re.compile('double|float|clampf|clampd')

EXCLUDE = re.compile('ATI|MESA')

def main():
    """Generates Gl bindings"""

    with open(IN_FILE, 'r') as f:
        data = f.read()
        json_in = json.loads(data)

    with open(OUT_FILE, 'w') as fout:

        w = lambda s: fout.write(s)
        wel = lambda: fout.write("\n")
        wl = lambda s: fout.write(s + "\n")

        wl("// generated by generategl.py - DON'T EDIT")
        wel()
        wel()
        wel()

        funcNames = []

        for func in json_in['functions']:

            print(func)
            name = func['name']
            params = func['parameters']

            if (func['return_type'] != "void"):
                print("SKIP " + name + " not void but " + func['return_type'])
                wl("// SKIP " + name + " because it returns " + func['return_type'])
                wel()
                continue

            if (re.search(EXCLUDE, name)):
                print("SKIP " + name)
                wl("// SKIP " + name)
                wel()
                continue

            for p in params:
                skip = False
                t = p['type']
                if (t.count('(') != 0 or t == 'GLLOGPROCREGAL' or t == 'GLDEBUGPROCAMD' or t == 'GLDEBUGPROCARB' or t == 'GLDEBUGPROC'):
                    print("SKIP " + name + " because it appears to have a function* param")
                    wl("// SKIP " + name)
                    wel()
                    skip = True
                    break
            if (skip): continue

            funcNames.append(func)

            if not func['always']:
                wl("#ifdef " + name)
            wl("FUNCTION_SIGNATURE(" + name + ") {")
            first = True
            wl("\tFUNCTION_BODY_START(" + str(len(params)) + ")")
            wl("\t" + name + "(")
            for i, p in enumerate(params):
                if (not first):
                    wl(",")
                else:
                    first = False
                #w("\t\tFUNCTION_BODY_ARG(" + str(i) + ", " + type + ")")
                w("\t\tgetArg<" + p['type'] + ">(args[" + str(i) + "]) /* " + p['full'] + " */")
            wel()
            wl("\t);")
            wl("}")
            if not func['always']:
                wl("#endif")
            wel()

        wl("DECLARE_FUNCTIONS_START")
        for fn in funcNames:
            if not fn['always']:
                wl("#ifdef " + fn['name'])
            wl("\tDECLARE_FUNCTION(" + fn['name'] + ")")
            if not fn['always']:
                wl("#endif")
        wl("DECLARE_FUNCTIONS_END")

def multiple_replace(dict, text):
  """ Replace in 'text' all occurences of any key in the given
  dictionary by its corresponding value.  Returns the new string."""

  # Create a regular expression  from the dictionary keys
  regex = re.compile("(%s)" % "|".join(map(re.escape, dict.keys())))

  # For each match, look-up corresponding value in dictionary
  return regex.sub(lambda mo: dict[mo.string[mo.start():mo.end()]], text)

if __name__ == '__main__': main()
