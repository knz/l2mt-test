#ifndef SLEXT_H
#define SLEXT_H

#define sl_redvararg(Var) sl_glarg(unsigned,,Var)
#define sl_redvar(Name) sl_glparm(unsigned, Name)
#define sl_setresult(Var, Value) __asm__ __volatile__("r_write %0, %1, %%g0" : : "r"(sl_getp(Var)|__slPI), "rI"(Value));
#define sl_getresult(Var) ({ unsigned __res; __asm__("r_read %1, %0" : "=r"(__res) : "r"(sl_getp(Var)|__slPI)); __res; })

#define sl_alloc_redvar(Var) do { \
	__asm__("r_allocsrb 0, %0" : "=r"(Var)); \
	for (int __iter = 0; __iter < 32; ++__iter) { \
	    __asm__("r_write %0, 0, %%g0" : : "r"(Var|__iter)); \
	} \
    } while(0)
#define sl_free_redvar(Var) __asm__ __volatile__("r_freesrb %0" : : "r"(Var));

#define sl_foldvar(Var, Type, Op, Init) ({	\
	    Type __res = (Init); \
	    for (unsigned __VarI = 0; __VarI < 32; ++__VarI) \
	    { \
		Type __thisval; \
		__asm__ ("r_read %1, %0" : "=&r"(__thisval) : "r"((Var)|__VarI)); \
		__res = __res Op __thisval; \
	    };				    \
	    __res; })

#endif
