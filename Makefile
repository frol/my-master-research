target = simulated_annealing
CPP = c++
BUILD_DIR = build

$(target) :
	@mkdir -p $(BUILD_DIR)
	$(CPP) $(target).cpp libsbox.cpp des_criteria.cpp -o $(BUILD_DIR)/$(target)

optimal :
	@mkdir -p $(BUILD_DIR)
	$(CPP) -O3 -march=i686 -fforce-addr -funroll-loops -frerun-cse-after-loop -frerun-loop-opt \
	$(target).cpp libsbox.cpp des_criteria.cpp -o $(BUILD_DIR)/$(target)

clean :
	rm -r $(BUILD_DIR)
