/*
*********************************************************************************************************
*                                                uC/OS-III
*                                          The Real-Time Kernel
*
*
*                           (c) Copyright 2009-2010; Micrium, Inc.; Weston, FL
*                    All rights reserved.  Protected by international copyright laws.
*
*                                        80x86/80x88 Specific code
*                                          PROTECTED MEMORY MODEL
*
*                                                VC++ 6.0
*
* File        : OS_CPU_C.C
* By          : Jean J. Labrosse
*
* LICENSING TERMS:
* ---------------
*             uC/OS-III is provided in source form to registered licensees ONLY.  It is
*             illegal to distribute this source code to any third party unless you receive
*             written permission by an authorized Micrium representative.  Knowledge of
*             the source code may NOT be used to develop a similar product.
*
*             Please help us continue to provide the Embedded community with the finest
*             software available.  Your honesty is greatly appreciated.
*
*             You can contact us at www.micrium.com.
*
* For         : 80x86/80x88
* Toolchain   : VC++6.0
* From        : �ļ� Email:ganganwen@163.com & Lingjun Chen(jorya_txj)(www.raw-os.com)
* Modify      : ���� Email:591881218@qq.com
*********************************************************************************************************
*/

#if 1
#define   OS_CPU_GLOBALS
#endif

#ifdef VSC_INCLUDE_SOURCE_FILE_NAMES
const  CPU_CHAR  *os_cpu_c__c = "$Id: $";
#endif

#include <os.h>
#include <system.h>
#include <stdio.h>
#include <windows.h>


static HANDLE MainHandle;	//���߳̾��
static CONTEXT Context;	    //���߳�������
CPU_SR FlagEn = DISABLE;	//�Ƿ�ִ��ʱ�ӵ��ȵı�־

#if (OS_MSG_TRACE > 0)
static int OSPrintf(char  *str, ...);
#endif


void VCInit(void)
{
	HANDLE cp,ct;


	cp = GetCurrentProcess();	//�õ���ǰ���̾��
	ct = GetCurrentThread();	//�õ���ǰ�߳�α���

	SetProcessAffinityMask(cp, 1L); //Select the first CPU

#if  FP_SAVE_RESTORE_EN > 0u
	Context.ContextFlags = CONTEXT_FULL; //Don't need to save CONTEXT_FLOATING_POINT
#else
	Context.ContextFlags = (CONTEXT_FULL | CONTEXT_FLOATING_POINT); //Important
#endif

	DuplicateHandle(cp, ct, cp, &MainHandle, 0, TRUE, 2);	//α���ת�����õ��߳�����
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          OSPrintf()
*
* Description: This function is analog of printf.
*
* Arguments  : str      is a pointer to format string output.
*********************************************************************************************************
*/

#if (OS_MSG_TRACE > 0)
static int OSPrintf(char  *str, ...)
{
    va_list  param;
    int      ret;
	CPU_SR_ALLOC();


	CPU_CRITICAL_ENTER();
    va_start(param, str);
    ret = vprintf(str, param);
    va_end(param);
	CPU_CRITICAL_EXIT();

    return (ret);
}
#endif


/*$PAGE*/
/*
**********************************************************************************************************
*                                       INITIALIZE A TASK'S STACK
*
* Description: This function is called by OS_Task_Create() or OSTaskCreateExt() to initialize the stack
*              frame of the task being created. This function is highly processor specific.
*
* Arguments  : p_task       Pointer to the task entry point address.
*
*              p_arg        Pointer to a user supplied data area that will be passed to the task
*                               when the task first executes.
*
*              p_stk_base   Pointer to the base address of the stack.
*
*              stk_size     Size of the stack, in number of CPU_STK elements.
*
*              opt          Options used to alter the behavior of OS_Task_StkInit().
*                            (see OS.H for OS_TASK_OPT_xxx).
*
* Returns    : Always returns the location of the new top-of-stack' once the processor registers have
*              been placed on the stack in the proper order.
*
* Note(s)    : 1) Interrupts are enabled when task starts executing.
*
*              2) All tasks run in Thread mode, using process stack.
**********************************************************************************************************
*/

CPU_STK  *OSTaskStkInit (OS_TASK_PTR    p_task,
                         void          *p_arg,
                         CPU_STK       *p_stk_base,
                         CPU_STK       *p_stk_limit,
                         CPU_STK_SIZE   stk_size,
                         OS_OPT         opt)
{
	CPU_INT32U  *stk;								/* console �¼Ĵ���Ϊ32λ��									 */
	CPU_INT32U   ptos = (CPU_INT32U)(p_stk_base + stk_size - 2);


    opt    = opt;									/* 'opt' is not used, prevent warning						 */
    stk    = (CPU_INT32U *)ptos;					/* Load stack pointer										 */
    *--stk = (CPU_INT32U)p_arg;					    /* Simulate call to function with argument					 */
    *--stk = (CPU_INT32U)0x00000000;				/*�ӳ����Ǵӵ�ǰesp��4��ȡ�ô���Ĳ��������Դ˴�Ҫ�ճ�4���ֽ�*/
    *--stk = (CPU_INT32U)p_task;					/* Put pointer to task   on top of stack					 */
    *--stk = (CPU_INT32U)0x00000202;				/* EFL = 0X00000202											 */
    *--stk = (CPU_INT32U)0xAAAAAAAA;                /* EAX = 0xAAAAAAAA											 */
    *--stk = (CPU_INT32U)0xCCCCCCCC;                /* ECX = 0xCCCCCCCC											 */
    *--stk = (CPU_INT32U)0xDDDDDDDD;                /* EDX = 0xDDDDDDDD											 */
    *--stk = (CPU_INT32U)0xBBBBBBBB;                /* EBX = 0xBBBBBBBB											 */
    *--stk = (CPU_INT32U)0x00000000;                /* ESP = 0x00000000							                 */
    *--stk = (CPU_INT32U)0x11111111;                /* EBP = 0x11111111											 */
    *--stk = (CPU_INT32U)0x22222222;                /* ESI = 0x22222222											 */
    *--stk = (CPU_INT32U)0x33333333;                /* EDI = 0x33333333											 */

	if(opt & OS_OPT_TASK_SAVE_FP)                   /* ��������ʱͨ������Optѡ��ʹ�ø���ջ                       */
	{
		stk -= FP_STK_SIZE;

		asm("\
			push %eax;\
			movl %eax,stk;\
			fsave eax;\
			pop %eax;\
			");
	}
                             
    return ((CPU_STK *)stk);
}


/*$PAGE*/
/*
/**********************************************************************************************************
;                                          START MULTITASKING
;                                       void OSStartHighRdy(void)
;
; The stack frame is assumed to look as follows:
;
; OSTCBHighRdy->OSTCBStkPtr --> EDI                               (Low memory)                           
;                               ESI
;                               EBP
;                               ESP
;                               EBX
;                               EDX
;                               ECX
;                               EAX
;                               Flags to load in PSW
;                               OFFSET  of task code address
;								4 empty byte
;                               OFFSET  of 'pdata'				  (High memory)
;                                              
;
; Note : OSStartHighRdy() MUST:
;           a) Call OSTaskSwHook() then,
;           b) Set OSRunning to TRUE,
;           c) Switch to the highest priority task.
;**********************************************************************************************************
*/

void OSStartHighRdy(void)
{
	OSTaskSwHook();
	OSRunning = TRUE;

	asm("\
		movl %ebx,OSTCBCurPtr;\
		movl %esp,%ebx;\
		");

	if(OSTCBCurPtr->Opt & OS_OPT_TASK_SAVE_FP)
	{
		asm("\
			frstor esp;\
			addl %esp, FP_BYTES;\
			");
	}

	asm("\
		pop ad;\
		pop fd;\
		ret;\
		");
}


/*$PAGE*/
/*
/*********************************************************************************************************
;                                PERFORM A CONTEXT SWITCH (From task level)
;                                           void OSCtxSw(void)
;
; Note(s): 1) �˺���Ϊ�ֶ������л�����
;
;          2) The stack frame of the task to suspend looks as follows:
;
;                 SP -> OFFSET  of task to suspend    (Low memory)
;                       PSW     of task to suspend    (High memory)
;
;          3) The stack frame of the task to resume looks as follows:
;
; OSTCBHighRdy->OSTCBStkPtr --> EDI                               (Low memory)                           
;                               ESI
;                               EBP
;                               ESP
;                               EBX
;                               EDX
;                               ECX
;                               EAX
;                               Flags to load in PSW
;                               OFFSET  of task code address	  (High memory)
;                                    
;**********************************************************************************************************
*/

void OSCtxSw(void)
{
	asm("\
		lea NEXTSTART,%eax;\
		push %eax;\
		pushf;\
		pusha;\
		");

	if(OSTCBCurPtr->Opt & OS_OPT_TASK_SAVE_FP)
	{
		asm("\
			addl %esp, -FP_BYTES;\
			fsave esp;\
			");
	}

	asm("\
		movl %ebx,OSTCBCurPtr;\
		movl %ebx,%esp;\
		");

	OSTaskSwHook();
	OSTCBCurPtr = OSTCBHighRdyPtr;
	OSPrioCur   = OSPrioHighRdy;

	FlagEn = ENABLE;            //����ʱ���ж�

	asm("\
		movl %ebx, OSTCBCurPtr;\
		movl %esp, %ebx;\
		");

	if(OSTCBCurPtr->Opt & OS_OPT_TASK_SAVE_FP)
	{
		asm("\
			frstor esp;\
			addl %esp, FP_BYTES;\
			");
	}

	asm("\
		pop ad;\
		pop fd;\
		ret;\
		");

NEXTSTART:					    //�����л����������е�ַ
	return;
}


/*$PAGE*/
/*
/*********************************************************************************************************
;                                PERFORM A CONTEXT SWITCH (From an ISR)
;                                        void OSIntCtxSw(void)
;
; Note(s): 1) Context���������߳����е������ģ�����л������Ƚϼ򵥣��ȱ�����Ӧ�Ĵ���
			  Ȼ���Ҫ���е����������������Context�ṹ�����棬����л�����ջ�������£�
;
; OSTCBHighRdy->OSTCBStkPtr --> EDI                               (Low memory)                           
;                               ESI
;                               EBP
;                               ESP
;                               EBX
;                               EDX
;                               ECX
;                               EAX
;                               Flags to load in PSW
;                               OFFSET  of task code address	  (High memory)
;***********************************************************************************************************
*/

void OSIntCtxSw(void)
{
	CPU_INT32U          *top;

#if  FP_SAVE_RESTORE_EN == 0u
	FLOATING_SAVE_AREA  *FloatSave;
#endif


	OSTaskSwHook();

	top       = (CPU_INT32U *)Context.Esp;				    //������߳�(��ǰ����)��ջָ��

#if  FP_SAVE_RESTORE_EN == 0u
	FloatSave = (FLOATING_SAVE_AREA *)(&Context.FloatSave); //��ȡԤ�ȱ���ĸ������ֳ�
#endif

	//������ջ�б������еļĴ���
	*--top = Context.Eip;
	*--top = Context.EFlags;
	*--top = Context.Eax;
	*--top = Context.Ecx;
	*--top = Context.Edx;
	*--top = Context.Ebx;
	*--top = Context.Esp;									//�����SP��û������
	*--top = Context.Ebp;
	*--top = Context.Esi;
	*--top = Context.Edi;

	if(OSTCBCurPtr->Opt & OS_OPT_TASK_SAVE_FP)
	{
		top -= FP_STK_SIZE;									//��������ջջ��ָ��

#if  FP_SAVE_RESTORE_EN > 0u
		asm("\
			push %eax;\
			movl %eax,top;\
			fsave eax;\
			pop %eax;\
			");
#else
		memcpy(top, FloatSave, sizeof(FLOATING_SAVE_AREA)); //���渡�����ֳ�������ջ
#endif
	}

	OSTCBCurPtr->StkPtr = (CPU_STK *)top;					//��������ջջ��ָ��

	OSTCBCurPtr = OSTCBHighRdyPtr;							//��õ�ǰ������������ȼ������TCB
	OSPrioCur   = OSPrioHighRdy;							//��õ�ǰ������������ȼ���������ȼ�

	top = (CPU_INT32U *)(OSTCBCurPtr->StkPtr);				//��ý�Ҫִ�е�����Ķ�ջָ��

	if(OSTCBCurPtr->Opt & OS_OPT_TASK_SAVE_FP)
	{
#if  FP_SAVE_RESTORE_EN > 0u
		asm("\
			push %eax;\
			movl %eax,top;\
			frstor eax;\
			pop %eax;\
			");
#else
		memcpy(FloatSave, top, sizeof(FLOATING_SAVE_AREA)); //������ջ�����������ֳ�
                                                            //�ָ��������ֳ�
		Context.FloatSave = *(FLOATING_SAVE_AREA *)FloatSave;
#endif

		top += FP_STK_SIZE;									//��������ջջ��ָ��
	}

	//������ջ�лָ����еļĴ���
	Context.Edi    = *top++;
	Context.Esi    = *top++;
	Context.Ebp    = *top++;
	Context.Esp    = *top++;								//�����SP��û������
	Context.Ebx    = *top++;
	Context.Edx    = *top++;
	Context.Ecx    = *top++;
	Context.Eax    = *top++;
	Context.EFlags = *top++;
	Context.Eip    = *top++;

	Context.Esp = (unsigned long)top;						//�����ȷ��esp

#if 0
	/*SetThreadContext(mainhandle, &Context);*/				//�������߳������ģ�����ʹ��
#endif
}


/*$PAGE*/
/*
/*********************************************************************************************************
;                                            HANDLE TICK ISR
;
; Description:  �˺���Ϊʱ�ӵ��ȹؼ�����ͨ��timeSetEvent����������ʱ�ӽ��ģ�timeSetEvent������һ��
;				�̵߳��ô˺������˺�������������ͬ�����У���˲������ж������������д˺����ģ����˺���
;               ��ģ���жϵĲ�������������жϴ��ڹر�״̬�����˳�
;
; Arguments  : none
;
; Returns    : none
;
; Note(s)    :  �˺������жϺ������Ĳ�ͬ��û����������Ĵ��������Ҳ�����iretָ�����OSIntCtxSw()ʵ��
                Ҳ��һ��������Ҫ���ص��ú�����
;**********************************************************************************************************
*/

void CALLBACK OSTickISR(unsigned int a,unsigned int b,unsigned long c,unsigned long d,unsigned long e)
{
	BOOL  Value;


	if(!FlagEn)
	{
		return;								//�����ǰ�жϱ������򷵻�
	}

	Value = SuspendThread(MainHandle);	    //��ֹ���̵߳����У�ģ���жϲ�������û�б���Ĵ���

	if(Value < 0)                           //�����߳�ʧ�ܣ�����
	{
		return;
	}

	if(!FlagEn)
	{										//��SuspendThread�����ǰ��FlagEn���ܱ��ٴθĵ�
		do {                                //ģ���жϷ��أ����̵߳��Լ���ִ��
			Value = ResumeThread(MainHandle);
		} while(Value > 0);

		return;								//�����ǰ�жϱ������򷵻�
	}

	GetThreadContext(MainHandle, &Context);	//������߳������ģ�Ϊ�л�������׼��

	OSIntNestingCtr++;						//�ж�Ƕ�ײ�����1

	OSTimeTick();							//uC/OS�ڲ�ʱ���жϷ����ӳ���
	OSIntExit();							//���ڲ���ʹ���жϷ���ָ����Դ˺�����Ҫ���ص�

	SetThreadContext(MainHandle, &Context); //�������̵߳�������

	do {
		Value = ResumeThread(MainHandle);   //ģ���жϷ��أ����̵߳��Լ���ִ��
	} while(Value > 0);
}


/*
*********************************************************************************************************
*                                           IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do
*              such things as STOP the CPU to conserve power.
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSIdleTaskHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u

#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSInitHook (void)
{

}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         STATISTIC TASK HOOK
*
* Description: This function is called every second by uC/OS-III's statistics task.  This allows your
*              application to add functionality to the statistics task.
*
* Arguments  : None.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSStatTaskHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u

	if(ExecEn == TRUE)
		DispTaskExecInfo();

#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          TASK CREATION HOOK
*
* Description: This function is called when a task is created.
*
* Arguments  : p_tcb        Pointer to the task control block of the task being created.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskCreateHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u

#else
    (void)p_tcb;                                            /* Prevent compiler warning                               */
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           TASK DELETION HOOK
*
* Description: This function is called when a task is deleted.
*
* Arguments  : p_tcb        Pointer to the task control block of the task being deleted.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskDelHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u

#else
    (void)p_tcb;                                            /* Prevent compiler warning                               */
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                            TASK RETURN HOOK
*
* Description: This function is called if a task accidentally returns.  In other words, a task should
*              either be an infinite loop or delete itself when done.
*
* Arguments  : p_tcb        Pointer to the task control block of the task that is returning.
*
* Note(s)    : None.
*********************************************************************************************************
*/

void  OSTaskReturnHook (OS_TCB  *p_tcb)
{
#if OS_CFG_APP_HOOKS_EN > 0u

#else
    (void)p_tcb;                                            /* Prevent compiler warning                               */
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                           TASK SWITCH HOOK
*
* Description: This function is called when a task switch is performed.  This allows you to perform other
*              operations during a context switch.
*
* Arguments  : None.
*
* Note(s)    : 1) Interrupts are disabled during this call.
*              2) It is assumed that the global pointer 'OSTCBHighRdyPtr' points to the TCB of the task
*                 that will be 'switched in' (i.e. the highest priority task) and, 'OSTCBCurPtr' points
*                 to the task being switched out (i.e. the preempted task).
*********************************************************************************************************
*/

void  OSTaskSwHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u

	if(ExecEn == TRUE)
		UpdateTaskExecInfo();

#else
    (void)p_tcb;                                            /* Prevent compiler warning                               */
#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                              TICK HOOK
*
* Description: This function is called every tick.
*
* Arguments  : None.
*
* Note(s)    : 1) This function is assumed to be called from the Tick ISR.
*********************************************************************************************************
*/

void  OSTimeTickHook (void)
{
#if OS_CFG_APP_HOOKS_EN > 0u

#endif
}


/*$PAGE*/
/*
*********************************************************************************************************
*                                          SYS TICK HANDLER
*
* Description: Handle the system tick (SysTick) interrupt, which is used to generate the uC/OS-II tick
*              interrupt.
*
* Arguments  : None.
*
* Note(s)    : 1) This function MUST be placed on entry 15 of the Cortex-M3 vector table.
*********************************************************************************************************
*/

void  OS_CPU_SysTickHandler (void)
{

}


/*$PAGE*/
/*
*********************************************************************************************************
*                                         INITIALIZE SYS TICK
*
* Description: Initialize the SysTick.
*
* Arguments  : cnts         Number of SysTick counts between two OS tick interrupts.
*
* Note(s)    : 1) This function MUST be called after OSStart() & after processor initialization.
*********************************************************************************************************
*/

void  OS_CPU_SysTickInit (CPU_INT32U  cnts)
{

}
