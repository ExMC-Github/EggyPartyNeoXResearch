import ctypes

neoxpython = ctypes.CDLL('C:\\Users\\ExRFy\\Desktop\\neoxpython.dll')

# 方法1：尝试获取函数指针
try:
    # 尝试访问一些常见的函数名
    func_names = ['version', 'init', 'NeoX', 'neox', 'PyInit', 'main', 'start']
    for name in func_names:
        try:
            func = getattr(neoxpython, name)
            print(f"Found function: {name} at {hex(ctypes.addressof(func))}")
        except AttributeError:
            pass
except Exception as e:
    print(f"Error: {e}")

# 方法2：使用Win32 API枚举导出函数
def list_dll_exports(dll_path):
    import os
    try:
        import pefile
        pe = pefile.PE(dll_path)
        if hasattr(pe, 'DIRECTORY_ENTRY_EXPORT'):
            print(f"\nDLL导出函数 ({len(pe.DIRECTORY_ENTRY_EXPORT.symbols)}个):")
            for exp in pe.DIRECTORY_ENTRY_EXPORT.symbols:
                if exp.name:
                    print(f"  - {exp.name.decode()}")
        else:
            print("这个DLL没有导出函数表")
    except ImportError:
        print("安装pefile库来查看导出函数: pip install pefile")

# 调用函数查看导出
list_dll_exports('C:\\Users\\ExRFy\\Desktop\\neoxpython.dll')
