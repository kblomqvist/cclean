#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "functions.h"

int n;
enum SState state = { CODE };

void teardown()
{
	n = 0;
	state = CODE;
}

/** Test can parse C99 comment */
void test_can_parse_c99_comment()
{
	char foo[] = "// comment\nputc('k');";

	teardown();
	n = rm_comments(foo, &state);

	assert(strcmp(foo, "\nputc('k');") == 0);
	assert(n == 10);
}

/** Test can parse C99 endline comment */
void test_can_parse_c99_endline_comment()
{
	char foo[] = "putc('k'); // puts a";

	teardown();
	n = rm_comments(foo, &state);

	assert(strcmp(foo, "putc('k'); ") == 0);
	assert(n == 9);
}

/** Test can parse star comment */
void test_can_parse_star_comment()
{
	char foo[] = "/* comment */";

	teardown();
	n = rm_comments(foo, &state);

	assert(strcmp(foo, "") == 0);
}

/** Test can parse multiline star comment */
void test_can_parse_multiline_star_comment()
{
	char foo[] = "/* bar\nbaz */";

	teardown();
	n = rm_comments(foo, &state);

	assert(strcmp(foo, "") == 0);
}

/** Test can continue parsing */
void test_can_continue_parsing()
{
	char foo[] = "/* foo";
	char bar[] = "bar */ baz";

	teardown();

	rm_comments(foo, &state);
	assert(state == COMMENT);

	rm_comments(bar, &state);
	assert(state == CODE);

	assert(strcmp(foo, "") == 0);
	assert(strcmp(bar, " baz") == 0);
}

/** Test ignores comments internal to "string" */
void test_ignores_comments_internal_to_string()
{
	char foo[] = "puts(\"/* comment */\")";
	char bar[] = "puts(\"// comment\")";

	teardown();
	rm_comments(foo, &state);
	rm_comments(bar, &state);

	assert(strcmp(foo, "puts(\"/* comment */\")") == 0);
	assert(strcmp(bar, "puts(\"// comment\")") == 0);
}

/** Test ignores escaped quotation mark when in string */
void test_ignores_escaped_quotationmark_when_in_string()
{
	char foo[] = "puts(\"foo and \\\"/*comment*/\\\"\")";

	teardown();
	rm_comments(foo, &state);

	assert(strcmp(foo, "puts(\"foo and \\\"/*comment*/\\\"\")") == 0);
}

/** Test ignores char constants which are quotation marks */
void test_ignores_quotationmark_char_constants()
{
	char foo[] = "if (char == '\"')";

	teardown();
	rm_comments(foo, &state);

	assert(state == CODE);
}

/** Test suite, run all test */
int main(int argc, char *argv[])
{
	test_can_parse_c99_comment();
	test_can_parse_c99_endline_comment();
	
	test_can_parse_star_comment();
	test_can_parse_multiline_star_comment();
	test_can_continue_parsing();

	test_ignores_comments_internal_to_string();
	test_ignores_escaped_quotationmark_when_in_string();
	test_ignores_quotationmark_char_constants();

	puts("Ok.");

	return 0;
}
