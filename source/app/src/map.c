
//////////////////////////////////////////////////////////////////////
//                              Includes
//////////////////////////////////////////////////////////////////////

#include "map.h"
#include "storage.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////
//                             Macros
//////////////////////////////////////////////////////////////////////

#define MAP_TYPE_STR 0
#define MAP_TYPE_U32 1

//////////////////////////////////////////////////////////////////////
//                         Private Global Variables
//////////////////////////////////////////////////////////////////////

static uint16_t itemsInMap = 0;

//////////////////////////////////////////////////////////////////////
//                      Public Functions definition
//////////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the map module.
 */
int8_t map_init(map_entry_log_t* pMapLog)
{
	if (-1 == storage_init())
	{
		return -1;
	}

	if (-1 == map_read_log(pMapLog))
	{
		return -1;
	}

	return 0;
}

/**
 * @brief 
 */
int8_t map_store_all()
{
	return storage_flush();
}

/**
 * @brief Adds a new map entry with a string value.
 */
int8_t map_add_entry_val_str(const char* pKey, const char* pVal)
{
	map_entry_t entry;
	size_t		keyLen = strlen(pKey);
	size_t		valLen = strlen(pVal);

	if (keyLen > MAP_MAX_KEY_LEN || valLen > MAP_MAX_VAL_LEN_STR)
	{
		return -1;
	}

	memset(&entry, 0, sizeof(entry));

	entry.type			   = MAP_TYPE_STR;
	entry.entryDeletedFlag = ENTRY_NOT_DELETED_VALUE;
	strncpy(entry.key, pKey, MAP_MAX_KEY_LEN - 1);
	strncpy(entry.valueStr, pVal, MAP_MAX_VAL_LEN_STR - 1);
	entry.valueU32 = 0;

	if (-1 == storage_store_entry((void*)&entry, sizeof(entry)))
	{
		return -1;
	}

	return 0;
}

/**
 * @brief Adds a new map entry with a uint32_t value.
 */
int8_t map_add_entry_val_u32(const char* pKey, uint32_t valueU32)
{
	map_entry_t entry;
	size_t		keyLen = strlen(pKey);

	// Check parameters validity
	if (keyLen > MAP_MAX_KEY_LEN)
	{
		return -1;
	}

	memset(&entry, 0, sizeof(entry));

	entry.type = MAP_TYPE_U32;
	strncpy(entry.key, pKey, MAP_MAX_KEY_LEN - 1);
	entry.valueU32 = valueU32;

	if (-1 == storage_store_entry((void*)&entry, sizeof(entry)))
	{
		return -1;
	}

	return 0;
}

/**
 * @brief De-initializes the map, freeing allocated memory.
 */
int8_t map_deInit(map_entry_log_t* pMapLog)
{
	map_entry_log_t* current = pMapLog->next;
	map_entry_log_t* next;

	while (current != NULL)
	{
		next = current->next;

		free(current);

		current = next;
	}

	pMapLog->next = NULL;

	return storage_deInit();
}

/**
 * @brief Reads all entries from storage and populates the in-memory
 *        linked list.
 */
int8_t map_read_log(map_entry_log_t* pMapLog)
{
	map_entry_t		 entry;
	map_entry_log_t* pCurrentNode = pMapLog;
	uint32_t		 entryNum	  = 0;
	uint8_t			 firstEntry	  = 1;

	while (-1 != storage_retrieve_entry_payload((void*)&entry, sizeof(map_entry_t), entryNum))
	{
		if (firstEntry)
		{
			pCurrentNode->entry = entry;
			firstEntry			= 0;
		}
		else
		{
			pCurrentNode->next	= (map_entry_log_t*)malloc(sizeof(map_entry_log_t));
			pCurrentNode		= pCurrentNode->next;
			pCurrentNode->entry = entry;
		}

		itemsInMap++;
		entryNum++;
	}

	pCurrentNode->next = NULL;

	return 0;
}

/**
 * @brief Prints the contents of the in-memory map log.
 */
void map_print_log(map_entry_log_t* pMapLog)
{
	uint32_t entryNum = 0;

	while (pMapLog && itemsInMap > 0)
	{
		if (pMapLog->entry.type == MAP_TYPE_U32)
		{
			printf("Entry %d -> Key: %s, valueU32: %d\r\n", entryNum, pMapLog->entry.key, pMapLog->entry.valueU32);
		}
		else
		{
			printf("Entry %d -> Key: %s, valueStr: %s\r\n", entryNum, pMapLog->entry.key, pMapLog->entry.valueStr);
		}

		pMapLog = pMapLog->next;
		entryNum++;
	}
}

/**
 * @brief Retrieves a map entry by its sequential number in the log.
 */
int8_t map_get_entry_via_num(map_entry_log_t* pMapLog, uint16_t entryNum, map_entry_t* pEntry)
{
	map_entry_log_t* pCurrentNode = pMapLog;
	uint16_t		 currentNum	  = 0;

	if (pMapLog == NULL || pEntry == NULL)
	{
		return -1;
	}

	while (pCurrentNode != NULL)
	{
		if (currentNum == entryNum)
		{
			*pEntry = pCurrentNode->entry;
			return 0;
		}

		pCurrentNode = pCurrentNode->next;
		currentNum++;
	}

	return -1;
}

/**
 * @brief Retrieves a map entry by its key.
 */
int8_t map_get_entry_via_key(map_entry_log_t* pMapLog, const char* key, map_entry_t* pEntry)
{
	map_entry_log_t* pCurrentNode = pMapLog;

	if (pMapLog == NULL || key == NULL || pEntry == NULL)
	{
		return -1;
	}

	// Iterate through the linked list
	while (pCurrentNode != NULL)
	{
		// Compare the key of the current entry with the provided key
		if (strcmp(pCurrentNode->entry.key, key) == 0)
		{
			*pEntry = pCurrentNode->entry;
			return 0;
		}

		pCurrentNode = pCurrentNode->next;
	}

	return -1;
}

/**
 * @brief 
 */
int8_t map_delete_entry(map_entry_log_t* pMapLog, const char* key)
{
	//
}