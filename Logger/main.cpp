#include "Windows.h"
#include "string"
#include <concepts>
#include <type_traits>
#include <format>
#include <limits>

namespace Log
{
	namespace details
	{
		static std::string Logs()
		{
			return ".";
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
				return std::string(" IS NULLPTR");
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
		static std::string Logs<std::string>(std::string arg)
		{
			return arg;
		}

		template <>
		static std::string Logs<void*>(void* arg)
		{
			return " IS VOID PTR ";
		}


		template <typename ... Types>
		static std::string Logs(Types ... args)
		{
			return (Logs(args) + ...);
		}



	}

	template < typename ... Types>
	static void LogWarning(Types ... args)
	{
		std::string Message = "warning " + details::Logs(args...) + "\n";
		::OutputDebugString(Message.c_str());
	}

	template < typename ... Types>
	static void LogError(Types ... args)
	{
		std::string Message = "error " + details::Logs(args...) + "\n";
		::OutputDebugString(Message.c_str());
	}

	template <typename ... Types>
	static void LogMessage(Types ... args)
	{
		std::string Message = "information " + details::Logs(args...) + "\n";
		::OutputDebugString(Message.c_str());
	}

}


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
	//Log::LogMessage(); 
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
	Log::LogMessage("Expression test - ", 42 > 0); // true
	Log::LogMessage("Expression test - ", 0 == 1); // false




	return 1;
}