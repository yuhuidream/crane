#ifndef h_smp_h
#define h_smp_h

typedef void (*smp_call_func_t)(void *info);

#define CPU_MASK  0x3
#define CSD_FLAG_LOCK   0x1
#define SGI_CALL_FUNCTION   GIC_INT_SW_14

#ifdef CONFIG_SMP
int on_each_cpu(void (*func) (void *info), void *info, int wait);
#else
#define on_each_cpu(func,info,wait)		\
	({					\
		func(info);			\
		0;				\
	})
#endif

#define CASE_ON_MP
#if defined(CONFIG_SMP) && defined(CASE_ON_MP)
#define DEF_MP_VAR(x)   x[CORE_NUM]
#define MP_VAR(x)       x[core_id]
#define DEF_GET_COREID    int core_id = smp_processor_id();
#define DEF_ARRAY_1(x)  x
#define DEF_ARRAY_2(x)  DEF_ARRAY_1(x), x
#define DEF_ARRAY_3(x)  DEF_ARRAY_2(x), x
#define DEF_ARRAY_4(x)  DEF_ARRAY_3(x), x
#define CAT(a, b)   a ## b
#define DEF_ARRAY(n, x) CAT(DEF_ARRAY_, n)(x)
#define DEF_CORE_ARRAY(x) DEF_ARRAY(CORE_NUM, x)
#else
#define DEF_MP_VAR(x)  x
#define MP_VAR(x)      x
#define DEF_GET_COREID
#define DEF_CORE_ARRAY(x)  x
#endif  

#endif
