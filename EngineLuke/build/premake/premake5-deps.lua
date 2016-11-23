-- This is the starting point of the build scripts for the project.
-- It defines the common build settings that all the projects share
-- and calls the build scripts of all the sub-projects.

dofile "helpers.lua"

ImportConfigs()

solution "deps"

	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }

	configuration {}

	flags { common_flags }
	language "C++"

	location (builddir)
	objdir (builddir .. "/obj/")
	targetdir (libdir)

	print("Searching for dependencies...")
	IncludePremakeProjects(path.join(depsdir,"*"))