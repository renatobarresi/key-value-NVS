/**
 * @brief 
 *  
 *  "Appends" entries into a non volatile memory 
 * 
 * 
 *  | Entry 1 | Entry 2 | Entry 3 .. | Entry N 
 * 
 *  | Entry Header | 
 * 
 *  Header, Payload, Magic Number (Indicates header is valid)
 * 
 *  TODO: Magic number should be a CRC that then is checked to indicate
 *        validity of entry  
 * 
 */

#ifndef STORAGE_H
#define STORAGE_H

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

#define MAX_STORAGE_ENTRY_PAYLOAD_LEN 102 /// Maximum size in bytes of the payload

//////////////////////////////////////////////////////////////////////
//                              Types
//////////////////////////////////////////////////////////////////////

/**
 * @name storage_init
 * @brief Initializes the storage module.
 * 
 * @details This function initializes the underlying non-volatile memory driver.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t storage_init();

/**
 * @name storage_deInit
 * @brief De-initializes the storage module.
 * 
 * @details This function de-initializes the underlying non-volatile memory driver.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t storage_deInit();

/**
 * @name storage_store_entry
 * @brief Stores a payload entry into non-volatile memory.
 * 
 * @param[in] pPayload Pointer to the payload data to be stored.
 * @param[in] payloadLen Length of the payload data in bytes.
 * 
 * @retval 0 on success, -1 on failure (e.g., payload too large).
 */
int8_t storage_store_entry(const void* pPayload, uint32_t payloadLen);

/**
 * @name storage_flush
 * @brief Flushes any pending buffered data to non-volatile memory.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t storage_flush();

/**
 * @name storage_retrieve_entry_payload
 * @brief Retrieves a payload entry from non-volatile memory by its index.
 * 
 * @param[out] pPayload Pointer to a buffer to store the retrieved payload.
 * @param[in]  payloadLen The expected length of the payload to retrieve.
 * @param[in]  entryNum The zero-based index of the entry to retrieve.
 * 
 * @retval 0 on success, -1 if the entry is not found or corrupted.
 */
int8_t storage_retrieve_entry_payload(void* pPayload, uint32_t payloadLen, uint16_t entryNum);

/**
 * @name _reset_storage_state
 * @brief Resets the internal state of the storage module. (for testing only)
 */
void _reset_storage_state();

#ifdef __cplusplus
}
#endif

#endif // STORAGE_H
