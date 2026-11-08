/* SPDX-License-Identifier: GPL-3.0-only
 * Original JIMBONES pixel skull for the 16x2 character LCD.
 * The image is 10 pixels wide and 12 high, spread across four custom
 * CGRAM glyphs, with 0..4 pixels vertical bounce (not borrowed artwork).
 */
#include "menu/JimbonesSkull.h"
#include <stdint.h>

static const uint16_t skull10x12[12] = {
    0x0FCu, /*   ######   */
    0x1FEu, /*  ########  */
    0x3FFu, /* ########## */
    0x333u, /* ##  ##  ## */
    0x333u, /* ##  ##  ## */
    0x3FFu, /* ########## */
    0x37Bu, /* ## #### ## */
    0x1FEu, /*  ########  */
    0x0FCu, /*   ######   */
    0x0A8u, /*   # # #    */
    0x0FCu, /*   ######   */
    0x078u, /*    ####    */
};
uint8_t jb_skull_yoffset(uint8_t frame) {
    static const uint8_t bounce[JB_SKULL_FRAMES]={0u,1u,2u,3u,4u,3u,2u,1u};
    return bounce[frame%JB_SKULL_FRAMES];
}
void jb_skull_make_frame(uint8_t frame,uint8_t out[JB_SKULL_GLYPHS][JB_SKULL_ROWS]) {
    if(!out)return;
    const uint8_t top=jb_skull_yoffset(frame);
    for(uint8_t cellrow=0u;cellrow<2u;cellrow++){
        for(uint8_t y=0u;y<8u;y++) {
            const uint8_t screeny=(uint8_t)(cellrow*8u+y);
            const int source=(int)screeny-(int)top;
            const uint16_t image=(source>=0 && source<12)?skull10x12[source]:0u;
            /* LCD custom glyphs are each 5 horizontal pixels, not bitmaps
             * with global frame addressing. LCD renders only low five bits. */
            out[cellrow*2u][y]=(uint8_t)((image>>5u)&31u);
            out[cellrow*2u+1u][y]=(uint8_t)(image&31u);
        }
    }
}
