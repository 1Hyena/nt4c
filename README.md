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

NT4C is a NestedText parser implementation written in accordance with the C23
standard of the C programming language. It includes the following features:

* **Compliance:** NT4C aims to comply with the latest version of the
  [NestedText specification](https://nestedtext.org/). However, it is currently
  only partially compliant with the
  [Minimal NestedText](https://nestedtext.org/en/latest/minimal-nestedtext.html)
  specification.

* **Performance:** NT4C is fast as it does not involve any heap memory
  allocations. It also avoids unnecessary memory copying by directly referencing
  the input text in the resulting graph.

* **Compactness:** The NT4C parser is implemented in a single header file with
  no dependencies other than the standard C library.

* **Embedding:** The NT4C parser is easy to reuse in other projects, with a
  straightforward API that consists of just one function, `nt_parse()`.

* **Tree model:** NT4C parses the entire document and constructs a graph (DOM)
  where each node directly references a segment from the input string.

* **Portability:** NT4C builds and functions on Linux. It should be relatively
  simple to make it run on most other platforms as long as the platform provides
  the C standard library.

* **Encoding:** NT4C expects UTF-8 encoding of the input text and does not
  attempt to detect Unicode encoding errors.

* **Permissive license:** NT4C is available under the [MIT license](LICENSE).


## Using NT4C ##################################################################

### Parsing NestedText #########################################################

If you need just to parse a NestedText document, you need to include `nt4c.h`
directly to your code base as the parser is only implemented in the single C
header file.

The main provided function is `nt_parse()`. It takes a text in the NestedText
syntax and a pointer to the `NT_PARSER` structure which provides the means to
fine tune the deserialization process.

The parser structure stores the parsing configuration and the state of the
parsing process. By default, it can hold up to `NT_PARSER_NCOUNT` nodes in its
integrated internal memory but with the `nt_parser_set_memory` function an
arbitrary array of `NT_NODE` structures could be used.

As `nt_parse()` processes the input, it populates the deserialization graph of
the document with nodes. The parser will continue until the end even if the
buffer provided for its output gets full.

On success the `nt_parse()` function returns the number of nodes contained in
the input text. That number could be used to determine how much memory is needed
for the storage of the complete deserialization graph. On failure the function
returns a negative number.


# License ######################################################################

NT4C has been authored by Erich Erstu and is released under the [MIT](LICENSE)
license.
