// VTEX custom-effect

DEFINE_GRADIENT_PALETTE(vtex_gp){
  0xf7, 0x19, 0x63, 0,  // rebel pink
  0xff, 0xc4, 0xdd, 0,  // bubble gun pink
  0xff, 0xe0, 0xef, 0,  // yougurt pink
  0xff, 0xf3, 0xf6, 0,  // soft pink
};

CRGBPalette16 vtex_palette = vtex_gp;

uint8_t vtex_pattern_index = 0;

void vtex() {
  fill_palette(leds, NUM_LEDS, vtex_pattern_index, 255 / NUM_LEDS, vtex_palette, gCurrentBrightness, LINEARBLEND);
  vtex_pattern_index++;
}