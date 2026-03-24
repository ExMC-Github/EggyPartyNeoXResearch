#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define MAX_LINE_LENGTH 4096

// DLL函数指针类型定义
typedef void (*PyInitFunc)(void);
typedef void (*PyRunStringFunc)(const char*);
typedef void (*PyFinalizeFunc)(void);
typedef void (*PySetPythonHomeFunc)(const wchar_t*);
typedef void (*Py_SetPathFunc)(const wchar_t*);
typedef void (*PySys_SetArgvFunc)(int, wchar_t**);

int main(int argc, char** argv) {
    HINSTANCE hPythonDLL;
    PyInitFunc Py_Initialize = NULL;
    PyRunStringFunc PyRun_SimpleString = NULL;
    PyFinalizeFunc Py_Finalize = NULL;
    PySetPythonHomeFunc Py_SetPythonHome = NULL;
    Py_SetPathFunc Py_SetPath = NULL;
    PySys_SetArgvFunc PySys_SetArgv = NULL;
    
    // 加载 neox_engine.dll
    hPythonDLL = LoadLibrary("neox_engine.dll");
    if (!hPythonDLL) {
        printf("Error: Cannot load neox_engine.dll (error code: %lu)\n", GetLastError());
        return 1;
    }
    
    // 获取需要的函数地址
    Py_Initialize = (PyInitFunc)GetProcAddress(hPythonDLL, "Py_Initialize");
    PyRun_SimpleString = (PyRunStringFunc)GetProcAddress(hPythonDLL, "PyRun_SimpleString");
    Py_Finalize = (PyFinalizeFunc)GetProcAddress(hPythonDLL, "Py_Finalize");
    Py_SetPythonHome = (PySetPythonHomeFunc)GetProcAddress(hPythonDLL, "Py_SetPythonHome");
    Py_SetPath = (Py_SetPathFunc)GetProcAddress(hPythonDLL, "Py_SetPath");
    PySys_SetArgv = (PySys_SetArgvFunc)GetProcAddress(hPythonDLL, "PySys_SetArgv");
    
    if (!Py_Initialize || !PyRun_SimpleString || !Py_Finalize) {
        printf("Error: Missing required functions (Py_Initialize, PyRun_SimpleString, Py_Finalize)\n");
        FreeLibrary(hPythonDLL);
        return 1;
    }
    
    // 尝试禁用 site 模块（防止因版本不匹配导致导入失败）
    // 方法1：直接设置 Py_NoSiteFlag 变量（如果导出）
    int *Py_NoSiteFlag_ptr = (int*)GetProcAddress(hPythonDLL, "Py_NoSiteFlag");
    if (Py_NoSiteFlag_ptr) {
        *Py_NoSiteFlag_ptr = 1;  // 禁用 site 模块
        // printf("Disabled site module via Py_NoSiteFlag\n");
    } else if (PySys_SetArgv) {
        // 方法2：通过 PySys_SetArgv 模拟命令行参数 -S
        wchar_t *argv_w[] = { L"", L"-S" };
        PySys_SetArgv(2, argv_w);
        // printf("Disabled site module via -S argument\n");
    } else {
        printf("Warning: Could not disable site module; it may cause errors.\n");
    }
    
    // 设置 Python 的 home 目录（以便找到标准库）
    if (Py_SetPythonHome) {
        Py_SetPythonHome(L".");
    } else {
        // 通过环境变量设置 PYTHONHOME
        wchar_t cwd[MAX_PATH];
        if (GetCurrentDirectoryW(MAX_PATH, cwd)) {
            wchar_t env[2 * MAX_PATH];
            swprintf(env, 2 * MAX_PATH, L"PYTHONHOME=%s", cwd);
            _wputenv(env);
        } else {
            _wputenv(L"PYTHONHOME=.");
        }
    }
    
    // 如果 Py_SetPath 可用，也可以直接设置搜索路径
    if (Py_SetPath) {
        Py_SetPath(L".;./Lib");
    }
    
    // 初始化 Python 解释器
    Py_Initialize();
    
    // 交互模式或执行脚本
    if (argc == 1) {
        // 交互模式：先执行初始化代码，将当前目录和 ./Lib 加入 sys.path
        const char* setup_code = 
            "import sys\n"
            "sys.path.insert(0, '.')\n"
            "sys.path.insert(0, './Lib')\n"
            "# 获取并显示版本信息\n"
            "try:\n"
            "    vi = sys.version\n"
            "    version = vi\n"
            "    print('Python %s on win32' % version)\n"
            "except:\n"
            "    print('Python '+sys.version+' on win32')\n";
        PyRun_SimpleString(setup_code);
        
        char line[MAX_LINE_LENGTH];
        while (1) {
            printf(">>> ");
            if (!fgets(line, sizeof(line), stdin)) {
                printf("\n");
                break;
            }
            // 移除换行符
            line[strcspn(line, "\n")] = 0;
            if (strlen(line) == 0) continue;
            if (strcmp(line, "exit()") == 0 || strcmp(line, "quit()") == 0) break;
            PyRun_SimpleString(line);
        }
    } else if (argc == 2) {
        // 执行脚本文件
        FILE* fp = fopen(argv[1], "rb");
        if (!fp) {
            printf("Error: Cannot open file %s\n", argv[1]);
        } else {
            fseek(fp, 0, SEEK_END);
            long file_len = ftell(fp);
            char* buf = (char*)malloc(file_len + 1);
            if (buf) {
                fseek(fp, 0, SEEK_SET);
                fread(buf, 1, file_len, fp);
                buf[file_len] = 0;
                PyRun_SimpleString(buf);
                free(buf);
            }
            fclose(fp);
        }
    }
    
    // 清理
    Py_Finalize();
    FreeLibrary(hPythonDLL);
    return 0;
}