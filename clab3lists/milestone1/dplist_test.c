#define _GNU_SOURCE

#include "dplist.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct
{
    int id;
    char *name;
} my_element_t;

void *element_copy(void *element);
void element_free(void **element);
int element_compare(void *x, void *y);

void *element_copy(void *element)
{
    my_element_t *copy = malloc(sizeof(my_element_t));
    assert(copy != NULL);
    char *new_name;
    asprintf(&new_name, "%s", ((my_element_t *)element)->name); // asprintf requires _GNU_SOURCE
    copy->id = ((my_element_t *)element)->id;
    copy->name = new_name;
    return (void *)copy;
}

void element_free(void **element)
{
    free((((my_element_t *)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void *x, void *y)
{
    return ((((my_element_t *)x)->id < ((my_element_t *)y)->id) ? -1 : (((my_element_t *)x)->id == ((my_element_t *)y)->id) ? 0
                                                                                                                            : 1);
}

void setup(void)
{
    // Implement pre-test setup
}

void teardown(void)
{
    // Implement post-test teardown
}

START_TEST(test_ListFree)
{
    // Test free NULL, don't use callback
    dplist_t *list = NULL;
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free NULL, use callback
    list = NULL;
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free empty list, don't use callback
    list = dpl_create(element_copy, element_free, element_compare);
    dpl_free(&list, false);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // Test free empty list, use callback
    list = dpl_create(element_copy, element_free, element_compare);
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // TODO : Test free with one element, also test if inserted elements are set to NULL
    list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 0, true);
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // TODO : Test free with multiple element, also test if inserted elements are set to NULL
    list = dpl_create(element_copy, element_free, element_compare);
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 0, true);
    element = malloc(sizeof(my_element_t));
    element->id = 2;
    element->name = "B";
    dpl_insert_at_index(list, element, 1, true);
    dpl_free(&list, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
}
END_TEST

START_TEST(test_ListInsertAtIndexListNULL)
{
    // Test inserting at index -1
    dplist_t *list = NULL;
    my_element_t *element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, -1, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // TODO : Test inserting at index 0
    list = NULL;
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 0, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // TODO : Test inserting at index 99
    list = NULL;
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 99, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
}
END_TEST

START_TEST(test_ListInsertAtIndexListEmpty)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, -1, true);

    // TODO : Test inserting at index 0
    list = dpl_create(element_copy, element_free, element_compare);
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 0, true);

    // TODO : Test inserting at index 99
    list = dpl_create(element_copy, element_free, element_compare);
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 99, true);
}
END_TEST

START_TEST(test_ListInsertAtIndexListNotEmpty)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, -1, true);

    // TODO : Test inserting at index 0
    list = dpl_create(element_copy, element_free, element_compare);
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 0, true);

    // TODO : Test inserting at index 99
    list = dpl_create(element_copy, element_free, element_compare);
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 99, true);
}
END_TEST

START_TEST(test_ListRemoveAtIndexListNULL)
{
    // Test inserting at index -1
    dplist_t *list = NULL;
    my_element_t *element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, -1, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // TODO : Test inserting at index 0
    list = NULL;
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 0, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

    // TODO : Test inserting at index 99
    list = NULL;
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 99, true);
    ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
}
END_TEST

START_TEST(test_ListRemoveAtIndexListEmpty)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, -1, true);

    // TODO : Test inserting at index 0
    list = dpl_create(element_copy, element_free, element_compare);
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 0, true);

    // TODO : Test inserting at index 99
    list = dpl_create(element_copy, element_free, element_compare);
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 99, true);
}
END_TEST

START_TEST(test_ListRemoveAtIndexListNotEmpty)
{
    // Test inserting at index -1
    dplist_t *list = dpl_create(element_copy, element_free, element_compare);
    my_element_t *element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, -1, true);

    // TODO : Test inserting at index 0
    list = dpl_create(element_copy, element_free, element_compare);
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 0, true);

    // TODO : Test inserting at index 99
    list = dpl_create(element_copy, element_free, element_compare);
    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 99, true);
}
END_TEST

START_TEST(test_ListSize)
{
    dplist_t *list = NULL;
    int size = dpl_size(list);
    ck_assert_msg(size == -1, "Failure: expected result to be -1");

    list = dpl_create(element_copy, element_free, element_compare);
    size = dpl_size(list);
    ck_assert_msg(size == 0, "Failure: expected result to be 0");

    my_element_t *element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 0, true);
    size = dpl_size(list);
    ck_assert_msg(size == 1, "Failure: expected result to be 1");
}
END_TEST

START_TEST(test_ListGetElementAtIndex)
{
    dplist_t *list = NULL;
    my_element_t *element = dpl_get_element_at_index(list, 0);
    ck_assert_msg(element == NULL, "Failure: expected result to be NULL");

    list = dpl_create(element_copy, element_free, element_compare);
    element = dpl_get_element_at_index(list, 0);
    ck_assert_msg(element == NULL, "Failure: expected result to be NULL");

    element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 0, true);
    element = dpl_get_element_at_index(list, 0);
    ck_assert_msg(element != NULL, "Failure: expected result to be not NULL");
}
END_TEST

START_TEST(test_ListGetIndexOfElement)
{
    dplist_t *list = NULL;
    int index = dpl_get_index_of_element(list, NULL);
    ck_assert_msg(index == -1, "Failure: expected result to be -1");

    list = dpl_create(element_copy, element_free, element_compare);
    index = dpl_get_index_of_element(list, NULL);
    ck_assert_msg(index == -1, "Failure: expected result to be -1");

    my_element_t *element = malloc(sizeof(my_element_t));
    element->id = 1;
    element->name = "A";
    dpl_insert_at_index(list, element, 0, true);
    index = dpl_get_index_of_element(list, element);
    ck_assert_msg(index == 0, "Failure: expected result to be 0");
}
END_TEST

START_TEST(test_ListGetReferenceAtIndex)
{
    // TODO : Test getting reference at index -1 from NULL list
    dplist_t *list = NULL;
    dplist_node_t *node = dpl_get_reference_at_index(list, 0);
    ck_assert_msg(node == NULL, "Failure: expected result to be NULL");

    // TODO : Test getting reference at index 0 from empty list
    list = dpl_create(element_copy, element_free, element_compare);
    node = dpl_get_reference_at_index(list, 0);
    ck_assert_msg(node == NULL, "Failure: expected result to be NULL");
}
END_TEST

START_TEST(test_ListGetElementAtReference)
{
    dplist_t *list = NULL;
    my_element_t *element = dpl_get_element_at_reference(list, NULL);
    ck_assert_msg(element == NULL, "Failure: expected result to be NULL");

    list = dpl_create(element_copy, element_free, element_compare);
    element = dpl_get_element_at_reference(list, NULL);
    ck_assert_msg(element == NULL, "Failure: expected result to be NULL");
}
END_TEST

int main(void)
{
    TCase *tc1 = tcase_create("test_ListFree");
    tcase_add_test(tc1, test_ListFree);
    tcase_add_checked_fixture(tc1, setup, teardown);

    TCase *tc2 = tcase_create("test_ListInsert");
    tcase_add_test(tc2, test_ListInsertAtIndexListNULL);
    tcase_add_test(tc2, test_ListInsertAtIndexListEmpty);
    tcase_add_test(tc2, test_ListInsertAtIndexListNotEmpty);
    tcase_add_checked_fixture(tc2, setup, teardown);

    TCase *tc3 = tcase_create("test_ListRemove");
    tcase_add_test(tc3, test_ListRemoveAtIndexListNULL);
    tcase_add_test(tc3, test_ListRemoveAtIndexListEmpty);
    tcase_add_test(tc3, test_ListRemoveAtIndexListNotEmpty);
    tcase_add_checked_fixture(tc3, setup, teardown);

    TCase *tc4 = tcase_create("test_ListSize");
    tcase_add_test(tc4, test_ListSize);
    tcase_add_checked_fixture(tc4, setup, teardown);

    TCase *tc5 = tcase_create("test_ListGet");
    tcase_add_test(tc5, test_ListGetElementAtIndex);
    tcase_add_test(tc5, test_ListGetIndexOfElement);
    tcase_add_test(tc5, test_ListGetReferenceAtIndex);
    tcase_add_test(tc5, test_ListGetElementAtReference);
    tcase_add_checked_fixture(tc5, setup, teardown);

    Suite *s1 = suite_create("LIST_MILESTONE_1");
    suite_add_tcase(s1, tc1);
    suite_add_tcase(s1, tc2);
    suite_add_tcase(s1, tc3);
    suite_add_tcase(s1, tc4);
    suite_add_tcase(s1, tc5);

    SRunner *sr = srunner_create(s1);
    srunner_run_all(sr, CK_VERBOSE);

    int nf;
    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
