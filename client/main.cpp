#include "client.hpp"
#include "settings.hpp"

#include <boost/program_options.hpp>

#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        math::client::SettingsParser parser{argv[0]};

        try {
            const auto settings = parser.parse(argc, argv);
            if (settings.exit_with_usage()) {
                parser.usage();
                return 0;
            }

            math::client::Client client{settings};
            client.run();
        } catch (const boost::program_options::error& e) {
            parser.usage_error(e);
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occured: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "An unknown error occured\n";
        return 1;
    }
    return 0;
}
