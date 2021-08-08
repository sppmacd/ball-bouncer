cmake_minimum_required(VERSION 3.13)
project(Ball)

find_package(SFML 2.5.1 COMPONENTS graphics audio REQUIRED)
add_executable(ball
	"Ball.cpp"
	"Box.cpp"
	"Game.cpp"
	"Helpers.cpp"
	"Particle.cpp"
	"PowerBox.cpp"
	"main.cpp"
)
target_link_libraries(ball PUBLIC sfml-graphics)

if(${MINGW})
	target_compile_options(ball PUBLIC -static -static-libgcc -static-libstdc++)
	target_link_options(ball PUBLIC -static -static-libgcc -static-libstdc++)
endif()
