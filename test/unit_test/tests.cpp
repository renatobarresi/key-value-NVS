#include "gtest/gtest.h"
#include "mx25_flash_driver.h"
#include "map.h"
#include "storage.h"
#include <fstream>
#include <vector>
#include <string>
#include <cstdio> 
 
// Test fixture for map tests
class MapTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize the storage and map. This also erases the mock flash file.
        mx25_flash_chip_erase();
        _reset_storage_state();
        map_init(&rtosComponents);
    }
 
    void TearDown() override {
        // De-initialize to free any allocated memory
        map_deInit(&rtosComponents);
    }

    map_entry_log_t rtosComponents;
};

TEST_F(MapTest, AddAndRetrieveMultipleEntries)
{
    ASSERT_EQ(0, map_add_entry_val_str("task1Name", "network"));
    ASSERT_EQ(0, map_add_entry_val_u32("timeout", 1234));
    ASSERT_EQ(0, map_add_entry_val_str("rtos", "nuttX"));
 
    // reset the storage tail pointer to read from the beginning
    _reset_storage_state();
    map_read_log(&rtosComponents);
 
    map_print_log(&rtosComponents);

    map_entry_t entry;
 
    ASSERT_EQ(0, map_get_entry_via_num(&rtosComponents, 0, &entry));
    EXPECT_STREQ("task1Name", entry.key);
    EXPECT_STREQ("network", entry.valueStr);
 
    ASSERT_EQ(0, map_get_entry_via_num(&rtosComponents, 1, &entry));
    EXPECT_STREQ("timeout", entry.key);
    EXPECT_EQ(1234, entry.valueU32);

    ASSERT_EQ(0, map_get_entry_via_num(&rtosComponents, 2, &entry));
    EXPECT_STREQ("rtos", entry.key);
    EXPECT_STREQ("nuttX", entry.valueStr);
}

TEST_F(MapTest, AddAndRetrieveMultipleOverwrittenEntries)
{
    ASSERT_EQ(0, map_add_entry_val_str("task1Name", "network"));
    ASSERT_EQ(0, map_add_entry_val_u32("timeout", 1234));
    ASSERT_EQ(0, map_add_entry_val_str("rtos", "nuttX"));
    ASSERT_EQ(0, map_add_entry_val_str("task1Name", "sensors"));
 
    // reset the storage tail pointer to read from the beginning
    _reset_storage_state();
    map_read_log(&rtosComponents);
 
    map_print_log(&rtosComponents);

    map_entry_t entry;
 
    ASSERT_EQ(0, map_get_entry_via_num(&rtosComponents, 0, &entry));
    EXPECT_STREQ("task1Name", entry.key);
    EXPECT_STREQ("sensors", entry.valueStr);
 
    ASSERT_EQ(0, map_get_entry_via_num(&rtosComponents, 1, &entry));
    EXPECT_STREQ("timeout", entry.key);
    EXPECT_EQ(1234, entry.valueU32);

    ASSERT_EQ(0, map_get_entry_via_num(&rtosComponents, 2, &entry));
    EXPECT_STREQ("rtos", entry.key);
    EXPECT_STREQ("nuttX", entry.valueStr);
}