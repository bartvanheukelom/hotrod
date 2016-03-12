#!/usr/bin/python

import json
import re

#input output files
IN_FILE = 'gl_h.json'
OUT_FILE = '../js/glconstants.js'


def main():

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

        for c in json_in['constants']:
            wl("const " + c['name'] + " = " + c['value'] + ";")



if __name__ == '__main__': main()
