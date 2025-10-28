#ifndef MAP_H
#define MAP_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////
//                              Includes
//////////////////////////////////////////////////////////////////////

#include <stdint.h>

//////////////////////////////////////////////////////////////////////
//                             Macros
//////////////////////////////////////////////////////////////////////

#define MAP_MAX_KEY_LEN 32		  /// Maximum length of a key string.
#define MAP_MAX_VAL_LEN_STR 64	  /// Maximum length of a string value.
#define ENTRY_NOT_DELETED_VALUE 0 /// Value indicating that an entry is not deleted.
#define ENTRY_DELETED_VALUE 1	  /// Value indicating that an entry has been marked as deleted.

//////////////////////////////////////////////////////////////////////
//                              Types
//////////////////////////////////////////////////////////////////////

/**
 * @brief Represents a single key-value entry in the map.
 */
typedef struct map_entry
{
	uint8_t	 type;
	uint8_t	 entryDeletedFlag;
	char	 key[MAP_MAX_KEY_LEN];
	char	 valueStr[MAP_MAX_VAL_LEN_STR];
	uint32_t valueU32;
} __attribute__((__packed__)) map_entry_t;

/**
 * @brief Linked list that contains all the -valid- mapped values stored in flash
 */
typedef struct map_entry_log
{
	map_entry_t			  entry;
	struct map_entry_log* next;
} map_entry_log_t;

//////////////////////////////////////////////////////////////////////
//                      Public Functions declaration
//////////////////////////////////////////////////////////////////////

/**
 * @name map_init
 * @brief Initializes the map module and reads existing entries from storage.
 * 
 * @param[out] pMapLog Pointer to the head of the map entry linked list to be populated.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t map_init(map_entry_log_t* pMapLog);

/**
 * @name map_deInit
 * @brief De-initializes the map, freeing allocated memory and closing storage.
 * 
 * @param[in] pMapLog Pointer to the head of the map entry linked list to be cleared.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t map_deInit(map_entry_log_t* pMapLog);

/**
 * @name map_store_all
 * @brief Flushes all buffered map entries to persistent storage.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t map_store_all();

/**
 * @name map_add_entry_val_str
 * @brief Adds a new map entry with a string value to storage.
 * 
 * @param[in] pKey The key for the new entry.
 * @param[in] pVal The string value for the new entry.
 * 
 * @retval 0 on success, -1 on failure (e.g., key/value too long).
 */
int8_t map_add_entry_val_str(const char* pKey, const char* pVal);

/**
 * @name map_add_entry_val_u32
 * @brief Adds a new map entry with a uint32_t value to storage.
 * 
 * @param[in] pKey The key for the new entry.
 * @param[in] valueU32 The uint32_t value for the new entry.
 * 
 * @retval 0 on success, -1 on failure (e.g., key too long).
 */
int8_t map_add_entry_val_u32(const char* pKey, uint32_t valueU32);

/**
 * @name map_get_entry_via_num
 * @brief Retrieves a map entry from the in-memory log by its sequential index.
 * 
 * @param[in] pMapLog Pointer to the head of the map entry linked list.
 * @param[in] entryNum The zero-based index of the entry to retrieve.
 * @param[out] pEntry Pointer to a map_entry_t struct to be filled with the data.
 * 
 * @retval 0 on success, -1 if the entry is not found.
 */
int8_t map_get_entry_via_num(map_entry_log_t* pMapLog, uint16_t entryNum, map_entry_t* pEntry);

/**
 * @name map_get_entry_via_key
 * @brief Retrieves the latest map entry from the in-memory log by its key.
 * 
 * @param[in] pMapLog Pointer to the head of the map entry linked list.
 * @param[in] key The key of the entry to retrieve.
 * @param[out] pEntry Pointer to a map_entry_t struct to be filled with the data.
 * 
 * @retval 0 on success, -1 if the key is not found.
 */
int8_t map_get_entry_via_key(map_entry_log_t* pMapLog, const char* key, map_entry_t* pEntry);

/**
 * @name map_delete_entry
 * @brief Marks an entry in storage as deleted by creating a new tombstone entry.
 * 
 * @param[in] pMapLog Pointer to the head of the map entry linked list.
 * @param[in] key The key of the entry to delete.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t map_delete_entry(map_entry_log_t* pMapLog, const char* key);

/**
 * @name map_print_log
 * @brief Prints all entries in the in-memory map log to the console.
 * 
 * @param[in] pMapLog Pointer to the head of the map entry linked list.
 */
void map_print_log(map_entry_log_t* pMapLog);

/**
 * @name map_read_log
 * @brief Reads the entire log from storage and populates the in-memory linked list.
 * 
 * @param[out] pMapLog Pointer to the head of the map entry linked list to be populated.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t map_read_log(map_entry_log_t* pMapLog);

int8_t map_delete_entry(map_entry_log_t* pMapLog, const char* key);

#ifdef __cplusplus
}
#endif

#endif // MAP_H