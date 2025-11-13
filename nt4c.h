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

#ifndef NT_PARSER_NCOUNT
#define NT_PARSER_NCOUNT 8
#endif

typedef struct NT_NODE      NT_NODE;
typedef struct NT_PARSER    NT_PARSER;

typedef enum : uint32_t {
    NT_NONE = 0,
    ////////////////////////////////////////////////////////////////////////////
    NT_TOP_DCT      = 1 <<  0,  // root node contains a dictionary
    NT_TOP_LST      = 1 <<  1,  // root node contains a list
    NT_TOP_MLS      = 1 <<  2,  // root node contains a multiline string
    NT_TOP_NIL      = 1 <<  3,  // root node does not hold any meaningful data
    NT_KEY_ROL      = 1 <<  4,  // name of the key for a rest-of-line string
    NT_KEY_MLS      = 1 <<  5,  // name of the key for a multiline string
    NT_KEY_LST      = 1 <<  6,  // name of the key for the following list
    NT_KEY_DCT      = 1 <<  7,  // name of the key for the following dictionary
    NT_KEY_NIL      = 1 <<  8,  // name of the key for missing value
    NT_SET_ROL      = 1 <<  9,  // node references a rest-of-line assigment
    NT_SET_MLS      = 1 << 10,  // node references a multiline assigment
    NT_SET_LST      = 1 << 11,  // node references a list assigment
    NT_SET_DCT      = 1 << 12,  // node references a dictionary assigment
    NT_SET_NIL      = 1 << 13,  // node references a nil assignment
    NT_TAG_MLS      = 1 << 14,  // node references the tag of a multiline string
    NT_TAG_COM      = 1 << 15,  // node references the tag of a comment line
    NT_TAG_LST_ROL  = 1 << 16,  // tag of the enlisted rest-of-line string
    NT_TAG_LST_MLS  = 1 << 17,  // tag of the enlisted multiline string
    NT_TAG_LST_LST  = 1 << 18,  // tag of the enlisted sublist
    NT_TAG_LST_DCT  = 1 << 19,  // tag of the enlisted dictionary
    NT_TAG_LST_NIL  = 1 << 20,  // tag of the enlisted nil value
    NT_STR_ROL      = 1 << 21,  // node references a rest-of-line string
    NT_STR_MLN      = 1 << 22,  // node references a multiline string
    NT_STR_COM      = 1 << 23,  // node references a comment string
    NT_NEWLINE      = 1 << 24,  // node references the new line data
    NT_SPACE        = 1 << 25,  // node references the (indentation) spaces
    NT_INVALID      = 1 << 26,  // node references a segment of invalid input
    NT_DEEP         = 1 << 27   // node that exceeds the maximum nesting depth
} NT_TYPE;

// Public API: /////////////////////////////////////////////////////////////////
static int      nt_parse(const char *text, size_t text_size, NT_PARSER *);
static void     nt_parser_set_memory(NT_PARSER *, NT_NODE *nodes, size_t count);
static void     nt_parser_set_recursion(NT_PARSER *, size_t depth);
static void     nt_parser_set_blacklist(NT_PARSER *, NT_TYPE blacklist);
static void     nt_parser_set_whitelist(NT_PARSER *, NT_TYPE whitelist);
////////////////////////////////////////////////////////////////////////////////

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
    size_t depth; // Maximum nesting depth of the structure being decoded.

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
        NT_NODE     integrated[NT_PARSER_NCOUNT];
        NT_NODE *   nodes;
        size_t      capacity;
    } memory;

    struct {
        bool debug:1;
        bool nomem:1;
    } bitset;
} NT_PARSER;

static NT_NODE *nt_parser_create_node(NT_PARSER *);
static NT_NODE *nt_parser_create_node_type(NT_PARSER *, NT_TYPE);
static bool     nt_parser_allows_all(NT_PARSER *, NT_TYPE);
static bool     nt_parser_allows_any(NT_PARSER *, NT_TYPE);
static void     nt_node_reverse(NT_NODE *);
static void     nt_node_set_data(NT_NODE *, const char *data, size_t size);
static void     nt_node_set_type(NT_NODE *, NT_TYPE);
static void     nt_node_to_node(NT_NODE *, NT_NODE *to);
static NT_NODE *nt_node_from_node(NT_NODE *node);

static const char *nt_parser_deserialize(
    NT_PARSER *, const char *str, size_t str_sz, size_t indent, NT_NODE *parent,
    size_t depth
);

static const char *nt_str_seg_skip_spaces(const char *str, size_t str_sz);
static const char *nt_str_seg_first_line_size(
    const char *str, size_t str_sz, size_t *line_size
);
static const char *nt_str_seg_skip_string_tag(const char *str, size_t str_sz);
static const char *nt_str_seg_skip_list_tag(const char *str, size_t str_sz);
static const char *nt_str_seg_skip_key(const char *str, size_t str_sz);
static const char *nt_str_seg_skip_key_op(const char *str, size_t str_sz);
static const char *nt_str_seg_skip_byte(const char *str, size_t str_sz);
static const char *nt_str_seg_skip_nest(const char *str, size_t str_sz, size_t indent);

static void     nt_print_log(char *fmt, ...);
static size_t   nt_long_to_size(long);
static long     nt_size_to_long(size_t);
static int      nt_size_to_int(size_t);

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

static inline void nt_parser_set_recursion(NT_PARSER *parser, size_t depth) {
    parser->depth = depth;
}

static inline bool nt_parser_allows_all(NT_PARSER *parser, NT_TYPE types) {
    return !(types & parser->node.blacklist);
}

static inline bool nt_parser_allows_any(NT_PARSER *parser, NT_TYPE types) {
    return !((types & parser->node.blacklist) == types);
}

static inline int nt_parse(const char *str, size_t str_sz, NT_PARSER *parser) {
    constexpr NT_TYPE top_collections = (
        NT_TOP_DCT | NT_TOP_LST | NT_TOP_MLS | NT_TOP_NIL
    );

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

    NT_NODE tmp_node = {};
    NT_NODE *top_node = nt_parser_create_node(parser);

    if (top_node) {
        parser->nest.root = top_node;
    }
    else {
        top_node = &tmp_node;
    }

    nt_node_set_type(top_node, top_collections);

    const char *s = str;
    str_sz = str_sz ? str_sz : strlen(s);

    while (*s && s < str + str_sz) {
        const char *next = nt_parser_deserialize(
            parser, s, str_sz - nt_long_to_size(s - str), 0, top_node, 1
        );

        if (next == nullptr) {
            return -1; // Parse error.
        }

        if (next == s) {
            break;
        }

        s = next;
    }

    if (top_node->type == NT_TOP_MLS) {
        NT_NODE *newline = nt_parser_create_node_type(
            parser, NT_NEWLINE // Always increase the node count for estimation.
        );

        for (NT_NODE *child = top_node->children; child; child = child->next) {
            if (child->type != NT_STR_MLN) {
                continue;
            }

            // The last multiline string must not have a newline in the end.

            size_t last_line_sz;
            nt_str_seg_first_line_size(child->data, child->size, &last_line_sz);

            const char *end_of_last_line = child->data + last_line_sz;
            size_t nlsz = child->size - last_line_sz;

            if (newline) {
                nt_node_set_data(newline, end_of_last_line, nlsz);
                nt_node_to_node(newline, top_node);
            }

            child->size = last_line_sz;
            nt_node_set_type(child, NT_STR_ROL);

            break;
        }
    }

    nt_node_reverse(top_node);

    if (top_node->type & NT_TOP_NIL) {
        // Top type remains ambiguous. Resolve it as nil.
        nt_node_set_type(top_node, NT_TOP_NIL);
    }

    if (!nt_parser_allows_all(parser, top_node->type)) {
        parser->node.count  = 0;
        parser->nest.begin  = nullptr;
        parser->nest.end    = nullptr;
        parser->nest.root   = nullptr;
    }

    return nt_size_to_int(parser->node.count);
}

static const char *nt_parser_deserialize(
    NT_PARSER *parser,
    const char *str, size_t str_sz, size_t indent, NT_NODE *parent, size_t depth
) {
    struct any_collection_type {
        NT_TYPE keys;
        NT_TYPE setters;
        NT_TYPE list_tags;
    };

    constexpr struct any_collection_type any_collection = {
        .keys = (
            NT_KEY_MLS | NT_KEY_LST | NT_KEY_DCT | NT_KEY_ROL | NT_KEY_NIL
        ),
        .setters = (
            NT_SET_MLS | NT_SET_LST | NT_SET_DCT | NT_SET_ROL | NT_SET_NIL
        ),
        .list_tags = (
            NT_TAG_LST_MLS | NT_TAG_LST_LST | NT_TAG_LST_DCT |
            NT_TAG_LST_ROL | NT_TAG_LST_NIL
        )
    };

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

    if (depth == parser->depth) {
        NT_NODE *node = nt_parser_create_node_type(parser, NT_DEEP);

        if (node) {
            nt_node_set_data(node, after_spaces, line_size - spaces);
            nt_node_to_node(node, parent);
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
    else if (*after_spaces == '#') {
        NT_NODE *tag_node = nt_parser_create_node_type(parser, NT_TAG_COM);

        if (tag_node) {
            nt_node_set_data(tag_node, after_spaces, 1);
            nt_node_to_node(tag_node, parent);
        }

        NT_NODE *val_node = nt_parser_create_node_type(parser, NT_STR_COM);

        if (val_node) {
            nt_node_set_data(
                val_node, after_spaces + 1, line_size - (spaces + 1)
            );

            nt_node_to_node(val_node, parent);
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
    else if (*after_spaces == '>') {
        const char *tag = after_spaces;
        const char *rol = nt_str_seg_skip_string_tag(tag, line_size - spaces);
        bool invalid = false;

        if (tag == rol) {
            invalid = true;
        }
        else {
            if (parent->type & NT_KEY_MLS) {
                nt_node_set_type(parent, NT_KEY_MLS);
            }
            else if (parent->type & NT_TAG_LST_MLS) {
                nt_node_set_type(parent, NT_TAG_LST_MLS);
            }
            else if (parent->type & NT_TOP_MLS) {
                nt_node_set_type(parent, NT_TOP_MLS);
            }
            else {
                invalid = true;
            }
        }

        if (invalid) {
            // Could not determine a valid multiline string tag.
            NT_NODE *node = nt_parser_create_node_type(parser, NT_INVALID);

            if (node) {
                nt_node_set_data(node, tag, line_size - spaces);
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

        NT_NODE *tag_node = nt_parser_create_node_type(parser, NT_TAG_MLS);
        size_t tag_sz = nt_long_to_size(rol - tag);

        if (tag_node) {
            nt_node_set_data(tag_node, after_spaces, tag_sz);
            nt_node_to_node(tag_node, parent);
        }

        NT_NODE *val_node = nt_parser_create_node_type(parser, NT_STR_MLN);

        if (val_node) {
            nt_node_set_data(
                val_node, after_spaces + tag_sz,
                nt_long_to_size(next_line - rol)
            );

            nt_node_to_node(val_node, parent);
        }

        return next_line;
    }

    bool skip_nest = false;
    NT_NODE *nest = nullptr;
    NT_TYPE nest_type = NT_NONE;
    size_t old_node_count = parser->node.count;

    if (*after_spaces == '-') {
        const char *tag = after_spaces;
        const char *rol = nt_str_seg_skip_list_tag(tag, line_size - spaces);
        bool invalid = false;

        if (tag == rol) {
            invalid = true;
        }
        else {
            if (parent->type & NT_KEY_LST) {
                nt_node_set_type(parent, NT_KEY_LST);
            }
            else if (parent->type & NT_TAG_LST_LST) {
                nt_node_set_type(parent, NT_TAG_LST_LST);
            }
            else if (parent->type & NT_TOP_LST) {
                nt_node_set_type(parent, NT_TOP_LST);
            }
            else {
                invalid = true;
            }
        }

        if (invalid) {
            // Could not determine a valid list tag.
            NT_NODE *node = nt_parser_create_node_type(parser, NT_INVALID);

            if (node) {
                nt_node_set_data(node, tag, line_size - spaces);
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

        size_t tag_sz = nt_long_to_size(rol - tag);
        NT_NODE *tag_node = nullptr;

        if (tag_sz == 2) {
            nest_type = NT_TAG_LST_ROL;
        }
        else {
            nest_type = any_collection.list_tags;
        }

        if (nt_parser_allows_any(parser, nest_type)) {
            tag_node = nt_parser_create_node(parser);

            if (tag_node) {
                nt_node_set_type(tag_node, nest_type);
            }
        }
        else skip_nest = true;

        if (tag_node) {
            nt_node_set_data(tag_node, tag, tag_sz);
        }

        if (tag_sz == 2) {
            NT_NODE *val_node = nt_parser_create_node_type(
                parser, NT_STR_ROL
            );

            if (val_node) {
                nt_node_set_data(
                    val_node, tag + tag_sz, line_size - (spaces + tag_sz)
                );

                nt_node_to_node(val_node, tag_node);
            }
        }

        nest = tag_node;
    }
    else {
        const char *key = after_spaces;
        const char *key_op = nt_str_seg_skip_key(key, line_size - spaces);
        const char *after_key_op = nullptr;
        bool invalid = false;

        if (key_op != key) {
            after_key_op = nt_str_seg_skip_key_op(
                key_op, line_size - nt_long_to_size(key_op - str)
            );
        }

        if (key_op == key || after_key_op == key_op) {
            invalid = true;
        }
        else {
            if (parent->type & NT_KEY_DCT) {
                nt_node_set_type(parent, NT_KEY_DCT);
            }
            else if (parent->type & NT_TAG_LST_DCT) {
                nt_node_set_type(parent, NT_TAG_LST_DCT);
            }
            else if (parent->type & NT_TOP_DCT) {
                nt_node_set_type(parent, NT_TOP_DCT);
            }
            else {
                invalid = true;
            }
        }

        if (invalid) {
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

        NT_NODE *key_node = nullptr;

        if (after_key_op && key_op[1] == ' ') {
            nest_type = NT_KEY_ROL;
        }
        else {
            nest_type = any_collection.keys;
        }

        if (nt_parser_allows_any(parser, nest_type)) {
            key_node = nt_parser_create_node(parser);

            if (key_node) {
                nt_node_set_type(key_node, nest_type);
            }
        }
        else skip_nest = true;

        if (key_node) {
            nt_node_set_data(
                key_node, key, nt_long_to_size(key_op - key)
            );
        }

        if (after_key_op) {
            NT_NODE *op_node = nullptr;

            if (after_key_op && key_op[1] == ' ') {
                op_node = nt_parser_create_node_type(parser, NT_SET_ROL);
            }
            else if (nt_parser_allows_any(parser, any_collection.setters)) {
                op_node = nt_parser_create_node(parser);

                if (op_node) {
                    nt_node_set_type(op_node, any_collection.setters);
                }
            }

            if (op_node) {
                nt_node_set_data(
                    op_node, key_op, nt_long_to_size(after_key_op - key_op)
                );

                nt_node_to_node(op_node, key_node);
            }
        }

        if (after_key_op && key_op[1] == ' ') {
            NT_NODE *val_node = nt_parser_create_node_type(parser, NT_STR_ROL);

            if (val_node) {
                nt_node_set_data(
                    val_node, after_key_op,
                    line_size - nt_long_to_size(after_key_op - str)
                );

                nt_node_to_node(val_node, key_node);
            }
        }

        nest = key_node;
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
    size_t next_depth = depth + 1;

    NT_NODE tmp_node = {
        .type = nest_type
    };

    if (!nest) {
        nest = &tmp_node;
    }

    while (*s && s < str + str_sz) {
        next_line = (
            skip_nest ? (
                nt_str_seg_skip_nest(
                    s, str_sz - nt_long_to_size(s - str), spaces
                )
            ) : (
                nt_parser_deserialize(
                    parser, s, str_sz - nt_long_to_size(s - str), spaces, nest,
                    next_depth
                )
            )
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

    if ((nest->type & (NT_KEY_MLS|NT_TAG_LST_MLS)) == nest->type) {
        NT_NODE *newline = nt_parser_create_node_type(
            parser, NT_NEWLINE // Always increase the node count for estimation.
        );

        for (NT_NODE *child = nest->children; child; child = child->next) {
            if (child->type != NT_STR_MLN) {
                continue;
            }

            // The last multiline string must not have a newline in the end.

            size_t last_line_sz;
            nt_str_seg_first_line_size(child->data, child->size, &last_line_sz);

            const char *end_of_last_line = child->data + last_line_sz;
            size_t nlsz = child->size - last_line_sz;

            if (newline) {
                nt_node_set_data(newline, end_of_last_line, nlsz);
                nt_node_to_node(newline, nest);
                newline = nullptr;
            }

            child->size = last_line_sz;
            nt_node_set_type(child, NT_STR_ROL);

            break;
        }

        if (newline) {
            abort(); // MLS containers must always contain at least one string.
        }
    }

    nt_node_reverse(nest);

    if (nest->type & (NT_KEY_NIL|NT_TAG_LST_NIL)) {
        // If the nest type remains ambiguous, default to nil.
        nt_node_set_type(nest, nest->type & (NT_KEY_NIL|NT_TAG_LST_NIL));
    }

    for (NT_NODE *child = nest->children; child; child = child->next) {
        if (child->type != any_collection.setters) {
            continue;
        }

        switch (nest->type) {
            case NT_KEY_NIL: {
                nt_node_set_type(child, NT_SET_NIL);
                break;
            }
            case NT_KEY_ROL: {
                nt_node_set_type(child, NT_SET_ROL);
                break;
            }
            case NT_KEY_MLS: {
                nt_node_set_type(child, NT_SET_MLS);
                break;
            }
            case NT_KEY_LST: {
                nt_node_set_type(child, NT_SET_LST);
                break;
            }
            case NT_KEY_DCT: {
                nt_node_set_type(child, NT_SET_DCT);
                break;
            }
            default: {
                nt_node_set_type(child, NT_INVALID);
                break;
            }
        }

        break;
    }

    if (!nt_parser_allows_all(parser, nest->type)) {
        // Since this nest is blacklisted, all of its children are too.
        // Therefore, the nest and its children must not add to the total node
        // count.

        parser->node.count = old_node_count;
        parser->nest.end = &parser->memory.nodes[parser->node.count];
    }
    else if (nest == &tmp_node) {
        if (nest->children) {
            // We use a temporary nest only if the parser does not have memory
            // for the creation of a persistent nest. That means there must also
            // not have been memory for the creation of any persistent children.
            // However, here we detected that the temporary nest has children.
            // This is a regression.

            abort();
        }
    }
    else {
        nt_node_to_node(nest, parent);

        for (NT_NODE *child = nest->children; child; child = child->next) {
            // If the nest ends with a newline node, relocate it to the end of
            // the current parent as trailing newline nodes are not considered
            // an integral part of the nest.

            if (child->next != nullptr) {
                continue;
            }

            if (child->type == NT_NEWLINE) {
                nt_node_from_node(child);
                nt_node_to_node(child, parent);
            }

            break;
        }
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
    if (!nt_parser_allows_all(parser, type)) {
        return nullptr;
    }

    NT_NODE *node = nt_parser_create_node(parser);

    if (node) {
        nt_node_set_type(node, type);
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

static const char *nt_str_seg_skip_string_tag(const char *str, size_t str_sz) {
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
                if (*s != '>') {
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

static const char *nt_str_seg_skip_list_tag(const char *str, size_t str_sz) {
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
                if (*s != '-') {
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

static const char *nt_str_seg_skip_nest(
    const char *str, size_t str_sz, size_t indent
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

    return next_line;
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
