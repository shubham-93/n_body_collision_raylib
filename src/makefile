n_body_collision_raylib: n_body_collision_raylib.c
	cc -o n_body_collision_raylib -Ofast n_body_collision_raylib.c $$(pkg-config --cflags --libs gsl raylib)

# `pkg-config --libs --cflags raylib` is also possible
# Use $ in bash,and $$ in makefile

# To understand more about how to use pkg-config inside makefile, see man pkg-config