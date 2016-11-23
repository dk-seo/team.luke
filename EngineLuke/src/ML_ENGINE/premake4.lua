project "ML_ENGINE"

	kind "ConsoleApp"

	SetupNativeProjects()

	files
	{
		-- local assignment files
		"*.lua"
		, "**.h"
		, "**.c"
		, "**.cpp"
	}

	includedirs
	{
		"../ML_ENGINE",
		"$(DXSDK_DIR)Include",
		Core.includedirs
	}

	defines
	{
		Core.defines,
		-- Defines
	}

  postbuildcommands 
  {
  }

	configuration "windows"
		includedirs( Core.msvcincludedirs );
		defines( Core.msvcdefines );
		links( Core.msvclinks );

	configuration "Debug"
		defines
		{
			"_DEBUG"
			, "MEMORY_DEBUGGING"
			,"_HAS_ITERATOR_DEBUGGING=1"
			, "_SECURE_SCL=1"
			, "_ITERATOR_DEBUG_LEVEL=2"
		}

		flags { "Symbols" }

	configuration "Release"
		defines {"NDEBUG", "RELEASE"}
		flags {"Optimize"}