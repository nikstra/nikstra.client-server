#ifndef STRING_LIST_H
#define STRING_LIST_H

#define INIT_SIZE 2

typedef struct string_list
{
	char** list;
	int capacity;
	int count;
} StringList;

StringList* new_StringList();
char* StringList_add(StringList* self, const char* item);
char* StringList_item_at(StringList* self, int pos);
void StringList_clear(StringList* self);
void StringList_destroy(StringList* self);

#endif /* !STRING_LIST_H */
