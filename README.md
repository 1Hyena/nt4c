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
  only compliant with the
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

To parse a NestedText document, include `nt4c.h` directly in your codebase as
the parser is implemented in a single C header file.

The main function provided is `nt_parse()`, which takes a text in NestedText
syntax and a pointer to the `NT_PARSER` structure for fine-tuning the
deserialization process.

The parser structure stores parsing configuration and the parsing process state.
By default, it can hold up to `NT_PARSER_NCOUNT` nodes in its internal memory,
but `nt_parser_set_memory` function allows using an arbitrary array of `NT_NODE`
structures.

During `nt_parse()` execution, the deserialization graph of the document is
populated with nodes. The parser continues processing even if the output buffer
becomes full.

Upon successful parsing, `nt_parse()` returns the number of nodes in the input
text, which can help determine the memory needed for storing the deserialization
graph. On failure, the function returns a negative number.

The deserialization graph is considered fully stored only when the value
returned by `nt_parse()` is non-negative and less than or equal to the output
buffer capacity.


### Examples ###################################################################

#### ex_hello ##################################################################

The [ex_hello](examples/src/ex_hello.c) example demonstrates how to use the NT4C
parser to generate the text "hello world" and display it on the screen.

https://github.com/1Hyena/nt4c/blob/1f8595884201722495c089d4b45086a23465b84a/examples/src/ex_hello.c#L5-L15

![screenshot](img/ex_hello.png "console output of ex_hello")


#### ex_echo ###################################################################

[This example](examples/src/ex_echo.c) shows how to use the NT4C parser to parse
and print a NestedText document on the screen. The input document is parsed
twice. First, the length of the document is determined. Then, a variable-length
array is initialized for storing the DOM.

https://github.com/1Hyena/nt4c/blob/1f8595884201722495c089d4b45086a23465b84a/examples/src/ex_echo.c#L10-L34

![screenshot](img/ex_echo.png "console output of ex_echo")


#### ex_pretty #################################################################

[This example](examples/src/ex_pretty.c) shows how to use the NT4C parser to
pretty-print a NestedText document. It reformats the input text and adds syntax
highlighting.

https://github.com/1Hyena/nt4c/blob/1f8595884201722495c089d4b45086a23465b84a/examples/src/ex_pretty.c#L65-L74

Here is a NestedText document before and after pretty-printing, as shown in the
screenshot below:

https://github.com/1Hyena/nt4c/blob/2c24be5802d1af9dd911616434e4df967f1bff57/examples/ugly.nt#L1-L29

![screenshot](img/ex_pretty.png "console output of ex_pretty")


#### ex_tree ###################################################################

[This example](examples/src/ex_tree.c) shows how to use the NT4C parser to print
the structure of a NestedText document on the screen.

https://github.com/1Hyena/nt4c/blob/1f8595884201722495c089d4b45086a23465b84a/examples/src/ex_tree.c#L79-L97

Here is a screenshot showing the structure of the parsed NestedText document:

![screenshot](img/ex_tree.png "console output of ex_tree")


# License ######################################################################

NT4C has been authored by Erich Erstu and is released under the [MIT](LICENSE)
license.
