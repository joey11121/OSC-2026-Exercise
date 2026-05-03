#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FDT_BEGIN_NODE 0x00000001
#define FDT_END_NODE   0x00000002
#define FDT_PROP       0x00000003
#define FDT_NOP        0x00000004
#define FDT_END        0x00000009

// Flattend Device Tree
// Big Endian
// off_dt_struct: tokens with data
struct fdt_header {
    uint32_t magic; //magic (0xd00dfeed)
    uint32_t totalsize;
    uint32_t off_dt_struct; //useful, contain the offset in bytes of the structure block(see Section 5.4) from the beginning of the header.
    uint32_t off_dt_strings; //useful, contain the offset in bytes of the strings block, property names
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys;
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
};

typedef struct {
    int len; 
    const char* start; 
}Pathcomp; 

static inline uint32_t bswap32(uint32_t x) {
    return __builtin_bswap32(x);
}

static inline uint64_t bswap64(uint64_t x) {
    return __builtin_bswap64(x);
}

static inline const void* align_up(const void* ptr, size_t align) {
    return (const void*)(((uintptr_t)ptr + align - 1) & ~(align - 1));
}

int component_count(const char* path) {
    int count = 0; 
    int inname = 0; 
    while(*path != '\0') {
        if(*path == '/') {
            inname = 1; 
        } else if(inname) {
            count++; 
            inname = 0; 
        }
        path++; 
    }
    return count; 
}

void split_path(Pathcomp* components, const char* path) {
    int i = 0;

    while (*path) {
        if (*path == '/') {
            path++;
            continue;
        }

        components[i].start = path;
        components[i].len = 0;

        while (*path && *path != '/') {
            components[i].len++;
            path++;
        }

        i++;
    }
}

int compcmp(const char* s1, const char* s2, int len) {
    int i = 0; 
    for(i = 0; i < len; i++) {
        if(s1[i] != s2[i] || s1[i] == '\0') {
            return 0; 
        }
    }
    return s1[len] == '\0' || s1[len] == '@'; 
}


/*int namecmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && (*s1 == *s2)) {
        s1++;
        s2++;
    }


    return *(unsigned char*) s1 - *(unsigned char*) s2;
}*/


int fdt_path_offset(const void* fdt, const char* path) {
    // TODO: Implement this function
    // Return the offset of a path relative in the FDT structure block
  
    // Read the header, Read the big-endian data in the little-endian CPU. 
    struct fdt_header* hdr = (struct fdt_header*) fdt; 
    uint32_t magic = bswap32(hdr -> magic); 
    uint32_t off_dt_struct = bswap32(hdr -> off_dt_struct);

    if (magic != 0xd00dfeed) {
        return -1;
    }

    if(strcmp("/", path) == 0) 
        return off_dt_struct; 
    uint32_t* p = (uint32_t*)((char*)fdt + off_dt_struct); //off_dt_struct is byte offset 
    uint32_t depth = 0, match_depth = 0; 
    uint32_t *token_addr; 

    int count = component_count(path); 
    Pathcomp* components = (Pathcomp*)malloc(sizeof(Pathcomp) * count); 
    if(!components) {
        return -1; 
    }
    split_path(components, path); 
    /*printf("Count of Components: %d\n", count); 
    for(int i = 0; i < count; i++) {
        printf("%s\n", components[i].start); 
        printf("length: %d\n", components[i].len); 
    }*/

    int matched_at_path[128] = {0}; 
    while(1) {
        token_addr = p; 
        switch(bswap32(*p)){
            case FDT_BEGIN_NODE:
                p += 1; //skip the token identifier. 
                depth++; 
                char* name = (char*) p;
                matched_at_path[depth] = 0;
                //printf("current name: %s\n", name); 
                if(depth >= 2 && match_depth < count) {
                    if(compcmp(name, components[match_depth].start, components[match_depth].len)) {
                        match_depth++; 
                        matched_at_path[depth] = 1; 
                        if(match_depth == count) {
                            free(components);
                            //printf("The return token address: %p\n", token_addr); 
                            //printf("The fdt address: %p\n", fdt); 
                            return (uint8_t*)token_addr - (uint8_t*)fdt; 
                        } 
                    } 
                }
               
                p = (uint32_t*)align_up((const void*)((char*)p + strlen(name) + 1), 4); // Important: Align up to the 4 bytes
                break;
            case FDT_END_NODE:
                if(matched_at_path[depth]){
                    match_depth--; 
                }
                depth--;
                p++; 
                break; 
            case FDT_PROP:
                p++; 
                uint32_t len = bswap32(*p); //The number of bytes of property node contains. 
                p += 2; // Skip the nameoff and the value
                p = (uint32_t*)align_up((const void*)((const char*)p + len), 4); 
                break; 
            case FDT_NOP:
                p++; 
                break; 
            case FDT_END:
                free(components); 
                return -1; 
            default:
                free(components); 
                return -1; 
        }
        
    }  
}

const void* fdt_getprop(const void* fdt,
                        int nodeoffset,
                        const char* name,
                        int* lenp) {
    // TODO: Implement this function
    // Return the pointer to property 
    struct fdt_header *hdr = (struct fdt_header*)fdt; 
    uint32_t off_dt_strings = bswap32(hdr -> off_dt_strings); 
    uint32_t* p = (uint32_t*)((uint8_t*)fdt + nodeoffset); 
    p++;
    const char* nodename = (const char*)p;
    p = (uint32_t*)align_up((const void*)(nodename + strlen(nodename) + 1), 4);
    while(1) {
        switch(bswap32(*p)){
            case FDT_BEGIN_NODE:
                return NULL; 
                break;
            case FDT_END_NODE:
                return NULL; 
                break; 
            case FDT_PROP:
                p++; //skip node
                uint32_t len = bswap32(*p); //The number of bytes of property node contains. 
                p++; //go to next field
                uint32_t nameoff = bswap32(*p); 
                p++; 
                const char* propname  = (const char*)fdt + off_dt_strings + nameoff; 
                if(strcmp(propname, name) == 0) {
                    *lenp = len; 
                    return p; 
                } else {
                    p = (uint32_t*)align_up((const void*)((const char*)p + len), 4);
                }
                
                break; 
            case FDT_NOP:
                p++; 
                break; 
            case FDT_END:
                return NULL; 
                break;
            default:
                break; 
        }
    }
    
}

int main() {
    /* Prepare the device tree blob */
    FILE* fp = fopen("qemu.dtb", "rb");
    if (!fp) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    fseek(fp, 0, SEEK_END); //set the file pointer
    long sz = ftell(fp);   //Access the position of the file pointer from start
    void* fdt = malloc(sz);
    fseek(fp, 0, SEEK_SET);
    if (fread(fdt, 1, sz, fp) != sz) {
        fprintf(stderr, "Failed to read the device tree blob\n");
        free(fdt);
        fclose(fp);
        return EXIT_FAILURE;
    }
    fclose(fp);

    /* Find the node offset */
    int offset = fdt_path_offset(fdt, "/cpus/cpu@0/interrupt-controller");
    if (offset < 0) {
        fprintf(stderr, "fdt_path_offset\n");
        free(fdt);
        return EXIT_FAILURE;
    }

    /* Get the node property */
    int len;
    const void* prop = fdt_getprop(fdt, offset, "compatible", &len);
    if (!prop) {
        fprintf(stderr, "fdt_getprop\n");
        free(fdt);
        return EXIT_FAILURE;
    }
    printf("compatible: %.*s\n", len, (const char*)prop);

    offset = fdt_path_offset(fdt, "/memory");
    if (offset < 0) {
        fprintf(stderr, "fdt_path_offset\n");
        free(fdt);
        return EXIT_FAILURE;
    }

    prop = fdt_getprop(fdt, offset, "reg", &len);
    const uint64_t* reg = (const uint64_t*)prop;
    printf("memory: base=0x%lx size=0x%lx\n", bswap64(reg[0]), bswap64(reg[1]));

    offset = fdt_path_offset(fdt, "/chosen");
    prop = fdt_getprop(fdt, offset, "linux,initrd-start", &len);
    const uint64_t* initrd_start = (const uint64_t*)prop;
    printf("initrd-start: 0x%lx\n", bswap64(initrd_start[0]));

    free(fdt);
    return 0;
}
