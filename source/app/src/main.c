#include "main.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
	map_entry_log_t rtosComponents;
	char			key[MAP_MAX_KEY_LEN];
	char			value[MAP_MAX_VAL_LEN_STR];

	memset(&rtosComponents, 0, sizeof(map_entry_log_t));

	if (map_init(&rtosComponents) != 0)
	{
		printf("Failed to initialize map.\n");
		return -1;
	}

	while (1)
	{
		printf("Enter key (or 'END' to finish): ");

		if (fgets(key, sizeof(key), stdin) == NULL)
		{
			break;
		}

		key[strcspn(key, "\n")] = 0;

		if (strcmp(key, "END") == 0)
		{
			break;
		}

		printf("Enter value: ");
		if (fgets(value, sizeof(value), stdin) == NULL)
		{
			break;
		}

		value[strcspn(value, "\n")] = 0;

		char* endptr;
		long  num = strtol(value, &endptr, 10);

		if (*endptr == '\0' && endptr != value)
		{
			if (-1 == map_add_entry_val_u32(key, (uint32_t)num))
			{
				printf("Failed to add entry.\n");
				break;
			}
		}
		else
		{
			if (-1 == map_add_entry_val_str(key, value))
			{
				printf("Failed to add entry.\n");
				break;
			}
		}
	}

	map_store_all();

	map_read_log(&rtosComponents);

	printf("\n--- Stored Map Entries ---\n");
	map_print_log(&rtosComponents);
	printf("--------------------------\n");

	map_deInit(&rtosComponents);
	printf("Memory released. Exiting.\n");

	return 0;
}