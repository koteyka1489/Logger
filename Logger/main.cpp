#include "Windows.h"
#include "string"
#include <concepts>
#include <type_traits>
#include <format>
#include <limits>
#include <unordered_map>
#include <vector>
#include <map>
#include <deque>
#include <list>
#include <array>
#include <memory>

namespace Log
{
	namespace details
	{



		template <typename T>
			requires std::is_convertible_v<T, std::string> &&
			(!std::is_same_v<std::decay_t<T>, void*>) &&
			(!std::is_same_v<std::decay_t<T>, std::nullptr_t>)
		static std::string Logs(T&& arg)
		{
			return std::string(std::forward<T>(arg));
		}

		static std::string Logs()
		{
			return "";
		}

		template <typename T>
			requires std::is_same_v<std::decay_t<T>, std::nullptr_t>
		static std::string Logs(T arg)
		{
			return " Is Nullptr";
		}

		template <typename T>
			requires std::is_floating_point_v<T> || std::is_integral_v<T>
		static std::string Logs(T arg)
		{
			return " " + std::to_string(arg) + " ";
		}

		template <typename T>
			requires std::is_pointer_v<T> && (!std::is_same_v<T, const char*>)
		static std::string Logs(T arg)
		{
			if (arg)
			{
				std::string address = std::format(" Address 0x{:x}", reinterpret_cast<uintptr_t>(arg));
				return address;
			}
			else
			{
				return std::string(" is nullptr ");
			}
		}

		template <>
		static std::string Logs<bool>(bool arg)
		{
			return arg ? " TRUE " : " FALSE ";
		}


		static std::string Logs(const char* arg)
		{
			if (!arg) return " Is nullptr";

			return std::string(arg);
		}

		template <>
		static std::string Logs<const wchar_t*>(const wchar_t* arg)
		{
			if (!arg) return " Is nullptr";

			int size = WideCharToMultiByte(CP_UTF8, 0, arg, -1, nullptr, 0, nullptr, nullptr);
			if (size == 0)
				return " Conversion error ";

			std::string result(size - 1, '\0');
			WideCharToMultiByte(CP_UTF8, 0, arg, -1, &result[0], size, nullptr, nullptr);

			return result;
		}

		template <>
		static std::string Logs<std::string&>(std::string& arg)
		{
			return arg;
		}

		static std::string Logs(std::wstring& arg)
		{
			if (arg.empty()) {
				return std::string();
			}

			int size = WideCharToMultiByte(CP_UTF8, 0, arg.c_str(), -1, nullptr, 0, nullptr, nullptr);
			if (size == 0) {
				throw std::runtime_error("Failed to convert wstring to string");
			}

			// Создаем буфер для результата
			std::string result(size - 1, '\0'); // size - 1, чтобы исключить завершающий ноль
			WideCharToMultiByte(CP_UTF8, 0, arg.c_str(), -1, &result[0], size, nullptr, nullptr);

			return result;
		}

		template <>
		static std::string Logs<void*>(void* arg)
		{
			return " IS VOID PTR ";
		}

		template <>
		static std::string Logs<char>(char arg)
		{
			return std::string(1, arg);
		}

		template <>
		static std::string Logs<wchar_t>(wchar_t arg)
		{
			return std::string(1, static_cast<char>(arg));
		}

		template <typename Key, typename Value>
		static std::string Logs(const std::pair<const Key, Value>& arg)
		{
			return Logs(arg.first) + ": " + Logs(arg.second);
		}

		// Contatiners concept
		template <typename T>
		concept Container = requires(T t)
		{
			t.begin();
			t.end();
				requires std::input_iterator<decltype(t.begin())>;
				requires !std::is_same_v<std::decay_t<T>, std::string>;
				requires !std::is_same_v<std::decay_t<T>, std::wstring>;
		};

		// containers handling
		template <typename T>
			requires Container<T>
		static std::string Logs(T& arg)
		{
			std::string result = "[";
			for (auto it = arg.begin(); it != arg.end(); ++it)
			{
				result += Logs(*it);
				if (std::next(it) != arg.end())
					result += ", ";
			}
			result += "]";
			return result;
		}

		// perfect forwarding 
		template <typename ... Types>
		static std::string Logs(Types&& ... args)
		{
			return (Logs(std::forward<Types>(args)) + ...);
		}

		// Main functions
		template <typename ... Types>
		static void LogMain(Types&& ... args)
		{
			std::string message;
			message.reserve(128);
			message = Logs(std::forward<Types>(args)...) + "\n";
			::OutputDebugString(message.c_str());
		}

	} // details namespace end


#define LOG_WARNING(...) details::LogMain("warning ", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) details::LogMain("error ", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_MESSAGE(...) details::LogMain("information ", __FILE__, __LINE__, __VA_ARGS__)

}

struct TestStruct
{
	std::string name{ "KURWA" };
};


int main()
{
	using namespace Log;
	int a = 7;
	int* ptr = &a;
	void* ptrVoid = nullptr;

	LOG_WARNING("dwdwdw");

	LOG_WARNING(" Vector X -", 1.0, " --- Is  ", true);
	LOG_WARNING("poiner - ", ptr, " ");
	LOG_MESSAGE("QUAT - W ", 0.56564, " X -  ", 0.12321, " Y - ", 1, " Z - ", 0.777777777777);
	LOG_MESSAGE("  ", ' ', "wdw", " ", nullptr, -556494949);
	LOG_MESSAGE("Void PTR", ptrVoid, &ptrVoid, 1525555.005255555555555555);
	LOG_MESSAGE();
	LOG_MESSAGE("");
	LOG_MESSAGE("Empty string test: ", "");

	double d = 3.14;
	double* ptrD = &d;

	LOG_MESSAGE("Double pointer - ", ptrD);
	LOG_MESSAGE("Nullptr test - ", nullptr);
	LOG_MESSAGE("Void pointer - ", static_cast<void*>(ptrD));
	LOG_MESSAGE("Int max - ", std::numeric_limits<int>::max);
	LOG_MESSAGE("Int min - ", std::numeric_limits<int>::min);
	LOG_MESSAGE("Float epsilon - ", std::numeric_limits<float>::epsilon());
	LOG_MESSAGE("Double infinity - ", std::numeric_limits<double>::infinity());
	LOG_MESSAGE("Negative zero - ", -0.0);
	LOG_MESSAGE("String literal - ", "Hello, World!");
	LOG_MESSAGE("StdString - ", std::string("Hello, World!"));
	LOG_MESSAGE("Special chars - ", "Newline:\nTab:\tQuote:\"");
	LOG_MESSAGE("True test - ", true);
	LOG_MESSAGE("False test - ", false);
	LOG_MESSAGE("Expression test - ", 42 > 0);
	LOG_MESSAGE("Expression test - ", 0 == 1);
	LOG_MESSAGE("Char test - ", 'A');
	LOG_MESSAGE("Wide char test - ", L'Ω');
	LOG_MESSAGE("Special char test - ", '\t', " ddd");
	LOG_MESSAGE("Non-printable char test - ", '\x01', " END");

	int arr[] = { 1, 2, 3 };
	LOG_MESSAGE("Array pointer - ", arr);



	LOG_MESSAGE("C++20 format test - ", std::format("Formatted: {}", 42));

	std::vector<int> vecI = { 1, 2, 3, 4 };
	std::vector<float> vecF = { 1.011f, 202.0522f, 3.000001f, 4.0017f };
	LOG_WARNING("Vector Int: ", vecI);
	LOG_WARNING("Vector Float: ", vecF);

	std::array<bool, 4> ArrayB = { false, true, false, true };
	LOG_WARNING("Array Bools: ", ArrayB);

	std::unordered_map<int, bool> UnorderedMapIntBool;
	UnorderedMapIntBool[0] = false;
	UnorderedMapIntBool[1] = true;
	UnorderedMapIntBool[22] = false;
	UnorderedMapIntBool[111] = false;
	LOG_WARNING("Unordered Map ", UnorderedMapIntBool);


	LOG_ERROR(
		"Many arguments: ",
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, "\n",
		"ERROR ", "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "\t", false, 1 > 0, nullptr, NULL, "ZOPA", ' ', "", "END", "\n",
		UnorderedMapIntBool, "\n"
	);

	std::unique_ptr<TestStruct> testUnique = std::make_unique<TestStruct>();
	std::unique_ptr<TestStruct> testUniqueNull = nullptr;
	LOG_MESSAGE("test unique - ", testUnique.get());
	LOG_MESSAGE("test unique - ", testUniqueNull.get());

	LOG_ERROR("Adress W - ", L"D:\\Logger\\Logger");
	LOG_ERROR("Adress  - ", "D:\\Logger\\Logger");

	std::wstring path = L"D:\\boxyboi\\License";
	LOG_MESSAGE(path);

	return 1;
}