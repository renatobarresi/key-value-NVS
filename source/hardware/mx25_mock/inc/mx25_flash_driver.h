#ifndef MX25_FLASH_DRIVER_H
#define MX25_FLASH_DRIVER_H

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

#define MX25_FLASH_SIZE_MEMORY_BYTES (256 * 1024) /// Total size of the flash memory in bytes (256 KB).
#define MX25_FLASH_ERASE_CELL_VAL 0xFF			  /// The value of a memory cell after being erased.
#define MX25_FLASH_SECTOR_SIZE (4 * 1024)		  /// Size of a flash sector in bytes (4 KB).
#define MX25_FLASH_BLOCK_SIZE_1 (32 * 1024)		  /// Size of a 32KB flash block in bytes.
#define MX25_FLASH_BLOCK_SIZE_2 (64 * 1024)		  /// Size of a 64KB flash block in bytes.
#define MX25_FLASH_PAGE_SIZE 256				  /// Size of a flash page in bytes. This is the maximum amount that can be written in one operation.

//////////////////////////////////////////////////////////////////////
//                      Public Functions declaration
//////////////////////////////////////////////////////////////////////

/**
 * @name mx25_flash_init
 * @brief Initializes the flash driver.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t mx25_flash_init();

/**
 * @name mx25_flash_deInit
 * @brief De-initializes the flash driver.
 * 
 * @retval 0 on success.
 */
int8_t mx25_flash_deInit();

/**
 * @name mx25_flash_write
 * @brief Writes data to a specified address in flash.
 * 
 * @param[in] WriteAddr The starting address to write to.
 * @param[in] pBuffer Pointer to the data buffer to write.
 * @param[in] size Number of bytes to write.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t mx25_flash_write(uint32_t WriteAddr, uint8_t* pBuffer, uint32_t size);

/**
 * @name mx25_flash_read
 * @brief Reads data from a specified address in flash.
 * 
 * @param[in] readAddr The starting address to read from.
 * @param[out] pBuffer Pointer to the buffer where read data will be stored.
 * @param[in] size Number of bytes to read.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t mx25_flash_read(uint32_t readAddr, uint8_t* pBuffer, uint32_t size);

/**
 * @name mx25_flash_page_erase
 * @brief Erases a specific page in flash.
 * 
 * @param[in] firstPage The index of the page to erase.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t mx25_flash_page_erase(uint16_t firstPage);

/**
 * @name mx25_flash_sector_read
 * @brief Reads a complete sector from flash into a buffer.
 * 
 * @param[in] readAddr An address within the sector to be read. The function calculates the sector start.
 * @param[out] pBuffer Pointer to the buffer to store the sector data (must be at least MX25_FLASH_SECTOR_SIZE).
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t mx25_flash_sector_read(uint32_t readAddr, uint8_t* pBuffer);

/**
 * @name mx25_flash_sector_erase
 * @brief Erases a specific sector in flash.
 * 
 * @param[in] firstSector The index of the sector to erase.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t mx25_flash_sector_erase(uint16_t firstSector);

/**
 * @name mx25_flash_chip_erase
 * @brief Erases the entire flash chip, setting all bytes to MX25_FLASH_ERASE_CELL_VAL.
 * 
 * @retval 0 on success, -1 on failure.
 */
int8_t mx25_flash_chip_erase();

#ifdef __cplusplus
}
#endif

#endif // MX25_FLASH_DRIVER_H