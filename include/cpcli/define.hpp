#ifndef __CPCLI_DEFINE_HPP__
#define __CPCLI_DEFINE_HPP__

#include <optional>
#include <vector>
#include <string_view>
#include <string>
#include <array>
#include <memory>

namespace cpcli
{
    constexpr std::array<const char*, 5> falsities{{"0", "n", "no", "f", "false"}};
    class define_group {
    public:
        using ptr = std::shared_ptr<define_group>;
        using values = std::vector<std::string_view, std::string_view>;
        define_group(define_group::ptr parent): parent_(parent), values_() {}
    private:
        inline std::optional<std::string_view> get_value(int index) {
            return values_.size() > index ? std::make_optional<values_[0].second> : std::nullopt;
        }
    public:
        template<class T>
        T make_add(const std::string_view& name) {
            T def = std::make_shared<T>(name);
            def.name(name);
            return def;
        }

        template<class T>
        std::optional<T> get(const std::string_view& name) {
            if(const auto v = get_value(0)){
                T value;
                if(std::istringstream(std::string(*v)) >> value) return value;
            }
            return std::nullopt;
        }
        template <>
        std::optional<std::string_view> get(const std::string_view& name) {
            return get_value(0);
        }
        template <>
        std::optional<std::string> get(const std::string_view& name) {
            if(const auto v = get_value(0)) { return std::string(*v); }
            return std::nullopt;
        }

        template <>
        std::optional<bool> get(const std::string_view& name) {
            if (const auto v = get_value(0)) {
                return std::none_of(falsities.begin(), falsities.end(),
                                    [&v](auto falsity) { return *v == falsity; });
            }
            return std::nullopt;
        }
    protected:
        std::string_view name_;
        std::string_view desc_;
        values values_;
        define_group::ptr parent_; // for help commands
        std::vector<define_group::ptr> child_;
    };

    class option: public define_group {
    public:
        option& name(const std::string_view& name)         { name_ = name; }
        option& desc(const std::string_view& description)  { desc_ = description; }
        option& abbr(const std::string_view& abbreviation) { desc_ = description; }
    private:
        std::string_view args_;
    };

    class command : public define_group{
    public:
        command& name(const std::string_view& name)         { name_ = name; }
        command& desc(const std::string_view& description)  { desc_ = description; }
        command& args(const std::string_view& arguments)    { args_ = arguments; }
    private:
        std::string_view args_;
        define_group options_;
        define_group commands_;
    };
}

#endif
