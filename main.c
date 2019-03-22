#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

// #include <dtrace.h>

#include "Python.h"
#include "ceval.h"
#include "unicodeobject.h"
#include "pythonrun.h"
#include "Python-ast.h"
#include "pyarena.h"
#include "frameobject.h"
#include "compile.h"

void handler(int sig) {
    void *array[10];
    size_t size;
    size = backtrace(array, 10);
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}

static int
fatal(int exitcode, const char *format, const char *message)
{
    fprintf(stderr, format, message);
    exit(exitcode);
}


int
main(int argc, char **argv)
{
    signal(SIGSEGV, handler);   // install our handler
    char* input;
    if (argc > 1){
        input = argv[1];
    } else {
        fatal(-1, "Provide Python commands...", NULL);
    }
    PyObject *co,*bytes,*unicode;
    PyArena *arena;
    PyCompilerFlags flags;
    flags.cf_flags = 0;
    flags.cf_feature_version = PY_MINOR_VERSION;

    Py_Initialize();

    char* fn = "test.py";
    char* errors;

    unicode = PyUnicode_DecodeASCII(input, strlen(input), &errors);
    if (unicode == NULL) {
        fatal(-1, "Failed to decode input command : %s", errors);
    }

    bytes = PyUnicode_AsUTF8String(unicode);
    Py_DECREF(unicode);
    char* str = PyBytes_AsString(bytes);
    co = Py_CompileStringFlags(str, fn, Py_single_input, NULL);

    if (co == NULL) {
        exit(-1);
    }
    arena = PyArena_New();
    if (arena == NULL)
        exit(-1);


    PyObject* main = PyImport_AddModule("__main__");
    PyObject* globals = PyModule_GetDict(main);
    PyObject* locals = PyDict_New();

    // Construct frame..

    PyThreadState* tstate = PyThreadState_Get();
    assert(tstate != NULL);
    if (tstate == NULL){
        fatal(1, "Failed to aquire threadstate", NULL);
    }
    PyFrameObject* f = PyFrame_New(tstate, co, globals, locals);
    if (f == NULL) {
        return NULL;
    }

    PyObject *ret = PyEval_EvalFrame(f);

    PyObject* result = PyObject_Str(ret);
    PyObject_Print(result, stdout, 0);

    PyArena_Free(arena);

    return 0;
}