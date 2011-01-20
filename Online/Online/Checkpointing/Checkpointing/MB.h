#define RMB asm volatile ("xorl %%eax,%%eax ; cpuid" : : : "eax", "ebx", "ecx", "edx", "memory")
#define WMB asm volatile ("xorl %%eax,%%eax ; cpuid" : : : "eax", "ebx", "ecx", "edx", "memory")
