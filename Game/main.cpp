#include "CoreLoop.hpp"

int main()
{
	auto root = std::make_shared<Node2D>();

	CORE_LOOP(1280u, 720u, "TDS", root);

	return 0;
}