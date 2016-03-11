solution "aogl"
   configurations { "Debug", "Release" }
   platforms {"native", "x64", "x32"}

   project "demo"
      kind "ConsoleApp"
      language "C++"
      files { "src/**.cpp"}
      includedirs { "lib/glfw/include", "src", "common", "lib/" }
      links {"imgui", "glfw", "glew", "stb"}
      defines { "GLEW_STATIC" }
      buildoptions { "-std=c++11",  "-Wall" }
     
      configuration { "linux" }
         links {"X11","Xrandr", "Xi", "Xxf86vm", "rt", "GL", "GLU", "pthread", "assimp"}
       
      configuration { "windows" }
         links {"glu32","opengl32", "gdi32", "winmm", "user32", "assimp"}

      configuration { "macosx" }
         linkoptions { "-framework OpenGL", "-framework CoreVideo" , "-framework Cocoa", "-framework IOKit"}
         
       
      configuration "Debug"
         defines { "DEBUG" }
         libdirs { "bin/debug/" }
         flags {"ExtraWarnings", "Symbols" }
         location "build/" 
         targetsuffix "_d"

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize"}  
         targetdir "bin/release"  

   -- GLFW Library
   project "glfw"
      kind "StaticLib"
      language "C"
      files { "lib/glfw/include/GL/glfw.h" }
      includedirs { "lib/glfw/lib", "lib/glfw/include"}

      configuration {"linux"}
         files { "lib/glfw/lib/*.c", "lib/glfw/*.h" }
         excludes { "lib/glfw/lib/egl_*", "lib/glfw/lib/cocoa_*",  "lib/glfw/lib/win32_*", "lib/glfw/lib/wgl_*" }
         defines { "_GLFW_GLX",  "_GLFW_X11", "_GLFW_USE_OPENGL", "_GLFW_USE_LINUX_JOYSTICKS", "_GLFW_HAS_XRANDR", "_GLFW_HAS_PTHREAD" ,"_GLFW_HAS_SCHED_YIELD", "_GLFW_HAS_GLXGETPROCADDRESS" }
         buildoptions { "-std=c++11",  "-Wall", "-pthread" }

      configuration {"windows"}
         files { "lib/glfw/lib/*.c", "lib/glfw/*.h" }
         excludes { "lib/glfw/lib/egl_*", "lib/glfw/lib/cocoa_*", "lib/glfw/lib/x11_*", "lib/glfw/lib/glx_*" }
         includedirs { "lib/glfw/lib/win32" }
         defines { "_GLFW_WGL", "_GLFW_WIN32", "_GLFW_USE_OPENGL", "_GLFW_HAS_PTHREAD" ,"_GLFW_HAS_SCHED_YIELD" }

      configuration {"Macosx"}
         files { "lib/glfw/lib/*.c", "lib/glfw/lib/*.m",  "lib/glfw/*.h" }
         excludes { "lib/glfw/lib/egl_*",  "lib/glfw/lib/glx_*", "lib/glfw/lib/x11_*", "lib/glfw/lib/win32_*", "lib/glfw/lib/wgl_*" }
         defines { "_GLFW_COCOA", "_GLFW_NSGL", "_GLFW_USE_OPENGL", "_GLFW_HAS_PTHREAD" ,"_GLFW_HAS_SCHED_YIELD"}
         buildoptions {  "-Wall", "-pthread" }

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
         targetdir "bin/debug"


      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }
         targetdir "bin/release"

   -- GLEW Library         
   project "glew"
      kind "StaticLib"
      language "C"
      files {"lib/glew/*.c", "lib/glew/*.h"}
      defines { "GLEW_STATIC" }

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
         targetdir "bin/debug"

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }    
         targetdir "bin/release"

   -- imgui Library         
   project "imgui"
      kind "StaticLib"
      language "C++"
      files {"lib/imgui/*.cpp", "lib/imgui/*.h"}
      includedirs { "lib/", "lib/glfw/include" }

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
         targetdir "bin/debug"

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }    
         targetdir "bin/release"

   -- stb Library         
   project "stb"
      kind "StaticLib"
      language "C"
      files {"lib/stb/*.c", "lib/stb/*.h"}

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols" }
         targetdir "bin/debug"

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize" }    
         targetdir "bin/release"
