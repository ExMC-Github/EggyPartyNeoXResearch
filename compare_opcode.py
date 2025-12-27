import sys

import marshal

opmap = {}

def compare(cobj1, cobj2):

    codestr1 = bytearray(cobj1.co_code)

    codestr2 = bytearray(cobj2.co_code)

    if len(codestr1) != len(codestr2):

        print("two cobj has different length, skipping")

        # return

    i = 0

    while i < len(codestr1):

        if codestr1[i] not in opmap:

            opmap[codestr1[i]] = codestr2[i]

        else:

            if opmap[codestr1[i]] != codestr2[i]:

                print("error: has wrong opcode")

                break

        if codestr1[i] < 90 and codestr2[i] < 90:

            i += 1

        elif codestr1[i] >= 90 and codestr2[i] >= 90:

            i += 3

        else:

            print("wrong opcode")

    for const1, const2 in zip(cobj1.co_consts, cobj2.co_consts):

        if hasattr(const1, 'co_code') and hasattr(const2, 'co_code'):

            compare(const1, const2)

def usage():

    print("Usage: %s filename1.pyc filename2.pyc")

def main():

    if len(sys.argv) != 3:

        usage()

        return

    with open(sys.argv[1], 'rb') as f:
        data1 = f.read()
    
    with open(sys.argv[2], 'rb') as f:
        data2 = f.read()
    
    cobj1 = marshal.loads(data1)
    cobj2 = marshal.loads(data2[8:])
    
    compare(cobj1, cobj2)
    print(opmap)

if __name__ == '__main__':

    main()
