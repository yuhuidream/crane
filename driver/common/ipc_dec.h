#ifndef _ipc_dec_h
#define _ipc_dec_h

typedef enum{
	IPC2AP = 1,
	IPC2CP,
	IPC2DSP,
}IpcTarget;

#define IPC  IPCC
#define THIS_CORE   "CR5"
#define THAT_CORE   "CA7"

#define  IPC_SET_GP_INT      (1<<10)
#define  IPC_SET_MSG_INT     (1<<9)   
#define  IPC_SET_CMD_INT     (1<<8)
#define  IPC_SET_ACK_INT     (1<<7)
#define  IPC_SET_PRINTF_INT  (1<<6)
#define  IPC_SET_OTHER_INT   (0x3F)
#define  IPC_SET_ALL_INT     (0x7FF)

#define  IPC_TEST_BEGIN      (0x7F000001)
#define  IPC_TEST_END        (0x7F000002)
#define  IPC_QEVENT_CMD      (0x7F000003)
#define  IPC_QEVENT_ACK      (0x7F000004)

void ipc_init(void);
void ipc_send_ack(uint32_t ack_type, IpcTarget core);
void ipc_send_cmd(uint32_t cmd_type, IpcTarget core);
void ipc_send_msg(uint32_t *msg_address, IpcTarget core);
void ipc_send_gp(uint32_t gp_type, IpcTarget core);
int bbu_ipc_test(int argc, uint8_t **argv);


#endif
