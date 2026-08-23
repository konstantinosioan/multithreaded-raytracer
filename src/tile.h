#ifndef TILE_H
#define TILE_H

/// @brief A rectangular region of the image, rendered as one unit of work
/// @note Bounds are half-open, so a tile covers x_start to x_end - 1. A
///       default-constructed tile is empty rather than covering a region
class Tile final
{
  public:
	int x_start{};
	int x_end{};
	int y_start{};
	int y_end{};
};

#endif // TILE_H
