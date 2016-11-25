-- This module checks for the all the project dependencies.

action = _ACTION or ""

srcdir = path.getabsolute("../../src");
bindir = path.getabsolute("../../bin");
builddir = path.getabsolute("../" .. action);

--examplesdir = path.getabsolute("../../examples");
--gendir = path.join(builddir, "gen");

common_flags = { "Symbols", "NoExceptions" }
msvc_buildflags = { "/wd4190", "/wd4996", "/wd4530", "/EHsc" }
gcc_buildflags = { "-Wno-invalid-offsetof", "-std=gnu++11" }

msvc_cpp_defines = { "_SECURE_SCL=0", "_HAS_ITERATOR_DEBUGGING=0" }

function debug(msg)
	-- print(msg)
end

function SafePath(path)
	return "\"" .. path .. "\""
end

dofile "config0.lua"

-- Copy a configuration build header if one does not exist yet.
function ImportConfigs()
    if not os.isfile("../config.lua") then
        print("Build configuration file 'config.lua' created")
        os.copyfile("config0.lua", "../config.lua")
    end
    dofile "../config.lua"
end

function IncludePremakeProjects(basedir)
    IncludePremake(basedir, "premake4.lua")
end

function IncludePremake(basedir, premakefile)
    local deps = os.matchdirs(basedir)
    
    for i,dep in ipairs(deps) do
        local fp = path.join(dep, premakefile)
        fp = path.join(os.getcwd(), fp)
        
        if os.isfile(fp) then
            print(string.format(" including %s", fp))
            dofile(fp)
        end
    end
end

function SetupNativeBuildFlags()
	-- Compiler-specific options

	--configuration "pnacl"
	--	system "nacl"
	--	architecture "pnacl"
	
	configuration { "vs*", "not pnacl" }
		buildoptions { msvc_buildflags, "/wd4251" }
		defines { msvc_cpp_defines }
		
	configuration "gcc"
		buildoptions { gcc_buildflags }

end

function SetupNativeProjects()
	location (path.join(builddir, "projects"))

	local c = configuration "Debug"
		defines { "_DEBUG" }
		
	configuration "Release"
		defines { "NDEBUG" }
	
	SetupNativeBuildFlags()
	configuration(c)
end

function EmbedFiles(resources)
    files (resources)

    local c = configuration (resources[1])
            buildaction "Embed"
            
    for resourceIt = 2, #resources do
        configuration (resources[resourceIt])
            buildaction "Embed"
    end
    
    configuration(c)
end

function DoNotBuildInNativeClient()
	local c = configuration "pnacl"
		buildaction "None"
	configuration(c)
end


function GenerateBuildConfig(conf)
	local contents = ""
	
	for key,value in pairs(conf) do
		local define = string.format("#define ENABLE_%s", key)
		if value then
			--print(string.format("Enabling %s", key))
		else
			define = "//" .. define
		end
		contents = contents .. define .. "\n"
	end
	
	return contents
end

function WriteConfigToFile(conf, path)
	file = io.open(path, "w")
	file:write(conf)
	file:close()
end