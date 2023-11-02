#include <iostream>
#include <type_traits>
#include <string>
#include <format>
#include <print>

template <typename T>
inline auto constexpr always_false{ false };

#define SATAICALLY_WIDEN(CharT, STR)                                                                                    \
    [] {                                                                                                                \
        if constexpr (std::is_same_v<CharT, char>)                                                                      \
        {                                                                                                               \
            return STR;                                                                                                 \
        }                                                                                                               \
        else if constexpr (std::is_same_v<CharT, wchar_t>)                                                              \
        {                                                                                                               \
            return L##STR;                                                                                              \
        }                                                                                                               \
        else if constexpr (std::is_same_v<CharT, char8_t>)                                                              \
        {                                                                                                               \
            return u8##STR;                                                                                             \
        }                                                                                                               \
        else if constexpr (std::is_same_v<CharT, char16_t>)                                                             \
        {                                                                                                               \
            return u##STR;                                                                                              \
        }                                                                                                               \
        else if constexpr (std::is_same_v<CharT, char32_t>)                                                             \
        {                                                                                                               \
            return U##STR;                                                                                              \
        }                                                                                                               \
        else                                                                                                            \
        {                                                                                                               \
            static_assert(always_false<CharT>, "CharT is one of char, wchar_t, char8_t, char16_t, char32_t.");          \
        }                                                                                                               \
    }()

enum class Color
{
    Red,
    Green,
    Blue,
	White
};

template <typename T>
requires requires {
	typename T::value_type;
}
T ToString(Color value)
{
	switch (value)
	{
	case Color::Red:
		return SATAICALLY_WIDEN(typename T::value_type, "Red");

	case Color::Green:
		return SATAICALLY_WIDEN(typename T::value_type, "Green");

	case Color::Blue:
		return SATAICALLY_WIDEN(typename T::value_type, "Blue");

	case Color::White:
		return SATAICALLY_WIDEN(typename T::value_type, "White");

	default:
		throw std::invalid_argument{ "Error" };
	}
}

template <typename T, typename CharT>
requires std::is_enum_v<T>
struct std::formatter<T, CharT>
{
	template <typename ParseContext>
	constexpr typename ParseContext::iterator parse(ParseContext& ctx)
	{
		auto it{ std::begin(ctx) };

		if (it != std::end(ctx))
		{
			switch (*it)
			{
			case SATAICALLY_WIDEN(CharT, 'c'):
				type = Type::All;
				++it;

				break;

			case SATAICALLY_WIDEN(CharT, 's'):
				type = Type::OnlyString;
				++it;

				break;

			case SATAICALLY_WIDEN(CharT, 'n'):
				type = Type::OnlyNumber;
				++it;

				break;

			default:
				break;
			}
		}

		return it;
	}

	template <typename FormatContext>
	constexpr typename FormatContext::iterator format(T const& t, FormatContext& ctx) const
	{		
		auto out{ ctx.out() };

		switch (type)
		{
		case Type::All:
			out = std::format_to(out, SATAICALLY_WIDEN(CharT, "{}({})"), ToString<std::basic_string<CharT>>(t), static_cast<std::underlying_type_t<T>>(t));

			break;

		case Type::OnlyString:
			out = std::format_to(out, SATAICALLY_WIDEN(CharT, "{}"), ToString<std::basic_string<CharT>>(t));

			break;

		case Type::OnlyNumber:
			out = std::format_to(out, SATAICALLY_WIDEN(CharT, "{}"), static_cast<std::underlying_type_t<T>>(t));

			break;

		default:
			break;
		}

		return out;
	}

private:
	enum class Type
	{
		All,
		OnlyString,
		OnlyNumber
	};

	Type type{};
};

int main()
{
	std::println("{}", Color::Red);
	std::println("{:c}", Color::Green);
	std::println("{:s}", Color::Blue);
	std::println("{:n}", Color::White);
}
