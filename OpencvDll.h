#pragma once

#include <stdexcept>
#include <windows.h>
#include <iostream>

#ifdef USINGOPENCV_EXPORTS  
#define USINGOPENCV_EXPORTS_API __declspec(dllexport)   
#else  
#define USINGOPENCV_EXPORTS_API __declspec(dllimport)   
#endif  

namespace UsingOpenCV
{
	class Functions
	{
		public:
			static USINGOPENCV_EXPORTS_API int Initialize(char* _Path);
			static USINGOPENCV_EXPORTS_API void Set_Eye_On_Screan_Pos();
			static USINGOPENCV_EXPORTS_API float Get_Eye_On_Screan_Pos_x();
			static USINGOPENCV_EXPORTS_API float Get_Eye_On_Screan_Pos_y();
	};
}
