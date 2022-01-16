#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys
import atexit, json
import _ctypes, ctypes


try:
    import pkg_resources
except ImportError:
    pkg_resources = None  # pragma: no cover
    

name = u"v8py"


def _get_lib_path(name):
    if os.name == "posix" and sys.platform == "darwin":
        prefix, ext = "lib", ".dylib"
    elif sys.platform == "win32":
        prefix, ext = "lib", ".dll"
    else:
        prefix, ext = "lib", ".so"
    fn = None
    meipass = getattr(sys, "_MEIPASS", None)
    if meipass is not None:
        fn = os.path.join(meipass, prefix + name + ext)
    if fn is None and pkg_resources is not None:
        try:
            fn = pkg_resources.resource_filename("knight_v8", prefix + name + ext)
        except Exception:
            pass
    if fn is None:
        root_dir = os.path.dirname(os.path.abspath(__file__))
        fn = os.path.join(root_dir, prefix + name + ext)
    return fn


EXTENSION_PATH = _get_lib_path(name)
EXTENSION_NAME = os.path.basename(EXTENSION_PATH) if EXTENSION_PATH is not None else None


if sys.version_info[0] < 3:
    UNICODE_TYPE = unicode
else:
    UNICODE_TYPE = str
    
    
def is_unicode(value):
    return isinstance(value, UNICODE_TYPE)


def _build_ext_handle():

    if EXTENSION_PATH is None or not os.path.exists(EXTENSION_PATH):
        raise RuntimeError(u"Native library not available at {}".format(EXTENSION_PATH))

    _ext_handle = ctypes.CDLL(EXTENSION_PATH)

    _ext_handle.version.restype = ctypes.c_char_p
    _ext_handle.new_evaluator.restype = ctypes.c_void_p
    _ext_handle.reset_evaluator.argtypes = [ctypes.c_void_p]
    _ext_handle.free_evaluator.argtypes = [ctypes.c_void_p]
    _ext_handle.free_evaluator.restype = ctypes.c_void_p
    _ext_handle.eval.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
    _ext_handle.eval.restype = ctypes.c_char_p
    _ext_handle.initialize()    
    return _ext_handle


_ext = _build_ext_handle()


def v8_version():
    return _ext.version().decode()


def enable_heap_report():
    _ext.enable_heap_report()


@atexit.register
def dispose():
    """
    释放 dll 句柄
    :return
    """
    _ext.dispose()
    if sys.platform == u"win32":
        _ctypes.FreeLibrary(_ext._handle)
    else:
        _ctypes.dlclose(_ext._handle)


class V8EvalException(Exception):

    def __init__(self, msg):
        self.msg = msg

    def __str__(self):
        return self.msg



class Knight:
    
    def __init__(self):
        self.evaluator = _ext.new_evaluator()
        atexit.register(self.dispose)
    
    def reset(self):
        _ext.reset_evaluator(self.evaluator)
    
    def compile(self, script):
        """
        预编译 js 脚本
        @params script: 预编译脚本
        :return 编译是否通过: bool
        """
        _ext.eval(self.evaluator, script.encode())
                
    def call(self, expr, *args):
        """
        调用函数
        :param expr: 
        :param args: 
        """
        json_args = json.dumps(args, separators=(u",", u":"))
        js = u"{expr}.apply(this, {json_args})".format(expr=expr, json_args=json_args)
        return self.eval(js)

    def eval(self, script):
        """
        eval 脚本
        :param script: 脚本字符串
        :return 脚本返回值
        """
        ret = _ext.eval(self.evaluator, script.encode()).decode()
        if not is_unicode(ret):
            raise ValueError(u"Unexpected return value type {}".format(type(ret)))
        try:
            return json.loads(ret)
        except:
            raise V8EvalException(ret)
        
    def dispose(self):
        """
        销毁 V8Evaluator 实例
        :return
        """
        if _ext is not None: 
            self.evaluator = _ext.free_evaluator(getattr(self, u"evaluator", None))

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.dispose()

    def __del__(self):
        self.dispose()
