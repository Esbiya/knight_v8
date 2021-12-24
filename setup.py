#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, sys, sysconfig
from distutils.core import setup
from setuptools import find_packages


def _get_libc_name():
    """Return the libc name of the current system."""
    target = sysconfig.get_config_var("HOST_GNU_TYPE")
    if target is not None and target.endswith("musl"):
        return "muslc"
    return "glibc"


def _get_lib_name():
    """Return the path of the library called `name` on the current system."""
    if os.name == "posix" and sys.platform == "darwin":
        prefix, ext = "lib", ".dylib"
    elif sys.platform == "win32":
        prefix, ext = "lib", ".dll"
    else:
        prefix, ext = "lib", ".{}.so".format(_get_libc_name())
    return prefix + "v8py" + ext


if os.path.exists("MANIFEST.in"):
    os.unlink("MANIFEST.in")


with open("MANIFEST.in", "w", encoding="utf-8") as f:
    f.write("""include MANIFEST.in
include README.rst
include knight_v8/__init__.py
include knight_v8/%s
global-exclude __pycache__ *.py[cod]""" % _get_lib_name())


with open("README.rst", "r") as f:
    long_description = f.read()

setup(
    name='knight_v8',
    version='0.1.0',
    description='Minimal, modern embedded V8 for Python',
    long_description=long_description,
    author='esbiya',
    author_email='2995438815@qq.com',
    url='https://github.com/Esbiya/v8py',
    install_requires=[],
    license='MIT',
    packages=find_packages(),
    package_dir={'v8py': 'v8py'},
    platforms=["all"],
    include_package_data=True,
    zip_safe=False,
    keywords='v8',
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: ISC License (ISCL)',
        'Natural Language :: English',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3'
    ],
)
