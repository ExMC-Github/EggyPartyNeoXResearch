import marshal

infile = 'py27opcode.py'

outfile = 'windows_py27opcode.pyc'

content = open(infile).read()

out_fd = open(outfile, 'wb')

cobj = compile(content, '', 'exec')

marshal.dump(cobj, out_fd)

out_fd.close()
