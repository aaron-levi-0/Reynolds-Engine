#include "file_handling.h"

#include "common.h"
#include "../vendor/cJSON.h"

int load_board_config(const char* filepath, const char* board_name, unsigned int* config) 
{
    FILE* file = fopen(filepath, "r");
	
	ASSERT(file, "@file: Error: Could not open file %s", filepath);

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = (char*)malloc(length + 1);

    if (!data) {
        REYNOLDS_INFO("@file: Error: Memory allocation failed");
        fclose(file);
        return 0;
    }

    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    cJSON* root = cJSON_Parse(data);
    free(data);

    ASSERT_LOG(root, "@file: Error: JSON parsing failed");

    cJSON* board_config = cJSON_GetObjectItem(root, board_name);
    if (!board_config) {
        REYNOLDS_INFO("@file: Error: Board configuration '%s' not found", board_name);
        cJSON_Delete(root);
        return 0;
    }

    cJSON* width = cJSON_GetObjectItem(board_config, "width");
    cJSON* height = cJSON_GetObjectItem(board_config, "height");
    cJSON* grid_width = cJSON_GetObjectItem(board_config, "grid_width");
    cJSON* grid_height = cJSON_GetObjectItem(board_config, "grid_height");
    cJSON* tiles_x = cJSON_GetObjectItem(board_config, "tiles_x");
    cJSON* tiles_y = cJSON_GetObjectItem(board_config, "tiles_y");
    cJSON* num_bombs = cJSON_GetObjectItem(board_config, "num_bombs");

    if (!cJSON_IsNumber(width) || !cJSON_IsNumber(height) || !cJSON_IsNumber(grid_width) ||
        !cJSON_IsNumber(grid_height) || !cJSON_IsNumber(tiles_x) || !cJSON_IsNumber(tiles_y) || !cJSON_IsNumber(num_bombs)) 
	{
        REYNOLDS_INFO("@file: Error: Invalid JSON format for '%s'", board_name);
        cJSON_Delete(root);
        return 0;
    }

    config[0] = width->valueint;
    config[1] = height->valueint;
    config[2] = grid_width->valueint;
    config[3] = grid_height->valueint;
    config[4] = tiles_x->valueint;
    config[5] = tiles_y->valueint;
    config[6] = num_bombs->valueint;

    cJSON_Delete(root);
	
	REYNOLDS_INFO("@file: successfully loaded '%s' config from path %s", board_name, filepath);

    return 1;
}
