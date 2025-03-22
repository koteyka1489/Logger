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

		static std::string Logs()
		{
			return "";
		}

		template <typename T>
			requires std::is_convertible_v<T, std::string> &&
			(!std::is_same_v<std::decay_t<T>, void*>) &&
			(!std::is_same_v<std::decay_t<T>, std::nullptr_t>)
		static std::string Logs(T arg)
		{
			return std::string(arg);
		}

		template <typename T>
			requires std::is_same_v<std::decay_t<T>, std::nullptr_t>
		static std::string Logs(T arg)
		{
			return " IS NULLPTR";
		}

		template <typename T>
			requires std::is_floating_point_v<T> || std::is_integral_v<T>
		static std::string Logs(T arg)
		{
			return " " + std::to_string(arg) + " ";
		}

		template <typename T>
			requires std::is_pointer_v<T> && (!std::is_same_v<std::decay_t<T>, const char*>)
		static std::string Logs(T arg)
		{
			if (arg)
			{
				std::string address = std::format(" Address 0x{:x}", reinterpret_cast<uintptr_t>(arg));
				return address;
			}
			else
			{
				return std::string(" IS NULL PTR");
			}
		}

		template <>
		static std::string Logs<bool>(bool arg)
		{
			return arg ? " TRUE " : " FALSE ";
		}

		template <>
		static std::string Logs<const char*>(const char* arg)
		{
			return std::string(arg);
		}

		template <>
		static std::string Logs<std::string&>(std::string& arg)
		{
			return arg;
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

		template <>
		static std::string Logs<char16_t>(char16_t arg)
		{
			return std::string(1, static_cast<char>(arg)); 
		}

		template <>
		static std::string Logs<char32_t>(char32_t arg)
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

	} // details namespace end


	// Main functions
	template <typename ... Types>
	static void LogWarning(Types&& ... args)
	{
		std::string Message = "warning " + details::Logs(std::forward<Types>(args)...) + "\n";
		::OutputDebugString(Message.c_str());
	}

	template <typename ... Types>
	static void LogError(Types&& ... args)
	{
		std::string Message = "error " + details::Logs(std::forward<Types>(args)...) + "\n";
		::OutputDebugString(Message.c_str());
	}

	template <typename ... Types>
	static void LogMessage(Types&& ... args)
	{
		std::string Message = "information " + details::Logs(std::forward<Types>(args)...) + "\n";
		::OutputDebugString(Message.c_str());
	}

}

struct TestStruct
{
	std::string name{ "KURWA" };
};


int main()
{
	int a = 7;
	int* ptr = &a;
	void* ptrVoid = nullptr;

	Log::LogWarning(" Vector X -", 1.0, " --- Is  ", true);
	Log::LogError("poiner - ", ptr, " ");
	Log::LogMessage("QUAT - W ", 0.56564, " X -  ", 0.12321, " Y - ", 1, " Z - ", 0.777777777777);
	Log::LogMessage("  ", ' ', "wdw", " ", nullptr, -556494949);
	Log::LogMessage("Void PTR", ptrVoid, &ptrVoid, 1525555.005255555555555555);
	Log::LogMessage(); 
	Log::LogMessage(""); 
	Log::LogMessage("Empty string test: ", ""); 

	double d = 3.14;
	double* ptrD = &d;

	Log::LogMessage("Double pointer - ", ptrD); 
	Log::LogMessage("Nullptr test - ", nullptr); 
	Log::LogMessage("Void pointer - ", static_cast<void*>(ptrD)); 

	Log::LogMessage("Int max - ", std::numeric_limits<int>::max);
	Log::LogMessage("Int min - ", std::numeric_limits<int>::min);
	Log::LogMessage("Float epsilon - ", std::numeric_limits<float>::epsilon());
	Log::LogMessage("Double infinity - ", std::numeric_limits<double>::infinity());
	Log::LogMessage("Negative zero - ", -0.0);

	Log::LogMessage("String literal - ", "Hello, World!");
	Log::LogMessage("StdString - ", std::string("Hello, World!"));
	Log::LogMessage("Special chars - ", "Newline:\nTab:\tQuote:\"");

	Log::LogMessage("True test - ", true);
	Log::LogMessage("False test - ", false);
	Log::LogMessage("Expression test - ", 42 > 0); 
	Log::LogMessage("Expression test - ", 0 == 1); 

	Log::LogMessage("Char test - ", 'A');
	Log::LogMessage("Wide char test - ", L'Ω'); 
	Log::LogMessage("Special char test - ", '\t', " ddd"); 
	Log::LogMessage("Non-printable char test - ", '\x01', " END"); 

	int arr[] = { 1, 2, 3 };
	Log::LogMessage("Array pointer - ", arr); 

	Log::LogMessage(
		"Many arguments: ",
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
		"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"
	);

	Log::LogMessage("C++20 format test - ", std::format("Formatted: {}", 42));

	std::vector<int> vecI = { 1, 2, 3, 4 };
	std::vector<float> vecF = { 1.011f, 202.0522f, 3.000001f, 4.0017f };
	Log::LogMessage("Vector Int: ", vecI);
	Log::LogMessage("Vector Float: ", vecF);

	std::array<bool, 4> ArrayB= { false, true, false, true };
	Log::LogMessage("Array Bools: ", ArrayB);

	std::unordered_map<int, bool> UnorderedMap;
	UnorderedMap[0] = false;
	UnorderedMap[1] = true;
	UnorderedMap[22] = false;
	UnorderedMap[111] = false;
	Log::LogMessage("Unordered Map ", UnorderedMap);

	std::unique_ptr<TestStruct> testUnique = std::make_unique<TestStruct>();
	//Log::LogMessage("test unique - ", testUnique)


	return 1;
}