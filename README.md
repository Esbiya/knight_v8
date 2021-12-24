<h1 align="center">Welcome to knight_v8 👋</h1>
<p>
  <img alt="Version" src="https://img.shields.io/badge/version-0.1.0-blue.svg?cacheSeconds=2592000" />
  <a href="#" target="_blank">
    <img alt="License: ISCL" src="https://img.shields.io/badge/License-ISCL-yellow.svg" />
  </a>
</p>

> Minimal, modern embedded V8 for Python.

### 🏠 [Homepage](https://github.com/Esbiya/knight_v8)

## Install

```sh
pip install knight_v8
```

or:

```sh
git clone https://github.com/Esbiya/knight_v8
python setup.py install
```

## Compatibility

knight_v8 is compatible with Python 2 & 3 and based on ctypes. Now this lib only support darwin(x86_64) and linux(glic), waiting for more platform support...

## Examples

check v8 version:

```python
>>> from knight_v8 import v8_version
>>> v8_version()
'9.6.180.12'
```

knight_v8 is straightforward to use:

```python
>>> from knight_v8 import Knight
>>> ctx = Knight()
>>> ctx.eval("1+1")
2
>>> ctx.eval("var x = {company: 'Sqreen'}; x.company")
'Sqreen'
>>> print(ctx.eval("'\N{HEAVY BLACK HEART}'"))
❤
```

Variables are kept inside of a context:

```python
>>> ctx.eval("x.company")
'Sqreen'
```

While ``eval`` only supports returning primitive data types such as
strings, ``call`` supports returning composite types such as objects:

```python
>>> ctx.compile("var fun = () => ({ foo: 1 });")
>>> ctx.call("fun")
{'foo': 1}
```

knight_v8 is ES6 capable:

```python
>>> ctx.eval("[1,2,3].includes(5)")
False
```

Please do not new a lot of Knight instances, if you have to do this, please use release the instance after using.

```python
>>> ctx = Knight()
>>> ctx.eval("1 + 1")
2
>>> ctx.dispose()
```

or use with statement, in this case, you don't have to actively release instance:

```python
>>> with Knight() as ctx:
        ctx.eval("1 + 1")
2
```

## Show your support

Give a ⭐️ if this project helped you!

***
_This README was generated with ❤️ by [readme-md-generator](https://github.com/kefranabg/readme-md-generator)_