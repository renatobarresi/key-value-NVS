/**
 * @brief 
 * 
 * Storage uses MX25 Flash as the NVM to store payloads,
 * 
 * The user defines how many pages it wants to reserve for the particluar application
 * 
 * In this case MAP
 * 
 * As first case, MAP space will be for 100 map entries, 
 * 
 * Each map entry has 1 + 32 + 64 + 4 bytes in size * 100 is the reserved space
 * 
 * TODO: Should upper layers define how much space will storage reserve for the application? 
 * 
 * In case of other layers beside MAP that want to use the same service, E.g blackbox component
 * 
 */

//////////////////////////////////////////////////////////////////////
//                              Includes
//////////////////////////////////////////////////////////////////////

#include "storage.h"
#include "mx25_flash_driver.h"
#include "stdlib.h"
#include "string.h"

//////////////////////////////////////////////////////////////////////
//                             Macros
//////////////////////////////////////////////////////////////////////

#define MAP_NUM_ENTRIES 100															/// Maximum number of map entries the storage can hold.
#define STORAGE_ENTRY_SIZE_BYTES (sizeof(storage_entry_t))							/// Total size of a single storage entry, including header, payload, and metadata.
#define MAP_RESERVED_SPACE (MAP_NUM_ENTRIES * STORAGE_ENTRY_SIZE_BYTES)				/// Total reserved space in flash for all map entries.
#define FLASH_PAGE_START_ADDRESS 0x00000000											/// The starting address in flash memory where storage begins.
#define FLASH_PAGE_LOG_LAST_ADDRESS (FLASH_PAGE_START_ADDRESS + MAP_RESERVED_SPACE) /// The end address of the reserved storage space.
#define ENTRY_HEADER_VALUE 0xDEADBEEF												/// Magic number used to identify a valid storage entry.
#define ENTRY_NOT_DELETED_VALUE 0													/// Value indicating that an entry is not deleted.
#define ENTRY_DELETED_VALUE 1														/// Value indicating that an entry has been marked as deleted.

//////////////////////////////////////////////////////////////////////
//                              Types
//////////////////////////////////////////////////////////////////////

/**
 * @brief Structure of entry that is stored in the non volatile memory
 */
typedef struct storage_entry
{
	uint32_t header;
	uint8_t	 payloadBuffer[MAX_STORAGE_ENTRY_PAYLOAD_LEN];
	uint32_t dataLen;
	uint32_t crc32;
} __attribute__((__packed__)) storage_entry_t;

//////////////////////////////////////////////////////////////////////
//                         Private Global Variables
//////////////////////////////////////////////////////////////////////

static storage_entry_t entry;									 ///
static uint32_t		   entryAddrHead = FLASH_PAGE_START_ADDRESS; /// Address in memory of the last valid entry
static uint32_t		   entryAddrTail = FLASH_PAGE_START_ADDRESS; /// Address in memory of the last entry
static uint8_t		   pTempBuffer[MX25_FLASH_SECTOR_SIZE];		 /// This buffer is used to store the entries temporaly

//////////////////////////////////////////////////////////////////////
//                         Private Functions declaration
//////////////////////////////////////////////////////////////////////

/**
 * @name crc_calculate_32
 * @brief Calculates the CRC32 checksum for a given data buffer.
 * 
 * @param data Pointer to the data for which to calculate the CRC.
 * @param len Length of the data in bytes.
 * 
 * @return The calculated 32-bit CRC value.
 */
static uint32_t crc_calculate_32(const void* data, size_t len);

/**
 * @name storage_get_last_entry_addr
 * @brief Scans the flash memory to find the address of the last valid entry.
 * 
 * @details This function iterates through the storage area from the beginning,
 *          validating each entry's header and CRC to find the first empty or
 *          corrupt slot, which indicates the end of the log.
 * 
 * @return The address of the next available slot for a new entry.
 */
static uint32_t storage_get_last_entry_addr();

//////////////////////////////////////////////////////////////////////
//                      Public Functions definition
//////////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the storage module.
 */
int8_t storage_init()
{
	uint32_t startSector;

	if (-1 == mx25_flash_init())
	{
		return -1;
	}

	entryAddrHead = storage_get_last_entry_addr();

	startSector = entryAddrHead / MX25_FLASH_SECTOR_SIZE;

	if (mx25_flash_sector_read(startSector * MX25_FLASH_SECTOR_SIZE, pTempBuffer) != 0)
	{
		memset(pTempBuffer, MX25_FLASH_ERASE_CELL_VAL, MX25_FLASH_SECTOR_SIZE);
	}

	return 0;
}

/**
 * @brief De-initializes the storage module.
 */
int8_t storage_deInit()
{
	return mx25_flash_deInit();
}

/**
 * @brief Buffers an entry to be written to non-volatile memory.
 */
int8_t storage_store_entry(const void* pPayload, uint32_t payloadLen)
{
	storage_entry_t entry;

	if (payloadLen > MAX_STORAGE_ENTRY_PAYLOAD_LEN)
	{
		return -1;
	}

	memset(&entry, 0, sizeof(storage_entry_t));

	entry.header  = ENTRY_HEADER_VALUE;
	entry.dataLen = payloadLen;
	memcpy(entry.payloadBuffer, pPayload, payloadLen);

	entry.crc32 = crc_calculate_32(entry.payloadBuffer, payloadLen);

	uint32_t entrySize			= sizeof(storage_entry_t);
	uint32_t currentSectorNum	= entryAddrHead / MX25_FLASH_SECTOR_SIZE;
	uint32_t nextEntrySectorNum = (entryAddrHead + entrySize - 1) / MX25_FLASH_SECTOR_SIZE;

	if (nextEntrySectorNum != currentSectorNum)
	{
		if (storage_flush() != 0)
		{
			return -1;
		}

		if (mx25_flash_sector_read(nextEntrySectorNum * MX25_FLASH_SECTOR_SIZE, pTempBuffer) != 0)
		{
			memset(pTempBuffer, MX25_FLASH_ERASE_CELL_VAL, MX25_FLASH_SECTOR_SIZE);
		}
	}

	uint32_t offsetInSector = entryAddrHead % MX25_FLASH_SECTOR_SIZE;
	memcpy(pTempBuffer + offsetInSector, (void*)&entry, entrySize);

	entryAddrHead += entrySize;

	return 0;
}

/**
 * @brief Retrieves a payload entry from non-volatile memory by its index.
 */
int8_t storage_retrieve_entry_payload(void* pPayload, uint32_t payloadLen, uint16_t entryNum)
{
	storage_entry_t entry;
	uint32_t		calculatedCrc;
	size_t			entrySize = sizeof(storage_entry_t);

	if (mx25_flash_read(entryNum * entrySize, (void*)&entry, entrySize) != 0)
	{
		return -1;
	}

	if (entry.header != ENTRY_HEADER_VALUE)
	{
		return -1;
	}

	calculatedCrc = crc_calculate_32(entry.payloadBuffer, entry.dataLen);

	if (calculatedCrc != entry.crc32)
	{
		return -1;
	}

	memcpy(pPayload, entry.payloadBuffer, payloadLen);

	return 0;
}

/**
 * @brief Flushes the temporary buffer to the flash memory.
 */
int8_t storage_flush()
{
	uint32_t currentSectorAddr = (entryAddrHead / MX25_FLASH_SECTOR_SIZE) * MX25_FLASH_SECTOR_SIZE;
	uint16_t currentSectorNum  = entryAddrHead / MX25_FLASH_SECTOR_SIZE;

	if (mx25_flash_sector_erase(currentSectorNum) != 0)
	{
		return -1;
	}

	return mx25_flash_write(currentSectorAddr, pTempBuffer, MX25_FLASH_SECTOR_SIZE);
}

// This function should only be used for testing purposes
/**
 * @brief Resets the internal state of the storage module. For testing only.
 */
void _reset_storage_state()
{
	entryAddrHead = FLASH_PAGE_START_ADDRESS;
	entryAddrTail = FLASH_PAGE_START_ADDRESS;
}

//////////////////////////////////////////////////////////////////////
//                         Private Functions definition
//////////////////////////////////////////////////////////////////////

/**
 * @brief Finds the address of the next available entry slot in flash.
 */
static uint32_t storage_get_last_entry_addr()
{
	storage_entry_t entry;
	uint32_t		calculatedCrc;
	size_t			entrySize = sizeof(storage_entry_t);
	uint32_t		offset	  = 0;

	for (uint16_t i = 0; i < MAP_NUM_ENTRIES; i++)
	{
		offset = i * entrySize;

		if (mx25_flash_read(offset, (void*)&entry, entrySize) != 0)
		{
			break;
		}

		if (entry.header != ENTRY_HEADER_VALUE)
		{
			break;
		}

		calculatedCrc = crc_calculate_32(entry.payloadBuffer, entry.dataLen);

		if (calculatedCrc != entry.crc32)
		{
			break;
		}
	}

	return (FLASH_PAGE_START_ADDRESS + offset);
}

/**
 * @brief Calculates the CRC32 checksum for a given data buffer.
 */
static uint32_t crc_calculate_32(const void* data, size_t len)
{
	uint32_t	   crc;
	const uint8_t* buf;

	buf = (const uint8_t*)data;
	crc = 0xFFFFFFFF;

	for (size_t i = 0; i < len; i++)
	{
		crc ^= buf[i];
		for (int j = 0; j < 8; j++)
		{
			crc = (crc >> 1) ^ (0xEDB88320U & -(int)(crc & 1));
		}
	}

	return ~crc;
}