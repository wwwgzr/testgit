#define FAILURE  (-1)
#define SUCCESS    0
#define TRUE       1
#define FALSE      0

#define	CS8900_Base					0x4000000

#define MAXLOOP       0x8888

#define FRAME_TYPE_IP       0x0800
#define FRAME_TYPE_ARP      0x0806
#define FRAME_TYPE_RARP     0x8035
#define FRAME_TYPE_NETWARE  0x8137

#define SIGNATURE_PKTPG_PTR 0x3000
#define EISA_NUM_CRYSTAL    0x630E
#define PROD_ID_MASK        0xE000
#define PROD_ID_CS8900      0x0000

#define PORT_RXTX_DATA     	(0x300)
#define PORT_RXTX_DATA_1   	(0x302)
#define PORT_TX_CMD        	(0x304)
#define PORT_TX_LENGTH     	(0x306)
#define PORT_ISQ           	(0x308)
#define PORT_PKTPG_PTR     	(0x30A)
#define PORT_PKTPG_DATA    	(0x30C)
#define PORT_PKTPG_DATA_1  	(0x30E)
#define CS8900_RXTX_DATA0		(*(volatile unsigned short*)(CS8900_Base + PORT_RXTX_DATA))
#define CS8900_RXTX_DATA1		(*(volatile unsigned short*)(CS8900_Base + PORT_RXTX_DATA_1))
#define CS8900_TX_CMD				(*(volatile unsigned short*)(CS8900_Base + PORT_TX_CMD))
#define CS8900_TX_LENGTH		(*(volatile unsigned short*)(CS8900_Base + PORT_TX_LENGTH))
#define CS8900_ISQ					(*(volatile unsigned short*)(CS8900_Base + PORT_ISQ))
#define CS8900_PKTPG_PTR		(*(volatile unsigned short*)(CS8900_Base + PORT_PKTPG_PTR))
#define CS8900_PKTPG_DATA		(*(volatile unsigned short*)(CS8900_Base + PORT_PKTPG_DATA))
#define CS8900_PKTPG_DATA1	(*(volatile unsigned short*)(CS8900_Base + PORT_PKTPG_DATA_1))


#define PKTPG_EISA_NUM     0x0000
#define PKTPG_PRODUCT_ID   0x0002
#define PKTPG_RX_CFG       0x0102
#define PKTPG_RX_CTL       0x0104
#define PKTPG_TX_CFG       0x0106
#define PKTPG_BUF_CFG      0x010A
#define PKTPG_LINE_CTL     0x0112
#define PKTPG_SELF_CTL     0x0114
#define PKTPG_BUS_CTL      0x0116
#define PKTPG_TX_EVENT     0x0128
#define PKTPG_SELF_ST      0x0136
#define PKTPG_BUS_ST       0x0138
#define PKTPG_IND_ADDR     0x0158

/* Self Control Register */
#define SELF_CTL_ID        0x0015
#define SELF_CTL_RESET     0x0040
#define SELF_CTL_HC1E      0x2000
#define SELF_CTL_HCB1      0x8000


/* Self Status Register */
#define SELF_ST_INIT_DONE  0x0080
#define SELF_ST_SI_BUSY    0x0100
#define SELF_ST_EEP_PRES   0x0200
#define SELF_ST_EEP_OK     0x0400
#define SELF_ST_EL_PRES    0x0800

/* Bus Control Register */
#define BUS_CTL_USE_SA     0x0200
#define BUS_CTL_MEM_MODE   0x0400
#define BUS_CTL_IOCHRDY    0x1000
#define BUS_CTL_INT_ENBL   0x8000

/* Line Control Register */
#define LINE_CTL_ID        0x0013
#define LINE_CTL_RX_ON     0x0040
#define LINE_CTL_TX_ON     0x0080
#define LINE_CTL_AUI_ONLY  0x0100
#define LINE_CTL_10BASET   0x0000

/* Receiver Configuration Register */
#define RX_CFG_ID          0x0003
#define RX_CFG_SKIP        0x0040
#define RX_CFG_RX_OK_IE    0x0100
#define RX_CFG_CRC_ERR_IE  0x1000
#define RX_CFG_RUNT_IE     0x2000
#define RX_CFG_X_DATA_IE   0x4000

/* Receiver Control Register */
#define RX_CTL_ID          0x0005
#define RX_CTL_RX_OK_A     0x0100
#define RX_CTL_MCAST_A     0x0200
#define RX_CTL_IND_A       0x0400
#define RX_CTL_BCAST_A     0x0800
#define RX_CTL_CRC_ERR_A   0x1000
#define RX_CTL_RUNT_A      0x2000
#define RX_CTL_X_DATA_A    0x4000

/* Transmit Configuration Register */
#define TX_CFG_ID          0x0007
#define TX_CFG_LOSS_CRS_IE 0x0040
#define TX_CFG_SQE_ERR_IE  0x0080
#define TX_CFG_TX_OK_IE    0x0100
#define TX_CFG_OUT_WIN_IE  0x0200
#define TX_CFG_JABBER_IE   0x0400
#define TX_CFG_16_COLL_IE  0x8000
#define TX_CFG_ALL_IE      0x8FC0

/* Buffer Configuration Register */
#define BUF_CFG_ID         0x000B
#define BUF_CFG_SW_INT     0x0040
#define BUF_CFG_RDY4TX_IE  0x0100
#define BUF_CFG_TX_UNDR_IE 0x0200

/* Register Numbers */
#define REG_NUM_MASK       0x003F
#define REG_NUM_RX_EVENT   0x0004
#define REG_NUM_TX_EVENT   0x0008
#define REG_NUM_BUF_EVENT  0x000C
#define REG_NUM_RX_MISS    0x0010
#define REG_NUM_TX_COL     0x0012

/* Receiver Event Register */
#define RX_EVENT_RX_OK     0x0100
#define RX_EVENT_IND_ADDR  0x0400
#define RX_EVENT_BCAST     0x0800
#define RX_EVENT_CRC_ERR   0x1000
#define RX_EVENT_RUNT      0x2000
#define RX_EVENT_X_DATA    0x4000

#define	MY_TCP_PORT          1024
#define	MY_UDP_PORT          1025

#define	ETH_HEADER_START     0
#define	IP_HEADER_START      7
#define	ARP_HEADER_START     7
#define	TCP_HEADER_START     17
#define	UDP_HEADER_START     17
#define	ICMP_HEADER_START    17
#define	USER_DATA_START      27

#define	CS8900_HEADER_SIZE   2 
#define	ETH_HEADER_SIZE      7
#define	IP_HEADER_SIZE       10
#define	TCP_HEADER_SIZE      10
#define	UDP_HEADER_SIZE      4
#define	ARP_FRAME_SIZE       14
#define	ICMP_HEADER_SIZE     2
#define	DUMMY_HEADER_SIZE    6
#define	MY_MAX_SEG_SIZE      1460

#define Frame_ARP            0x0806
#define Frame_IP             0x0800
#define Ip_Edition           0x4500              //Ip 版本和IP首部长度
#define DEFUALT_TTL          128
#define ICMP_ECHO            8
#define ICMP_ECHO_REPLY      0

#define HARDW_ETH            1
#define IP_HLEN_PLEN         0x0604
#define OP_ARP_REQUEST       1
#define OP_ARP_ANSWER        2

#define PROTOCOL_ICMP        1
#define PROTOCOL_TCP         6
#define PROTOCOL_UDP         17

#define TCP_CODE_FIN         0x0001
#define TCP_CODE_SYN         0x0002
#define TCP_CODE_RST         0x0004
#define TCP_CODE_PSH         0x0008
#define TCP_CODE_ACK         0x0010
#define TCP_CODE_URG         0x0020

#define TCP_STATE_LISTEN        0
#define TCP_STATE_SYN_RCVD      1
#define TCP_STATE_SYN_SENT      2
#define TCP_STATE_ESTABLISHED   3
#define TCP_STATE_FIN_WAIT1     4
#define TCP_STATE_FIN_WAIT2     5
#define TCP_STATE_CLOSING       6
#define TCP_STATE_CLOSE_WAIT    7
#define TCP_STATE_LAST_ACK      8
#define TCP_STATE_CLOSED        9
#define TCP_STATE_TIME_WAIT     10

// 20110221 上传参数
typedef struct
{
	unsigned char cChinaEn;
	unsigned char cSprinklerCount;
	unsigned char cSprinklerNo;
	unsigned char cBidirectional;
	unsigned char cLineWidth;
	unsigned char cMultiDotLine12;
	unsigned char cMultiDotLine23;
	unsigned char cMultiDotLine34;
	unsigned short MyIP[2];
	unsigned short SubnetMask[2];
	unsigned short GatewayIP[2];
	unsigned short sPlotType;
	unsigned short sNull1;
	int iOriginalPos;
	int	iYDotSpace12;
	int	iYDotSpace23;
	int	iYDotSpace34;
	double dErrorX;
	double dErrorY;
	double dPictureErrorX;
	double dPictureErrorY;
}SendPara;
