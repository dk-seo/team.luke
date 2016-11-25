/******************************************************************************/
/*!
\file  Engine.h
\project  CS460
\author Jorge Antonio Robles

Copyright (C) 2015 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************************/
#pragma once

#define WIN32_LEAN_AND_MEAN	
#include <Windows.h>

#include <string>
#include <vector>
#include "..\UI\UserInterface.h"
class Engine
{
	public:

		//CONSTRUCTORS
		Engine(void);
		~Engine(void);

		//SYSTEM METHODS
		bool Initialize(HINSTANCE hInstance);
		void Shutdown(void);
		void Run(void);

		bool IsRunning() const;

		void Quit(void);

		//Get Engine Instance
		static Engine* GetEngine(void);

	private:
		bool m_IsRunning;
};