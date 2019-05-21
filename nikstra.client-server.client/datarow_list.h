#ifndef DATAROW_LIST_H
#define DATAROW_LIST_H

#define INIT_SIZE 2

typedef struct data_row
{
	/* "%10s%18s%16s%5s%11s%11s\r\n" */
	char date[10 + 1];
	char externalid[18 + 1];
	char number[16 + 1];
	char constant[5 + 1];
	char digits[11 + 1];
	char decimals[11 + 1];
} DataRow;


typedef struct datarow_list
{
	DataRow** list;
	int capacity;
	int count;
} DataRowList;

DataRowList* new_DataRowList();
DataRow* DataRowList_add(DataRowList* self, const DataRow* item);
DataRow* DataRowList_item_at(DataRowList* self, int pos);
void DataRowList_clear(DataRowList* self);
void DataRowList_destroy(DataRowList* self);

#endif /* !DATAROW_LIST_H */
