
IncludeDir = {}
IncludeDir["SDL2"]      = "%{wks.location}/Ciao/Deps/sdl2/include"
IncludeDir["Glad"]      = "%{wks.location}/Ciao/Deps/glad/include"
IncludeDir["glm"]       = "%{wks.location}/Ciao/Deps/glm/glm"
IncludeDir["spdlog"]    = "%{wks.location}/Ciao/Deps/spdlog/include"


LibraryDir = {}
LibraryDir["SDL2"]      = "%{wks.location}/Ciao/Deps/sdl2/lib"


Library = {}
Library["SDL2"]         = "%{LibraryDir.SDL2}/SDL2.lib"