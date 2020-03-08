#ifndef CONTROL_H
#define CONTROL_H

#define ctrlCONTROL_TASK_PRIORITY       ( tskIDLE_PRIORITY + 1 )

BaseType_t xStartControlTask( void );
portTASK_FUNCTION_PROTO( vControlTask, pvParameters );

#endif /* CONTROL_H */

