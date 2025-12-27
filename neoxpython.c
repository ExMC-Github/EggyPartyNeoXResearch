#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s script.py\n", argv[0]);
        return 1;
    }

    // 读取Python脚本文件
    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        printf("Error: Cannot open file %s\n", argv[1]);
        return 1;
    }
    
    fseek(fp, 0, SEEK_END);
    long file_len = ftell(fp);
    char *buf = (char *)malloc(file_len + 1);
    if (!buf) {
        printf("Error: Memory allocation failed\n");
        fclose(fp);
        return 1;
    }
    
    fseek(fp, 0, SEEK_SET);
    size_t read_len = fread(buf, 1, file_len, fp);
    buf[read_len] = 0;
    fclose(fp);

    // 加载neoxpython.dll
    HINSTANCE hDLL = LoadLibrary("neoxpython.dll");
    if (!hDLL) {
        printf("Error: Cannot load neoxpython.dll (Error: %lu)\n", GetLastError());
        free(buf);
        return 1;
    }

    // 获取函数指针
    void (*Py_Initialize)(void) = (void(*)(void))GetProcAddress(hDLL, "Py_Initialize");
    void (*PyRun_SimpleString)(const char*) = (void(*)(const char*))GetProcAddress(hDLL, "PyRun_SimpleString");
    void (*Py_Finalize)(void) = (void(*)(void))GetProcAddress(hDLL, "Py_Finalize");

    if (!Py_Initialize || !PyRun_SimpleString || !Py_Finalize) {
        printf("Error: Failed to get function addresses (Error: %lu)\n", GetLastError());
        FreeLibrary(hDLL);
        free(buf);
        return 1;
    }

    // 执行Python代码
    Py_Initialize();
    PyRun_SimpleString(buf);
    Py_Finalize();

    // 清理资源
    FreeLibrary(hDLL);
    free(buf);
    
    return 0;
}