#include <iostream>
#include <list>
#include <vector>

#define PAGE_SIZE (1UL << 12)
#define NUM_PAGES 0x280000
#define MAX_ORDER 10

typedef unsigned long phys_addr_t;

struct page {
    int order = 0;
    int refcount = 0;
};

std::vector<page> mem_map;
std::vector<std::list<page*>> free_area;

struct page* get_buddy(struct page* page, unsigned int order) {
    return &mem_map[(page - mem_map.data()) ^ (1 << order)];
}

void memory_reserve(phys_addr_t base, size_t size) {
    // TODO: Implement this function
    int start_pfn = base / PAGE_SIZE;
    int end_pfn = (base + size + PAGE_SIZE - 1) / PAGE_SIZE; 

    for(int i = MAX_ORDER; i >= 0; i--) {
        if(free_area[i].empty())
            continue; 
        auto it = free_area[i].begin(); 

        while(it != free_area[i].end()) {
            int block_start_pfn = (*it) - mem_map.data(); 
            int block_end_pfn = block_start_pfn + (1 << i); 
            if(block_start_pfn >= end_pfn || block_end_pfn <= start_pfn) {
                it++; 
                continue; 
            } else
            if(block_start_pfn >= start_pfn && block_end_pfn <= end_pfn) {
                (*it) -> refcount = 1; 
                it = free_area[i].erase(it);
                continue; 
            } else {
                //partially occupied
                //i is the order, find the buddy of *it
                int j = i - 1; 
                struct page* buddy = get_buddy((*it), j); 
                //push the page and the buddy intor the lower order
    
                free_area[j].push_back((*it)); 
                free_area[j].push_back(buddy); 
                (*it) -> order = j; 
                buddy -> order = j; 
                it = free_area[i].erase(it); 
                continue; 
            }
            
        }
    }    
}

void dump() {
    for (int i = MAX_ORDER; i >= 0; i--)
        std::cout << "free_area[" << i << "] " << free_area[i].size()
                  << std::endl;
}

void mm_init() {
    mem_map.resize(NUM_PAGES);
    free_area.resize(MAX_ORDER + 1);
    for (size_t i = 0; i < NUM_PAGES; i += (1 << MAX_ORDER)) {
        mem_map[i].order = MAX_ORDER;
        free_area[MAX_ORDER].push_back(&mem_map[i]);
    }
    memory_reserve(0, 0x82a69510);
}

int main() {
    mm_init();
    dump();
    return 0;
}
