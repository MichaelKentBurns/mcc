
#include </sys/ins/base.ins.c>
#include </sys/ins/cal.ins.c>
unsigned long int mccgettick()
 {  
   time_$clock_t clock;
   cal_$get_local_time (clock); 
   return ((unsigned long int) clock.c2.low32);
 } 
