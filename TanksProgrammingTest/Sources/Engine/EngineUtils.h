#pragma once

#include "Windows.h"
#include <string>
#include <SDL.h>

namespace EngineUtils
{
#define checkMsg(condition, errorMessage)                             \
    do {                                                           \
        if (!(condition)) {                                        \
            std::cerr << "Assertion failed: " << (errorMessage)    \
                      << "\nCondition: " << #condition             \
                      << "\nFile: " << __FILE__                    \
                      << "\nLine: " << __LINE__ << std::endl;      \
            assert(condition);                                     \
        }                                                          \
    } while (false)
	
	class ProfileScope
	{
	public:
#if ENABLE_PROFILING
		ProfileScope(const std::string& label)
			: m_Label(label), m_StartTime(SDL_GetTicks())
		{
		}
		
		~ProfileScope()
		{
			Uint32 endTime = SDL_GetTicks();
			Uint32 elapsed = endTime - m_StartTime;

			printf("[PROFILE] %s took %i ms\n", m_Label.c_str(), elapsed);
		}

	private:
		std::string m_Label;
		Uint32 m_StartTime;
#else
		ProfileScope(const std::string& label)
		{
		}
#endif
	};

	inline std::string WstringToString(std::wstring WstringToConvert)
	{
		if (!WstringToConvert.empty())
		{
			int WstringSize = static_cast<int>(WstringToConvert.size());
			int Size = WideCharToMultiByte(CP_UTF8, 0, &WstringToConvert[0], WstringSize, nullptr, 0, nullptr, nullptr);
			std::string ConvertedString(Size, 0);
			WideCharToMultiByte(CP_UTF8, 0, &WstringToConvert[0], WstringSize, &ConvertedString[0], Size, nullptr, nullptr);
			return ConvertedString;
		}

		return std::string();
	}

	inline std::wstring StringToWstring(std::string StringToConvert)
	{
		if (!StringToConvert.empty())
		{
			int StringSize = static_cast<int>(StringToConvert.size());
			int Size = MultiByteToWideChar(CP_UTF8, 0, &StringToConvert[0], StringSize, nullptr, 0);
			std::wstring ConvertedWstring(Size, 0);
			MultiByteToWideChar(CP_UTF8, 0, &StringToConvert[0], StringSize, &ConvertedWstring[0], Size);
			return ConvertedWstring;
		}

		return std::wstring();
	}

}
