# EggyPartyNeoXResearch
针对蛋仔派对的网易NeoX引擎的研究

## 参考
[NeoxResearch](https://github.com/ZhangFengze/NeoxResearch)

[NeteaseUnpackTools](https://github.com/yuanbi/NeteaseUnpackTools)

[pymarshal.py](https://gist.github.com/fate0/3e1d23bce9d4d2cfa93848dd92aba3d4)

fate0的博客 阴阳师：一个非酋的逆向旅程 (自行搜索)

## 逆向思路
网上已经有一些前人们的工作可参考，但更新大多数停留在几年前，不能直接拿来用

由于逆向的是蛋仔派对，而蛋仔又有针对电脑的构建([Eggitor](https://adl.netease.com/d/g/party/c/mgc))，可以看PPK文件

安装之后，里面可以看到很多没有后缀名的文件，可以在PPK里面找可执行文件（MZ开头的）

找到后，一般为DLL，而蛋仔老版本的[neox.xml](https://github.com/ExMC-Github/EggyPartyNeoXResearch/blob/main/neox.xml)里面又写了调用了Python

所以这个可执行文件是这两个中的其中一个，我们要找的是Python.dll

找到之后，由于CPython是给C语言用的，导出函数的方式可能不一样，得用python寻找

直接看仓库目录下的那个find_functions.py文件，先下载下来然后修改一下DLL的目录（我不小心写成绝对路径了），直接运行

就可以看到很多导出的函数，可以看到Py_开头的字样，就说明找的就是Python.dll

然后再version = Py_GetVersion(); print("Python version:", version.decode('utf-8'))获得Python版本后，得到Python版本为2.7.3，再去跟计算opcode跟2.7.3的opcode做对比

#### 修复pyc
根据前人们的工作，得知NeoX修改了CPython的opcode

直接用Windows找到的NeoXPython运行一段py文件，得到用NeoXPython生成的pyc后

再用正常的python2.7.3生成一次，删除 division 的注释后再运行一次，得到四个pyc文件

再用compare_opcode.py对文件进行分析，得出opcode映射代码

使用pyc_decryptor.py修复opcode就可以了

#### 从Pyc文件得到Py源码
uncompyle6可以反编译pyc文件

由于NeoXPython是2.7,整个流程基本都是用2.7做的，uncompyle6在2.7下处理unicode docstring有问题

所以需要切换到Python3，切换要特殊处理一下
