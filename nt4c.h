////////////////////////////////////////////////////////////////////////////////
// MIT License                                                                //
//                                                                            //
// Copyright (c) 2025 Erich Erstu                                             //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included in //
// all copies or substantial portions of the Software.                        //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    //
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER //
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    //
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        //
// DEALINGS IN THE SOFTWARE.                                                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef NT4C_H_09_11_2025
#define NT4C_H_09_11_2025

#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>

typedef struct NT_NODE      NT_NODE;
typedef struct NT_PARSER    NT_PARSER;

typedef enum : uint16_t {
    NT_NONE = 0,
    ////////////////////////////////////////////////////////////////////////////
    NT_ROOT         = 1 <<  0,          NT_KEY          = 1 <<  1,
    NT_STRING       = 1 <<  2,          NT_LIST_KEY     = 1 <<  3,
    NT_LIST_TAG     = 1 <<  4,          NT_DICT_KEY     = 1 <<  5,
    NT_COMMENT      = 1 <<  6,          NT_COMMENT_TAG  = 1 <<  7,
    NT_NEWLINE      = 1 <<  8,          NT_SPACE        = 1 <<  9,
    NT_INVALID      = 1 << 10,          NT_OP_SET       = 1 << 11,
    NT_OP_SET_ROL   = 1 << 12
} NT_TYPE;

static int      nt_parse(const char *text, size_t text_size, NT_PARSER *);
static NT_NODE *nt_parser_create_node(NT_PARSER *);
static NT_NODE *nt_parser_create_node_type(NT_PARSER *, NT_TYPE);
static void     nt_parser_set_memory(NT_PARSER *, NT_NODE *nodes, size_t count);
static void     nt_parser_set_blacklist(NT_PARSER *, NT_TYPE blacklist);
static void     nt_parser_set_whitelist(NT_PARSER *, NT_TYPE whitelist);
static bool     nt_parser_allows(NT_PARSER *, NT_TYPE);
static void     nt_node_reverse(NT_NODE *);
static void     nt_node_set_data(NT_NODE *, const char *data, size_t size);
static void     nt_node_set_type(NT_NODE *, NT_TYPE);
static void     nt_node_to_node(NT_NODE *, NT_NODE *to);
static NT_NODE *nt_node_from_node(NT_NODE *node);

static const char *nt_parser_deserialize(
    NT_PARSER *, const char *str, size_t str_sz, size_t indent, NT_NODE *parent
);

static const char *nt_str_seg_skip_spaces(const char *str, size_t str_sz);
static const char *nt_str_seg_first_line_size(
    const char *str, size_t str_sz, size_t *line_size
);
static const char *nt_str_seg_skip_key(const char *str, size_t str_sz);
static const char *nt_str_seg_skip_key_op(const char *str, size_t str_sz);
static const char *nt_str_seg_skip_byte(const char *str, size_t str_sz);

static void     nt_print_log(char *fmt, ...);
static size_t   nt_long_to_size(long);
static long     nt_size_to_long(size_t);
static int      nt_size_to_int(size_t);

typedef struct NT_NODE {
    NT_NODE *       next;
    NT_NODE *       prev;
    NT_NODE *       parent;
    NT_NODE *       children;
    const char *    data;
    size_t          size;
    NT_TYPE         type;
} NT_NODE;

typedef struct NT_PARSER {
    struct {
        size_t      count;
        NT_TYPE     blacklist;
    } node;

    struct {
        NT_NODE *   root;
        NT_NODE *   begin;
        NT_NODE *   end;
    } nest;

    struct {
        NT_NODE     integrated[8];
        NT_NODE *   nodes;
        size_t      capacity;
    } memory;

    struct {
        bool debug:1;
        bool nomem:1;
    } bitset;
} NT_PARSER;

static inline void nt_parser_set_memory(
    NT_PARSER *parser, NT_NODE *nodes, size_t count
) {
    parser->memory.nodes = nodes;
    parser->memory.capacity = count;
    parser->bitset.nomem = !count;
}

static inline void nt_parser_set_blacklist(
    NT_PARSER *parser, NT_TYPE blacklist
) {
    parser->node.blacklist = blacklist;
}

static inline void nt_parser_set_whitelist(
    NT_PARSER *parser, NT_TYPE whitelist
) {
    nt_parser_set_blacklist(parser, ~whitelist);
}

static inline bool nt_parser_allows(NT_PARSER *parser, NT_TYPE types) {
    return !(types & parser->node.blacklist);
}

static inline int nt_parse(const char *str, size_t str_sz, NT_PARSER *parser) {
    NT_PARSER default_parser = {};

    if (!parser) {
        parser = &default_parser;
    }
    else {
        if (parser->memory.capacity == 0
        &&  parser->memory.nodes == nullptr
        &&  parser->bitset.nomem == false) {
            // Parser's memory seems to be in the default state. In this case we
            // use the integrated memory.

            nt_parser_set_memory(
                parser, parser->memory.integrated, (
                    sizeof(parser->memory.integrated) /
                    sizeof(parser->memory.integrated[0])
                )
            );
        }

        parser->node.count  = 0;
        parser->nest.begin  = nullptr;
        parser->nest.end    = nullptr;
        parser->nest.root   = nullptr;
    }

    nt_parser_create_node_type(parser, NT_ROOT);

    if (parser->nest.root) {
        nt_node_set_type(parser->nest.root, NT_ROOT);
    }

    const char *s = str;
    str_sz = str_sz ? str_sz : strlen(s);

    while (*s && s < str + str_sz) {
        const char *next = nt_parser_deserialize(
            parser, s, str_sz - nt_long_to_size(s - str), 0, parser->nest.root
        );

        if (next == nullptr) {
            return -1;
        }

        if (next == s) {
            break;
        }

        s = next;
    }

    if (parser->nest.root) {
        nt_node_reverse(parser->nest.root);
    }

    return nt_size_to_int(parser->node.count);
}

static const char *nt_parser_deserialize(
    NT_PARSER *parser,
    const char *str, size_t str_sz, size_t indent, NT_NODE *parent
) {
    size_t line_size;
    const char *next_line = nt_str_seg_first_line_size(str, str_sz, &line_size);

    if (next_line == str) {
        return str;
    }

    const char *after_spaces = nt_str_seg_skip_spaces(str, str_sz);
    size_t spaces = nt_long_to_size(after_spaces - str);

    if (spaces < indent) {
        return str;
    }

    if (spaces) {
        NT_NODE *node = nt_parser_create_node_type(parser, NT_SPACE);

        if (node != nullptr) {
            nt_node_set_data(node, str, spaces);
            nt_node_to_node(node, parent);
        }
    }

    const char *end_of_line = str + line_size;
    size_t newline_size = nt_long_to_size(next_line - (str + line_size));

    if (after_spaces - str == nt_size_to_long(line_size)) {
        // Empty lines are not ignored.

        if (newline_size) {
            NT_NODE *newline = nt_parser_create_node_type(parser, NT_NEWLINE);

            if (newline) {
                nt_node_set_data(newline, end_of_line, newline_size);
                nt_node_to_node(newline, parent);
            }
        }

        return next_line;
    }

    if (parser->bitset.debug) {
        nt_print_log("%.*s\n", nt_size_to_int(line_size), str);
    }

    if (*after_spaces == '#') {
        NT_NODE *node = nt_parser_create_node_type(parser, NT_COMMENT_TAG);

        if (node) {
            nt_node_set_data(node, after_spaces, 1);
            nt_node_to_node(node, parent);
        }

        if (line_size - (spaces + 1)) {
            NT_NODE *val = nt_parser_create_node_type(parser, NT_COMMENT);

            if (val) {
                nt_node_set_data(
                    val, after_spaces + 1, line_size - (spaces + 1)
                );

                nt_node_to_node(val, node ? node : parent);
            }
        }

        if (newline_size) {
            NT_NODE *newline = nt_parser_create_node_type(parser, NT_NEWLINE);

            if (newline) {
                nt_node_set_data(newline, end_of_line, newline_size);
                nt_node_to_node(newline, parent);
            }
        }

        return next_line;
    }

    NT_NODE *nest = nullptr;

    if (*after_spaces == '-') {
        NT_NODE *node = nt_parser_create_node_type(parser, NT_LIST_TAG);

        if (node) {
            nt_node_set_data(node, after_spaces, 1);
        }

        if (parent) {
            if (parent->type == NT_NONE) {
                nt_node_set_type(parent, NT_LIST_KEY);
            }
            else if (parent->type != NT_LIST_KEY) {
                if (node) {
                    nt_node_set_type(node, NT_INVALID);
                }
            }
        }

        if (after_spaces + 1 < str + line_size && after_spaces[1] == ' ') {
            if (node) {
                nt_node_set_data(node, after_spaces, 2);
            }

            NT_NODE *val = nt_parser_create_node_type(parser, NT_STRING);

            if (val) {
                nt_node_set_data(
                    val, after_spaces + 2, line_size - (spaces + 2)
                );

                nt_node_to_node(val, node ? node : parent);
            }

            if (node) {
                nt_node_to_node(node, parent);
            }

            if (newline_size) {
                NT_NODE *newline = nt_parser_create_node_type(
                    parser, NT_NEWLINE
                );

                if (newline) {
                    nt_node_set_data(newline, end_of_line, newline_size);
                    nt_node_to_node(newline, parent);
                }
            }

            return next_line;
        }

        nest = node;
    }
    else {
        const char *key = after_spaces;
        const char *key_op = nt_str_seg_skip_key(key, line_size - spaces);
        const char *after_key_op = nullptr;

        if (key_op != key) {
            after_key_op = nt_str_seg_skip_key_op(
                key_op, line_size - nt_long_to_size(key_op - str)
            );
        }

        if (key_op == key || after_key_op == key_op) {
            // Could not determine a valid key.

            NT_NODE *node = nt_parser_create_node_type(parser, NT_INVALID);

            if (node) {
                nt_node_set_data(node, key, line_size - spaces);
                nt_node_to_node(node, parent);
            }

            if (newline_size) {
                NT_NODE *newline = nt_parser_create_node_type(
                    parser, NT_NEWLINE
                );

                if (newline) {
                    nt_node_set_data(newline, end_of_line, newline_size);
                    nt_node_to_node(newline, parent);
                }
            }

            return next_line;
        }

        NT_NODE *node = nullptr;

        if (after_key_op && key_op[1] == ' ') {
            node = nt_parser_create_node_type(parser, NT_KEY);
        }
        else {
            node = nt_parser_create_node(parser);
        }

        if (node) {
            nt_node_set_data(
                node, key, nt_long_to_size(key_op - key)
            );
        }

        if (after_key_op) {
            NT_NODE *op = nullptr;

            if (after_key_op && key_op[1] == ' ') {
                op = nt_parser_create_node_type(parser, NT_OP_SET_ROL);
            }
            else {
                op = nt_parser_create_node_type(parser, NT_OP_SET);
            }

            if (op) {
                nt_node_set_data(
                    op, key_op, nt_long_to_size(after_key_op - key_op)
                );

                nt_node_to_node(op, node ? node : parent);
            }
        }

        if (after_key_op && key_op[1] == ' ') {
            NT_NODE *val = nt_parser_create_node_type(parser, NT_STRING);

            if (val) {
                nt_node_set_data(
                    val, after_key_op,
                    line_size - nt_long_to_size(after_key_op - str)
                );

                nt_node_to_node(val, node ? node : parent);
            }

            if (node) {
                nt_node_reverse(node);

                if (parent) {
                    if (parent->type == NT_NONE) {
                        nt_node_set_type(parent, NT_DICT_KEY);
                    }

                    nt_node_to_node(node, parent);
                }
                else abort();
            }

            if (newline_size) {
                NT_NODE *newline = nt_parser_create_node_type(
                    parser, NT_NEWLINE
                );

                if (newline) {
                    nt_node_set_data(newline, end_of_line, newline_size);
                    nt_node_to_node(newline, parent);
                }
            }

            return next_line;
        }

        nest = node;
    }

    if (newline_size) {
        NT_NODE *newline = nt_parser_create_node_type(parser, NT_NEWLINE);

        if (newline) {
            nt_node_set_data(newline, end_of_line, newline_size);
            nt_node_to_node(newline, nest);
        }
    }

    ++spaces;

    const char *s = next_line;

    while (*s && s < str + str_sz) {
        next_line = nt_parser_deserialize(
            parser, s, str_sz - nt_long_to_size(s - str), spaces, nest
        );

        if (!next_line) {
            // Parse error.
            return nullptr;
        }

        if (next_line == s) {
            break;
        }

        s = next_line;
    }

    if (nest) {
        nt_node_reverse(nest);

        if (parent) {
            if (parent->type == NT_NONE) {
                nt_node_set_type(parent, NT_DICT_KEY);
            }

            nt_node_to_node(nest, parent);
        }
        else abort();
    }

    return s;
}

static NT_NODE *nt_parser_create_node(NT_PARSER *parser) {
    static NT_NODE zero_node;
    NT_NODE *node = nullptr;

    if (parser->node.count < parser->memory.capacity) {
        node = &parser->memory.nodes[parser->node.count];
        *node = zero_node;
    }

    parser->node.count++;

    if (node && parser->nest.root) {
        if (parser->nest.begin == nullptr) {
            parser->nest.begin = node;
        }

        parser->nest.end = node + 1;
    }

    return node;
}

static NT_NODE *nt_parser_create_node_type(NT_PARSER *parser, NT_TYPE type) {
    if (!nt_parser_allows(parser, type)) {
        return nullptr;
    }

    NT_NODE *node = nt_parser_create_node(parser);

    if (node) {
        nt_node_set_type(node, type);

        if (type == NT_ROOT && parser->nest.root == nullptr) {
            parser->nest.root = node;
            parser->nest.begin = nullptr;
            parser->nest.end = nullptr;
        }
    }

    return node;
}

static void nt_node_reverse(NT_NODE *node) {
    NT_NODE *child_next;

    for (NT_NODE *child = node->children; child; child = child_next) {
        child_next = child->next;

        child->next = child->prev;
        child->prev = child_next;

        if (!child_next) {
            node->children = child;
        }
    }
}

static void nt_node_set_data(NT_NODE *node, const char *data, size_t size) {
    node->data = data;
    node->size = size;
}

static void nt_node_set_type(NT_NODE *node, NT_TYPE type) {
    node->type = type;
}

static void nt_node_to_node(NT_NODE *node, NT_NODE *container) {
    if (node->parent) {
        abort(); // Let's require explicit removal from old parent.
    }

    if (container == nullptr) {
        abort();
    }

    nt_node_from_node(node);

    if (container->children) {
        container->children->prev = node;
    }

    node->next = container->children;
    container->children = node;
    node->parent = container;
}

static NT_NODE *nt_node_from_node(NT_NODE *node) {
    NT_NODE *parent = node->parent;

    if (parent == nullptr) {
        return node;
    }

    if (parent->children == node) {
        parent->children = node->next;

        if (node->next) {
            node->next->prev = nullptr;
        }
    }
    else {
        if (node->prev == nullptr) {
            abort();
        }
        else {
            node->prev->next = node->next;

            if (node->next) {
                node->next->prev = node->prev;
            }
        }
    }

    node->parent = nullptr;
    node->next = nullptr;
    node->prev = nullptr;

    return node;
}

static const char *nt_str_seg_first_line_size(
    const char *str, size_t str_sz, size_t *line_size
) {
    const char *begin = str;
    const char *found = nullptr;
    char after_found;

    const char *s = str;

    while (*s && s < str + str_sz) {
        if (*s == '\n') {
            found = s;
            after_found = '\r';
            break;
        }
        else if (*s == '\r') {
            found = s;
            after_found = '\n';
            break;
        }

        ++s;
    }

    if (found) {
        if (*(s + 1) == after_found) {
            s += 2;
        }
        else {
            s += 1;
        }
    }

    if (line_size) {
        *line_size = (
            found ? nt_long_to_size(found - begin) : nt_long_to_size(s - begin)
        );
    }

    return s;
}

static const char *nt_str_seg_skip_spaces(const char *str, size_t str_sz) {
    const char *s = str;

    while (*s && s < str + str_sz) {
        const char *next = nt_str_seg_skip_byte(
            s, str_sz - nt_long_to_size(s - str)
        );

        if (next == s) {
            break;
        }

        --next;

        if (*next != ' ') {
            return s;
        }

        s = next + 1;
    }

    return s;
}

static const char *nt_str_seg_skip_byte(const char *str, size_t str_sz) {
    return str_sz ? str + 1 : str;
}

static const char *nt_str_seg_skip_key(const char *str, size_t str_sz) {
    const char *s = str;
    const char *colon = nullptr;

    while (*s && s < str + str_sz) {
        const char *next = nt_str_seg_skip_byte(
            s, str_sz - nt_long_to_size(s - str)
        );

        if (next == s) {
            break;
        }

        if (next - s == 1) {
            if (s == str) {
                switch (*s) {
                    case ' ':
                    case '[':
                    case '{': {
                        return str;
                    }
                    default: break;
                }
            }
            else if (colon) {
                if (*s == ' ' || *s == '\n') {
                    return colon;
                }
                else {
                    colon = nullptr;
                }
            }
            else if (*s == ':') {
                colon = s;
            }
        }

        s = next;
    }

    return colon ? colon : str;
}

static const char *nt_str_seg_skip_key_op(const char *str, size_t str_sz) {
    const char *s = str;

    while (*s && s < str + str_sz) {
        const char *next = nt_str_seg_skip_byte(
            s, str_sz - nt_long_to_size(s - str)
        );

        if (next == s) {
            break;
        }

        if (next - s == 1) {
            if (s == str) {
                if (*s != ':') {
                    return str;
                }
            }
            else {
                if (*s == ' ' || *s == '\n') {
                    return next;
                }
                else return str;
            }
        }
        else return str;

        s = next;
    }

    return s;
}

static void nt_print_log(char *fmt, ...) {
    va_list args;

    va_start (args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

static size_t nt_long_to_size(long a) {
    return a < 0 ? 0 : (size_t) a;
}

static long nt_size_to_long(size_t a) {
    return a > LONG_MAX ? LONG_MAX : (long) a;
}

static int nt_size_to_int(size_t a) {
    return a > INT_MAX ? INT_MAX : (int) a;
}

#endif
