target = simulated_annealing
CPP = c++
BUILD_DIR = build

$(target) :
	@mkdir -p $(BUILD_DIR)
	$(CPP) $(target).cpp libsbox.cpp des_criteria.cpp -o $(BUILD_DIR)/$(target)

clean :
	rm -r $(BUILD_DIR)
