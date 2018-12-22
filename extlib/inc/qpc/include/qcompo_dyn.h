 /****************************************************************************
 *  This is a generic dynamic component malloc and free mechanism.           *
 *  In this mechanism, we set several reserved components that will not be   *
 *  free. And change this reserve num based on component "new" and "free"    *
 *  frequency.								     *
 *  Every time we "new" a component, we test if there is any existing        *
 *  component that not in use. If we find an available one, we return it,    *
 *  else we return a new allocated one.					     *
 *                                                                           *
 *  To do this, you must provide certain domains in your component's father  *
 *  QActive. Assume your component's father QActive type is "T_", the        *
 *  component type is auto named as T__comp.                                 *
 *         .uint32 comp_newed     (Component we have newed.)                 *
 *         .uint32 comp_reserved  (Component num reserved.)                  *
 *         .uint32 comp_num       (Current component num.)                   *
 *         .T_* T__comp_head      (Component list head pointer.)             *
 *  Also You must provide a next pointer in your component. This is used for *
 *  the component list.                                                      *
 *         .T__comp *next                                                    *
 *                                                                           *
 *  Two call back functions are set to do something after new or free a      *
 *  component. You should provide them in parameters of NEW_COMPONENT and    *
 *  FREE_COMPONENT.                                                          *
 *         .NewCallBack(T_*)                                                 *
 *         .FreeCallBack(T_*)                                                *
 *                                                                           *
 *  A "CanFree" call back function is needed, in FREE_COMPONENT, to decide   *
 *  whether free a component or not. You should provide your own free        *
 *  strategy.                                                                *
 *         .CanFree(T_*)                                                     *
 *                                                                           *
 *  To use this mechanism, you should call "DynamicCompInit(T_)" first to    *
 *  initialize a serise of functions:                                        *
 *      . FindAvailableComp   (Find a free component that exist.)            *
 *      . ComponentValid      (Test a component pointer.)                    *
 *      . NewComponent        (Allocate a new component, if cann't find an   *
 *                             available one.)                               *
 *      . FreeComponent       (Free a component, if current component number *
 *                             exceed reserved component num.)               *
 *  Second, implement two call back functions for new and free.              *
 *  Then call them where you want.                                           *
 *                                                                           *
 *  NOTE! These functions is called in the following way(Don't forget"(T_)"):*
 *      NEW_COMPONENT(T_)(T_* , NewCallBack);                                *
 *      FREE_COMPONENT(T_)(T_*, T__comp*, CanFree, FreeCallBack);            *
 *      FIND_AVAILABLE_COMP(T_)(T_*);                                        *
 *      COMPONENT_VALID(T_)(T_*, T__comp *);                                 *
 *                                                                           *
 *****************************************************************************/

#ifndef _DYNAMIC_H_
#define _DYNAMIC_H_

typedef char Bool;
#define true 1
#define false 0

#define DynamicCompDeclare(T_)\
T_##_comp * T_##_FindAvailableComp(T_ *me);\
Bool T_##_ComponentValid(T_ *me, T_##_comp * component);\
T_##_comp* T_##_NewComponent(T_ *me, void (*NewCallBack)(T_*));\
void T_##_FreeComponent(T_ * me, T_##_comp *comp, Bool (*CanFree)(T_*), void (*FreeCallBack)(T_*));\

#define DynamicCompInit(T_)\
FindAvailableComp(T_);\
ComponentValid(T_);\
NewComponent(T_);\
FreeComponent(T_);

/***** Find an available component. ****/
#define FindAvailableComp(T_) \
T_##_comp * T_##_FindAvailableComp(T_ *me){ \
	T_##_comp * x = me->T_##_comp_head; \
	while((x != NULL) && (x->inuse == true)){ \
		x = x->next; \
	} \
	return x; \
}

#define FIND_AVAILABLE_COMP(T_) T_##_FindAvailableComp

/***** Test a component. ****/
#define ComponentValid(T_)\
Bool T_##_ComponentValid(T_ *me, T_##_comp * component) { \
	T_##_comp * x = me->T_##_comp_head; \
 \
	while ((x != NULL)) \
	{ \
		if (x == component) break; \
		x = x->next; \
	} \
 \
	if (x == NULL){ \
		return false; \
	} else { \
		return true; \
	} \
}

#define COMPONENT_VALID(T_) T_##_ComponentValid

/***** New a component. ****/
#define NewComponent(T_) \
T_##_comp* T_##_NewComponent(T_ *me, void (*NewCallBack)(T_*)) { \
	T_##_comp * freeComp = FIND_AVAILABLE_COMP(T_)(me); \
	if (freeComp) \
	{ \
		freeComp->inuse = FF_TRUE; \
		return freeComp; \
	} else { \
		T_##_comp * newComp = bbu_malloc(sizeof(T_##_comp)); \
		if (newComp) \
		{ \
			memset(newComp, '\0', sizeof(T_##_comp)); \
\
			T_##_comp_ctor(newComp); \
			T_##_comp_init(newComp); \
\
			newComp->inuse = FF_TRUE; \
\
			newComp->next = me->T_##_comp_head; \
			me->T_##_comp_head = newComp; \
\
			me->comp_newed += 1; \
			me->comp_num += 1; \
			if (me->comp_newed > 5) \
			{ \
				me->comp_reserved += 1; \
				me->comp_newed = 0; \
			} \
\
			NewCallBack(me); \
		} else { \
			bbu_printf(#T_,": Malloc Error!\n\rBBU> "); \
		} \
		return newComp; \
	} \
}

#define NEW_COMPONENT(T_) T_##_NewComponent

/******* Free a component. *****/
#define FreeComponent(T_)\
void T_##_FreeComponent(T_ * me, T_##_comp *comp, Bool (*CanFree)(T_*), void (*FreeCallBack)(T_*)) { \
	if( !COMPONENT_VALID(T_)(me, comp) ){ \
		bbu_printf("%s: Free a NULL component\n\rBBU> ", #T_); \
	} else if (CanFree(me)){ \
		T_##_comp *p1 = me->T_##_comp_head; \
 \
		if (p1 == comp){ \
			me->T_##_comp_head = p1->next; \
		} else { \
			while (p1->next != comp) \
			{ \
				p1 = p1->next; \
			} \
 \
			p1->next = comp->next; \
		} \
 \
		bbu_mfree(comp); \
 \
		me->comp_num -= 1; \
 \
		FreeCallBack(me); \
	} else { \
		comp->inuse = false; \
	} \
}

#define FREE_COMPONENT(T_) T_##_FreeComponent

#endif
