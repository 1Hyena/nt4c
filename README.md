# NT4C Readme ##################################################################

* Home: https://github.com/1Hyena/nt4c
* Issue tracker: https://github.com/1Hyena/nt4c/issues

NT4C stands for "NestedText for C" and that is exactly what this project is
about.


## What is NestedText ##########################################################

In short, NestedText is a file format for holding structured data.

The following resources can explain more if you are unfamiliar with it:
* [Documentation](https://nestedtext.org/)
* [Questions, suggestions and issues](https://github.com/KenKundert/nestedtext)


## What is NT4C

NT4C is a NestedText parser implementation written following the C23 standard of
the C programming language and it comes with the following features:

* **Compliance:** Generally, NT4C aims to be compliant to the latest version of
  [NestedText specification](https://nestedtext.org/). However, currently we are
  only partially compliant to the
  [Minimal NestedText](https://nestedtext.org/en/latest/minimal-nestedtext.html)
  specification.

* **Performance:** NT4C is fast considering that it does not do any heap memory
  allocations. Also, it avoids needless memory copying by referencing the input
  text directly in the resulting graph.

* **Compactness:** NT4C parser is implemented in one header file. There are no
  dependencies other than standard C library.

* **Embedding:** NT4C parser is easy to reuse in other projects, its API is
  very straightforward: There is actually just one function, `nt_parse()`.

* **Tree model:** NT4C parses the complete document and constructs a graph (DOM)
  where each node references directly a segment from the input string.

* **Portability:** NT4C builds and works on Linux. It should be simple to make
  it run also on most other platforms, at least as long as the platform provides
  the C standard library.

* **Encoding:** NT4C by default expects UTF-8 encoding of the input text. It
  does not attempt to detect Unicode encoding errors.

* **Permissive license:** NT4C is available under the [MIT license](LICENSE).


# License ######################################################################

NT4C has been authored by Erich Erstu and is released under the [MIT](LICENSE)
license.
