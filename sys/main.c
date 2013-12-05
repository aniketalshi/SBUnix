#include <defs.h>
#include <screen.h>
#include <sys/common.h>
#include <sys/init_desc_table.h>
#include <sys/irq_common.h>
#include <sys/types.h>
#include <sys/paging.h>
#include <sys/phys_mm.h>
#include <sys/virt_mm.h>
#include <sys/kmalloc.h>
#include <sys/proc_mngr.h>
#include <sys/elf.h>
#include <sys/tarfs.h>
#include <sys/fs.h>
#include <sys/dirent.h>
#include <io_common.h>
#include <sys/ahci.h>

#define K_MEM_PAGES 518
#define INITIAL_STACK_SIZE 4096

char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
extern bool InitScheduling;

void start(uint32_t* modulep, void* physbase, void* physfree)
{
    uint64_t phys_size = 0x0, phys_base = 0x0;

    struct smap_t {
        uint64_t base, length;
        uint32_t type;
    }__attribute__((packed)) *smap;
    while(modulep[0] != 0x9001) modulep += modulep[1]+2;
    for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
        if (smap->type == 1 /* memory */ && smap->length != 0) {
            set_cursor_pos(3, 5);
            kprintf("Available Physical Memory [%x-%x]", smap->base, smap->base + smap->length);
            phys_base = smap->base;
            phys_size = smap->length;
        }
    }

    // kernel starts here

    phys_init(phys_base, (uint64_t) physfree, phys_size); 

    init_paging((uint64_t)&kernmem, (uint64_t)physbase, K_MEM_PAGES);

    // Reset the kernel stack
    __asm__ __volatile__("movq %0, %%rbp" : :"a"(&stack[0]));
    __asm__ __volatile__("movq %0, %%rsp" : :"a"(&stack[INITIAL_STACK_SIZE]));

    // Initialize tarfs structure of FS
    init_tarfs();

    // Schedule an Idle Kernel Process 
    create_idle_process();

    // Create an init process to invoke shell
    create_elf_proc("/rootfs/bin/init", NULL);
    
    // Disable Process Scheduling
    InitScheduling = FALSE;

    // Allow interrupts
    sti;

    // Init Disk and don't force create a new File System
    init_disk(FALSE);

    // Enable Process Scheduling
    InitScheduling = TRUE;

    while(1);
}


void boot(void)
{
    // note: function changes rsp, local stack variables can't be practically used
    __asm__(
            "movq %%rsp, %0;"
            "movq %1, %%rsp;"
            :"=g"(loader_stack)
            :"r"(&stack[INITIAL_STACK_SIZE])
           );

    // Disable interrupts
    cli;

    // Intialize
    init_gdt();
    init_tss();
    init_idt();
    init_pic();
    init_screen();
    init_timer(1);
    init_keyboard();

    set_cursor_pos(0, 25);
    kprintf("________________");
    set_cursor_pos(1, 25);
    kprintf("|    SBUnix    |");
    set_cursor_pos(2, 25);
    kprintf("----------------");

    start(
            (uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
            &physbase,
            (void*)(uint64_t)loader_stack[4]
         );

    while(1);
}

