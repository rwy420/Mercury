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

typedef enum
{
	USB_LEGACY_SUPPORT = 1,
	SUPPORTED_PROTOCOL = 2,
} XHCI_EXTENTED_CAPABILITY_ID;

typedef enum
{
	CONTROLLER_NOT_READY = 1 << 11
} XHCI_USB_STATUS;

#endif
