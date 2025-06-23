#ifndef __MERCURY__HARDWARE__USB__XHCI_STRUCTS_H
#define __MERCURY__HARDWARE__USB__XHCI_STRUCTS_H

#include <common/types.h>

// https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/extensible-host-controler-interface-usb-xhci.pdf

typedef struct
{
	uint32_t cap_length		: 8;
	uint32_t rsvd1			: 8;
	uint32_t hci_version	: 16;

	union
	{
		uint32_t hcsparams_1_raw;

		struct
		{
			uint32_t max_slots	: 8;
			uint32_t max_ints	: 11;
			uint32_t rsvd2		: 5;
			uint32_t max_ports	: 8;
		} hcsparams_1;
	};

	union
	{
		uint32_t hcsparams_2_raw;
		
		struct
		{
			uint32_t isocronous_sched_treshold		: 4;
			uint32_t event_ring_segment_table_max	: 4;
			uint32_t rsvd3							: 13;
			uint32_t max_scratchpad_buffers_hi		: 5;
			uint32_t scrachpad_restore				: 1;
			uint32_t max_scratchpad_buffers_lo		: 5;
		} hcsparams_2;
	};

	union
	{
		uint32_t hcsparams_3_raw;

		struct
		{
			uint32_t u1_device_exit_latency : 8;
			uint32_t rsvd4					: 8;
			uint32_t u2_device_exit_latency : 16;
		} hcsparams_3;
	};

	union
	{
		uint32_t hccparams_1_raw;

		struct
		{
			uint32_t addressing_capability_64 : 1;
			uint32_t bw_negotiation_capability : 1;
			uint32_t context_size : 1;
			uint32_t port_power_control : 1;
			uint32_t port_indicators : 1;
			uint32_t light_hc_reset_capability : 1;
			uint32_t latency_tolerance_messaging_capability : 1;
			uint32_t no_secondary_sid_support : 1;
			uint32_t parse_all_event_data : 1;
			uint32_t stopped_short_packet_capability : 1;
			uint32_t stopped_edtla_capability : 1;
			uint32_t contignous_frame_id_capability : 1;
			uint32_t maximum_primary_stream_array_size : 4;
			uint32_t xhci_extented_capability_pointer : 16;
		} hccparams_1;
	};

	uint32_t dboff;
	uint32_t rstoff;
	
	union
	{
		uint32_t hccparams_2_raw;

		struct
		{
			uint32_t u3_entry_capability : 1;
			uint32_t configure_endpoint_command_max_latency_too_large_capability : 1;
			uint32_t force_save_context_capability : 1;
			uint32_t compliance_transition_capability : 1;
			uint32_t large_esit_payload_capability : 1;
			uint32_t configuration_information_capability : 1;
			uint32_t extented_tbc_capability : 1;
			uint32_t extented_tbc_trb_status_capability : 1;
			uint32_t get_set_extented_property_capability : 1;
			uint32_t rsvd5 : 22;
		} hccparams_2;
	};

} xHCICapabilityRegs;

typedef struct
{
	uint32_t capablity_id : 8;
	uint32_t next_capability : 8;
	uint32_t rsvd : 16;
} xHCIExtentedCap;

typedef struct
{
	uint32_t capablity_id;
	uint32_t next_capability;
	uint32_t hc_bios_owned : 1;
	uint32_t rsvd1 : 7;
	uint32_t hc_os_owned : 1;
	uint32_t rsvd2;
} xHCILegacyUSBSupportCap;

typedef struct
{
	uint32_t capablity_id : 8;
	uint32_t next_capability : 8;
	uint32_t minor_version : 8;
	uint32_t major_version  : 8;
	uint32_t name_string : 32;
	uint32_t compatible_port_offset : 8;
	uint32_t compatible_port_count : 8;
	uint32_t protocol_defied : 12;
	uint32_t protocol_speed_id_ount : 4;
	uint32_t protocol_slot_type : 5;
	uint32_t rsvd : 27;
} xHCISupportedProtocolCap;

typedef struct
{
	uint32_t portsc;
	uint32_t portmsc;
	uint32_t portli;
	uint32_t porthlpmc;
} xHCIPortRegs;

typedef struct
{
	uint8_t revision_major;
	uint8_t revision_minor;
	uint8_t slot_id;
} xHCIPort;

typedef struct
{
	union
	{
		uint32_t usb_cmd_raw;

		struct
		{
			uint32_t run_stop : 1;
			uint32_t host_controller_reset : 1;
			uint32_t interrupter_enable : 1;
			uint32_t host_system_error_enable : 1;
			uint32_t rsvd1 : 3;
			uint32_t light_host_controller_reset : 1;
			uint32_t controller_save_state : 1;
			uint32_t controller_restore_state : 1;
			uint32_t controller_wrap_event : 1;
			uint32_t enable_u3_mf_index_stop : 1;
			uint32_t rsvd2 : 1;
			uint32_t cem_enable : 1;
			uint32_t extented_tbc_enable : 1;
			uint32_t extented_tbc_trb_status_enable : 1;
			uint32_t vtio_enable : 1;
			uint16_t rsvd3 : 15;
		} usb_cmd;
	};

	uint32_t usbsts;
	uint32_t page_size;
	uint32_t rsvd3[2];
	uint32_t dnctrl;
	uint32_t crcr_lo;
	uint32_t crcr_hi;
	uint32_t rsvd4[4];
	uint32_t dcbaap_lo;
	uint32_t dcbaap_hi;

	union
	{
		uint32_t config_raw;

		struct
		{
			uint32_t max_device_slots_enabled : 8;
			uint32_t u3_entry_enable : 1;
			uint32_t configuration_information_enable : 1;
			uint32_t rsvd5 : 22;
		} config;
	};

	uint32_t rsvd5[241];
	xHCIPortRegs ports[];
} xHCIOperationalRegs;

typedef struct
{
	uint32_t iman;
	uint32_t imod;
	uint32_t erstsz;
	uint32_t rsvd;
	uint64_t erstba;
	uint64_t erdp;
} xHCIInterruptRegs;

typedef struct
{
	uint32_t mf_index;
	uint32_t rsvd[7];
	xHCIInterruptRegs irs[];
} xHCIRuntimeRegs;

typedef struct
{
	union
	{
		struct
		{
			uint64_t parameter : 64;
			uint32_t status : 32;
			uint32_t cycle : 1;
			uint32_t ent : 1;
			uint32_t rsvd : 8;
			uint32_t trb_type : 6;
			uint32_t control : 16;
		};

		struct
		{
			uint32_t dw0;
			uint32_t dw1;
			uint32_t dw2;
			uint32_t dw3;
		} raw;

		struct
		{
			uint64_t data_buffer_pointer : 64;
			uint32_t trb_transfer_length : 17;
			uint32_t td_size : 5;
			uint32_t interrupt_target : 10;
			uint32_t cycle_bit : 1;
			uint32_t evaluate_next_trb : 1;
			uint32_t interrupt_on_short_packet : 1;
			uint32_t no_snoop : 1;
			uint32_t chain_bit : 1;
			uint32_t interrupt_on_completion : 1;
			uint32_t immediate_data : 1;
			uint32_t rsvd1 : 2;
			uint32_t block_event_interrupt : 1;
			uint32_t trb_type : 6;
			uint32_t rsvd2 : 16;
		} normal;

		struct
		{
			uint32_t b_request_type : 8;
			uint32_t b_request : 8;
			uint32_t w_value : 16;
			uint32_t w_index : 16;
			uint32_t w_length : 16;
			uint32_t trb_transfer_length : 17;
			uint32_t rsvd1 : 5;
			uint32_t interrupt_target : 10;
			uint32_t cycle_bit : 1;
			uint32_t rsvd2 : 4;
			uint32_t interrupt_on_completion : 1;
			uint32_t immediate_data : 1;
			uint32_t rsvd3 : 3;
			uint32_t trb_type : 6;
			uint32_t transfer_type : 2;
			uint32_t rsvd4 : 14;
		} setup_stage;

		struct
		{
			uint64_t data_buffer_pointer : 64;
			uint32_t trb_transfer_length : 17;
			uint32_t td_size : 5;
			uint32_t interrupt_target : 10;
			uint32_t cycle_bit : 1;
			uint32_t evaluate_next_trb : 1;
			uint32_t interrupt_on_short_packet : 1;
			uint32_t no_snoop : 1;
			uint32_t chain_bit : 1;
			uint32_t interrupt_on_completion : 1;
			uint32_t immediate_data : 1;
			uint32_t rsvd1 : 3;
			uint32_t trb_type : 6;
			uint32_t direction : 1;
			uint32_t rsvd2 : 15;
		} data_stage;

		struct
		{
			uint32_t rsvd1 : 32;
			uint32_t rsvd2 : 32;
			uint32_t rsvd3 : 22;
			uint32_t interrupt_target : 10;
			uint32_t cycle_bit : 1;
			uint32_t evaluate_next_trb : 1;
			uint32_t rsvd4 : 2;
			uint32_t chain_bit : 1;
			uint32_t interrupt_on_completion : 1;
			uint32_t rsvd5 : 4;
			uint32_t trb_type : 6;
			uint32_t direction : 1;
			uint32_t rsvd6 : 15;
		} status_range;

		struct
		{
			uint64_t trb_pointer : 64;
			uint32_t trb_transfer_length : 24;
			uint32_t completion_code : 8;
			uint32_t cycle_bit : 1;
			uint32_t rsvd1 : 1;
			uint32_t event_data : 1;
			uint32_t rsvd2 : 7;
			uint32_t trb_type : 6;
			uint32_t endpoint_id : 5;
			uint32_t rsvd3 : 3;
			uint32_t slot_id : 8;
		} transfer_event;

		struct
		{
			uint64_t command_trb_pointer : 64;
			uint32_t command_completion_parameter : 24;
			uint32_t completion_code : 8;
			uint32_t cycle_bit : 1;
			uint32_t rsvd1 : 9;
			uint32_t trb_type : 6;
			uint32_t vf_id : 8;
			uint32_t slot_id : 8;
		} command_completion_event;

		struct
		{
			uint32_t rsvd1 : 24;
			uint32_t port_id : 8;
			uint32_t rsvd2 : 32;
			uint32_t rsvd3 : 24;
			uint32_t completion_code : 8;
			uint32_t cycle_bit : 1;
			uint32_t rsvd4 : 9;
			uint32_t trb_type : 6;
			uint32_t rsvd5 : 16;
		} port_status_change_event;

		struct
		{
			uint32_t rsvd1 : 32;
			uint32_t rsvd2 : 32;
			uint32_t rsvd3 : 32;
			uint32_t cycle_bit : 1;
			uint32_t rsvd4 : 9;
			uint32_t trb_type : 6;
			uint32_t slot_type : 5;
			uint32_t rsvd5 : 11;
		} enable_slot_command;
		
		struct
		{
			uint32_t rsvd1 : 32;
			uint32_t rsvd2 : 32;
			uint32_t rsvd3 : 32;
			uint32_t cycle_bit : 1;
			uint32_t rsvd4 : 9;
			uint32_t trb_type : 6;
			uint32_t rsvd5 : 8;
			uint32_t slot_id : 8;
		} disable_slot_command;

		struct
		{
			uint64_t input_context_pointer : 64;
			uint32_t rsvd1 : 32;
			uint32_t cycle_bit : 1;
			uint32_t rsvd2 : 8;
			uint32_t block_set_address_request : 1;
			uint32_t trb_type : 6;
			uint32_t rsvd3 : 8;
			uint32_t slot_id : 8;
		} address_device_command;

		struct
		{
			uint64_t input_context_pointer : 64;
			uint32_t rsvd1 : 32;
			uint32_t cycle_bit : 1;
			uint32_t rsvd2 : 8;
			uint32_t deconfigure : 1;
			uint32_t trb_type : 6;
			uint32_t rsvd3 : 8;
			uint32_t slot_id : 8;
		} configure_endpoint_command;

		struct
		{
			uint64_t ring_segment_pointer : 64;
			uint32_t rsvd1 : 22;
			uint32_t interrupt_target : 10;
			uint32_t cycle_bit : 1;
			uint32_t toggle_cycle : 1;
			uint32_t rsvd2 : 2;
			uint32_t chain_bit : 1;
			uint32_t interrupt_on_completion : 1;
			uint32_t rsvd3 : 4;
			uint32_t trb_type : 6;
			uint32_t rsvd4 : 16;
		} link_trb;
	};
} xHCITRB;

typedef struct
{
	uint64_t rsba;
	uint32_t rsz;
	uint32_t rsvd;
} xHCIEventRingTableEntry;

typedef enum
{
	USB_LEGACY_SUPPORT = 1,
	SUPPORTED_PROTOCOL = 2,
} XHCI_EXTENTED_CAPABILITY_ID;

typedef enum
{
	HC_HALTED = 1 << 0,
	HOST_SYSTEM_ERROR = 1 << 2,
	EVENT_INTERRUPT = 1 << 3,
	PORT_CHANGE_DETECT = 1 << 4,
	SAVE_STATE_STATUS = 1 << 8,
	RESTORE_STATE_STATUS = 1 << 9,
	SAVE_RESTORE_ERROR = 1 << 10,
	CONTROLLER_NOT_READY = 1 << 11,
	HOST_CONTROLLER_ERROR = 1 << 12
} XHCI_USB_STATUS;

typedef enum : uint32_t
{
	RING_CYCLE_STATE = 1 << 0,
	COMMAND_STOP = 1 << 1,
	COMMAND_ABOIRT = 1 << 2,
	COMMAND_RING_RUNNING = 1 << 3
} XHCI_CRCR;

typedef enum
{
	EVENT_HANDLER_BUSY = 1 << 3
} XHCI_ERDP;

typedef enum : uint32_t
{
	INTERRUPT_PEDNING = 1 << 0,
	INTERRUPT_ENBLE = 1 << 1
} XHCI_EMAN;

typedef enum : uint32_t
{
	CCS = 1 << 0,
	PED = 1 << 1,
	PR = 1 << 4,
	PP = 1 << 9,
	CSC = 1 << 17,
	PRC = 1 << 21,

	PORT_SPEED_SHIFT = 10,
	PORT_SPEED_MASK = 0xF
} PORTSC;

typedef enum
{
	LINK = 6,
	ENABLE_SLOT_COMMAND = 9
} xHCITRBCommandType;

#endif
