#ifndef __EINK_DISPLAY_APP__
#define __EINK_DISPLAY_APP__

enum pelion_state_t {
	CONNECTING, 
	CONNECTED,
	REGISTERED,
	REQUESTED,
	PROCESSING,
	DOWNLOADING,
	INSTALLING,
	COMPLETE,
	DEREGISTERED
}; 

/* Function prototypes */
int eink_display_app_start(void);
void set_pelion_state(pelion_state_t state);
void set_pelion_download_percent(uint8_t p);
void set_fw_version(uint8_t maj, uint8_t min, uint8_t pat);

#endif
