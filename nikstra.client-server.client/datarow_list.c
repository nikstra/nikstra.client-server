#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "datarow_list.h"

DataRowList* new_DataRowList()
{
	DataRowList* list = malloc(sizeof(DataRowList));
	if (list != NULL)
	{
		list->list = malloc(INIT_SIZE * sizeof(DataRow*));
		if (list->list != NULL)
		{
			list->capacity = INIT_SIZE;
			list->count = 0;
			return list;
		}
	}

	return NULL;
}

DataRow* DataRowList_add(DataRowList* self, const DataRow* item)
{
	if (self->count == self->capacity)
	{
		int new_capacity = self->capacity * 2;
		DataRow** tmp_list = realloc(self->list, new_capacity * sizeof(DataRow*));
		if (tmp_list == NULL)
		{
			return NULL;
		}

		self->list = tmp_list;
		self->capacity = new_capacity;
	}

	DataRow* tmp_item = malloc(sizeof(DataRow));
	if (tmp_item == NULL)
	{
		return NULL;
	}
	
	/* TODO: Does this need a deep copy? */
	*tmp_item = *item;
	self->list[self->count++] = tmp_item;
	return tmp_item;
}

DataRow* DataRowList_item_at(DataRowList* self, int pos)
{
	if (pos > self->count || pos < 0)
	{
		return NULL;
	}

	return self->list[pos];
}

void DataRowList_clear(DataRowList* self)
{
	for (int i = 0; i < self->count; i++)
	{
		free(self->list[i]);
	}
}

void DataRowList_destroy(DataRowList* self)
{
	DataRowList_clear(self);
	free(self);
}
