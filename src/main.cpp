#include <iostream>

int main()
{
	constexpr int IMAGE_WIDTH{256};
	constexpr int IMAGE_HEIGHT{256};
	constexpr int MAX_COLOUR_VALUE{255};
	// 256 would truncate to out-of-range at r = 1.0, so scale by 255.999 instead
	constexpr double COLOUR_SCALE{MAX_COLOUR_VALUE + 0.999};

	std::cout << "P3\n"
			  << IMAGE_WIDTH << ' ' << IMAGE_HEIGHT << '\n'
			  << MAX_COLOUR_VALUE << '\n';

	// Each pixel is written out left to right, and each row of pixels is
	// written out top to bottom
	for (int j{0}; j < IMAGE_HEIGHT; ++j)
	{
		for (int i{0}; i < IMAGE_WIDTH; ++i)
		{
			auto r = static_cast<double>(i) / (IMAGE_WIDTH - 1);
			auto g = static_cast<double>(j) / (IMAGE_HEIGHT - 1);
			auto b = 0.0;

			int ir{static_cast<int>(COLOUR_SCALE * r)};
			int ig{static_cast<int>(COLOUR_SCALE * g)};
			int ib{static_cast<int>(COLOUR_SCALE * b)};

			std::cout << ir << ' ' << ig << ' ' << ib << '\n';
		}
	}
}
