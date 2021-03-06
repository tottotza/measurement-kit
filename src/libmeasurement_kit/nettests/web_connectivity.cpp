// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.

#include "src/libmeasurement_kit/common/utils.hpp"
#include "src/libmeasurement_kit/nettests/runnable.hpp"
#include "src/libmeasurement_kit/ooni/nettests.hpp"
#include "src/libmeasurement_kit/http/http.hpp"

namespace mk {
namespace nettests {

WebConnectivityRunnable::WebConnectivityRunnable() noexcept {
    test_name = "web_connectivity";
    test_version = "0.0.1";
    needs_input = true;
    test_helpers_data = {{"web-connectivity", "backend"}};
}

void WebConnectivityRunnable::main(std::string input, Settings options,
                                   Callback<SharedPtr<nlohmann::json>> cb) {
    ooni::web_connectivity(input, options, cb, reactor, logger);
}

void WebConnectivityRunnable::fixup_entry(nlohmann::json &entry) {
    try {
        auto backend = entry["test_helpers"]["backend"].get<std::string>();
        if (mk::startswith(backend, "https://")) {
            entry["test_helpers"]["backend"] = {{
                "address", backend
            }, {
                "type", "https"
            }};
        } else {
            /* TODO: Here we should deal with this case. Or, even better,
                     we should probably enhance the model such that the backend
                     isn't a string but a more structured object. This will
                     probably happen when we finish the cloudfronted code. */
            logger->warn("We are sending a string-only backend entry.");
        }
    } catch (const std::exception &exc) {
        logger->warn("Cannot fixup entry: %s", exc.what());
    }
}

} // namespace nettests
} // namespace mk
