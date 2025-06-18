#ifndef __MERCURY__HARDWARE__USB__XHCI_STRUCTS_H
#define __MERCURY__HARDWARE__USB__XHCI_STRUCTS_H

#include <common/types.h>

// https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/extensible-host-controler-interface-usb-xhci.pdf

typedef struct
{
	uint8_t cap_length;
	uint8_t rsvd1;
	uint16_t hci_version;

	union
	{	
		uint32_t hcsparams_1_raw;

		struct
		{
			uint8_t max_slots;
			uint16_t max_ints : 11;
			uint8_t rsvd2 : 5;
			uint8_t max_ports;
		} hcsparams_1;
	};

	union
	{
		uint32_t hcsparams_2_raw;

		struct
		{
			uint8_t isocronous_sched_treshold : 4;
			uint8_t event_ring_segment_table_max : 4;
			uint16_t rsvd3 : 13;
			uint8_t max_scratchpad_buffers_hi : 5;
			uint8_t scrachpad_restore : 1;
			uint8_t max_scratchpad_buffers_lo : 5;
		} hcsparams_2;
	};

	union
	{
		uint32_t hcsparams_3_raw;
		
		struct
		{
			uint8_t u1_device_exit_latency;
			uint8_t rsvd4;
			uint16_t u2_device_exit_latency;
		} hcsparams_3;
	};

	union
	{
		uint32_t hssparams_1_raw;

		struct
		{
			uint8_t addressing_capability_64 : 1;
			uint8_t bw_negotiation_capability : 1;
			uint8_t context_size : 1;
			uint8_t port_power_control : 1;
			uint8_t port_indicators : 1;
			uint8_t light_hc_reset_capability : 1;
			uint8_t latency_tolerance_messaging_capability : 1;
			uint8_t no_secondary_sid_support : 1;
			uint8_t parse_all_event_data : 1;
			uint8_t stopped_short_packet_capability : 1;
			uint8_t stopped_edtla_capability : 1;
			uint8_t contignous_frame_id_capability : 1;
			uint8_t maximum_primary_stream_array_size : 4;
			uint16_t xhci_extented_capability_pointer;
		} hccparams_1;
	};

	union
	{
		uint32_t hccparams_2_raw;

		struct
		{
			uint8_t u3_entry_capability : 1;
			uint8_t configure_endpoint_command_max_latency_too_large_capability : 1;
			uint8_t force_save_context_capability : 1;
			uint8_t compliance_transition_capability : 1;
			uint8_t large_esit_payload_capability : 1;
			uint8_t configuration_information_capability : 1;
			uint8_t extented_tbc_capability : 1;
			uint8_t extented_tbc_trb_status_capability : 1;
			uint8_t get_set_extented_property_capability : 1;
			uint32_t rsvd5 : 22;
		} hccparams_2;
	};

} xHCICapabilityRegisters;

typedef struct
{
	uint8_t capablity_id;
	uint8_t next_capability;
	uint16_t rsvd;
} ExtentedCap;

typedef struct
{
	uint8_t capablity_id;
	uint8_t next_capability;
	uint8_t hc_bios_owned : 1;
	uint8_t rsvd1 : 7;
	uint8_t hc_os_owned : 1;
	uint8_t rsvd2;
} LegacyUSBSupportCap;

typedef enum
{
	USB_LEGACY_SUPPORT = 1
} EXTENTED_CAPABILITY_ID;

#endif
