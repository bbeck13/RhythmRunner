#include <Python/Python.h>
#include <PythonExtension.h>
#include <iostream>

namespace PythonExtension {
  void GetYoutubeVideo(char* filename, char* functioname, char* url) {
    setenv("PYTHONPATH", ".", 1);
    PyObject *pName, *pModule, *pDict, *pFunc, *pValue, *pResult;
    Py_Initialize();
    pName = PyString_FromString(filename);
    pModule = PyImport_Import(pName);
    pDict = PyModule_GetDict(pModule);
    pFunc = PyDict_GetItemString(pDict, functioname);
    if (PyCallable_Check(pFunc)) {
      pValue = Py_BuildValue("(z)", url);
      PyErr_Print();
      pResult=PyObject_CallObject(pFunc, pValue);
      PyErr_Print();
    } else {
      PyErr_Print();
    }
    Py_DECREF(pValue);
    Py_DECREF(pModule);
    Py_DECREF(pName);
    Py_Finalize();
  }
}
