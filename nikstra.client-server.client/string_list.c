#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "string_list.h"

StringList* new_StringList()
{
	StringList* list = malloc(sizeof(StringList));
	if (list != NULL)
	{
		list->list = malloc(INIT_SIZE * sizeof(char*));
		if (list->list != NULL)
		{
			list->capacity = INIT_SIZE;
			list->count = 0;
			return list;
		}
	}

	return NULL;
}

char* StringList_add(StringList* self, const char* item)
{
	if (self->count == self->capacity)
	{
		int new_capacity = self->capacity * 2;
		char** tmp_list = realloc(self->list, new_capacity * sizeof(char));
		if (tmp_list == NULL)
		{
			return NULL;
		}

		self->list = tmp_list;
		self->capacity = new_capacity;
	}

	int length = strlen(item) + 1;
	char* tmp_item = malloc(length);
	if (tmp_item == NULL)
	{
		return NULL;
	}
	
	strcpy_s(tmp_item, length, item);
	self->list[self->count++] = tmp_item;
	return tmp_item;
}

char* StringList_item_at(StringList* self, int pos)
{
	if (pos > self->count || pos < 0)
	{
		return NULL;
	}

	return self->list[pos];
}

void StringList_clear(StringList* self)
{
	/* TODO: Don't free string literals. */
	for (int i = 0; i < self->count; i++)
	{
		free(self->list[i]);
	}
}

void StringList_destroy(StringList* self)
{
	StringList_clear(self);
	free(self);
}
