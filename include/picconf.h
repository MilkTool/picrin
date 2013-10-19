#ifndef PICCONF_H__
#define PICCONF_H__

/* switch normal VM and direct threaded VM */
#define PIC_DIRECT_THREADED_VM 1

/* enable readline module */
#define PIC_ENABLE_READLINE 1

/* initial memory size (to be dynamically extended if necessary) */
#define PIC_ARENA_SIZE 100
#define PIC_HEAP_SIZE 16384
#define PIC_STACK_SIZE 1024
#define PIC_IREP_SIZE 256
#define PIC_GLOBALS_SIZE 1024

/* enable all debug flags */
#define DEBUG 1


#if DEBUG
# define GC_DEBUG 1
# define VM_DEBUG 1
#endif

#endif
