//////////////////////////////////////////////////////////////////////
//                              Includes
//////////////////////////////////////////////////////////////////////

#include "mx25_flash_driver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////
//                             Macros
//////////////////////////////////////////////////////////////////////

#define PATH_TO_MOCK_FILE "../test/mx25_flash_mock/mx25_flash_mock.bin"

//////////////////////////////////////////////////////////////////////
//                      Public Functions definition
//////////////////////////////////////////////////////////////////////

/**
 * @brief Initializes the mock flash memory, creating the file if it doesn't exist.
 */
int8_t mx25_flash_init(void)
{
	FILE*  pFile  = fopen(PATH_TO_MOCK_FILE, "rb");
	int8_t retVal = 0;

	if (pFile == NULL)
	{
		pFile = fopen(PATH_TO_MOCK_FILE, "wb");
		if (pFile == NULL)
		{
			return -1;
		}

		uint8_t* pBuff = (uint8_t*)malloc(MX25_FLASH_SIZE_MEMORY_BYTES);
		if (pBuff == NULL)
		{
			fclose(pFile);
			return -1;
		}

		memset(pBuff, MX25_FLASH_ERASE_CELL_VAL, MX25_FLASH_SIZE_MEMORY_BYTES);

		size_t written = fwrite(pBuff, 1, MX25_FLASH_SIZE_MEMORY_BYTES, pFile);
		if (written != MX25_FLASH_SIZE_MEMORY_BYTES)
		{
			retVal = -1;
		}

		free(pBuff);
	}

	if (pFile)
	{
		fclose(pFile);
	}

	return retVal;
}

/**
 * @brief De-initializes the mock flash memory.
 */
int8_t mx25_flash_deInit()
{
	return 0;
}

/**
 * @brief Reads data from a specified address in the mock flash.
 */
int8_t mx25_flash_read(uint32_t readAddr, uint8_t* pBuffer, uint32_t size)
{
	if (!pBuffer || (readAddr + size) > MX25_FLASH_SIZE_MEMORY_BYTES)
	{
		return -1;
	}

	FILE* pFile = fopen(PATH_TO_MOCK_FILE, "rb");
	if (!pFile)
	{
		return -1;
	}

	fseek(pFile, readAddr, SEEK_SET);
	size_t read = fread(pBuffer, 1, size, pFile);

	fclose(pFile);

	return (read == size) ? 0 : -1;
}

/**
 * @brief Read a complete sector from the mock flash.
 */
int8_t mx25_flash_sector_read(uint32_t readAddr, uint8_t* pBuffer)
{
	uint16_t sector = readAddr / MX25_FLASH_SECTOR_SIZE;

	return mx25_flash_read(sector * MX25_FLASH_SECTOR_SIZE, pBuffer, MX25_FLASH_SECTOR_SIZE);
}

/**
 * @brief Writes data to a specified address in the mock flash, simulating NOR flash behavior.
 * 
 * \todo MX25 can only write 256 bytes at at time, mock this behaviour
 */
int8_t mx25_flash_write(uint32_t writeAddr, uint8_t* pBuffer, uint32_t size)
{
	if (!pBuffer || (writeAddr + size) > MX25_FLASH_SIZE_MEMORY_BYTES)
	{
		return -1;
	}

	// Load full flash into memory
	FILE* pFile = fopen(PATH_TO_MOCK_FILE, "rb");
	if (!pFile)
	{
		return -1;
	}

	uint8_t* flashData = malloc(MX25_FLASH_SIZE_MEMORY_BYTES);
	if (!flashData)
	{
		fclose(pFile);
		return -1;
	}

	fread(flashData, 1, MX25_FLASH_SIZE_MEMORY_BYTES, pFile);
	fclose(pFile);

	// Apply write respecting NOR rule: only 1 → 0 transitions allowed
	for (uint32_t i = 0; i < size; i++)
	{
		uint8_t prev = flashData[writeAddr + i];
		uint8_t newv = pBuffer[i];

		if ((~prev) & newv)
		{
			printf("[MX25 MOCK] Write violation: trying to flip 0->1 at addr 0x%08X\n", writeAddr + i);
			free(flashData);
			return -1;
		}

		flashData[writeAddr + i] = newv;
	}

	// Write entire flash back to file
	pFile = fopen(PATH_TO_MOCK_FILE, "wb");
	if (!pFile)
	{
		free(flashData);
		return -1;
	}

	fwrite(flashData, 1, MX25_FLASH_SIZE_MEMORY_BYTES, pFile);
	fclose(pFile);
	free(flashData);

	return 0;
}

/**
 * @brief Erases a specific sector in the mock flash.
 */
int8_t mx25_flash_sector_erase(uint16_t firstSector)
{
	uint32_t startAddr = firstSector * MX25_FLASH_SECTOR_SIZE;
	if (startAddr >= MX25_FLASH_SIZE_MEMORY_BYTES)
	{
		return -1;
	}

	FILE* pFile = fopen(PATH_TO_MOCK_FILE, "rb+");
	if (!pFile)
	{
		return -1;
	}

	uint8_t* eraseBuf = malloc(MX25_FLASH_SECTOR_SIZE);
	if (!eraseBuf)
	{
		fclose(pFile);
		return -1;
	}

	memset(eraseBuf, MX25_FLASH_ERASE_CELL_VAL, MX25_FLASH_SECTOR_SIZE);

	fseek(pFile, startAddr, SEEK_SET);
	fwrite(eraseBuf, 1, MX25_FLASH_SECTOR_SIZE, pFile);

	free(eraseBuf);
	fclose(pFile);

	return 0;
}

/**
 * @brief Erases a 32KB block in the mock flash.
 */
int8_t mx25_flash_block_erase_32k(uint16_t firstBlock)
{
	uint32_t startAddr = firstBlock * MX25_FLASH_BLOCK_SIZE_1;

	if (startAddr >= MX25_FLASH_SIZE_MEMORY_BYTES)
	{
		return -1;
	}

	FILE* pFile = fopen(PATH_TO_MOCK_FILE, "rb+");
	if (!pFile)
	{
		return -1;
	}

	uint8_t* eraseBuf = malloc(MX25_FLASH_BLOCK_SIZE_1);
	if (!eraseBuf)
	{
		fclose(pFile);
		return -1;
	}

	memset(eraseBuf, MX25_FLASH_ERASE_CELL_VAL, MX25_FLASH_BLOCK_SIZE_1);
	fseek(pFile, startAddr, SEEK_SET);
	fwrite(eraseBuf, 1, MX25_FLASH_BLOCK_SIZE_1, pFile);

	free(eraseBuf);
	fclose(pFile);

	return 0;
}

/**
 * @brief Erases a 64KB block in the mock flash.
 */
int8_t mx25_flash_block_erase_64k(uint16_t firstBlock)
{
	uint32_t startAddr = firstBlock * MX25_FLASH_BLOCK_SIZE_2;

	if (startAddr >= MX25_FLASH_SIZE_MEMORY_BYTES)
	{
		return -1;
	}

	FILE* pFile = fopen(PATH_TO_MOCK_FILE, "rb+");
	if (!pFile)
	{
		return -1;
	}

	uint8_t* eraseBuf = malloc(MX25_FLASH_BLOCK_SIZE_2);
	if (!eraseBuf)
	{
		fclose(pFile);
		return -1;
	}

	memset(eraseBuf, MX25_FLASH_ERASE_CELL_VAL, MX25_FLASH_BLOCK_SIZE_2);
	fseek(pFile, startAddr, SEEK_SET);
	fwrite(eraseBuf, 1, MX25_FLASH_BLOCK_SIZE_2, pFile);

	free(eraseBuf);
	fclose(pFile);

	return 0;
}

/**
 * @brief Erases the entire mock flash chip.
 */
int8_t mx25_flash_chip_erase(void)
{
	FILE* pFile = fopen(PATH_TO_MOCK_FILE, "wb");
	if (!pFile)
	{
		return -1;
	}

	uint8_t* eraseBuf = malloc(MX25_FLASH_SIZE_MEMORY_BYTES);
	if (!eraseBuf)
	{
		fclose(pFile);
		return -1;
	}

	memset(eraseBuf, MX25_FLASH_ERASE_CELL_VAL, MX25_FLASH_SIZE_MEMORY_BYTES);
	fwrite(eraseBuf, 1, MX25_FLASH_SIZE_MEMORY_BYTES, pFile);

	free(eraseBuf);
	fclose(pFile);

	return 0;
}
