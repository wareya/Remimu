# Remimu: Single-Header C/C++ Regex Library

Compatible with C99 and C++11 and later standards. Uses backtracking and relatively standard regex syntax.

    #include "my_regex.h"

## Functions

    // Returns 0 on success, or -1 on invalid or unsupported regex, or -2 on not enough tokens given to parse regex.
    static inline int regex_parse(
        const char * pattern,       // Regex pattern to parse.
        RegexToken * tokens,        // Output buffer of token_count regex tokens.
        int16_t * token_count,      // Maximum allowed number of tokens to write
        int32_t flags               // Optional bitflags.
    )
    
    // Returns match length, or -1 on no match, or -2 on out of memory, or -3 if the regex is invalid.
    static inline int64_t regex_match(
        const RegexToken * tokens,  // Parsed regex to match against text.
        const char * text,          // Text to match against tokens.
        size_t start_i,             // index value to match at.
        uint16_t cap_slots,         // Number of allowed capture info output slots.
        int64_t * cap_pos,          // Capture position info output buffer.
        int64_t * cap_span          // Capture length info output buffer.
    ) 
    
    static inline void print_regex_tokens(
        RegexToken * tokens     // Regex tokens to spew to stdout, for debugging.
    )

Remimu doesn't have a searching API.

## Performance

On simple cases, Remimu's match speed is similar to PCRE2. Regex parsing/compilation is also much faster (around 4x to 10x), so single-shot regexes are often faster than PCRE2.

HOWEVER: Remimu is a pure backtracking engine, and has `O(2^x)` complexity on regexes with catastrophic backtracking. It can be much, much, MUCH slower than PCRE2. Beware!

Remimu uses length-checked fixed memory buffers with no recursion, so memory usage is statically known.

## Features

- Lowest-common-denominator common regex syntax
- Based on backtracking (slow in the worst case, but fast in the best case)
- 8-bit only, no utf-16 or utf-32
- Statically known memory usage (no heap allocation or recursion)
- Groups with or without capture, and with or without quantifiers
- Supported escapes:
- - 2-digit hex: e.g. `\x00`, `\xFF`, or lowercase, or mixed case
- - `\r`, `\n`, `\t`, `\v`, `\f` (whitespace characters)
- - `\d`, `\s`, `\w`, `\D`, `\S`, `\W` (digit, space, and word character classes)
- - `\b`, `\B` word boundary and non-word-boundary anchors (not fully supported in zero-size quantified groups, but even then, usually supported)
- - Escaped literal characters: `{}[]-()|^$*+?:./\`
- - - Escapes work in character classes, except for `b`
- Character classes, including disjoint ranges, proper handling of bare `[` and trailing `-`, etc
- - Dot (`.`) matches all characters, including newlines, unless `REMIMU_FLAG_DOT_NO_NEWLINES` is passed as a flag to `regex_parse`
- - Dot (`.`) only matches at most one byte at a time, so matching `\r\n` requires two dots (and not using `REMIMU_FLAG_DOT_NO_NEWLINES`)
- Anchors (`^` and `$`)
- - Same support caveats as \b, \B apply
- Basic quantifiers (`*`, `+`, `?`)
- - Quantifiers are greedy by default.
- Explicit quantifiers (`{2}`, `{5}`, `{5,}`, `{5,7}`)
- Alternation e.g. `(asdf|foo)`
- Lazy quantifiers e.g. `(asdf)*?` or `\w+?`
- Possessive greedy quantifiers e.g. `(asdf)*+` or `\w++`
- - NOTE: Capture groups for and inside of possessive groups return no capture information.
- Atomic groups e.g. `(?>(asdf))`
- - NOTE: Capture groups inside of atomic groups return no capture information.

## Not Supported

- Strings with non-terminal null characters
- Unicode character classes (matching single utf-8 characters works regardless)
- Exact POSIX regex semantics (posix-style greediness etc)
- Backreferences
- Lookbehind/Lookahead
- Named groups
- Most other weird flavor-specific regex stuff
- Capture of or inside of possessive-quantified groups (still take up a capture slot, but no data is returned)

## Usage

    // minimal:
    
    RegexToken tokens[1024];
    int16_t token_count = 1024;
    int e = regex_parse("[0-9]+\\.[0-9]+", tokens, &token_count, 0);
    assert(!e);
    
    int64_t match_len = regex_match(tokens, "23.53) ", 0, 0, 0, 0);
    printf("########### return: %zd\n", match_len);
    
    // with captures:
    
    RegexToken tokens[256];
    int16_t token_count = sizeof(tokens)/sizeof(tokens[0]);
    int e = regex_parse("((a)|(b))++", tokens, &token_count, 0);
    assert(!e);
    
    int64_t cap_pos[5];
    int64_t cap_span[5];
    memset(cap_pos, 0xFF, sizeof(cap_pos));
    memset(cap_span, 0xFF, sizeof(cap_span));
    
    int64_t matchlen = regex_match(tokens, "aaaaaabbbabaqa", 0, 5, cap_pos, cap_span);
    printf("Match length: %zd\n", matchlen);
    for (int i = 0; i < 5; i++)
        printf("Capture %d: %zd plus %zd\n", i, cap_pos[i], cap_span[i]);
        
    // for debugging
    print_regex_tokens(tokens);

## Testing

`my_regex_tests.cpp` is a C++11 program that throws a matrix of regexes and test strings into PCRE2 and validates that they're matched the same way in Remimu (for supported features). It contains a good number of gotcha regexes.

## License

Creative Commons Zero, public domain.
