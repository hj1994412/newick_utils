#include <string.h>
#include <stdio.h>

#include "hash.h"
#include "list.h"
#include "masprintf.h"

struct test_data {
	int value;
	char *name;
};

int test_simple()
{
	char *test_name = "test_simple";
	char *test_value = "a string datum";
	struct hash *h;
	struct test_data td1 = { 0, "myname"} ;

	h = create_hash(100);

	hash_set(h, "one", test_value);
	hash_set(h, "td1", &td1);

	if (2 != h->count) {
		printf ("%s: expected hash count to be 2, got %d.\n", test_name,
				h->count);
		return 1;
	}

	char *str = (char *) hash_get(h, "one");
	if (0 != strcmp(str, test_value)) {
		printf ("%s: expected %s, got %s.\n", test_name, test_value,
				str);
		return 1;
	}
	struct test_data *tdr = (struct test_data *) hash_get(h, "td1");
	if (0 != tdr->value) {
		printf ("%s: expected 0, got %d.\n", test_name, tdr->value);
		return 1;
	}
	if (0 != strcmp("myname", tdr->name)) {
		printf ("%s: expected 'myname', got '%s'.\n", test_name, tdr->name);
		return 1;
	}
	printf ("%s ok.\n", test_name);
	return 0;
}

int llist_find_string(void *s1, void *s2)
{
	return (! strcmp((char *) s1, (char *) s2));
}

int test_keys()
{
	char *test_name = "test_keys";

	struct hash *h = create_hash(4);

	hash_set(h, "one", "uno");
	hash_set(h, "two", "dos");
	hash_set(h, "three", "tres");
	hash_set(h, "four", "cuatro");
	hash_set(h, "five", "cinco"); 	/* one more elem than hash size - forces clash */

	struct llist *keys = hash_keys(h);

	if (h->count != keys->count) {
		printf ("%s: hash count and hash key count do not match.\n",
				test_name);
		return 1;
	}
	if (5 != keys->count) {
		printf ("%s: expected 5 keys, got %d.\n", test_name, keys->count);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "one")) {
		printf ("%s: 'one' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "two")) {
		printf ("%s: 'two' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "three")) {
		printf ("%s: 'three' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "four")) {
		printf ("%s: 'four' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "five")) {
		printf ("%s: 'five' should be among the keys.\n", test_name);
		return 1;
	}

	printf ("%s ok.\n", test_name);
	return 0;
}

int test_destroy()
{
	char *test_name = "test_destroy";
	int i;
	/* Do this several times in a row: it's more likely that any error will
	 * cause a problem. */
	for (i = 0; i < 100; i++) {
		struct hash *h = create_hash(4);
		hash_set(h, "one", "uno");
		hash_set(h, "two", "dos");
		hash_set(h, "three", "tres");
		hash_set(h, "four", "cuatro");
		/* one more elem than hash size - forces clash */
		hash_set(h, "five", "cinco"); 	
		destroy_hash(h);
		/* would be nce to check memory, but I don't know how to do this. */
	}

	printf ("%s ok.\n", test_name);
	return 0;
}

int test_make_hash_key()
{
	char *test_name = "test_make_hash_key";
	struct llist *list = create_llist();
	char *list_address = masprintf("%p", list);

	if (strcmp(list_address, make_hash_key(list)) != 0) {
		printf ("%s: expected key %s, got %s.\n", test_name,
				list_address, make_hash_key(list));
		return 1;
	}

	printf ("%s ok.\n", test_name);
	return 0;
}

int test_resize()
{
	const char *test_name = __func__;

	struct hash *h = create_hash(4);
	double load;

	hash_set(h, "one", "uno");
	hash_set(h, "two", "dos");
	hash_set(h, "three", "tres");
	hash_set(h, "four", "cuatro");
	hash_set(h, "five", "cinco"); 	/* one more elem than hash size - forces clash */

	struct llist *keys = hash_keys(h);

	if (h->count != keys->count) {
		printf ("%s: hash count and hash key count do not match.\n",
				test_name);
		return 1;
	}
	if (5 != keys->count) {
		printf ("%s: expected 5 keys, got %d.\n", test_name, keys->count);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "one")) {
		printf ("%s: 'one' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "two")) {
		printf ("%s: 'two' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "three")) {
		printf ("%s: 'three' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "four")) {
		printf ("%s: 'four' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "five")) {
		printf ("%s: 'five' should be among the keys.\n", test_name);
		return 1;
	}

	load = load_factor(h);
	if (1.25 != load) {
		printf ("%s: load factor should be 1.2, but is %.2f\n",
				test_name, load);
		return 1;
	}
	/* Now, resize the hash */

	load = resize_hash(h, 10);
	if (0.5 != load) {
		printf ("%s: load factor should be 0.5, but is %.2f\n",
				test_name, load);
		return 1;
	}

	/* Same tests on the resized list */

	keys = hash_keys(h);

	if (h->count != keys->count) {
		printf ("%s: hash count and hash key count do not match.\n",
				test_name);
		return 1;
	}
	if (5 != keys->count) {
		printf ("%s: expected 5 keys, got %d.\n", test_name, keys->count);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "one")) {
		printf ("%s: 'one' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "two")) {
		printf ("%s: 'two' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "three")) {
		printf ("%s: 'three' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "four")) {
		printf ("%s: 'four' should be among the keys.\n", test_name);
		return 1;
	}
	if (-1 == llist_index_of_f(keys, llist_find_string, "five")) {
		printf ("%s: 'five' should be among the keys.\n", test_name);
		return 1;
	}

	printf ("%s ok.\n", test_name);
	return 0;

}

int test_self_resizing()
{
	const char *test_name = __func__;

	struct hash *h = create_dynamic_hash(4, 0.75, 2);
	double load;

	load = load_factor(h);
	if (0.0 != load) {
		printf ("%s: load factor should be 0.0, but is %.2f\n",
				test_name, load);
		return 1;
	}

	hash_set(h, "one", "uno");
	hash_set(h, "two", "dos");
	hash_set(h, "three", "tres");

	load = load_factor(h);
	if (0.75 != load) {
		printf ("%s: load factor should be 0.75, but is %.2f\n",
				test_name, load);
		return 1;
	}

	hash_set(h, "four", "cuatro");

	load = load_factor(h);
	if (0.5 != load) {
		printf ("%s: load factor should be 0.5, but is %.2f\n",
				test_name, load);
		return 1;
	}

	hash_set(h, "five", "cinco");
	hash_set(h, "six", "seis");

	load = load_factor(h);
	if (0.75 != load) {
		printf ("%s: load factor should be 0.75, but is %.2f\n",
				test_name, load);
		return 1;
	}

	/*
	 * now check keys & values*/


	printf ("%s ok.\n", test_name);
	return 0;

}

int main()
{
	int failures = 0;
	printf("Starting hash test...\n");
	failures += test_simple();
	failures += test_keys();
	failures += test_destroy();
	failures += test_make_hash_key();
	failures += test_resize();
	failures += test_self_resizing();
	if (0 == failures) {
		printf("All tests ok.\n");
	} else {
		printf("%d test(s) FAILED.\n", failures);
		return 1;
	}

	return 0;
}
