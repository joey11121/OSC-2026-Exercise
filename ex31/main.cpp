#include <iostream>
#include <list>
#include <vector>

#define NUM_PAGES 0x280000
#define MAX_ORDER 10

struct page {
    int order = 0;
    int refcount = 0; //allocated flag
};

std::vector<page> mem_map;
std::vector<std::list<page*>> free_area;

struct page* get_buddy(struct page* page, unsigned int order) {
    //Get the page index in the mem_map and flip the mem_map in the 
    return &mem_map[(page - mem_map.data()) ^ (1 << order)];
}

struct page* alloc_pages(unsigned int order) {
    // TODO: Implement this function
    if(order > MAX_ORDER)
        return nullptr; 
    for(int i = order; i <= MAX_ORDER; i++) {
        if(free_area[i].empty()) continue;
        //Get the front page in the list
        struct page* page = free_area[i].front(); 
        free_area[i].pop_front(); 

        //Find the buddy and find the most suitable order to store the buddy
        while(i > (int)order) {
            i--;
            struct page* buddy = get_buddy(page, i);
            buddy -> order = i; 
            free_area[i].push_back(buddy); 
        }
        page -> order = order; 
        page -> refcount = 1; 
        return page; 
    }
    return nullptr; 
}

void free_pages(struct page* page) {
    // TODO: Implement this function
    unsigned int order = page -> order; 
    page -> refcount = 0; 
    //Find the buddy in each order
    while(order < MAX_ORDER) {
        struct page* buddy = get_buddy(page, order); 
        if(buddy -> refcount == 1 && buddy -> order != order) 
            break; 
    
        if(buddy < page) {
            page = buddy; 
        }
        free_area[order].remove(buddy); 
        order++; 
    }
    page->order = order;
    free_area[order].push_back(page);
}

void dump() {
    for (int i = MAX_ORDER; i >= 0; i--)
        std::cout << "free_area[" << i << "] " << free_area[i].size()
                  << std::endl;
}

int main() {
    mem_map.resize(NUM_PAGES);
    free_area.resize(MAX_ORDER + 1);
    for (size_t i = 0; i < NUM_PAGES; i += (1 << MAX_ORDER)) {
        mem_map[i].order = MAX_ORDER;
        free_area[MAX_ORDER].push_back(&mem_map[i]);
    }

    std::cout << "\np1:\n";
    struct page* p1 = alloc_pages(1);
    dump();

    std::cout << "\np2:\n";
    struct page* p2 = alloc_pages(1);
    dump();

    std::cout << "\np3:\n";
    struct page* p3 = alloc_pages(1);
    dump();

    free_pages(p1);
    free_pages(p2);
    free_pages(p3);

    std::cout << "\nfree:\n";
    dump();
    return 0;
}
