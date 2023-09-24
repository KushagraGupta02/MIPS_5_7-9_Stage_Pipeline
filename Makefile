compile : 
	g++ 5stage.cpp 5stage.hpp -o 5stage.out
	g++ 5stage_bypass.cpp 5stage.hpp -o 5stage_bypass.out
	g++ 79stage.cpp 79stage.hpp -o 79stage.out
	g++ 79stage_bypass.cpp 79stage.hpp -o 79stage_bypass.out

run_5stage: 
	./5stage.out ./input.asm

run_5stage_bypass: 
	./5stage_bypass.out ./input.asm

run_79stage: 
	./79stage.out ./input.asm

run_79stage_bypass: 
	./79stage_bypass.out ./input.asm	

clean:
	rm 5stage.out 
	rm 5stage_bypass.out
	rm 79stage.out 
	rm 79stage_bypass.out