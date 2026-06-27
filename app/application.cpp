#include "application.hpp"

#include <iostream>

namespace mercury::app
{

Application::Application()
    :
    gateway_
    {
        10000,          // Maximum order quantity
        100000000       // Maximum order value

    }
{
}

void Application::run()
{
    std::cout << '\n';

    std::cout
        << "========================================\n";

    std::cout
        << "        Mercury Exchange Engine\n";

    std::cout
        << "========================================\n\n";

    std::cout
        << "Application initialized successfully.\n";

    std::cout
        << "Gateway ready.\n";

    std::cout
        << "Waiting for order flow...\n\n";
}

} // namespace mercury::app