#include "quantum.h"

void eeconfig_init_kb(void) {
#ifdef RGBLIGHT_ENABLE
    rgblight_enable(); 				// Enable RGB by default
    rgblight_sethsv(0, 200, 120);  	// Set default HSV - orange hue, high saturation, high brightness
#ifdef RGBLIGHT_EFFECT_RAINBOW_SWIRL
    rgblight_mode(RGBLIGHT_MODE_RAINBOW_SWIRL + 2); // RGB_RAINBOW_SWIRL to be set as default on power on default
#endif
#endif

    eeconfig_update_kb(0);
    eeconfig_init_user();
}
