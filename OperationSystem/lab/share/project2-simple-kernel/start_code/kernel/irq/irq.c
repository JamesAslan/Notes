#include "irq.h"
#include "time.h"
#include "sched.h"
#include "string.h"
#include "screen.h"

#define PREEMPTIVE_INTERVAL 1

uint32_t preemptive_cnt;

static void irq_timer()
{
    // TODO clock interrupt handler.
    // scheduler, time counter in here to do, emmmmmm maybe.
    // 时钟中断的触发涉及 CP0_COUNT、CP0_COMPARE 寄存器，CP0_COUNT 寄存器的值每个时钟周期会自动增加， 当 CP0_COUNT 和 CP0_COMPARE 寄存器的值相等时会触发一个时钟中断
    time_elapsed++;
    {
    // check_sleeping();
        if(queue_is_empty(&sleep_queue))
        {
            //do nothing
        }else
        {
            pcb_t* proc=sleep_queue.head;
            pcb_t* temp_pcb_p=0;
            uint32_t timepassed=0;
            do
            {
                timepassed=time_elapsed-proc->block_time;
                if(timepassed>proc->sleep_time)
                {
                    proc->status=TASK_READY;
                    temp_pcb_p=proc;
                    proc=proc->next;
                    queue_remove(&sleep_queue, temp_pcb_p);
                    queue_push(&ready_queue, temp_pcb_p);
                }else
                {
                    proc=proc->next;
                }
            }while(proc!=NULL);
        }
    }

    current_running->cursor_x=screen_cursor_x;
    current_running->cursor_y=screen_cursor_y;
    do_scheduler();//FIXIT
    screen_cursor_x=current_running->cursor_x;
    screen_cursor_y=current_running->cursor_y;

    screen_reflush();
    // set_CP0_COMPARE(TIMER_INTERVAL) in preemptive_scheduler();
    return;
}

void interrupt_helper(uint32_t status, uint32_t cause)
{
    // TODO interrupt handler.
    // Leve3 exception Handler.
    // read CP0 register to analyze the type of interrupt.
    uint32_t ip=status>>8;
    ip&=0x000000FF;
    if(ip==(1<<7))//ip[7]==1
    {
        irq_timer();
        return;
    }
}

void other_exception_handler()
{
    // TODO other exception handler
}