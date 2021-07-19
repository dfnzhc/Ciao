
IncludeDir = {}
IncludeDir["SDL2"]          = "%{wks.location}/Ciao/Deps/sdl2/include"
IncludeDir["Glad"]          = "%{wks.location}/Ciao/Deps/glad/include"
IncludeDir["glm"]           = "%{wks.location}/Ciao/Deps/glm/glm"
IncludeDir["spdlog"]        = "%{wks.location}/Ciao/Deps/spdlog/include"
IncludeDir["FreeImage"]     = "%{wks.location}/Ciao/Deps/FreeImage/include"


LibraryDir = {}
LibraryDir["SDL2"]          = "%{wks.location}/Ciao/Deps/sdl2/lib"
LibraryDir["FreeImage"]     = "%{wks.location}/Ciao/Deps/FreeImage/lib"
LibraryDir["FreeImage_DLL"] = "%{wks.location}/Ciao/Deps/FreeImage/bin"

Library = {}
Library["SDL2"]             = "%{LibraryDir.SDL2}/SDL2.lib"
Library["FreeImage"]        = "%{LibraryDir.FreeImage}/FreeImage.lib"
