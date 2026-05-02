# NT4C Readme ##################################################################

* Version: 1.0.3 (01.05.2026)
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

* **Embedding:** The NT4C parser is easily reusable in other projects with a
  simple API that includes a few key functions, primarily `nt_parse()`.

* **Callbacks:** NT4C parses the entire document and calls a callback function
  provided by the application to inform it about each NestedText unit.

* **Tree model:** If sufficient memory is provided to the NT4C parser, it
  constructs a graph where each node directly references a segment from the
  input text.

* **Portability:** NT4C builds and functions on Linux. It should be relatively
  simple to make it run on most other platforms as long as the platform provides
  the C standard library.

* **Encoding:** NT4C expects UTF-8 encoding of the input text and does not
  attempt to detect Unicode encoding errors.

* **Permissive license:** NT4C is available under the [MIT license](LICENSE).


## Using NT4C ##################################################################

### Parsing NestedText #########################################################

To parse a NestedText document, you can include the [nt4c.h](nt4c.h) header file
directly in your codebase. The parser is implemented in a single C header file
for easy integration.

The main functions to use are `nt_parse()` and `nt_parser_parse()`. The former
is a convenience function for simple callback based parsing whereas the latter
takes a pointer to the `NT_PARSER` structure as its first argument and is to be
used for customized parsing.

The `NT_PARSER` structure stores parsing configuration and the parsing process
state. By default, it can handle up to `NT_PARSER_NCOUNT` nodes in its internal
memory. However, you can use the `nt_parser_set_memory` function to work with a
custom array of `NT_NODE` structures.

When you call `nt_parser_parse()`, the parser populates the document graph with
nodes. It continues processing even if the output buffer reaches its capacity.

After a successful parsing operation, both `nt_parse()` and `nt_parser_parse()`
return the number of nodes in the input text. This information can help you to
determine the memory required for storing the full graph of the document. If
parsing fails, the function returns a negative value.

The graph of the document is considered fully stored when the value returned by
`nt_parser_parse()` is non-negative and does not exceed the output buffer's
capacity.


### Examples ###################################################################

#### exhello ###################################################################

The [exhello](examples/src/exhello.c) example demonstrates how to use the NT4C
parser to generate the text "hello world" and display it on the screen.

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/exhello.c#L5-L14

![screenshot](img/exhello.png "console output of exhello")


#### excallback ################################################################

The [excallback](examples/src/excallback.c) example demonstrates how to make
the NT4C parser call a user-specified function each time it parses the next
logical portion of the input document.

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/excallback.c#L6-L37

![screenshot](img/excallback.png "console output of excallback")


#### execho ####################################################################

[This example](examples/src/execho.c) demonstrates how to utilize the NT4C
parser to parse and display a NestedText document on the screen. The input
document undergoes parsing twice. Initially, the length of the document is
calculated. Subsequently, a variable-length array is set up to store the
Document Object Model (DOM).

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/execho.c#L33-L35

![screenshot](img/execho.png "console output of execho")


#### expretty ##################################################################

[This example](examples/src/expretty.c) shows how to use the NT4C parser to
pretty-print a NestedText document. It reformats the input text and adds syntax
highlighting.

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/expretty.c#L70-L78

Here is a NestedText document before and after pretty-printing, as shown in the
screenshot below:

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/ugly.nt#L1-L29

![screenshot](img/expretty.png "console output of expretty")


#### extree ####################################################################

[This example](examples/src/extree.c) shows how to use the NT4C parser to print
the structure of a NestedText document on the screen.

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/extree.c#L84-L98

Here is a screenshot showing the structure of the parsed NestedText document:

![screenshot](img/extree.png "console output of extree")


#### exquery ###################################################################

The [exquery](examples/src/exquery.c) example demonstrates how to query an
arbitrary NestedText document for a specific key and print the contents of the
matching node on screen.

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/repository.nt#L1-L21

![screenshot](img/exquery.png "console output of exquery")


### API ########################################################################

* [Initialization](#initialization)
  - [nt_make_parser](#nt_make_parser) () → `NT_PARSER`
  - [nt_parser_init](#nt_parser_init) (&*parser*)

* [Parsing](#parsing)
  - [nt_parse](#nt_parse) (*text*, *text size*, *callback*, *userdata*) → `int`
  - [nt_parser_parse](#nt_parser_parse) (&*parser*, *text*, *text size*) → `int`

* [Configuration](#configuration)
  - [nt_parser_set_memory](#nt_parser_set_memory) (&*parser*, &*nodes*, *node count*)
  - [nt_parser_set_recursion](#nt_parser_set_recursion) (&*parser*, *depth*)
  - [nt_parser_set_blacklist](#nt_parser_set_blacklist) (&*parser*, *banned types*)
  - [nt_parser_set_whitelist](#nt_parser_set_whitelist) (&*parser*, *allowed types*)
  - [nt_parser_set_userdata](#nt_parser_set_userdata) (&*parser*, &*userdata*)
  - [nt_parser_set_callback](#nt_parser_set_callback) (&*parser*, &*callback*)

* [Miscellaneous](#miscellaneous)
  - [nt_type_code](#nt_type_code) (*type*) → `const char *`
  - [nt_type_type](#nt_type_code) (*type*) → `NT_TYPE`

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L48-L87

Specify the size of the integrated memory buffer of the `NT_PARSER` structure by
defining the `NT_PARSER_NCOUNT` macro before including the `nt4c.h` header. The
integrated memory was added to increase the API usage convenience in cases where
the size of the input document is always known to be small (see
[expretty](examples/src/expretty.c)).

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L41-L43


#### Initialization ############################################################

##### nt_make_parser ###########################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L113-L115

Examples:
[execho](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/execho.c#L23),
[expretty](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/expretty.c#L70),
[exquery](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/exquery.c#L35),
[extree](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/extree.c#L86)


##### nt_parser_init ###########################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L117-L121


#### Parsing ###################################################################

##### nt_parse #################################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L98-L111

Examples:
[excallback](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/excallback.c#L34),
[execho](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/execho.c#L15),
[exhello](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/exhello.c#L12)


##### nt_parser_parse ##########################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L123-L133

Examples:
[execho](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/execho.c#L27)
[expretty](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/expretty.c#L73),
[exquery](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/exquery.c#L40),
[extree](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/extree.c#L90)

#### Configuration #############################################################

##### nt_parser_set_memory #####################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L135-L142

Examples:
[execho](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/execho.c#L25),
[extree](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/extree.c#L88)


##### nt_parser_set_recursion ##################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L144-L149


##### nt_parser_set_blacklist ##################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L151-L157

Examples:
[expretty](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/expretty.c#L71),
[exquery](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/exquery.c#L39)


##### nt_parser_set_whitelist ##################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L159-L165


##### nt_parser_set_userdata ###################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L167-L173

Examples:
[exquery](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/exquery.c#L38)


##### nt_parser_set_callback ###################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L175-L181

Examples:
[exquery](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/exquery.c#L37)


#### Miscellaneous #############################################################

##### nt_type_code #############################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L183-L188

Examples:
[excallback](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/excallback.c#L14),
[extree](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/extree.c#L44)


##### nt_type_type #############################################################

https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/nt4c.h#L190-L194

Examples:
[excallback](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/excallback.c#L11),
[exquery](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/exquery.c#L51),
[extree](https://github.com/1Hyena/nt4c/blob/6908ef491ae1793ac9412c36d23abe302892756e/examples/src/extree.c#L47)


# License ######################################################################

NT4C has been authored by Erich Erstu and is released under the [MIT](LICENSE)
license.
