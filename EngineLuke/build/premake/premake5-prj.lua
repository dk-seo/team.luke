-- This is the starting point of the build scripts for the project.
-- It defines the common build settings that all the projects share
-- and calls the build scripts of all the sub-projects.

dofile "helpers.lua"

solution "MLEngine"
	
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	--defaultplatform "x32"
	
	flags { common_flags }
	language "C++"
	framework "4.5"
	
	location (builddir)
	objdir (builddir .. "/obj/")
	
	debugdir (bindir)

	-- Build configuration options

	configuration "Debug"
		defines { "DEBUG" }
		
	configuration {}

	-- group "core"
	-- 	dofile( fwdir .. "/premake4.lua")

	group "projects"
		print("Searching for projects...")
		IncludePremakeProjects(path.join(srcdir,"*"))
