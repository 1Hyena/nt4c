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

typedef struct NT_NODE      NT_NODE;
typedef struct NT_PARSER    NT_PARSER;

typedef enum : unsigned char {
    NT_NONE = 0,
    ////////////////////////////////////////////////////////////////////////////
    NT_COMMENT, NT_KEY, NT_REST_OF_LINE,
    ////////////////////////////////////////////////////////////////////////////
    MAX_NT_TYPE
} NT_TYPE;

static int      nt_parse(const char *text, size_t text_size, NT_PARSER *);
static NT_NODE *nt_parser_create_node(NT_PARSER *);
static void     nt_parser_set_memory(NT_PARSER *, NT_NODE *nodes, size_t count);
static void     nt_node_reverse(NT_NODE *);
static void     nt_node_set_data(NT_NODE *, const char *data, size_t size);
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
        NT_NODE *   root;
        size_t      count;
    } node;

    struct {
        NT_NODE *   nodes;
        size_t      count;
    } memory;

    struct {
        bool debug:1;
    } bitset;
} NT_PARSER;

static inline void nt_parser_set_memory(
    NT_PARSER *parser, NT_NODE *nodes, size_t count
) {
    parser->memory.nodes = nodes;
    parser->memory.count = count;
}

static inline int nt_parse(const char *str, size_t str_sz, NT_PARSER *parser) {
    parser->node.count = 0;
    parser->node.root = nt_parser_create_node(parser);

    const char *s = str;
    str_sz = str_sz ? str_sz : strlen(s);

    while (*s && s < str + str_sz) {
        const char *next = nt_parser_deserialize(
            parser, s, str_sz - nt_long_to_size(s - str), 0, parser->node.root
        );

        if (next == nullptr) {
            return -1;
        }

        if (next == s) {
            break;
        }

        s = next;
    }

    if (parser->node.root) {
        nt_node_reverse(parser->node.root);
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

    if (after_spaces - str == nt_size_to_long(line_size)) {
        // Empty lines are ignored.
        return next_line;
    }

    if (spaces < indent) {
        return str;
    }

    if (parser->bitset.debug) {
        nt_print_log("%.*s\n", nt_size_to_int(line_size), str);
    }

    NT_NODE *node = nullptr;

    if (*after_spaces == '#') {
        if (parent) {
            node = nt_parser_create_node(parser);

            if (node != nullptr) {
                node->type = NT_COMMENT;

                nt_node_set_data(
                    node, after_spaces + 1, line_size - (spaces + 1)
                );

                nt_node_to_node(node, parent);
            }
        }

        return next_line;
    }
    else if (*after_spaces == '-') {
        if (parent) {
            node = nt_parser_create_node(parser);

            if (node != nullptr
            && after_spaces + 1 < str + line_size && after_spaces[1] == ' ') {
                nt_node_set_data(
                    node, after_spaces + 2, line_size - (spaces + 2)
                );
            }

            if (node != nullptr) {
                nt_node_to_node(node, parent);
            }
        }
    }
    else if (parent) {
        const char *key_op = nt_str_seg_skip_key(
            after_spaces, line_size - spaces
        );

        if (key_op == after_spaces) {
            // Could not determine a valid key.
            if (parser->bitset.debug) {
                nt_print_log(
                    "%lu, %lu, [%.*s]", line_size, spaces,
                    nt_size_to_int(line_size - spaces), after_spaces
                );
            }

            return nullptr;
        }

        node = nt_parser_create_node(parser);

        if (node) {
            node->type = NT_KEY;

            nt_node_set_data(
                node, after_spaces, nt_long_to_size(key_op - after_spaces)
            );
        }

        if (key_op[1] == ' ') {
            const char *after_key_op = nt_str_seg_skip_key_op(
                key_op, line_size - nt_long_to_size(key_op - str)
            );

            if (after_key_op == key_op) {
                // Could not determine a valid value.
                return nullptr;
            }

            NT_NODE *val = nt_parser_create_node(parser);

            if (val != nullptr) {
                val->type = NT_REST_OF_LINE;

                nt_node_set_data(
                    val, after_key_op,
                    line_size - nt_long_to_size(after_key_op - str)
                );

                nt_node_to_node(val, node);
            }
        }
    }

    ++spaces;

    const char *s = next_line;

    while (*s && s < str + str_sz) {
        next_line = nt_parser_deserialize(
            parser, s, str_sz - nt_long_to_size(s - str), spaces, node
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

    if (node) {
        nt_node_reverse(node);
        nt_node_to_node(node, parent);
    }

    return s;
}

static NT_NODE *nt_parser_create_node(NT_PARSER *parser) {
    static NT_NODE zero_node;
    NT_NODE *node = nullptr;

    if (parser->node.count < parser->memory.count) {
        node = &parser->memory.nodes[parser->node.count];
        *node = zero_node;
    }

    parser->node.count++;

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

static void nt_node_to_node(NT_NODE *node, NT_NODE *container) {
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
                    break;
                }
            }
            else {
                if (*s == ' ' || *s == '\n') {
                    return next;
                }
                else break;
            }
        }
        else break;

        s = next;
    }

    return str;
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
