#ifndef __CPCLI_VALUE_HPP__
#define __CPCLI_VALUE_HPP__

#include <string_view>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <array>
#include <sstream>
#include "cpcli/define.hpp"

namespace cpcli {

    using values = std::vector<std::string_view>;
    using argument_map = std::unordered_map<std::string_view, std::string_view>;

    class parser{
    public:
        parser(const int argc, char** argv) { for (int i = 1; i < argc; ++i) { parse(argv[i]); } }
        parser(values values) { for (auto value : values) { parse(value); } }
    private:
        void parse(const std::string_view& text) {
            if(values_) on_value(text);
            else text.at(0) == '-' ? on_option(text) : on_value(text);
        }
        // prefix text suffix
        void on_option(const std::string_view& text) {
            auto opt = text;
            opt.remove_prefix(opt.find_first_not_of('-')); // remove - or --
            const auto delimiter = opt.find_first_not_of('=');
            if(delimiter != std::string_view::npos) {
                auto value = opt;
                value.remove_prefix(delimiter + 1); // rmeove =
                opt.remove_suffix(opt.size() - delimiter); // remove =...
                options_.emplace(opt, value);
            }
        }
        void on_value(const std::string_view& text) {
            if(values_) {
                values_->emplace_back(text);
            }
            else {
                values_ = values();
                values_->emplace_back(text);
            }
        }
    private:
        argument_map options_;
        std::optional<values> values_ = std::nullopt;
    };

    inline std::optional<std::string_view> get_value(
        const argument_map& options, const std::string_view& option) {
        const auto it = options.find(option);
        return it != options.end() ? std::make_optional(it->second) : std::nullopt;
    }

    template <class T>
    std::optional<T> get(const argument_map& options,
                        const std::string_view& option) {
        if (const auto view = get_value(options, option)) {
            T value;
            if (std::istringstream(std::string(*view)) >> value) return value;
        }
        return std::nullopt;
    }

    template <>
    std::optional<std::string_view> get(const argument_map& options,
                                        const std::string_view& option) {
        return get_value(options, option);
    }

    template <>
    std::optional<std::string> get(const argument_map& options,
                                const std::string_view& option) {
        if (const auto view = get<std::string_view>(options, option)) {
            return std::string(*view);
        }
        return std::nullopt;
    }

    constexpr std::array<const char*, 5> falsities{{"0", "n", "no", "f", "false"}};
    template <>
    std::optional<bool> get(const argument_map& options,
                            const std::string_view& option) {
        if (const auto value = get_value(options, option)) {
            return std::none_of(falsities.begin(), falsities.end(),
                                [&value](auto falsity) { return *value == falsity; });
        }
        return std::nullopt;
    }
}


#endif
