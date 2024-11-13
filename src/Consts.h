#pragma once

namespace VARS
{
    extern const char* res_path;
    const int block_size_x = 30;
    const int block_size_y = 30;
    const int player_size_x = 85;
    const int player_size_y = 85;
    const int player_speed = 500;
    const int max_size_x = 200000 * block_size_x;
    const int max_size_y = 200000 * block_size_y;
    const int chunk_size = 20 * block_size_x;
    const int chunk_border_width = 2;
    const int reach_distance = 500;
    const int max_view_distance = 2500;
    const int default_char_size = 24;
} // namespace VARS

namespace CVARS
{
    extern bool render_chunk_bg;
    extern bool debug;
    extern bool default_view;
} // namespace CVARS