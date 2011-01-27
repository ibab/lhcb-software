#define RMB __asm__ volatile ("xorl %%eax,%%eax ; cpuid" : : : "eax", "ebx", "ecx", "edx", "memory")
#define WMB __asm__ volatile ("xorl %%eax,%%eax ; cpuid" : : : "eax", "ebx", "ecx", "edx", "memory")
