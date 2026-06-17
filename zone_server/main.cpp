#include <asio.hpp>
#include <iostream>
#include "framework.h"

int main()
{
	try
	{
		fncore();

		asio::io_context io;

		std::cout << "zone server started on port 9000\n";

		io.run();
	}
	catch (std::exception ex)
	{
		std::cerr << "exception: " << ex.what() << "\n";
	}

	return 0;
}