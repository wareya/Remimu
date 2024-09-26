#include "my_regex.h"

int main(void)
{
    RegexToken tokens[1024];
    int16_t token_count = 1024;
    int e = regex_parse("[0-9]+\\.[0-9]+", tokens, &token_count, 0);
    if (e) return (puts("regex has error"), 0);
    print_regex_tokens(tokens);
    
    int64_t match_len = regex_match(tokens, "23.53) ", 0, 0, 0, 0);
    printf("########### return: %zd\n", match_len);
    
    return 0;
}
