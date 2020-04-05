ourmake: main.c make_modules.c segmentation.c main_memory.c l1_tlb.c l2_tlb.c L1_cache.c L2_cache.c
	gcc -Wall main.c make_modules.c segmentation.c main_memory.c l1_tlb.c l2_tlb.c L1_cache.c L2_cache.c -o out
run: 
	./out