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


int TraceFunc(PyObject * self, struct _frame * frame, int type, PyObject * arg){
    char* type_msg = NULL;
    switch (type){
        case PyTrace_CALL:
            type_msg = "Call()";
            break;
        case PyTrace_EXCEPTION:
            type_msg = "Exception";
            break;
        case PyTrace_LINE:
            type_msg = "Line trace";
            break;
        case PyTrace_RETURN:
            type_msg = "Return";
            break;
        case PyTrace_C_CALL:
            type_msg = "C call()";
            break;
        case PyTrace_C_EXCEPTION:
            type_msg = "C Exception";
            break;
        case PyTrace_C_RETURN:
            type_msg = "C Return";
            break;
        case PyTrace_OPCODE:
            type_msg = "OPCODE";
            break;
    }
    printf("Caught %s : %s\n", type_msg, PyUnicode_AsUTF8(PyObject_ASCII(arg)));
    return 0;
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
        PyErr_Print();
        exit(-1);
    }
    arena = PyArena_New();
    if (arena == NULL)
        exit(-1);


    PyObject* main = PyImport_AddModule("__main__");
    PyObject* globals = PyModule_GetDict(main);
    // Turn on super-verbose tracing!
    // PyDict_SetItem(globals, PyUnicode_FromString("__ltrace__"), PyLong_FromLong(1));

    PyObject* locals = PyDict_New();

    // Construct frame..

    PyThreadState* tstate = PyThreadState_Get();
    PyEval_SetTrace(TraceFunc, NULL);

    if (tstate == NULL){
        fatal(1, "Failed to aquire threadstate", NULL);
    }
    PyCodeObject *co_f = (PyCodeObject*)co;
    PyFrameObject* f = PyFrame_New(tstate, co, globals, locals);
    if (f == NULL) {
        return NULL;
    }

    f->f_trace_lines = 0 ;
    f->f_trace_opcodes = 1;

    PyObject *ret = PyEval_EvalFrame(f);

    PyObject* result = PyObject_Str(ret);
    PyObject_Print(result, stdout, 0);

    PyArena_Free(arena);

    return 0;
}